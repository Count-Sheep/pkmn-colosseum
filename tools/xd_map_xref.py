#!/usr/bin/env python3
"""Cross-reference the Pokemon XD JP demo linker map with Colosseum (GC6E01).

The XD JP demo disc (NXXJ01) ships its CodeWarrior linker map (Master.MAP),
published as `NXXJ01.map` in github.com/StarsMmd/Colo-XD-PBR-symbol-maps
(HEAD 6b51d3af).  It lists every object's linked symbols and every
dead-stripped ("UNUSED") function and data object, with sizes, in object
order.  See docs/recon/cross_build_evidence.md and docs/recon/xd_demo_map_xref.md.

This tool:
  * parses the map into objects -> ordered symbols per section
    (live/UNUSED, size, demo address);
  * locates each demo object in XD retail (GXXE01): names from
    TeamOrre/xd-decomp config/GXXE01/symbols.txt, bodies from
    trevor403/xd-asm code/func_*.s, and flags retail functions inside the
    object's retail range that the demo does not name (candidates for a
    demo-UNUSED function that is live in retail);
  * aligns each demo object's .text list (live + UNUSED, in order) with the
    Colosseum functions around its anchors: name anchors from
    config/GC6E01/symbols.txt (C++ base names), plus body anchors (the
    Colosseum function most similar to the XD retail body, register-agnostic,
    via tools/find_inline_expansions.py's normaliser; Colosseum bodies come
    from build/GC6E01/asm, so run ninja once first).  The anchor chain may run
    in either direction: MWCC's deferred inlining emits a TU in reverse source
    order, and the two builds need not agree.  Between anchors a small
    dynamic-programming alignment pairs the rest by size and body;
  * reports, per object (or per Colosseum split unit / address), the matching
    Colosseum range and units, the live and stripped function lists, and the
    stripped data (--all-data: every data symbol).

Only NXXJ01.map is primary evidence.  An alignment row is a lead, not a
rename.  The `renames` subcommand lists only pairs where the XD function is
live, the Colosseum one is unnamed, both sit in the same object in the same
order, the sizes are equal and the retail body matches (>= --min-sim); check
each by hand before committing it.

External inputs are not in the tree; clone them and pass the paths (defaults
point at a lane-private `.lane/` directory):
  .lane/maps      github.com/StarsMmd/Colo-XD-PBR-symbol-maps @ 6b51d3af
  .lane/xd-decomp github.com/TeamOrre/xd-decomp @ 4989794e
  .lane/xd-asm    github.com/trevor403/xd-asm @ b1087f18

Usage:
  tools/xd_map_xref.py objects                     # every object, one line
  tools/xd_map_xref.py object people.o             # one object in detail
  tools/xd_map_xref.py addr 0x8018FC50             # object covering a Colosseum address
  tools/xd_map_xref.py tu game/people/people.c     # objects overlapping a split unit
  tools/xd_map_xref.py retail-live                 # demo-UNUSED fns live in XD retail
  tools/xd_map_xref.py renames [--min-sim 0.95]    # rename candidates (see above)
  tools/xd_map_xref.py json out.json               # everything, machine-readable
"""

from __future__ import annotations

import argparse
import bisect
import difflib
import json
import re
import sys
from dataclasses import dataclass, field
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / "tools"))
import find_inline_expansions as fie  # noqa: E402

LANE = ROOT / ".lane"
DEF_MAP = LANE / "maps" / "NXXJ01.map"
DEF_RETAIL_SYMS = LANE / "xd-decomp" / "config" / "GXXE01" / "symbols.txt"
DEF_XDASM = LANE / "xd-asm" / "code"
COLO_SYMS = ROOT / "config" / "GC6E01" / "symbols.txt"
COLO_SPLITS = ROOT / "config" / "GC6E01" / "splits.txt"
COLO_ASM = [ROOT / "build" / "GC6E01" / "asm"]

SECTIONS = (".init", ".text", ".ctors", ".dtors", ".rodata", ".data", ".bss",
            ".sdata", ".sbss", ".sdata2", ".sbss2")

# ---------------------------------------------------------------- map parsing

LIVE_RE = re.compile(
    r"^\s+([0-9a-f]{8}) ([0-9a-f]{6}) ([0-9a-f]{8}) ([0-9a-f]{8})\s+(\d+)?\s*(\S+)(?: \(entry of [^)]*\))?\s*\t(.*)$")
