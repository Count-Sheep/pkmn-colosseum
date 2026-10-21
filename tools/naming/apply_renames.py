#!/usr/bin/env python3
"""Apply a batch of function renames safely (docs/NAMING_SCHEME.md, section 3).

Input: a batch CSV with the ledger columns of docs/function_renames.csv
(address,module,old_name,new_name,tier,evidence,evidence_ref,unit,source_file,
batch_id,commit,date). `unit`, `source_file` and `date` may be blank; they are
filled in. `commit` stays blank (check_naming.py --fill-commits fills it).

For every row the tool updates the live reference set:
  * every module's symbols.txt;
  * C/C++ sources and headers under src/ and include/: code and comments only,
    never a string or character literal;
  * other text under src/ and include/ (.md/.json/.txt/.py/.s);
  * configure.py (force_active lists and everything else);
  * docs/RULE_EXCEPTIONS.md, docs/asm_evidence/registry.json and the evidence
    documents under docs/asm_evidence/ (not probes/);
  * .github/regression_policy_cleanup.json;
  * config/GC6E01/subsystem_map.json.
It adds the `/* NAME-T<n> fn_XXXXXXXX */` tag above every C definition and
appends the rows to the ledger.

Safety:
  * Identifiers are matched as whole C identifiers ([A-Za-z0-9_] boundaries),
    so unit/file names such as OSEXI_fn_8009E7A8.c never match; in non-C text a
    token followed by a file extension (fn_X.c, fn_X.txt) is a file name and is
    left alone.
  * A row is refused when its old name occurs inside a string or character
    literal, inside an ASSERT-style macro invocation, or when the function body
    uses __FUNCTION__/__func__ (all of these put the name into data bytes).
  * A row is refused when the new name already exists in any symbols.txt or is
    defined in the C sources as a different function or macro (a header
    prototype or an alias `#define NEW fn_X` / `#define NEW(a, b) fn_X(a, b)` is fine; the
    alias line is dropped).
  * A row is refused when its old name sits inside an asm block that includes
    a .inc file (a dead asm-wrapper stub): .github/scripts/quality_scan.py
    rejects any edit there.
  * All-or-nothing: every edit is computed in memory and checked first; nothing
    is written unless the whole batch is valid. The original content of each
    touched file is saved to a journal, and `--rollback JOURNAL` restores it.
  * Idempotent: rows already applied (ledger row present and symbols.txt at the
    address already carries the new name) are skipped; running a batch twice is
    a no-op.

Exit status: 0 applied (or nothing to do), 1 batch refused, 2 usage/IO error.
"""

from __future__ import annotations

import argparse
import base64
import csv
import datetime as dt
import json
import re
import sys
from pathlib import Path

TOOLS = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(TOOLS))
import check_naming as cn  # noqa: E402

REPO_ROOT = TOOLS.parent
sys.path.insert(0, str(REPO_ROOT / ".github" / "scripts"))
import quality_scan as qs  # noqa: E402
FILE_EXT_AFTER = re.compile(r"\.(?:c|h|cpp|hpp|cp|s|S|inc|o|txt|json|md|py|map|elf|dol|rel)\b")
ASSERT_CALL = re.compile(r"\b\w*ASSERT\w*\s*\(")
FUNC_MACROS = re.compile(r"\b(__FUNCTION__|__func__|__PRETTY_FUNCTION__)\b")
TEXT_EXTS = {".md", ".json", ".txt", ".py", ".s", ".S"}
EXTRA_LIVE = ("configure.py", "docs/RULE_EXCEPTIONS.md", ".github/regression_policy_cleanup.json",
              "config/GC6E01/subsystem_map.json")


class BatchError(Exception):
    pass


# --------------------------------------------------------------------------- helpers

def ident_re(names) -> re.Pattern:
    alt = "|".join(re.escape(n) for n in sorted(names, key=len, reverse=True))
    return re.compile(rf"(?<![A-Za-z0-9_])(?:{alt})(?![A-Za-z0-9_])")


def replace_plain(text: str, mapping: dict[str, str], pat: re.Pattern) -> tuple[str, int]:
    """Whole-identifier replace in non-C text, skipping file-name tokens."""
    n = 0

    def sub(m: re.Match) -> str:
        nonlocal n
        if FILE_EXT_AFTER.match(text, m.end()):
            return m.group(0)
        n += 1
        return mapping[m.group(0)]

    return pat.sub(sub, text), n


