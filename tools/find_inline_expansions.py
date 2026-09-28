#!/usr/bin/env python3
"""Search the retail disassembly for repeated expansions of an instruction block.

Evidence helper for the "Reconstructed inline helpers" policy in
docs/CAMPAIGN_OPERATIONS.md: a `static inline` helper is admissible when the
same instruction sequence appears at two or more call sites in the target.
This tool takes a block of retail instructions (an address range) and looks
for the same block, normalised for register allocation, in every other
function of the dtk-split disassembly (build/GC6E01/asm and the REL asm).

Normalisation: GPR/FPR numbers become per-window canonical names in order of
first appearance (so `mr r27,r3` and `mr r30,r3` compare equal, but a value
flowing through two registers still differs from one flowing through one),
branch targets become `L`, stack offsets stay, immediates stay, relocated
symbols stay (`@ha`/`@l`/`@sda21`), and `bl` keeps its callee.  With
`--loose-regs` (for `show`) every renamed register becomes `R`/`F`; `block`
always scores register-agnostically and prints the canonical score beside it.

Usage:
  tools/find_inline_expansions.py block 0x80112BA4 0x80112C30
  tools/find_inline_expansions.py block 0x80112BA4 0x80112C30 --min-score 0.8
  tools/find_inline_expansions.py calls floorDataBiosGetCurrentPtr \
      floorDataBiosGetGroupID GSresGetResource HSD_ArchiveGetPublicAddress
  tools/find_inline_expansions.py show 0x80112BA4 0x80112C30
  tools/find_inline_expansions.py block 0x80181DC8 0x80181E84 \
      --ghidra-dir xd-asm/code --ghidra-symbols xd-decomp/config/GXXE01/symbols.txt

With --ghidra-dir the query (still taken from the Colosseum asm) is searched
in a Ghidra-style per-function dump of another build, such as Pokemon XD's
(github.com/trevor403/xd-asm, names from TeamOrre/xd-decomp's symbols.txt);
both sides are then rewritten to one dialect (see canon_dialect).

`block` reports every window (in any function, including the query's own
function outside the query range) whose normalised instructions score at or
above --min-score against the query (difflib ratio); `calls` reports every
function whose call sequence contains the given callee subsequence
(contiguous by default, `--gapped` allows other calls in between).
"""

from __future__ import annotations

import argparse
import difflib
import re
import sys
from dataclasses import dataclass, field
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
DEFAULT_ASM_DIRS = [
    ROOT / "build" / "GC6E01" / "asm",
    ROOT / "build" / "GC6E01" / "common_rel" / "asm",
]

LINE_RE = re.compile(
    r"^/\*\s+([0-9A-F]{8})\s+[0-9A-F]+\s+(?:[0-9A-F]{2}\s+){4}\*/\s+(\S+)\s*(.*)$"
)
FN_RE = re.compile(r"^\.fn\s+([^,\s]+)")
ENDFN_RE = re.compile(r"^\.endfn\s+")
REG_RE = re.compile(r"\b([rf])(\d{1,2})\b")


@dataclass
class Insn:
    addr: int
    mnem: str
    ops: str


@dataclass
class Func:
    name: str
    path: Path
    insns: list[Insn] = field(default_factory=list)

    @property
    def start(self) -> int:
        return self.insns[0].addr if self.insns else 0


def load_functions(dirs: list[Path]) -> list[Func]:
    """Parse every .s file; keep one function per start address (the dtk
    candidate/range files overlap, so the first file seen wins)."""
    seen: set[tuple[str, int]] = set()
    funcs: list[Func] = []
    for base in dirs:
        if not base.exists():
            continue
        for path in sorted(base.rglob("*.s")):
            is_rel = "common_rel" in path.parts
            cur: Func | None = None
            with path.open(errors="replace") as fh:
                for raw in fh:
                    line = raw.strip()
                    m = FN_RE.match(line)
                    if m:
                        cur = Func(m.group(1), path)
                        continue
                    if ENDFN_RE.match(line):
                        if cur is not None and cur.insns:
                            key = ("rel" if is_rel else "dol", cur.start)
                            if key not in seen:
                                seen.add(key)
                                funcs.append(cur)
                        cur = None
                        continue
                    if cur is None:
                        continue
                    m = LINE_RE.match(line)
                    if m:
                        cur.insns.append(
                            Insn(int(m.group(1), 16), m.group(2), m.group(3).strip())
                        )
    funcs.sort(key=lambda f: f.start)
    return funcs