UNUSED_RE = re.compile(r"^\s+UNUSED\s+([0-9a-f]{6}) \.{8} \.{8}\s+(\S+) (.*)$")
SECTION_RE = re.compile(r"^(\S+) section layout")


@dataclass
class Entry:
    name: str
    size: int
    addr: int | None          # demo virtual address; None when UNUSED
    unused: bool
    kind: str                 # "section" (object's section header), "entry", "sym"

    @property
    def live(self) -> bool:
        return not self.unused


@dataclass
class XdObject:
    key: str                  # "lib obj" text as printed in the map
    index: int                # first appearance order in .text (or any section)
    sections: dict[str, list[Entry]] = field(default_factory=dict)

    @property
    def short(self) -> str:
        path = self.key.split()[-1] if self.key.split() else self.key
        return path.replace("\\", "/").split("/")[-1]

    def funcs(self) -> list[Entry]:
        return [e for e in self.sections.get(".text", []) if e.kind == "sym"]


def parse_map(path: Path) -> dict[str, XdObject]:
    objs: dict[str, XdObject] = {}
    section = None
    for raw in path.open(errors="replace"):
        line = raw.rstrip("\r\n")
        m = SECTION_RE.match(line)
        if m:
            section = m.group(1)
            continue
        if section not in SECTIONS:
            continue
        m = LIVE_RE.match(line)
        if m:
            name, owner = m.group(6), m.group(7).strip()
            kind = "sym"
            if "(entry of" in line:
                kind = "entry"
            elif m.group(5) == "1" and name == section:
                kind = "section"
            ent = Entry(name, int(m.group(2), 16), int(m.group(3), 16), False, kind)
        else:
            m = UNUSED_RE.match(line)
            if not m:
                continue
            name, owner = m.group(2), m.group(3).strip()
            kind = "entry" if name.startswith("...") else "sym"
            ent = Entry(name, int(m.group(1), 16), None, True, kind)
        obj = objs.get(owner)
        if obj is None:
            obj = objs[owner] = XdObject(owner, len(objs))
        obj.sections.setdefault(section, []).append(ent)
    return objs


def base_name(name: str) -> str:
    """C++ mangled -> plain function name (XD's app code is C++, Colosseum's C):
    `_peopleWillMove__FP13tagPeopleWork` -> `_peopleWillMove`."""
    m = re.match(r"^(.+?[^_])__(?:F|Q\d|\d)", name)
    return m.group(1) if m else name


# ----------------------------------------------------------- symbol tables

@dataclass
class Sym:
    name: str
    addr: int
    size: int
    section: str


SYM_RE = re.compile(r"^(\S+) = (\.\w+):0x([0-9A-Fa-f]+);(.*)$")


def load_syms(path: Path) -> list[Sym]:
    out = []
    for line in path.open():
        m = SYM_RE.match(line)
        if not m:
            continue
        sz = re.search(r"size:0x([0-9A-Fa-f]+)", m.group(4))
        out.append(Sym(m.group(1), int(m.group(3), 16), int(sz.group(1), 16) if sz else 0, m.group(2)))
    return out


def load_splits(path: Path) -> list[tuple[str, str, int, int]]:
    out, unit = [], None
    for line in path.open():
        if line and not line[0].isspace() and line.rstrip().endswith(":"):
            unit = line.rstrip()[:-1]
            continue
        m = re.match(r"\s+(\.\w+)\s+start:0x([0-9A-Fa-f]+) end:0x([0-9A-Fa-f]+)", line)
        if m and unit and unit != "Sections":
            out.append((unit, m.group(1), int(m.group(2), 16), int(m.group(3), 16)))
    return out


def is_auto(name: str) -> bool:
    return bool(re.match(r"^(fn|zz|FUN|func|lbl|sub)_[0-9A-Fa-f]{6,8}(_\w+)?$", name))


def load_xdasm_index(base: Path) -> dict[int, tuple[int, Path]]:
    """Retail function start -> (size, file) from xd-asm metadata headers."""
    idx: dict[int, tuple[int, Path]] = {}
    if not base.exists():
        return idx
    for p in base.glob("func_*.s"):
        with p.open(errors="replace") as fh:
            head = fh.readline()
        m = re.search(r'"startAddress": "0x([0-9a-f]+)", "size": (\d+)', head)
        if m:
            idx[int(m.group(1), 16)] = (int(m.group(2)), p)
    return idx