def replace_c(text: str, mapping: dict[str, str], pat: re.Pattern) -> tuple[str, int]:
    """Whole-identifier replace in C code and comments; literals are copied verbatim."""
    out, pos, n = [], 0, 0
    spans = [(a, b) for kind, a, b in cn.lex(text) if kind in ("s", "ch")]
    for a, b in spans + [(len(text), len(text))]:
        chunk = text[pos:a]
        new, k = pat.subn(lambda m: mapping[m.group(0)], chunk)
        out.append(new)
        n += k
        out.append(text[a:b])
        pos = b
    return "".join(out), n


def blank_directives(code: str) -> str:
    """Blank preprocessor lines (with continuations) in already comment-blanked code."""
    lines = code.split("\n")
    cont = False
    for i, line in enumerate(lines):
        if cont or line.lstrip().startswith("#"):
            cont = line.rstrip().endswith("\\")
            lines[i] = " " * len(line)
        else:
            cont = False
    return "\n".join(lines)


def decl_start(code: str, name_off: int) -> int:
    """Offset of the first character of the declaration containing name_off."""
    j = name_off - 1
    while j >= 0 and code[j] not in ";{}":
        j -= 1
    j += 1
    while j < name_off and code[j] in " \t\r\n":
        j += 1
    return j


def is_alias_macro(rest: str, target: str) -> bool:
    """True when `#define NAME<rest>` only forwards to target: `#define NAME target`, or the
    function-like pass-through `#define NAME(a, b) target(a, b)` (same parameters, same order)."""
    if rest[:1] == "(":
        m = re.fullmatch(r"\(([^()]*)\)\s*" + re.escape(target) + r"\s*\(([^()]*)\)\s*", rest)
        if not m:
            return False
        params = [x.strip() for x in m.group(1).split(",")] if m.group(1).strip() else []
        args = [x.strip() for x in m.group(2).split(",")] if m.group(2).strip() else []
        return params == args
    return rest.strip() == target


def drop_alias_defines(text: str, tags) -> str:
    """Remove the `#define NEW NEW` / `#define NEW(a) NEW(a)` lines left by renaming an alias
    `#define NEW fn_X` or `#define NEW(a) fn_X(a)`."""
    def keep(m: re.Match) -> str:
        rest = cn.blank_spans(m.group(2), {"c"}).rstrip()
        return "" if is_alias_macro(rest, m.group(1)) else m.group(0)
    for new in tags:
        text = re.sub(rf"^[ \t]*#[ \t]*define[ \t]+({re.escape(new)})((?:\(|[ \t])[^\n]*)?\n", keep, text,
                      flags=re.M)
    return text


def add_tags(text: str, tags: dict[str, tuple[str, str]]) -> tuple[str, int]:
    """Insert /* NAME-Tn fn_X */ above every definition of each new name lacking it."""
    inserts: list[tuple[int, str]] = []
    code = blank_directives(cn.blank_spans(text, {"c", "s", "ch"}))
    lines = text.split("\n")
    line_starts = [0]
    for ln in lines[:-1]:
        line_starts.append(line_starts[-1] + len(ln) + 1)
    for name, (tier, old) in tags.items():
        if name not in code:
            continue
        tag = f"NAME-{tier} {old}"
        for start, _bo, _bc in cn.find_definitions(code, name):
            name_line = code.count("\n", 0, start)
            window = "\n".join(lines[max(0, name_line - cn.TAG_WINDOW):name_line + 1])
            if tag in window:
                continue
            ds = decl_start(code, start)
            ds_line = code.count("\n", 0, ds)
            if name_line - ds_line > cn.TAG_WINDOW - 2:
                ds_line = name_line  # long specifier run: tag the name line itself
            at = line_starts[ds_line]
            indent = re.match(r"[ \t]*", lines[ds_line]).group(0)
            inserts.append((at, f"{indent}/* {tag} */\n"))
    for at, s in sorted(set(inserts), reverse=True):
        text = text[:at] + s + text[at:]
    return text, len(set(inserts))


# --------------------------------------------------------------------------- batch

def read_batch(path: Path) -> list[dict[str, str]]:
    with path.open(newline="", encoding="utf-8") as fh:
        reader = csv.DictReader(fh)
        missing = [c for c in cn.COLUMNS if c not in (reader.fieldnames or [])]
        if missing:
            raise BatchError(f"{path}: missing column(s) {', '.join(missing)}")
        return [{c: (r.get(c) or "").strip() for c in cn.COLUMNS} for r in reader]