GHIDRA_FN_RE = re.compile(r"^(FUN_[0-9a-fA-F]{8}|[A-Za-z_][\w@$.]*):\s*#\s*0x([0-9a-fA-F]{8})")
GHIDRA_INSN_RE = re.compile(r"^\s+([a-z][\w.+-]*)\s*([^#]*)")


def load_symbols(path: Path) -> dict[int, str]:
    """Address -> name from a dtk symbols.txt (used to name Ghidra FUN_ calls)."""
    names: dict[int, str] = {}
    if path and path.exists():
        for line in path.open():
            m = re.match(r"(\S+) = \.text:0x([0-9A-Fa-f]+);", line)
            if m:
                names[int(m.group(2), 16)] = m.group(1)
    return names


def load_ghidra_functions(base: Path, symbols: dict[int, str]) -> list[Func]:
    """Parse a Ghidra-style per-function asm dump (one `code/func_*.s` per
    function, e.g. github.com/trevor403/xd-asm for Pokemon XD). Calls to
    `FUN_xxxxxxxx` are renamed through `symbols` so they compare with dtk's
    symbol names; instruction addresses are start + 4*index."""
    funcs: list[Func] = []
    for path in sorted(base.rglob("*.s")):
        cur: Func | None = None
        addr = 0
        with path.open(errors="replace") as fh:
            for raw in fh:
                m = GHIDRA_FN_RE.match(raw)
                if m:
                    addr = int(m.group(2), 16)
                    cur = Func(symbols.get(addr, m.group(1)), path)
                    funcs.append(cur)
                    continue
                if cur is None or raw.startswith(("#", ".", "LAB_")) or not raw.strip():
                    continue
                m = GHIDRA_INSN_RE.match(raw)
                if not m:
                    continue
                mnem, ops = m.group(1), m.group(2).strip()
                if mnem == "bl":
                    t = re.match(r"FUN_([0-9a-fA-F]{8})", ops)
                    if t:
                        ops = symbols.get(int(t.group(1), 16), ops)
                cur.insns.append(Insn(addr, mnem, ops))
                addr += 4
    funcs = [f for f in funcs if f.insns]
    funcs.sort(key=lambda f: f.start)
    return funcs


def _imm(text: str) -> int:
    return int(text, 0)


def canon_dialect(mnem: str, ops: str) -> tuple[str, str]:
    """Rewrite dtk's simplified mnemonics and relocations into the forms a
    Ghidra dump uses (and vice versa) so the two corpora compare: mr -> or,
    mflr/mtlr -> mfspr/mtspr, clrlwi/slwi/srwi/extrwi/extlwi -> rlwinm, and
    every small-data/@ha/@l relocation (or r2/r13-relative access) -> SYM."""
    ops = re.sub(r"\s*,\s*", ",", ops.strip()).lower()
    ops = re.sub(r"[\w.$@]+@sda21\(r0\)", "SYM", ops)
    ops = re.sub(r"-?0x[0-9a-f]+\(r(?:2|13)\)", "SYM", ops)
    ops = re.sub(r"[\w.$]+@(?:ha|l|h)\b", "SYM", ops)
    rec = mnem.endswith(".")
    base = mnem.rstrip(".")
    dot = "." if rec else ""
    a = ops.split(",")
    try:
        if base == "mr" and len(a) == 2:
            return f"or{dot}", f"{a[0]},{a[1]},{a[1]}"
        if base == "mflr":
            return "mfspr", f"{a[0]},lr"
        if base == "mtlr":
            return "mtspr", f"lr,{a[0]}"
        if base == "clrlwi" and len(a) == 3:
            return f"rlwinm{dot}", f"{a[0]},{a[1]},0x0,{_imm(a[2]):#x},0x1f"
        if base == "slwi" and len(a) == 3:
            n = _imm(a[2])
            return f"rlwinm{dot}", f"{a[0]},{a[1]},{n:#x},0x0,{31 - n:#x}"
        if base == "srwi" and len(a) == 3:
            n = _imm(a[2])
            return f"rlwinm{dot}", f"{a[0]},{a[1]},{(32 - n) % 32:#x},{n:#x},0x1f"
        if base == "extrwi" and len(a) == 4:
            n, b = _imm(a[2]), _imm(a[3])
            return f"rlwinm{dot}", f"{a[0]},{a[1]},{(b + n) % 32:#x},{32 - n:#x},0x1f"
        if base == "extlwi" and len(a) == 4:
            n, b = _imm(a[2]), _imm(a[3])
            return f"rlwinm{dot}", f"{a[0]},{a[1]},{b:#x},0x0,{n - 1:#x}"
    except ValueError:
        pass
    if base == "lis":
        ops = re.sub(r",-?0x[0-9a-f]+$", ",SYM", ops)
    return mnem, ops