# ------------------------------------------------------------------ context

class Ctx:
    def __init__(self, args: argparse.Namespace):
        self.args = args
        self.objs = parse_map(args.map)
        self.colo = [s for s in load_syms(COLO_SYMS)]
        self.colo_fn = sorted((s for s in self.colo if s.section in (".text", ".init")), key=lambda s: s.addr)
        self.colo_by_name: dict[str, list[Sym]] = {}
        for s in self.colo_fn:
            self.colo_by_name.setdefault(base_name(s.name), []).append(s)
        self.splits = load_splits(COLO_SPLITS)
        self.retail = load_syms(args.retail_symbols) if args.retail_symbols.exists() else []
        self.retail_by_name: dict[str, list[Sym]] = {}
        for r in self.retail:
            if r.section == ".text":
                self.retail_by_name.setdefault(r.name, []).append(r)
        self._retail_of: dict[int, int | None] = {}
        self._retail_done: set[str] = set()
        self.retail_by_addr = {s.addr: s for s in self.retail if s.section == ".text"}
        self._xdasm_idx = None
        self._colo_bodies = None
        self._xd_bodies: dict[int, list[str]] = {}
        self._anchor_cache: dict[str, list] = {}
        self._orient: dict[str, int] = {}
        self._anchor_owner: dict[int, set[str]] | None = None

    # bodies ---------------------------------------------------------------
    def xdasm_idx(self):
        if self._xdasm_idx is None:
            self._xdasm_idx = load_xdasm_index(self.args.xd_asm)
        return self._xdasm_idx

    def colo_body(self, addr: int) -> list[str] | None:
        if self._colo_bodies is None:
            fie.CROSS = True
            funcs = fie.load_functions([d for d in COLO_ASM if d.exists()])
            self._colo_bodies = {f.start: fie.normalise(f.insns, loose=True) for f in funcs}
        return self._colo_bodies.get(addr)

    def xd_body(self, retail_addr: int) -> list[str] | None:
        if retail_addr in self._xd_bodies:
            return self._xd_bodies[retail_addr]
        ent = self.xdasm_idx().get(retail_addr)
        body = None
        if ent:
            fie.CROSS = True
            names = {a: s.name for a, s in self.retail_by_addr.items()}
            func = _load_one_ghidra(ent[1], names)
            if func and func.insns:
                body = fie.normalise(func.insns, loose=True)
        self._xd_bodies[retail_addr] = body
        return body

    # objects --------------------------------------------------------------
    def text_objects(self) -> list[XdObject]:
        return [o for o in self.objs.values() if o.funcs()]

    def find_object(self, needle: str) -> list[XdObject]:
        n = needle.lower().replace("/", "\\")
        exact = [o for o in self.objs.values() if o.short.lower() == n]
        return exact or [o for o in self.objs.values() if n in o.key.lower()]

    def map_retail(self, obj: XdObject) -> None:
        """Retail (GXXE01) address of each live demo function of the object.
        Static names repeat across objects, so the demo->retail offset is
        taken from the object's unique names (median) and each name resolves
        to the candidate nearest demo address + offset."""
        if obj.key in self._retail_done:
            return
        self._retail_done.add(obj.key)
        live = [e for e in obj.funcs() if e.live]
        offs = sorted(c[0].addr - e.addr for e in live
                      if len(c := self.retail_by_name.get(e.name, [])) == 1)
        if not offs:
            for e in live:
                self._retail_of[id(e)] = None
            return
        off = offs[len(offs) // 2]
        for e in live:
            cands = self.retail_by_name.get(e.name, [])
            best = min(cands, key=lambda c: abs(c.addr - (e.addr + off)), default=None)
            ok = best is not None and abs(best.addr - (e.addr + off)) <= 0x4000
            self._retail_of[id(e)] = best.addr if ok else None

    def retail_addr(self, e: Entry) -> int | None:
        return self._retail_of.get(id(e))

    def anchors(self, obj: XdObject) -> list[tuple[int, Sym, str, float | None]]:
        """(index in obj.funcs(), Colosseum symbol, "name"|"body", similarity).

        Name anchors: exact base-name matches with a unique, non-automatic
        Colosseum name.  Body anchors (unless --no-bodies): for a live XD
        function with a retail body, the Colosseum function in the window
        around the name anchors whose body is most similar (>= --anchor-sim
        and clearly ahead of the runner-up).  The longest chain that is
        monotonic in either direction is kept (MWCC's deferred mode emits a
        TU in reverse source order, e.g. Colosseum people.c), then the
        densest cluster (a static name such as OnReset can collide across
        objects, and Colosseum names partly come from a noisy port)."""
        key = obj.key
        if key in self._anchor_cache:
            return self._anchor_cache[key]
        self.map_retail(obj)
        funcs = obj.funcs()
        out = []
        for i, e in enumerate(funcs):
            if re.search(r"__(?:Q\d|\d+[A-Za-z@])", e.name):
                continue      # C++ method: its bare name (main, init...) is not unique
            cands = self.colo_by_name.get(base_name(e.name))
            if cands and len(cands) == 1 and not is_auto(cands[0].name):
                out.append((i, cands[0], "name", None))
        if not self.args.no_bodies and out:
            out = self._add_body_anchors(obj, out)
        up, down = _lis(out, 1), _lis(out, -1)
        chain, orient = (up, 1) if len(up) >= len(down) else (down, -1)
        clusters: list[list] = []
        for p in chain:
            if clusters:
                i0, s0 = clusters[-1][-1][0], clusters[-1][-1][1]
                xd_bytes = sum(e.size for e in funcs[i0:p[0]])
                if abs(p[1].addr - s0.addr) <= 2 * xd_bytes + 0x1000:
                    clusters[-1].append(p)
                    continue
            clusters.append([p])
        best = max(clusters, key=len) if clusters else []
        self._anchor_cache[key] = best
        self._orient[key] = orient if len(best) > 1 else 1
        return best

    def _add_body_anchors(self, obj: XdObject, out: list) -> list:
        funcs = obj.funcs()
        total = sum(e.size for e in funcs)
        lo = min(p[1].addr for p in out) - total - 0x400
        hi = max(p[1].addr for p in out) + total + 0x400
        taken = {p[1].addr for p in out}
        named = {p[0] for p in out}
        window = [s for s in self.colo_fn if lo <= s.addr < hi and s.addr not in taken and is_auto(s.name)]
        for i, e in enumerate(funcs):
            if i in named or e.unused:
                continue
            ra = self.retail_addr(e)
            xb = self.xd_body(ra) if ra is not None else None
            if not xb or len(xb) < 4:
                continue
            scored = []
            for s in window:
                if min(e.size, s.size) / max(e.size, s.size) < 0.6:
                    continue
                sim = similarity(xb, self.colo_body(s.addr))
                if sim:
                    scored.append((sim, s))
            scored.sort(key=lambda t: -t[0])
            if scored and scored[0][0] >= self.args.anchor_sim and (
                    len(scored) == 1 or scored[0][0] - scored[1][0] >= 0.05):
                out.append((i, scored[0][1], "body", scored[0][0]))
        out.sort(key=lambda p: p[0])
        return out

    def is_foreign_anchor(self, sym: Sym, obj: XdObject) -> bool:
        """True when sym is a name anchor of another object."""
        if self._anchor_owner is None:
            self._anchor_owner = {}
            for o in self.text_objects():
                for i, e in enumerate(o.funcs()):
                    if re.search(r"__(?:Q\d|\d+[A-Za-z@])", e.name):
                        continue
                    c = self.colo_by_name.get(base_name(e.name))
                    if c and len(c) == 1 and not is_auto(c[0].name):
                        self._anchor_owner.setdefault(c[0].addr, set()).add(o.key)
        owners = self._anchor_owner.get(sym.addr)
        return bool(owners) and obj.key not in owners

    def orientation(self, obj: XdObject) -> int:
        self.anchors(obj)
        return self._orient.get(obj.key, 1)

    def colo_range(self, obj: XdObject) -> tuple[int, int] | None:
        anc = self.anchors(obj)
        if not anc:
            return None
        lo = min(anc, key=lambda p: p[1].addr)[1]
        hi = max(anc, key=lambda p: p[1].addr)[1]
        return lo.addr, hi.addr + hi.size

    def units_in(self, lo: int, hi: int) -> list[str]:
        seen = []
        for unit, sec, a, b in self.splits:
            if sec == ".text" and a < hi and b > lo and unit not in seen:
                seen.append(unit)
        return seen

    def unit_of(self, addr: int) -> str | None:
        for unit, sec, a, b in self.splits:
            if sec == ".text" and a <= addr < b:
                return unit
        return None


def _load_one_ghidra(path: Path, names: dict[int, str]):
    cur = None
    addr = 0
    with path.open(errors="replace") as fh:
        for raw in fh:
            m = fie.GHIDRA_FN_RE.match(raw)
            if m:
                if cur is not None:
                    break
                addr = int(m.group(2), 16)
                cur = fie.Func(names.get(addr, m.group(1)), path)
                continue
            if cur is None or raw.startswith(("#", ".", "LAB_")) or not raw.strip():
                continue
            m = fie.GHIDRA_INSN_RE.match(raw)
            if not m:
                continue
            mnem, ops = m.group(1), m.group(2).strip()
            if mnem == "bl":
                t = re.match(r"FUN_([0-9a-fA-F]{8})", ops)
                if t:
                    ops = names.get(int(t.group(1), 16), ops)
            cur.insns.append(fie.Insn(addr, mnem, ops))
            addr += 4
    return cur


def _lis(pairs: list, sign: int = 1) -> list:
    """Longest chain of pairs whose Colosseum address moves in direction sign."""
    if not pairs:
        return []
    n = len(pairs)
    best = [1] * n
    prev = [-1] * n
    for i in range(n):
        for j in range(i):
            if sign * (pairs[i][1].addr - pairs[j][1].addr) > 0 and best[j] + 1 > best[i]:
                best[i], prev[i] = best[j] + 1, j
    k = max(range(n), key=lambda i: best[i])
    out = []
    while k >= 0:
        out.append(pairs[k])
        k = prev[k]
    return out[::-1]


def similarity(a: list[str] | None, b: list[str] | None) -> float | None:
    if not a or not b:
        return None
    if min(len(a), len(b)) / max(len(a), len(b)) < 0.5:
        return 0.0
    return difflib.SequenceMatcher(None, a, b, autojunk=False).ratio()


# ---------------------------------------------------------------- alignment

@dataclass
class Row:
    xd: Entry | None
    colo: Sym | None
    how: str = ""             # "name", "size", "body", ""
    sim: float | None = None
    retail: int | None = None


def align(ctx: Ctx, obj: XdObject, use_bodies: bool = True) -> list[Row]:
    """Align the object's .text entries (demo order) with the Colosseum
    functions between consecutive anchors (in the object's orientation);
    entries before the first / after the last anchor stay unpaired."""
    funcs = obj.funcs()
    anc = ctx.anchors(obj)
    if not anc:
        return [Row(e, None, retail=ctx.retail_addr(e)) for e in funcs]
    sign = ctx.orientation(obj)
    rows: list[Row] = []
    bounds = [(-1, None, "", None)] + anc + [(len(funcs), None, "", None)]
    for (i0, s0, how0, sim0), (i1, s1, _h, _s) in zip(bounds, bounds[1:]):
        if s0 is not None:
            rows.append(Row(funcs[i0], s0, how0, sim0, ctx.retail_addr(funcs[i0])))
        xs = funcs[i0 + 1:i1]
        cs = []
        if s0 is not None and s1 is not None:
            a, b = sorted((s0.addr, s1.addr))
            cs = [s for s in ctx.colo_fn if a < s.addr < b]
        elif xs:
            # head / tail: Colosseum functions next to the outer anchor, up to
            # 1.5x the XD bytes left over (other objects' anchors stop it)
            span = int(sum(e.size for e in xs) * 1.5) + 0x40
            edge = s1 if s0 is None else s0
            before = (s0 is None) == (sign > 0)
            if before:
                cs = [s for s in ctx.colo_fn if edge.addr - span <= s.addr < edge.addr]
            else:
                cs = [s for s in ctx.colo_fn if edge.addr < s.addr <= edge.addr + edge.size + span]
            cs = [s for s in cs if not ctx.is_foreign_anchor(s, obj)]
            if before:
                # keep only the contiguous run next to the anchor
                while cs and ctx.is_foreign_anchor(cs[0], obj):
                    cs.pop(0)
        cs.sort(key=lambda s: s.addr * sign)
        rows.extend(_dp(ctx, xs, cs, use_bodies))
    return rows


def _pair_score(ctx: Ctx, e: Entry, s: Sym, use_bodies: bool) -> tuple[float, float | None]:
    if not is_auto(s.name) and base_name(e.name) != base_name(s.name):
        return -5.0, None
    r = min(e.size, s.size) / max(e.size, s.size, 1)
    score = 2.0 * (r - 0.6)
    sim = None
    if use_bodies and e.live:
        ra = ctx.retail_addr(e)
        if ra is not None:
            sim = similarity(ctx.xd_body(ra), ctx.colo_body(s.addr))
            if sim is not None:
                score += 4.0 * (sim - 0.55)
    return score, sim


def _dp(ctx: Ctx, xs: list[Entry], cs: list[Sym], use_bodies: bool) -> list[Row]:
    n, m = len(xs), len(cs)
    if m == 0:
        return [Row(e, None, retail=ctx.retail_addr(e)) for e in xs]
    if n == 0:
        return [Row(None, s) for s in cs]
    skip_x = [0.0 if e.unused else -0.6 for e in xs]
    skip_c = -1.0
    memo = {}
    score = [[0.0] * (m + 1) for _ in range(n + 1)]
    back = [[None] * (m + 1) for _ in range(n + 1)]
    for i in range(1, n + 1):
        score[i][0] = score[i - 1][0] + skip_x[i - 1]
        back[i][0] = "x"
    for j in range(1, m + 1):
        score[0][j] = score[0][j - 1] + skip_c
        back[0][j] = "c"
    for i in range(1, n + 1):
        for j in range(1, m + 1):
            ps, sim = _pair_score(ctx, xs[i - 1], cs[j - 1], use_bodies)
            memo[(i, j)] = sim
            opts = [(score[i - 1][j - 1] + ps, "p"), (score[i - 1][j] + skip_x[i - 1], "x"),
                    (score[i][j - 1] + skip_c, "c")]
            score[i][j], back[i][j] = max(opts)
    rows = []
    i, j = n, m
    while i or j:
        b = back[i][j]
        if b == "p":
            e, s = xs[i - 1], cs[j - 1]
            sim = memo[(i, j)]
            rows.append(Row(e, s, "body" if sim is not None else "size", sim, ctx.retail_addr(e)))
            i, j = i - 1, j - 1
        elif b == "x":
            rows.append(Row(xs[i - 1], None, retail=ctx.retail_addr(xs[i - 1])))
            i -= 1
        else:
            rows.append(Row(None, cs[j - 1]))
            j -= 1
    return rows[::-1]


# ---------------------------------------------------------------- retail-live

def retail_extras(ctx: Ctx, obj: XdObject) -> list[tuple[int, int, str, list[str]]]:
    """Retail functions inside the object's retail range that no live demo
    function accounts for: (retail addr, size, retail name, demo UNUSED
    names between the neighbouring live functions)."""
    ctx.map_retail(obj)
    funcs = obj.funcs()
    live = [(i, ctx.retail_addr(e)) for i, e in enumerate(funcs) if e.live]
    live = [(i, a) for i, a in live if a is not None]
    if len(live) < 2:
        return []
    # every retail function start: xd-asm (Ghidra) plus GXXE01 symbols.txt
    # (dtk's own analysis), so a function Ghidra missed still shows up
    idx = {a: v[0] for a, v in ctx.xdasm_idx().items()}
    for a, sym in ctx.retail_by_addr.items():
        idx.setdefault(a, sym.size)
    starts = sorted(idx)
    known = {a for _, a in live}
    out = []
    for (i0, a0), (i1, a1) in zip(live, live[1:]):
        demo_gap = funcs[i1].addr - funcs[i0].addr
        if a1 <= a0 or a1 - a0 > 2 * demo_gap + 0x400:
            continue          # the two names did not resolve consistently
        inner = starts[bisect.bisect_right(starts, a0):bisect.bisect_left(starts, a1)]
        # bytes of the retail interval that no known function covers
        end0 = a0 + idx.get(a0, funcs[i0].size)
        cover = end0
        for a in inner:
            if a > cover + 4:
                out.append((cover, a - cover, "<uncovered bytes>", [e.name for e in funcs[i0 + 1:i1] if e.unused]))
            cover = max(cover, a + idx[a])
        if a1 > cover + 4 and cover > end0 - 1:
            out.append((cover, a1 - cover, "<uncovered bytes>", [e.name for e in funcs[i0 + 1:i1] if e.unused]))
        for a in inner:
            if a in known:
                continue
            size = idx[a]
            name = ctx.retail_by_addr[a].name if a in ctx.retail_by_addr else f"FUN_{a:08x}"
            gap = [e.name for e in funcs[i0 + 1:i1] if e.unused]
            out.append((a, size, name, gap))
    return out


# ------------------------------------------------------------------ output

def fmt_obj_line(ctx: Ctx, obj: XdObject) -> str:
    f = obj.funcs()
    live = sum(e.live for e in f)
    rng = ctx.colo_range(obj)
    if rng:
        units = ctx.units_in(*rng)
        rev = "R" if ctx.orientation(obj) < 0 else " "
        where = f"{rng[0]:08X}-{rng[1]:08X}{rev} {len(ctx.anchors(obj)):3d} anchors  {', '.join(units[:3])}{' …' if len(units) > 3 else ''}"
    else:
        where = "no Colosseum name anchor"
    return f"{obj.short:32s} live {live:3d} stripped {len(f) - live:3d}  {where}"


def print_object(ctx: Ctx, obj: XdObject, bodies: bool) -> None:
    args_all_data = getattr(ctx.args, "all_data", False)
    print(f"== {obj.key}")
    rng = ctx.colo_range(obj)
    if rng:
        anc = ctx.anchors(obj)
        nb = sum(1 for p in anc if p[2] == "body")
        order = "reverse (deferred-mode TU)" if ctx.orientation(obj) < 0 else "same"
        print(f"   Colosseum {rng[0]:08X}-{rng[1]:08X}; {len(anc)} anchors ({nb} by body); order {order}")
        print(f"   units: {', '.join(ctx.units_in(*rng))}")
    print("   .text (XD demo order; colo = aligned Colosseum function)")
    for r in align(ctx, obj, bodies):
        x = r.xd
        xd = (f"{'UNUSED' if x.unused else f'{x.addr:08X}'} {x.size:#7x} {x.name}" if x else "")
        ret = f" retail {r.retail:08X}" if r.retail else ""
        co = f"{r.colo.addr:08X} {r.colo.size:#7x} {r.colo.name}" if r.colo else "-"
        how = r.how + (f" {r.sim:.2f}" if r.sim is not None else "")
        print(f"   {xd:70s}{ret:16s} | {co:44s} {how}")
    ex = retail_extras(ctx, obj)
    if ex:
        print("   retail functions not named by the demo (possible demo-UNUSED live in retail):")
        for a, sz, nm, gap in ex:
            print(f"     {a:08X} {sz:#x} {nm}  between-demo-UNUSED: {', '.join(gap) or '-'}")
    for sec in SECTIONS:
        if sec == ".text":
            continue
        ents = [e for e in obj.sections.get(sec, []) if e.kind != "section"]
        if not ents:
            continue
        un = [e for e in ents if e.unused]
        print(f"   {sec}: {len(ents)} symbols, {len(un)} stripped")
        for e in ents:
            if e.unused or e.kind == "entry" or args_all_data:
                tag = "UNUSED" if e.unused else f"{e.addr:08X}"
                print(f"     {tag} {e.size:#6x} {e.name}")


def cmd_objects(ctx, args):
    for o in ctx.text_objects():
        print(fmt_obj_line(ctx, o))


def cmd_object(ctx, args):
    objs = ctx.find_object(args.name)
    if not objs:
        print("no such object", file=sys.stderr)
        return 1
    for o in objs:
        print_object(ctx, o, not args.no_bodies)
    return 0


def _objects_near(ctx, lo, hi):
    out = []
    for o in ctx.text_objects():
        r = ctx.colo_range(o)
        if r and r[0] < hi and r[1] > lo:
            out.append(o)
    return out


def cmd_addr(ctx, args):
    a = int(args.addr, 16)
    objs = _objects_near(ctx, a, a + 1)
    if not objs:
        # nearest anchored objects on either side
        ranged = [(ctx.colo_range(o), o) for o in ctx.text_objects() if ctx.colo_range(o)]
        below = max((x for x in ranged if x[0][1] <= a), key=lambda x: x[0][1], default=None)
        above = min((x for x in ranged if x[0][0] > a), key=lambda x: x[0][0], default=None)
        print(f"{a:08X} lies between anchored objects:")
        for x in (below, above):
            if x:
                print("  " + fmt_obj_line(ctx, x[1]))
        return 0
    for o in objs:
        print_object(ctx, o, not args.no_bodies)
    return 0


def cmd_tu(ctx, args):
    rngs = [(a, b) for u, s, a, b in ctx.splits if s == ".text" and (u == args.unit or args.unit in u)]
    if not rngs:
        print("no such unit", file=sys.stderr)
        return 1
    lo, hi = min(a for a, _ in rngs), max(b for _, b in rngs)
    print(f"# {args.unit}: .text {lo:08X}-{hi:08X}")
    for o in _objects_near(ctx, lo, hi):
        print_object(ctx, o, not args.no_bodies)
    return 0


def cmd_retail_live(ctx, args):
    for o in ctx.text_objects():
        ex = retail_extras(ctx, o)
        if not ex:
            continue
        rng = ctx.colo_range(o)
        print(f"== {o.short}" + (f"  (Colosseum {rng[0]:08X}-{rng[1]:08X})" if rng else ""))
        for a, sz, nm, gap in ex:
            print(f"   retail {a:08X} {sz:#6x} {nm:40s} demo UNUSED here: {', '.join(gap) or '-'}")


def cmd_renames(ctx, args):
    for o in ctx.text_objects():
        if args.object and args.object.lower() not in o.key.lower():
            continue
        for r in align(ctx, o, True):
            if not (r.xd and r.colo and r.xd.live and is_auto(r.colo.name)):
                continue
            if r.sim is None or r.sim < args.min_sim:
                continue
            if abs(r.xd.size - r.colo.size) > args.max_size_delta:
                continue
            print(f"{r.colo.addr:08X} {r.colo.size:#6x} {r.colo.name:24s} -> {base_name(r.xd.name):40s} "
                  f"xd {r.xd.size:#6x} retail {r.retail:08X} sim {r.sim:.3f}  [{o.short}]")


def cmd_json(ctx, args):
    out = []
    for o in ctx.objs.values():
        d = {"object": o.key, "short": o.short, "sections": {}}
        for sec, ents in o.sections.items():
            d["sections"][sec] = [{"name": e.name, "size": e.size, "addr": e.addr, "unused": e.unused,
                                   "kind": e.kind} for e in ents]
        if o.funcs():
            rng = ctx.colo_range(o)
            d["colosseum_range"] = [rng[0], rng[1]] if rng else None
            d["colosseum_units"] = ctx.units_in(*rng) if rng else []
            d["alignment"] = [{"xd": r.xd.name if r.xd else None, "colo": r.colo.name if r.colo else None,
                               "colo_addr": r.colo.addr if r.colo else None, "how": r.how, "sim": r.sim,
                               "retail": r.retail} for r in align(ctx, o, not args.no_bodies)]
        out.append(d)
    Path(args.out).write_text(json.dumps(out, indent=1))
    print(f"wrote {args.out} ({len(out)} objects)")


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--map", type=Path, default=DEF_MAP)
    ap.add_argument("--retail-symbols", type=Path, default=DEF_RETAIL_SYMS)
    ap.add_argument("--xd-asm", type=Path, default=DEF_XDASM)
    ap.add_argument("--no-bodies", action="store_true", help="align by name and size only")
    ap.add_argument("--all-data", action="store_true", help="list live data symbols too, not only stripped ones")
    ap.add_argument("--anchor-sim", type=float, default=0.8, help="body-anchor similarity threshold")
    sub = ap.add_subparsers(dest="cmd", required=True)
    sub.add_parser("objects")
    p = sub.add_parser("object"); p.add_argument("name")
    p = sub.add_parser("addr"); p.add_argument("addr")
    p = sub.add_parser("tu"); p.add_argument("unit")
    sub.add_parser("retail-live")
    p = sub.add_parser("renames")
    p.add_argument("--min-sim", type=float, default=0.9)
    p.add_argument("--max-size-delta", type=int, default=0)
    p.add_argument("--object")
    p = sub.add_parser("json"); p.add_argument("out")
    args = ap.parse_args()
    if not args.map.exists():
        print(f"map not found: {args.map} (clone StarsMmd/Colo-XD-PBR-symbol-maps)", file=sys.stderr)
        return 2
    ctx = Ctx(args)
    return {"objects": cmd_objects, "object": cmd_object, "addr": cmd_addr, "tu": cmd_tu,
            "retail-live": cmd_retail_live, "renames": cmd_renames, "json": cmd_json}[args.cmd](ctx, args) or 0


if __name__ == "__main__":
    raise SystemExit(main())