def load_splits(root: Path) -> list[tuple[str, int, int]]:
    out, unit = [], None
    p = root / "config" / "GC6E01" / "splits.txt"
    for line in p.read_text().splitlines():
        if line and not line[0].isspace() and line.rstrip().endswith(":"):
            unit = line.rstrip()[:-1]
            continue
        m = re.match(r"\s+\.(?:text|init)\s+start:0x([0-9A-Fa-f]+) end:0x([0-9A-Fa-f]+)", line)
        if m and unit and unit != "Sections":
            out.append((unit, int(m.group(1), 16), int(m.group(2), 16)))
    return out


def symbol_at(root: Path, module: str, addr: int) -> tuple[Path, int, str] | None:
    path = cn.symbol_files(root).get(module)
    if not path or not path.exists():
        return None
    for i, line in enumerate(path.read_text(errors="replace").splitlines()):
        m = cn.SYM_LINE.match(line)
        if m and int(m.group(3), 16) == addr and "type:function" in m.group(4):
            return path, i, m.group(1)
    return None


def live_text_files(root: Path) -> tuple[list[Path], list[Path]]:
    """(C files, plain-text files) of the live reference set (symbols.txt excluded)."""
    c_files, text_files = [], []
    for d in ("src", "include"):
        base = root / d
        if not base.exists():
            continue
        for p in sorted(base.rglob("*")):
            if not p.is_file():
                continue
            if p.suffix in cn.C_EXTS:
                c_files.append(p)
            elif p.suffix in TEXT_EXTS:
                text_files.append(p)
    for rel in EXTRA_LIVE:
        if (root / rel).exists():
            text_files.append(root / rel)
    ev = root / "docs" / "asm_evidence"
    if ev.exists():
        text_files += sorted(p for p in ev.glob("*") if p.is_file() and p.suffix in {".md", ".json"})
    return c_files, text_files


