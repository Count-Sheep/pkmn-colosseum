#!/usr/bin/env python3
"""Validate docs/function_renames.csv against symbols.txt and the sources.

Implements the mechanical checks of docs/NAMING_SCHEME.md:

  * CSV schema, per-address rename chains, duplicate rows/names;
  * every chain's final new_name is a function at that address in its
    module's symbols.txt, and no superseded name survives in any symbols.txt;
  * superseded names appear in the live reference set only on NAME-T tag lines;
  * tier syntax (evidence_ref forms, T3 grammar, prefix registration, verbs,
    banned tokens, length cap, XD-vocabulary and uniqueness collisions);
  * a `NAME-T<n> fn_<addr>` comment tag above every C definition, no orphan
    or mismatched tags;
  * renamed functions do not use __FUNCTION__/__func__ and are not named
    inside an ASSERT macro (both put the name into data bytes);
  * --base REV: no string or character literal changed in src/ or include/.

Extra modes: --remap-report, --sample, --fill-commits (see --help).
Exit status: 0 clean, 1 violations, 2 usage/IO error.
"""

from __future__ import annotations

import argparse
import csv
import json
import math
import random
import re
import subprocess
import sys
from collections import Counter, defaultdict
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parents[1]

COLUMNS = [
    "address", "module", "old_name", "new_name", "tier", "evidence",
    "evidence_ref", "unit", "source_file", "batch_id", "commit", "date",
]
TIERS = {"T1", "T2", "T3"}
LIBRARY_UNIT_ROOTS = ("dolphin/", "hsd/", "musyx/", "trk/", "crt/")
T3_MAX_LEN = 40
T2_MAX_LEN = 64
PREFIX_MIN_USES = 3
TAG_WINDOW = 5  # lines above the name line that may hold the tag

VERBS = set("""
Init Exit Term Create Delete Destroy Alloc Free Release Open Close Main Ctrl
Proc Exec Update Draw Disp Render Set Get Is Has Check Calc Conv Make Setup
Load Read Write Save Clear Reset Start Stop Wait Play Add Remove Erase Push
Pop Find Search Select Sort Copy Swap Cmp Compare Count Move Enable Disable
Entry Regist Change Apply Begin End Inc Dec Callback CB Handler
""".split())
BANNED_TOKENS = {"Unk", "Unknown", "Misc", "Helper", "Thing", "Stuff", "Temp", "Tmp", "Fn"}

IDENT = re.compile(r"[A-Za-z_][A-Za-z0-9_]*")
C_IDENT_FULL = re.compile(r"^[A-Za-z_][A-Za-z0-9_]*$")
AUTO_NAME = re.compile(r"^(fn|lbl|func|sub)_[0-9A-Fa-f]+$")
ADDR_SUFFIX = re.compile(r"_8[0-9A-Fa-f]{7}(?![0-9A-Fa-f])")
HEX_RUN = re.compile(r"(?=[0-9A-Fa-f]*\d)[0-9A-Fa-f]{6,}")
FUNC_DIGIT = re.compile(r"Func\d")
TAG = re.compile(r"NAME-(T[123]) (fn_[0-9A-F]{8})\b")
SYM_LINE = re.compile(r"^(\S+) = (\.\w+):(0x[0-9A-Fa-f]+);\s*//(.*)$")
GAME_T3 = re.compile(r"^(GS[a-z0-9]+|[a-z][a-z0-9]*)[A-Z0-9][A-Za-z0-9]*$")
LIB_T3 = re.compile(r"^(HSD_)?[A-Za-z][A-Za-z0-9]*$")
PREFIX_RE = re.compile(r"^(HSD_|GS[a-z0-9]+|[a-z][a-z0-9]*|[A-Z]+(?=[A-Z][a-z]))")
CAMEL_TOKENS = re.compile(r"[A-Z]+(?=[A-Z][a-z]|\d|$)|[A-Z]?[a-z]+|[A-Z]+|\d+")

REF_FORMS = {
    "T1": [
        re.compile(r"^map:NXXJ01:\S+\.o:(?P<name>\S+)$"),
        re.compile(r"^str:0x8[0-9A-Fa-f]{7}$"),
        re.compile(r"^lib:[^/\s]+/[^@\s]+@[0-9a-f]{7,40}:\S+:(?P<name>\S+)$"),
        re.compile(r"^xd:0x80[0-9A-Fa-f]{6}\+(str:0x8[0-9A-Fa-f]{7}|map:\S+\.o)$"),
    ],
    "T2": [re.compile(r"^pattern:(?P<names>[A-Za-z_][A-Za-z0-9_]*(,[A-Za-z_][A-Za-z0-9_]*)+)$")],
    "T3": [re.compile(r"^ctx:\S.*$")],
}