def is_branch(mnem: str) -> bool:
    """Local (label) branch: b, beq, blt, bdnz, ... but not calls/returns."""
    base = mnem.rstrip("+-")
    if base in ("bl", "bla", "blr", "blrl", "bctr", "bctrl") or base.endswith("lr"):
        return False
    return base.startswith("b")


STRICT_ARGS = False
CROSS = False  # comparing against a Ghidra-dialect corpus


def normalise(insns: list[Insn], loose: bool) -> list[str]:
    """Normalise a window. Registers are renamed by first appearance within
    the window; r1, r2 and r13 keep their names (fixed roles), and so does
    r0 except in loose mode (r0 as a load/store base is always literal). With
    --strict-args the argument/volatile registers (r3-r12, f1-f13) also keep
    their names."""
    mapping: dict[str, str] = {}
    counters = {"r": 0, "f": 0}
    out: list[str] = []

    def rename(m: re.Match[str]) -> str:
        kind, num = m.group(1), m.group(2)
        reg = f"{kind}{num}"
        if kind == "r" and num in ("1", "2", "13"):
            return reg
        if kind == "r" and num == "0" and not loose:
            return reg
        if STRICT_ARGS and kind == "r" and 3 <= int(num) <= 12:
            return reg
        if STRICT_ARGS and kind == "f" and 1 <= int(num) <= 13:
            return reg
        if loose:
            return "R" if kind == "r" else "F"
        if reg not in mapping:
            counters[kind] += 1
            mapping[reg] = f"{kind.upper()}{counters[kind]}"
        return mapping[reg]

    for insn in insns:
        mnem = insn.mnem.rstrip("+-")
        if is_branch(mnem):
            ops = re.sub(r"(?:\.L_|LAB_)[0-9A-Fa-f]+", "L", insn.ops)
            ops = re.sub(r"\s*,\s*", ",", ops)
            ops = REG_RE.sub(rename, ops)
            out.append(f"{mnem} {ops}")
            continue
        if mnem == "bl":
            out.append(f"bl {insn.ops}")
            continue
        ops = insn.ops
        if CROSS:
            mnem, ops = canon_dialect(mnem, ops)
        ops = ops.replace("(r0)", "(ZERO)")  # r0 as a base means literal 0
        out.append(f"{mnem} {REG_RE.sub(rename, ops)}")
    return out


def find_func(funcs: list[Func], addr: int) -> Func | None:
    for f in funcs:
        if f.insns and f.insns[0].addr <= addr <= f.insns[-1].addr:
            return f
    return None


def slice_range(func: Func, start: int, end: int) -> list[Insn]:
    return [i for i in func.insns if start <= i.addr < end]


def calls_of(insns: list[Insn]) -> list[str]:
    return [i.ops for i in insns if i.mnem == "bl"]


def cmd_show(args: argparse.Namespace, funcs: list[Func]) -> int:
    start, end = int(args.start, 16), int(args.end, 16)
    func = find_func(funcs, start)
    if func is None:
        print("no function contains", args.start, file=sys.stderr)
        return 1
    block = slice_range(func, start, end)
    for insn, norm in zip(block, normalise(block, args.loose_regs)):
        print(f"{insn.addr:08X}  {insn.mnem:8s} {insn.ops:40s} | {norm}")
    return 0