def plan(root: Path, batch: list[dict[str, str]], ledger_rows: list[dict[str, str]]):
    """Validate the batch and compute {path: new_text}. Raises BatchError."""
    errors: list[str] = []
    today = dt.date.today().isoformat()
    ledger_keys = {(r["module"], r["address"], r["old_name"], r["new_name"]) for r in ledger_rows}
    symbols = cn.load_symbols(root)
    splits = load_splits(root)

    pending, skipped = [], []
    seen_addr, seen_new = set(), set()
    for r in batch:
        loc = f"batch row {r['address']} {r['old_name']}->{r['new_name']}"
        if not r["module"]:
            r["module"] = "main"
        if r["module"] != "main":
            errors.append(f"{loc}: only main-module renames are supported")
            continue
        if not re.fullmatch(r"0x[0-9A-F]{8}", r["address"]):
            errors.append(f"{loc}: address must be 0x + 8 upper-case hex digits")
            continue
        addr = int(r["address"], 16)
        if not cn.C_IDENT_FULL.match(r["new_name"]):
            errors.append(f"{loc}: new name is not a C identifier")
            continue
        if r["tier"] not in cn.TIERS:
            errors.append(f"{loc}: tier must be T1/T2/T3")
            continue
        if addr in seen_addr or r["new_name"] in seen_new:
            errors.append(f"{loc}: address or new name repeated in the batch")
            continue
        seen_addr.add(addr)
        seen_new.add(r["new_name"])
        cur = symbol_at(root, r["module"], addr)
        if cur is None:
            errors.append(f"{loc}: no function at {r['address']} in symbols.txt")
            continue
        key = (r["module"], r["address"], r["old_name"], r["new_name"])
        if cur[2] == r["new_name"] and key in ledger_keys:
            skipped.append(r)
            continue
        if cur[2] != r["old_name"]:
            errors.append(f"{loc}: symbols.txt has {cur[2]} at that address")
            continue
        if not r["old_name"].startswith("fn_") and not r["evidence"].startswith("human-approved"):
            errors.append(f"{loc}: only fn_ names may be renamed without 'human-approved' evidence")
            continue
        if r["new_name"] in symbols:
            errors.append(f"{loc}: new name already defined in a symbols.txt")
            continue
        if not r["unit"]:
            r["unit"] = next((u for u, a, b in splits if a <= addr < b), "")
        r["date"] = r["date"] or today
        r["commit"] = ""
        pending.append(r)

    if not pending:
        if errors:
            raise BatchError("\n".join(errors))
        return {}, [], skipped

    mapping = {r["old_name"]: r["new_name"] for r in pending}
    tags = {r["new_name"]: (r["tier"], r["old_name"]) for r in pending}
    inverse = {v: k for k, v in mapping.items()}
    old_pat = ident_re(mapping)
    new_pat = ident_re(set(mapping.values()))
    edits: dict[Path, str] = {}

    # symbols.txt (every module): whole-line identifier replace.
    for path in cn.symbol_files(root).values():
        if path.exists():
            t = path.read_text(errors="replace")
            nt, k = replace_plain(t, mapping, old_pat)
            if k:
                edits[path] = nt

    c_files, text_files = live_text_files(root)
    defined_in: dict[str, list[str]] = {n: [] for n in mapping.values()}
    for path in c_files:
        text = path.read_text(errors="replace")
        rel = str(path.relative_to(root))
        has_old = old_pat.search(text)
        code = cn.blank_spans(text, {"c", "s", "ch"})
        if new_pat.search(code):
            # Declarations and calls of the new name are fine (a header prototype
            # of the real name, or a `#define NEW fn_X` alias that is dropped
            # below). A definition, or a macro meaning something else, is not.
            for new in set(new_pat.findall(code)):
                if any(True for _ in cn.find_definitions(blank_directives(code), new)):
                    errors.append(f"{rel}: new name {new} is already defined here as a different function")
                for m in re.finditer(rf"^[ \t]*#[ \t]*define[ \t]+{re.escape(new)}\b(.*)$", text, re.M):
                    rest = cn.blank_spans(m.group(1), {"c"}).rstrip()
                    if not is_alias_macro(rest, inverse[new]):
                        errors.append(f"{rel}: new name {new} is already a macro ({rest.strip()[:40]})")
        if not has_old:
            continue
        for kind, a, b in cn.lex(text):
            if kind in ("s", "ch"):
                for m in old_pat.finditer(text, a, b):
                    errors.append(f"{rel}:{text.count(chr(10), 0, a) + 1}: {m.group(0)} appears inside a "
                                  f"literal; renaming it would leave a superseded reference or change data")
        for m in ASSERT_CALL.finditer(code):
            end = cn.match_close(code, m.end() - 1, "(", ")")
            if end > 0:
                for mm in old_pat.finditer(code, m.end(), end):
                    errors.append(f"{rel}:{code.count(chr(10), 0, m.start()) + 1}: {mm.group(0)} is named "
                                  f"inside an ASSERT (stringized into data)")
        for old in set(old_pat.findall(code)):
            for _s, bo, bc in cn.find_definitions(code, old):
                if FUNC_MACROS.search(code, bo, bc):
                    errors.append(f"{rel}: {old} uses __FUNCTION__/__func__; its name is data")
        # The quality scan rejects any added line inside an asm block that
        # includes a .inc file (a dead `#if 0` asm wrapper counts too), so a
        # rename may not touch one. Clean the wrapper up first.
        if ".inc" in text and "asm" in text:
            for region in qs.find_asm_regions(text):
                if not any(qs.INC_INCLUDE.match(ln) for ln in region["body"].splitlines()):
                    continue
                lines = text.split("\n")[region["start_line"] - 1:region["end_line"]]
                for old in set(old_pat.findall("\n".join(lines))):
                    errors.append(f"{rel}:{region['start_line']}: {old} appears in a .inc asm wrapper "
                                  f"(dead stub); the quality scan rejects edits there, remove the stub first")
        nt, k = replace_c(text, mapping, old_pat)
        nt = drop_alias_defines(nt, tags)
        nt, _ = add_tags(nt, tags)
        ncode = cn.blank_spans(nt, {"c", "s", "ch"})
        for new in tags:
            if new in ncode and any(True for _ in cn.find_definitions(ncode, new)):
                defined_in[new].append(rel)
        if nt != text:
            edits[path] = nt

    for path in text_files:
        text = path.read_text(errors="replace")
        nt, k = replace_plain(text, mapping, old_pat)
        if k:
            edits[path] = nt

    for r in pending:
        defs = defined_in[r["new_name"]]
        if not defs:
            errors.append(f"{r['old_name']}: no C definition under src/ or include/ to carry the NAME tag")
            continue
        if not r["source_file"]:
            prim = [d for d in defs if not d.endswith((".h", ".hpp"))] or defs
            # Prefer the file the unit names; otherwise the first non-header definition.
            unit_src = f"src/{r['unit']}" if r["unit"] else ""
            r["source_file"] = unit_src if unit_src in prim else prim[0]

    if errors:
        raise BatchError("\n".join(sorted(set(errors))))
    return edits, pending, skipped