C_EXTS = {".c", ".h", ".cpp", ".hpp", ".cp"}
LIVE_TEXT_EXTS = C_EXTS | {".s", ".S", ".txt", ".json", ".md", ".py"}


class Report:
    def __init__(self) -> None:
        self.errors: list[str] = []

    def err(self, msg: str) -> None:
        self.errors.append(msg)


# --------------------------------------------------------------------------- C lexing

_LEX = re.compile(
    r'(?P<lc>//[^\n]*)|(?P<bc>/\*.*?\*/)|(?P<str>"(?:\\.|[^"\\\n])*")|(?P<chr>\'(?:\\.|[^\'\\\n])*\')',
    re.S,
)


def lex(text: str):
    """Yield (kind, start, end) for comments ('c'), strings ('s'), chars ('ch')."""
    for m in _LEX.finditer(text):
        kind = m.lastgroup
        yield ("c" if kind in ("lc", "bc") else "s" if kind == "str" else "ch"), m.start(), m.end()


def blank_spans(text: str, kinds: set[str]) -> str:
    out = list(text)
    for kind, a, b in lex(text):
        if kind in kinds:
            for i in range(a, b):
                if out[i] != "\n":
                    out[i] = " "
    return "".join(out)


def literals(text: str) -> list[str]:
    return [text[a:b] for kind, a, b in lex(text) if kind in ("s", "ch")]


def match_close(text: str, i: int, open_ch: str, close_ch: str) -> int:
    depth = 0
    for j in range(i, len(text)):
        if text[j] == open_ch:
            depth += 1
        elif text[j] == close_ch:
            depth -= 1
            if depth == 0:
                return j
    return -1


# --------------------------------------------------------------------------- inputs

def symbol_files(root: Path) -> dict[str, Path]:
    cfg = root / "config" / "GC6E01"
    files = {"main": cfg / "symbols.txt"}
    for p in sorted(cfg.glob("*/symbols.txt")):
        files[p.parent.name] = p
    return files


def load_symbols(root: Path):
    """name -> list of (module, address, attrs)."""
    by_name: dict[str, list[tuple[str, int, str]]] = defaultdict(list)
    for module, path in symbol_files(root).items():
        if not path.exists():
            continue
        for line in path.read_text(errors="replace").splitlines():
            m = SYM_LINE.match(line)
            if m:
                by_name[m.group(1)].append((module, int(m.group(3), 16), m.group(4)))
    return by_name


def live_files(root: Path) -> list[Path]:
    out = list(symbol_files(root).values())
    for d in ("src", "include"):
        base = root / d
        if base.exists():
            out += [p for p in base.rglob("*") if p.is_file() and p.suffix in LIVE_TEXT_EXTS]
    for rel in ("configure.py", "docs/RULE_EXCEPTIONS.md", ".github/regression_policy_cleanup.json",
                "config/GC6E01/subsystem_map.json"):
        out.append(root / rel)
    ev = root / "docs" / "asm_evidence"
    if ev.exists():
        out += [p for p in ev.rglob("*") if p.is_file() and p.suffix in {".json", ".md"}]
    return [p for p in out if p.exists()]


def c_sources(root: Path) -> list[Path]:
    out = []
    for d in ("src", "include"):
        base = root / d
        if base.exists():
            out += [p for p in base.rglob("*") if p.is_file() and p.suffix in C_EXTS]
    return out


def read_csv(path: Path, rep: Report) -> list[dict[str, str]]:
    if not path.exists():
        rep.err(f"{path}: missing (create it with the header row from docs/NAMING_SCHEME.md)")
        return []
    with path.open(newline="", encoding="utf-8") as fh:
        reader = csv.DictReader(fh)
        if reader.fieldnames != COLUMNS:
            rep.err(f"{path}: header must be exactly {','.join(COLUMNS)}")
            return []
        return [dict(r, _line=str(i + 2)) for i, r in enumerate(reader)]


def unit_of(row: dict[str, str]) -> str:
    return row["unit"].strip()