def cmd_block(args: argparse.Namespace, funcs: list[Func]) -> int:
    corpus = funcs
    if args.ghidra_dir:
        global CROSS
        CROSS = True
        corpus = load_ghidra_functions(args.ghidra_dir, load_symbols(args.ghidra_symbols))
        print(f"corpus: {len(corpus)} functions from {args.ghidra_dir}")
    """Score every window by the register-agnostic form (every renamable
    register -> R/F; scheduling differences cost a little via difflib), and
    report the per-window canonical (first-appearance renaming) score too:
    a canonical 1.000 means the same data flow, not just the same shape."""
    start, end = int(args.start, 16), int(args.end, 16)
    qfunc = find_func(funcs, start)
    if qfunc is None:
        print("no function contains", args.start, file=sys.stderr)
        return 1
    query = slice_range(qfunc, start, end)
    qloose = normalise(query, True)
    qcanon = normalise(query, False)
    qcalls = calls_of(query)
    n = len(query)
    print(f"query: {qfunc.name} {start:08X}-{end:08X}, {n} insns, calls={qcalls}")
    hits = []
    for func in corpus:
        insns = func.insns
        if len(insns) < n * 0.6:
            continue
        fcalls = set(calls_of(insns))
        # cheap pre-filter: the function must share most of the query's callees
        if qcalls and not args.no_call_filter:
            shared = sum(1 for c in set(qcalls) if c in fcalls)
            if shared < max(1, int(len(set(qcalls)) * args.min_score)):
                continue
        best_by_start: list[tuple[float, int]] = []
        for i in range(0, max(1, len(insns) - n + 1)):
            if func is qfunc and not (insns[i].addr + 4 * n <= start or insns[i].addr >= end):
                continue
            window = insns[i : i + n]
            sm = difflib.SequenceMatcher(None, qloose, normalise(window, True), autojunk=False)
            if sm.real_quick_ratio() < args.min_score or sm.quick_ratio() < args.min_score:
                continue
            score = sm.ratio()
            if score >= args.min_score:
                best_by_start.append((score, i))
        # keep local maxima (non-overlapping)
        best_by_start.sort(reverse=True)
        taken: list[int] = []
        for score, i in best_by_start:
            if any(abs(i - t) < n for t in taken):
                continue
            taken.append(i)
            hits.append((score, func, i))
    hits.sort(key=lambda h: -h[0])
    for score, func, i in hits[: args.limit]:
        w = func.insns[i : i + n]
        wcanon = normalise(w, False)
        canon = difflib.SequenceMatcher(None, qcanon, wcanon, autojunk=False).ratio()
        same = " (same function)" if func is qfunc else ""
        print(
            f"{score:.3f} canon={canon:.3f}  {func.name:32s} {w[0].addr:08X}-{w[-1].addr + 4:08X}"
            f"{same}  calls={calls_of(w)}  [{func.path.name}]"
        )
        if args.diff:
            for line in difflib.unified_diff(qcanon, wcanon, "query", "hit", n=0, lineterm=""):
                print("      " + line)
    if not hits:
        print("no windows at or above", args.min_score)
    return 0


def cmd_calls(args: argparse.Namespace, funcs: list[Func]) -> int:
    want = args.callees
    for func in funcs:
        calls = [(i.addr, i.ops) for i in func.insns if i.mnem == "bl"]
        names = [c for _, c in calls]
        for k in range(len(names)):
            if args.gapped:
                j = 0
                for m in range(k, len(names)):
                    if names[m] == want[j]:
                        j += 1
                        if j == len(want):
                            break
                ok = j == len(want) and names[k] == want[0]
            else:
                ok = names[k : k + len(want)] == want
            if ok:
                print(f"{func.name:40s} {calls[k][0]:08X}  [{func.path.relative_to(ROOT)}]")
    return 0


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--asm-dir", action="append", type=Path, help="asm root(s); default build/GC6E01/asm + REL asm")
    ap.add_argument("--loose-regs", action="store_true", help="abstract every renamed register to R/F")
    ap.add_argument("--strict-args", action="store_true", help="keep r3-r12/f1-f13 literal (calling convention)")
    sub = ap.add_subparsers(dest="cmd", required=True)
    s = sub.add_parser("show")
    s.add_argument("start")
    s.add_argument("end")
    b = sub.add_parser("block")
    b.add_argument("start")
    b.add_argument("end")
    b.add_argument("--min-score", type=float, default=0.7)
    b.add_argument("--limit", type=int, default=25)
    b.add_argument("--diff", action="store_true")
    b.add_argument("--no-call-filter", action="store_true", help="scan functions sharing none of the query's callees too")
    b.add_argument("--ghidra-dir", type=Path, help="search a Ghidra-style dump instead (e.g. xd-asm/code)")
    b.add_argument("--ghidra-symbols", type=Path, help="dtk symbols.txt naming that dump's FUN_ calls")
    c = sub.add_parser("calls")
    c.add_argument("callees", nargs="+")
    c.add_argument("--gapped", action="store_true")
    args = ap.parse_args()
    global STRICT_ARGS
    STRICT_ARGS = args.strict_args
    funcs = load_functions(args.asm_dir or DEFAULT_ASM_DIRS)
    return {"show": cmd_show, "block": cmd_block, "calls": cmd_calls}[args.cmd](args, funcs)


if __name__ == "__main__":
    raise SystemExit(main())
