#!/usr/bin/env python3
"""Backfill docs/function_renames.csv with the fn_ renames made before the campaign.

Walks the git history of config/GC6E01/symbols.txt (and every module's
symbols.txt), finds every commit that changed a function's name at the same
address, and rebuilds each address's rename chain starting from its
`fn_XXXXXXXX` name. A chain is written only when it ends at the name the
address has in the current symbols.txt; reverted renames (back to fn_) and
chains that history cannot connect are skipped and counted. Intermediate names
that a symbols.txt now uses for another address are folded out of the chain
(check_naming forbids a superseded name that is still defined).

Rows are tier `historical`, evidence_ref `git:<commit>`, with the commit hash
and the commit date; batch_id `YYYYMMDD-historical-1` from that date. The
ledger is append-only: rows already present are not written again.

Usage: tools/naming/backfill_history.py [--dry-run] [--root R]
"""

from __future__ import annotations

import argparse
import csv
import re
import subprocess
import sys
from collections import Counter, defaultdict
from io import StringIO
from pathlib import Path

TOOLS = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(TOOLS))
import check_naming as cn  # noqa: E402

REPO = TOOLS.parent
DIFF_SYM = re.compile(r"^([+-])(\S+) = (\.\w+):0x([0-9A-Fa-f]+);\s*//(.*)$")
COMMIT = re.compile(r"^@@@ ([0-9a-f]{40}) (\d{4}-\d{2}-\d{2}) (.*)$")
FILE = re.compile(r"^diff --git a/(\S+) b/(\S+)")


def module_of(path: str) -> str | None:
    m = re.fullmatch(r"config/GC6E01/(?:([^/]+)/)?symbols\.txt", path)
    if not m:
        return None
    return m.group(1) or "main"


def history_events(root: Path):
    """Yield (module, addr, old, new, sha, date, subject) in commit order (oldest first)."""
    out = subprocess.run(
        ["git", "log", "--full-history", "--no-merges", "--date-order", "--reverse", "--date=short",
         "--format=@@@ %H %ad %s", "-U0", "-p", "--no-renames", "--",
         "config/GC6E01/symbols.txt", "config/GC6E01/*/symbols.txt"],
        cwd=root, check=True, capture_output=True, text=True, errors="replace").stdout
    sha = date = subject = None
    module = None
    removed: dict[tuple[str, str, int], str] = {}
    added: dict[tuple[str, str, int], str] = {}

    def flush():
        for key, old in removed.items():
            new = added.get(key)
            if new and new != old:
                yield key[0], key[2], old, new, sha, date, subject

    for line in out.splitlines():
        m = COMMIT.match(line)
        if m:
            if sha:
                yield from flush()
            sha, date, subject = m.groups()
            removed, added = {}, {}
            continue
        m = FILE.match(line)
        if m:
            module = module_of(m.group(2))
            continue
        m = DIFF_SYM.match(line)
        if m and module and "type:function" in m.group(5):
            key = (module, m.group(3), int(m.group(4), 16))
            (removed if m.group(1) == "-" else added)[key] = m.group(2)
    if sha:
        yield from flush()