# --------------------------------------------------------------------------- checks

def check_rows(rows, rep: Report):
    """Schema + chains. Returns chains: (module, addr) -> ordered rows."""
    chains: dict[tuple[str, int], list[dict]] = defaultdict(list)
    seen = set()
    for r in rows:
        loc = f"CSV line {r['_line']}"
        for col in COLUMNS:
            if col != "commit" and not r[col].strip():
                rep.err(f"{loc}: column '{col}' is empty")
        if not re.fullmatch(r"0x[0-9A-F]{1,8}", r["address"].strip()):
            rep.err(f"{loc}: address must be upper-case hex like 0x8001ABCD")
            continue
        if r["tier"] not in TIERS:
            rep.err(f"{loc}: tier must be one of T1/T2/T3")
        if r["date"] and not re.fullmatch(r"\d{4}-\d{2}-\d{2}", r["date"]):
            rep.err(f"{loc}: date must be YYYY-MM-DD")
        if r["commit"] and not re.fullmatch(r"[0-9a-f]{7,40}", r["commit"]):
            rep.err(f"{loc}: commit must be a git hash")
        if len(r["evidence"]) > 160:
            rep.err(f"{loc}: evidence longer than 160 chars")
        if not re.fullmatch(r"\d{8}-[A-Za-z0-9_.-]+-\d+", r["batch_id"]):
            rep.err(f"{loc}: batch_id must be YYYYMMDD-<unit-stem>-<n>")
        key = (r["module"], r["address"], r["old_name"], r["new_name"])
        if key in seen:
            rep.err(f"{loc}: duplicate row")
        seen.add(key)
        if r["old_name"] == r["new_name"]:
            rep.err(f"{loc}: old_name equals new_name")
        chains[(r["module"], int(r["address"], 16))].append(r)

    for (module, addr), chain in chains.items():
        first = chain[0]
        expect = f"fn_{addr:08X}"
        if first["old_name"] != expect and not first["evidence"].startswith("human-approved"):
            rep.err(f"CSV line {first['_line']}: first rename of {module}:{addr:#x} must start from "
                    f"{expect} (renaming a non-fn_ name needs evidence starting 'human-approved')")
        for prev, cur in zip(chain, chain[1:]):
            if cur["old_name"] != prev["new_name"]:
                rep.err(f"CSV line {cur['_line']}: chain break, old_name {cur['old_name']} != previous "
                        f"new_name {prev['new_name']}")
    return chains


def check_symbols(chains, symbols, rows, rep: Report):
    finals = {}
    for (module, addr), chain in chains.items():
        final = chain[-1]
        finals[(module, addr)] = final
        defs = symbols.get(final["new_name"], [])
        here = [d for d in defs if d[0] == module and d[1] == addr]
        if not here:
            rep.err(f"{final['new_name']}: not defined at {module}:{addr:#x} in symbols.txt")
        elif "type:function" not in here[0][2]:
            rep.err(f"{final['new_name']}: symbols.txt entry is not type:function")
        for r in chain:
            if r["old_name"] in symbols:
                rep.err(f"{r['old_name']}: superseded name still defined in symbols.txt "
                        f"({', '.join(f'{m}:{a:#x}' for m, a, _ in symbols[r['old_name']])})")
    names = Counter(f["new_name"] for f in finals.values())
    for name, n in names.items():
        if n > 1:
            tiers = {f["tier"] for f in finals.values() if f["new_name"] == name}
            if tiers != {"T1"}:
                rep.err(f"{name}: final name used by {n} addresses (only T1 statics may repeat)")
    for f in finals.values():
        defs = symbols.get(f["new_name"], [])
        if f["tier"] in ("T2", "T3") and len(defs) > 1:
            rep.err(f"{f['new_name']}: {f['tier']} name is defined {len(defs)} times across symbols.txt files")
        if f["tier"] == "T1" and len(defs) > 1 and not all("scope:local" in d[2] for d in defs):
            rep.err(f"{f['new_name']}: repeated T1 name must be scope:local in every module")
    return finals


def name_tokens(rest: str) -> list[str]:
    return CAMEL_TOKENS.findall(rest)