def write_journal(journal: Path, root: Path, paths: list[Path]) -> None:
    journal.parent.mkdir(parents=True, exist_ok=True)
    data = {"root": str(root), "files": {}}
    for p in paths:
        data["files"][str(p.relative_to(root))] = base64.b64encode(p.read_bytes()).decode() if p.exists() else None
    journal.write_text(json.dumps(data))


def rollback(journal: Path) -> int:
    data = json.loads(journal.read_text())
    root = Path(data["root"])
    for rel, b64 in data["files"].items():
        p = root / rel
        if b64 is None:
            if p.exists():
                p.unlink()
        else:
            p.write_bytes(base64.b64decode(b64))
    print(f"rolled back {len(data['files'])} file(s) from {journal}")
    return 0


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("batch", nargs="?", type=Path, help="batch CSV (ledger columns)")
    ap.add_argument("--root", type=Path, default=REPO_ROOT)
    ap.add_argument("--dry-run", action="store_true", help="validate and list the files that would change")
    ap.add_argument("--journal", type=Path, help="where to save original file contents "
                    "(default: build/naming/journal-<batch>.json)")
    ap.add_argument("--drop-refused", type=Path, metavar="CSV",
                    help="drop rows that fail validation (written here with the reason) and apply the rest; "
                         "without it one bad row refuses the whole batch")
    ap.add_argument("--rollback", type=Path, metavar="JOURNAL", help="restore files from a journal and exit")
    args = ap.parse_args()

    if args.rollback:
        return rollback(args.rollback)
    if not args.batch:
        ap.error("batch CSV required")
    root = args.root.resolve()
    ledger = root / "docs" / "function_renames.csv"
    rep = cn.Report()
    ledger_rows = cn.read_csv(ledger, rep)
    if rep.errors:
        print("\n".join(rep.errors), file=sys.stderr)
        return 2
    refused: list[tuple[dict, str]] = []
    try:
        batch = read_batch(args.batch)
        while True:
            try:
                edits, pending, skipped = plan(root, [dict(r) for r in batch], ledger_rows)
                break
            except BatchError as e:
                if not args.drop_refused:
                    raise
                # Attribute each error to the rows whose old or new name it mentions.
                drop = {}
                for line in str(e).splitlines():
                    toks = set(cn.IDENT.findall(line))
                    hit = [r for r in batch if r["old_name"] in toks or r["new_name"] in toks]
                    if not hit:
                        raise BatchError(f"unattributable error: {line}") from None
                    for r in hit:
                        drop.setdefault(id(r), (r, line))
                refused += list(drop.values())
                batch = [r for r in batch if id(r) not in drop]
    except BatchError as e:
        print(f"apply_renames: batch refused, nothing written:\n{e}", file=sys.stderr)
        return 1
    except OSError as e:
        print(f"apply_renames: {e}", file=sys.stderr)
        return 2
    if args.drop_refused:
        with args.drop_refused.open("w", newline="", encoding="utf-8") as fh:
            w = csv.DictWriter(fh, fieldnames=cn.COLUMNS + ["refusal"], lineterminator="\n")
            w.writeheader()
            for r, why in refused:
                w.writerow(dict(r, refusal=why))
        print(f"apply_renames: {len(refused)} row(s) refused -> {args.drop_refused}")
        for r, why in refused:
            print(f"  refused {r['old_name']} -> {r['new_name']}: {why}")

    print(f"apply_renames: {len(pending)} to apply, {len(skipped)} already applied, "
          f"{len(edits)} file(s) to edit")
    if args.dry_run or not pending:
        for p in sorted(edits):
            print(f"  {'would edit' if args.dry_run else 'edit'} {p.relative_to(root)}")
        return 0

    # Ledger append.
    with ledger.open(newline="", encoding="utf-8") as fh:
        ledger_text = fh.read()
    if not ledger_text.endswith("\n"):
        ledger_text += "\n"
    from io import StringIO
    buf = StringIO()
    w = csv.DictWriter(buf, fieldnames=cn.COLUMNS, lineterminator="\n", extrasaction="ignore")
    w.writerows(pending)
    edits[ledger] = ledger_text + buf.getvalue()

    journal = args.journal or root / "build" / "naming" / f"journal-{pending[0]['batch_id']}.json"
    write_journal(journal, root, list(edits))
    try:
        for p, t in edits.items():
            p.write_text(t)
    except OSError as e:
        print(f"apply_renames: write failed ({e}); rolling back", file=sys.stderr)
        rollback(journal)
        return 2
    for p in sorted(edits):
        print(f"  edited {p.relative_to(root)}")
    print(f"journal: {journal}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