def definitions_index(root: Path) -> dict[str, str]:
    """Function name -> first C file under src/ that defines it (non-headers first)."""
    idx: dict[str, str] = {}
    files = sorted(p for p in (root / "src").rglob("*") if p.suffix in cn.C_EXTS)
    files.sort(key=lambda p: p.suffix in (".h", ".hpp"))
    call = re.compile(r"(?<![A-Za-z0-9_])([A-Za-z_][A-Za-z0-9_]*)\s*\(")
    for p in files:
        code = cn.blank_spans(p.read_text(errors="replace"), {"c", "s", "ch"})
        for m in call.finditer(code):
            name = m.group(1)
            if name in idx:
                continue
            close = cn.match_close(code, m.end() - 1, "(", ")")
            if close < 0:
                continue
            j = close + 1
            while j < len(code) and code[j] in " \t\r\n":
                j += 1
            if j < len(code) and code[j] == "{":
                idx[name] = str(p.relative_to(root))
    return idx


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--root", type=Path, default=REPO)
    ap.add_argument("--dry-run", action="store_true")
    args = ap.parse_args()
    root = args.root.resolve()
    ledger = root / "docs" / "function_renames.csv"

    rep = cn.Report()
    existing = cn.read_csv(ledger, rep)
    if rep.errors:
        print("\n".join(rep.errors), file=sys.stderr)
        return 2
    have = {(r["module"], r["address"], r["old_name"], r["new_name"]) for r in existing}
    have_addr = {(r["module"], int(r["address"], 16)) for r in existing if r["tier"] != cn.HISTORICAL}

    symbols = cn.load_symbols(root)
    current: dict[tuple[str, int], str] = {}
    for name, defs in symbols.items():
        for module, addr, attrs in defs:
            if "type:function" in attrs:
                current[(module, addr)] = name

    events = defaultdict(list)
    for module, addr, old, new, sha, date, subject in history_events(root):
        events[(module, addr)].append((old, new, sha, date, subject))

    stats = Counter()
    chains = {}
    for key, evs in events.items():
        module, addr = key
        start = f"fn_{addr:08X}"
        if not any(old == start for old, *_ in evs):
            continue
        stats["addresses with an fn_ rename"] += 1
        if key in have_addr:
            stats["skipped: address already has campaign rows"] += 1
            continue
        chain, cur = [], start
        for ev in evs:
            old, new = ev[0], ev[1]
            if old != cur:
                continue
            names = [start] + [c[1] for c in chain]
            if new in names:  # cycle (e.g. reverted): cut back to that point
                chain = chain[:names.index(new)]
            else:
                chain.append(ev)
            cur = new
        final = current.get(key)
        if not chain:
            stats["skipped: reverted to fn_"] += 1
            continue
        if cur != final:
            stats["skipped: chain does not reach the current name"] += 1
            continue
        if cn.AUTO_NAME.match(final):
            stats["skipped: final name is auto-style"] += 1
            continue
        # Fold out intermediate names that symbols.txt now defines elsewhere.
        folded = []
        for ev in chain:
            if folded and folded[-1][1] in symbols:
                prev = folded.pop()
                ev = (prev[0], ev[1], ev[2], ev[3], ev[4])
                stats["intermediate names folded"] += 1
            folded.append(ev)
        while len(folded) > 1 and folded[0][1] in symbols and folded[0][1] != final:
            a, b = folded[0], folded[1]
            folded[:2] = [(a[0], b[1], b[2], b[3], b[4])]
            stats["intermediate names folded"] += 1
        chains[key] = folded

    # Splits for the unit column; definitions for source_file.
    splits = []
    unit = None
    for line in (root / "config/GC6E01/splits.txt").read_text().splitlines():
        if line and not line[0].isspace() and line.rstrip().endswith(":"):
            unit = line.rstrip()[:-1]
            continue
        m = re.match(r"\s+\.(?:text|init)\s+start:0x([0-9A-Fa-f]+) end:0x([0-9A-Fa-f]+)", line)
        if m and unit and unit != "Sections":
            splits.append((unit, int(m.group(1), 16), int(m.group(2), 16)))
    defs = definitions_index(root)

    rows = []
    for (module, addr), chain in sorted(chains.items(), key=lambda kv: (kv[0][0], kv[0][1])):
        u = next((n for n, a, b in splits if a <= addr < b), "") if module == "main" else module
        for old, new, sha, date, subject in chain:
            row = {
                "address": f"0x{addr:08X}", "module": module, "old_name": old, "new_name": new,
                "tier": cn.HISTORICAL,
                "evidence": f"pre-campaign rename in {sha[:9]}: {subject}"[:160],
                "evidence_ref": f"git:{sha[:12]}",
                "unit": u or "(unsplit)",
                "source_file": defs.get(chain[-1][1]) or (f"src/{u}" if u and (root / "src" / u).exists()
                                                          else "(no C definition)"),
                "batch_id": f"{date.replace('-', '')}-historical-1",
                "commit": sha[:12], "date": date,
            }
            if (module, row["address"], old, new) not in have:
                rows.append(row)
    stats["chains written"] = len(chains)
    stats["rows"] = len(rows)
    for k, v in stats.items():
        print(f"{v:6d}  {k}")
    if args.dry_run or not rows:
        return 0
    buf = StringIO()
    w = csv.DictWriter(buf, fieldnames=cn.COLUMNS, lineterminator="\n")
    w.writerows(rows)
    text = ledger.read_text(encoding="utf-8")
    if not text.endswith("\n"):
        text += "\n"
    ledger.write_text(text + buf.getvalue(), encoding="utf-8")
    print(f"appended {len(rows)} row(s) to {ledger.relative_to(root)}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