def check_syntax(rows, symbols, xd_vocab: set[str], rep: Report):
    t3_names = {r["new_name"] for r in rows if r["tier"] == "T3"}
    proven = [n for n in symbols if not AUTO_NAME.match(n) and n not in t3_names]
    for r in rows:
        loc = f"CSV line {r['_line']} ({r['new_name']})"
        name, tier, ref = r["new_name"], r["tier"], r["evidence_ref"].strip()
        if tier not in TIERS:
            continue
        if not C_IDENT_FULL.match(name):
            rep.err(f"{loc}: not a C identifier")
            continue
        if AUTO_NAME.match(name) or ADDR_SUFFIX.search(name):
            rep.err(f"{loc}: auto-style name or address suffix")
        forms = REF_FORMS[tier]
        m = next((f.match(ref) for f in forms if f.match(ref)), None)
        if not m:
            rep.err(f"{loc}: evidence_ref '{ref}' is not a valid {tier} form")
        elif "name" in m.groupdict() and m.group("name") and m.group("name") != name:
            rep.err(f"{loc}: evidence_ref names '{m.group('name')}', not the new name")
        if tier == "T2":
            if "__" in name:
                rep.err(f"{loc}: T2 may not produce mangled or '__' names")
            if len(name) > T2_MAX_LEN:
                rep.err(f"{loc}: T2 name longer than {T2_MAX_LEN}")
            if m and "names" in m.groupdict():
                for sib in m.group("names").split(","):
                    if sib not in symbols or AUTO_NAME.match(sib):
                        rep.err(f"{loc}: pattern sibling '{sib}' is not a named symbol")
        if tier in ("T2", "T3") and name in xd_vocab:
            rep.err(f"{loc}: {tier} name equals an XD vocabulary name; prove it as T1 or choose another")
        if tier != "T3":
            continue
        library = unit_of(r).startswith(LIBRARY_UNIT_ROOTS)
        if len(name) > T3_MAX_LEN:
            rep.err(f"{loc}: T3 name longer than {T3_MAX_LEN}")
        if name.startswith("_"):
            rep.err(f"{loc}: T3 may not start with '_'")
        if not (LIB_T3 if library else GAME_T3).match(name):
            rep.err(f"{loc}: T3 name does not match the {'library' if library else 'game'} grammar")
            continue
        if library and "lib-miss:" not in r["evidence"]:
            rep.err(f"{loc}: library T3 needs 'lib-miss:<repo>@<commit>' in evidence")
        pm = PREFIX_RE.match(name)
        prefix = pm.group(1) if pm else ""
        rest = name[len(prefix):]
        if not prefix or not rest:
            rep.err(f"{loc}: no module prefix + body")
            continue
        uses = sum(1 for n in proven if n.lstrip("_").startswith(prefix)
                   and (prefix.endswith("_") or len(n.lstrip("_")) == len(prefix)
                        or not n.lstrip("_")[len(prefix)].islower()))
        if uses < PREFIX_MIN_USES:
            rep.err(f"{loc}: prefix '{prefix}' starts only {uses} proven names (need {PREFIX_MIN_USES}); "
                    f"new prefixes need coordinator approval")
        toks = name_tokens(rest)
        if not any(t in VERBS for t in toks):
            rep.err(f"{loc}: no verb token from the allowed list")
        bad = [t for t in toks if t in BANNED_TOKENS]
        if bad or FUNC_DIGIT.search(name) or HEX_RUN.search(rest):
            rep.err(f"{loc}: banned token(s) {bad or 'Func<digit>/hex run'}")


def check_live_refs(root: Path, chains, rep: Report):
    superseded = {}
    for chain in chains.values():
        for r in chain:
            superseded[r["old_name"]] = r["new_name"]
    if not superseded:
        return
    for path in live_files(root):
        try:
            text = path.read_text(errors="replace")
        except OSError:
            continue
        for lineno, line in enumerate(text.splitlines(), 1):
            if TAG.search(line):
                continue
            for tok in IDENT.findall(line):
                if tok in superseded:
                    rep.err(f"{path.relative_to(root)}:{lineno}: superseded name {tok} still referenced")


def find_definitions(code: str, name: str):
    """Yield (name_offset, body_open, body_close) for C definitions of name."""
    for m in re.finditer(rf"(?<![A-Za-z0-9_]){re.escape(name)}\s*\(", code):
        close = match_close(code, m.end() - 1, "(", ")")
        if close < 0:
            continue
        j = close + 1
        while j < len(code) and code[j] in " \t\r\n":
            j += 1
        if j < len(code) and code[j] == "{":
            end = match_close(code, j, "{", "}")
            if end > 0:
                yield m.start(), j, end


def check_sources(root: Path, finals, rep: Report):
    want = {f["new_name"]: (f"fn_{addr:08X}", f["tier"]) for (mod, addr), f in finals.items() if mod == "main"}
    found_def: set[str] = set()
    tag_seen: dict[str, set[str]] = defaultdict(set)
    for path in c_sources(root):
        text = path.read_text(errors="replace")
        rel = path.relative_to(root)
        lines = text.splitlines()
        # Tags: only inside comments; record every tag for orphan checks.
        comment_mask = [False] * len(text)
        for kind, a, b in lex(text):
            if kind == "c":
                for i in range(a, b):
                    comment_mask[i] = True
        for m in TAG.finditer(text):
            if not comment_mask[m.start()]:
                rep.err(f"{rel}: NAME tag outside a comment at offset {m.start()}")
            tag_seen[m.group(2)].add(m.group(1))
        if not want:
            continue
        code = blank_spans(text, {"c", "s", "ch"})
        idents = set(IDENT.findall(code))
        for name in want.keys() & idents:
            old, tier = want[name]
            for start, body_open, body_close in find_definitions(code, name):
                found_def.add(name)
                lineno = code.count("\n", 0, start)
                window = "\n".join(lines[max(0, lineno - TAG_WINDOW):lineno + 1])
                if f"NAME-{tier} {old}" not in window:
                    rep.err(f"{rel}:{lineno + 1}: definition of {name} lacks '/* NAME-{tier} {old} */' "
                            f"within {TAG_WINDOW} lines above")
                body = code[body_open:body_close]
                if re.search(r"\b(__FUNCTION__|__func__|__PRETTY_FUNCTION__)\b", body):
                    rep.err(f"{rel}:{lineno + 1}: {name} uses __FUNCTION__; its name is data and must not change")
            for m in re.finditer(r"\b\w*ASSERT\w*\s*\(", code):
                end = match_close(code, m.end() - 1, "(", ")")
                if end > 0 and re.search(rf"(?<![A-Za-z0-9_]){re.escape(name)}(?![A-Za-z0-9_])", code[m.end():end]):
                    rep.err(f"{rel}:{code.count(chr(10), 0, m.start()) + 1}: {name} appears in an ASSERT "
                            f"(stringized into data)")
    for name in want.keys() - found_def:
        rep.err(f"{name}: no C definition found under src/ or include/ to carry the NAME tag")
    final_by_old = {old: tier for old, tier in want.values()}
    for old, tiers in tag_seen.items():
        if old not in final_by_old:
            rep.err(f"NAME tag for {old} has no CSV row")
        elif tiers != {final_by_old[old]}:
            rep.err(f"NAME tag(s) {sorted(tiers)} for {old} do not match final tier {final_by_old[old]}")


def check_literals(root: Path, base: str, rep: Report):
    try:
        out = subprocess.run(["git", "diff", "--name-only", base, "--", "src", "include"],
                             cwd=root, check=True, capture_output=True, text=True).stdout
    except (subprocess.CalledProcessError, FileNotFoundError) as e:
        rep.err(f"--base {base}: git diff failed: {e}")
        return
    for rel in filter(None, out.splitlines()):
        p = root / rel
        if Path(rel).suffix not in C_EXTS:
            continue
        old = subprocess.run(["git", "show", f"{base}:{rel}"], cwd=root, capture_output=True, text=True)
        if old.returncode != 0:
            rep.err(f"{rel}: file added or removed since {base}; renames must not add or delete files")
            continue
        if not p.exists():
            rep.err(f"{rel}: file deleted since {base}")
            continue
        a, b = literals(old.stdout), literals(p.read_text(errors="replace"))
        if a != b:
            idx = next((i for i, (x, y) in enumerate(zip(a, b)) if x != y), min(len(a), len(b)))
            ax = a[idx] if idx < len(a) else "<none>"
            bx = b[idx] if idx < len(b) else "<none>"
            rep.err(f"{rel}: string/char literal changed since {base}: {ax[:60]} -> {bx[:60]}")


# --------------------------------------------------------------------------- modes

def final_map(rows) -> dict[str, str]:
    """Every name ever superseded -> the current name."""
    nxt = {r["old_name"]: r["new_name"] for r in rows}
    out = {}
    for old in nxt:
        cur, hops = old, 0
        while cur in nxt and hops < 64:
            cur, hops = nxt[cur], hops + 1
        out[old] = cur
    return out


def remap_report(rows, src: Path, dst: Path) -> int:
    mapping = final_map(rows)
    report = json.loads(src.read_text())
    n = 0
    for unit in report.get("units", []):
        fns = list(unit.get("functions", []))
        for sec in unit.get("sections", []):
            fns += sec.get("functions", [])
        for f in fns:
            if f.get("name") in mapping:
                f["name"] = mapping[f["name"]]
                n += 1
    dst.write_text(json.dumps(report))
    print(f"remapped {n} function name(s) -> {dst}")
    return 0


def sample(rows, batch_id: str) -> int:
    t3 = [r for r in rows if r["batch_id"] == batch_id and r["tier"] == "T3"]
    if not t3:
        print(f"no T3 rows in batch {batch_id}")
        return 0
    k = min(len(t3), max(3, math.ceil(len(t3) * 0.10)))
    for r in random.Random(batch_id).sample(t3, k):
        print(f"{r['address']} {r['old_name']} -> {r['new_name']}  [{r['evidence_ref']}] {r['evidence']}")
    return 0


def fill_commits(root: Path, csv_path: Path, rows) -> int:
    cache: dict[str, str] = {}
    changed = 0
    for r in rows:
        if r["commit"]:
            continue
        bid = r["batch_id"]
        if bid not in cache:
            out = subprocess.run(["git", "log", "--reverse", "--format=%H", "-F", f"--grep=[batch {bid}]"],
                                 cwd=root, capture_output=True, text=True).stdout.split()
            cache[bid] = out[0][:12] if out else ""
        if cache[bid]:
            r["commit"] = cache[bid]
            changed += 1
    with csv_path.open("w", newline="", encoding="utf-8") as fh:
        w = csv.DictWriter(fh, fieldnames=COLUMNS, extrasaction="ignore", lineterminator="\n")
        w.writeheader()
        w.writerows(rows)
    print(f"filled {changed} commit cell(s)")
    return 0


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--root", type=Path, default=REPO_ROOT, help="repository root (default: this repo)")
    ap.add_argument("--csv", type=Path, help="ledger path (default: <root>/docs/function_renames.csv)")
    ap.add_argument("--base", help="git revision; fail if any src/include literal changed since it")
    ap.add_argument("--remap-report", nargs=2, metavar=("IN", "OUT"),
                    help="write IN report.json with renamed functions mapped to current names")
    ap.add_argument("--sample", metavar="BATCH_ID", help="print the seeded T3 spot-check sample")
    ap.add_argument("--fill-commits", action="store_true", help="fill blank commit cells from git log")
    args = ap.parse_args()

    root = args.root.resolve()
    csv_path = args.csv or root / "docs" / "function_renames.csv"
    rep = Report()
    rows = read_csv(csv_path, rep)
    if rep.errors:
        print("\n".join(rep.errors), file=sys.stderr)
        return 2

    if args.remap_report:
        return remap_report(rows, Path(args.remap_report[0]), Path(args.remap_report[1]))
    if args.sample:
        return sample(rows, args.sample)
    if args.fill_commits:
        return fill_commits(root, csv_path, rows)

    symbols = load_symbols(root)
    xd_vocab: set[str] = set()
    vocab_path = root / "config" / "GC6E01" / "symbolmap" / "xd_vocabulary.json"
    if vocab_path.exists():
        for names in json.loads(vocab_path.read_text()).values():
            xd_vocab.update(names)

    chains = check_rows(rows, rep)
    finals = check_symbols(chains, symbols, rows, rep)
    check_syntax(rows, symbols, xd_vocab, rep)
    check_live_refs(root, chains, rep)
    check_sources(root, finals, rep)
    if args.base:
        check_literals(root, args.base, rep)

    for e in rep.errors:
        print(f"error: {e}")
    tiers = Counter(f["tier"] for f in finals.values())
    print(f"check_naming: {len(rows)} row(s), {len(finals)} function(s) "
          f"(T1 {tiers['T1']} / T2 {tiers['T2']} / T3 {tiers['T3']}), {len(rep.errors)} error(s)")
    return 1 if rep.errors else 0


if __name__ == "__main__":
    sys.exit(main())
