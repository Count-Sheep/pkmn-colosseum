#!/usr/bin/env python3
"""Validate XD name candidates against the T1 `xd` rule and write a rename batch.

docs/NAMING_SCHEME.md, tier T1 kind `xd`: a name from XD retail
(TeamOrre/xd-decomp config/GXXE01/symbols.txt @ 4989794e) counts only when
  1. the same name is also in NXXJ01.map (the XD JP demo's own linker map),
  2. the bodies are identical apart from register and address differences
     (checked here as: equal size and an equal normalised instruction stream,
     registers -> R/F/CR, symbols/labels/branch targets -> SYM), and
  3. they share a rare string (fan-out <= 2 in both games), or the function
     has the same position in the map object (an adjacent Colosseum neighbour
     is already named, or accepted in an earlier round, with a name that sits in
     the same NXXJ01 object at a consistent distance and orientation).
Plus the house collision rules: the name must not already be a Colosseum
symbol, and no two candidates may claim one name.

Inputs (defaults are the decomp layout; the NXXJ01 map is external):
  --candidates  build/xd_ref/candidates/dryrun/xd_candidates_union.json
  --map         build/xd_ref/lane/maps/NXXJ01.map  (StarsMmd/Colo-XD-PBR-symbol-maps @ 6b51d3af)
  --xd-symbols  tools/symbolmap/xd_ref/GXXE01/symbols.txt
  --xd-asm      build/xd_ref/_xdsplit/asm   (dtk split of the XD retail DOL)
  --colo-asm    build/GC6E01/asm
Outputs: a batch CSV (ledger columns) of accepted rows, and a JSON report with
every candidate's verdict and reasons.
"""

from __future__ import annotations

import argparse
import csv
import datetime as dt
import json
import re
import sys
from collections import Counter, defaultdict
from pathlib import Path

TOOLS = Path(__file__).resolve().parents[1]
REPO = TOOLS.parent
sys.path.insert(0, str(TOOLS))
import check_naming as cn  # noqa: E402

XD_DECOMP = "TeamOrre/xd-decomp@4989794e"
FN_START = re.compile(r"^\.fn\s+(\S+?),")
FN_END = re.compile(r"^\.endfn\b")
INS = re.compile(r"^/\*\s*([0-9A-F]{8})\s+[0-9A-F]+\s+(?:[0-9A-F]{2}\s?){4}\s*\*/\s*(\S+)\s*(.*)$")
OBJ_HDR = re.compile(r"^# \.(\w+):0x[0-9A-Fa-f]+ \| 0x([0-9A-Fa-f]+) \| size: 0x([0-9A-Fa-f]+)")
OBJ = re.compile(r'^\.obj\s+("?)(.+?)\1,')
STRING = re.compile(r'^\s*\.string\s+"(.*)"\s*$')
SYMREF = re.compile(r'"?(@?[A-Za-z_.$@][\w.$@]*)"?@(?:ha|h|l|sda21|sda2)\b')
REG = re.compile(r"\b(?:r|f|cr|qr)\d+\b")
MAP_LIVE = re.compile(
    r"^\s+([0-9a-f]{8}) ([0-9a-f]{6}) ([0-9a-f]{8}) ([0-9a-f]{8})\s+(\d+)?\s*(\S+)(?: \(entry of [^)]*\))?\s*\t(.*)$")
MAP_UNUSED = re.compile(r"^\s+UNUSED\s+([0-9a-f]{6}) \.{8} \.{8}\s+(\S+) (.*)$")
MAP_SECTION = re.compile(r"^(\S+) section layout")


def base_name(name: str) -> str:
    m = re.match(r"^(.+?[^_])__(?:F|Q\d|\d)", name)
    return m.group(1) if m else name


# --------------------------------------------------------------------------- asm

def parse_asm(asm_dir: Path):
    """-> (funcs: name -> {addr, ins:[(mn, ops)], refs:set}, strings: label -> (addr, text))."""
    funcs: dict[str, dict] = {}
    strings: dict[str, tuple[int, str]] = {}
    for s in sorted(asm_dir.rglob("*.s")):
        cur = None
        obj_addr, obj_name = None, None
        for line in s.read_text(encoding="utf-8", errors="replace").splitlines():
            m = OBJ_HDR.match(line)
            if m:
                obj_addr = int(m.group(2), 16)
                continue
            m = OBJ.match(line)
            if m:
                obj_name = m.group(2)
                continue
            m = STRING.match(line)
            if m and obj_name and obj_addr is not None and obj_name not in strings:
                strings[obj_name] = (obj_addr, m.group(1))
                continue
            m = FN_START.match(line)
            if m:
                cur = {"name": m.group(1), "addr": None, "ins": [], "refs": set()}
                continue
            if cur is None:
                continue
            if FN_END.match(line):
                funcs.setdefault(cur["name"], cur)
                cur = None
                continue
            mi = INS.match(line)
            if mi:
                cur["addr"] = cur["addr"] if cur["addr"] is not None else int(mi.group(1), 16)
                ops = mi.group(3).strip()
                cur["ins"].append((mi.group(2), ops))
                cur["refs"].update(x for x in SYMREF.findall(ops))
    return funcs, strings


def normalise(ins: list[tuple[str, str]]) -> list[str]:
    out = []
    for mn, ops in ins:
        o = SYMREF.sub("SYM", ops)
        if mn.startswith("b") and mn not in ("bctr", "bctrl", "blr", "blrl") and ops:
            parts = o.split(",")
            parts[-1] = "SYM"  # branch target (label or function)
            o = ",".join(parts)
        o = REG.sub(lambda m: re.sub(r"\d+", "", m.group(0)), o)
        out.append(f"{mn} {o}")
    return out


def string_refs(fn: dict, strings: dict) -> dict[str, int]:
    """text -> string address for the strings a function references."""
    out = {}
    for r in fn["refs"]:
        if r in strings:
            addr, text = strings[r]
            if len(text) >= 4:
                out.setdefault(text, addr)
    return out


# --------------------------------------------------------------------------- map

def parse_map(path: Path):
    """-> (objects: key -> ordered .text entries [(name, size, unused)], name -> [object keys])."""
    objs: dict[str, list[tuple[str, int, bool]]] = defaultdict(list)
    section = None
    for raw in path.open(errors="replace"):
        line = raw.rstrip("\r\n")
        m = MAP_SECTION.match(line)
        if m:
            section = m.group(1)
            continue
        if section not in (".text", ".init"):
            continue
        m = MAP_LIVE.match(line)
        if m:
            name, owner = m.group(6), m.group(7).strip()
            if "(entry of" in line or (m.group(5) == "1" and name == section):
                continue
            objs[owner].append((name, int(m.group(2), 16), False))
            continue
        m = MAP_UNUSED.match(line)
        if m and not m.group(2).startswith("..."):
            objs[m.group(3).strip()].append((m.group(2), int(m.group(1), 16), True))
    where: dict[str, list[str]] = defaultdict(list)
    for key, ents in objs.items():
        for n, _s, _u in ents:
            where[base_name(n)].append(key)
    return objs, where


def short(key: str) -> str:
    return key.split()[-1].replace("\\", "/").split("/")[-1] if key.split() else key


def load_syms(path: Path):
    out = []
    for line in path.read_text(errors="replace").splitlines():
        m = cn.SYM_LINE.match(line)
        if not m or "type:function" not in m.group(4):
            continue
        sz = re.search(r"size:0x([0-9A-Fa-f]+)", m.group(4))
        out.append((m.group(1), int(m.group(3), 16), int(sz.group(1), 16) if sz else 0))
    return out


# --------------------------------------------------------------------------- position

def position_ok(fn_name, xd_name, colo_order, idx, names_now, objs, where, window=3):
    """Same position in the map object as an adjacent named Colosseum neighbour."""
    keys = where.get(base_name(xd_name), [])
    for key in keys:
        ents = objs[key]
        pos_all = {base_name(n): i for i, (n, _s, _u) in enumerate(ents)}
        live = [base_name(n) for n, _s, u in ents if not u]
        pos_live = {n: i for i, n in enumerate(live)}
        me = base_name(xd_name)
        if me not in pos_all:
            continue
        i = idx[fn_name]
        for step in (-1, 1):
            for d in range(1, window + 1):
                j = i + step * d
                if not 0 <= j < len(colo_order):
                    break
                nb = names_now.get(colo_order[j])
                if nb is None:
                    continue  # unnamed neighbour: look further
                nbb = base_name(nb)
                if nbb not in pos_all:
                    break  # nearest named neighbour is outside this object
                gap_c = d - 1
                for orient in (1, -1):
                    dm_all = (pos_all[nbb] - pos_all[me]) * orient * step
                    if dm_all <= 0:
                        continue
                    if me in pos_live and nbb in pos_live:
                        lo = abs(pos_live[nbb] - pos_live[me]) - 1
                    else:
                        lo = 0
                    if lo <= gap_c <= dm_all - 1:
                        return key, nb
                break
    return None


# --------------------------------------------------------------------------- main

def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--root", type=Path, default=REPO)
    ap.add_argument("--candidates", type=Path)
    ap.add_argument("--conf", default="HIGH", help="comma list of confidence levels to validate")
    ap.add_argument("--map", type=Path)
    ap.add_argument("--xd-symbols", type=Path)
    ap.add_argument("--xd-asm", type=Path)
    ap.add_argument("--colo-asm", type=Path)
    ap.add_argument("--batch-tag", default="xdhigh", help="unit-stem part of the batch_id")
    ap.add_argument("--out-batch", type=Path, required=True)
    ap.add_argument("--out-report", type=Path, required=True)
    args = ap.parse_args()

    root = args.root.resolve()
    shared = root
    cand_path = args.candidates or root / "build/xd_ref/candidates/dryrun/xd_candidates_union.json"
    map_path = args.map or root / "build/xd_ref/lane/maps/NXXJ01.map"
    xd_syms_path = args.xd_symbols or root / "tools/symbolmap/xd_ref/GXXE01/symbols.txt"
    xd_asm = args.xd_asm or shared / "build/xd_ref/_xdsplit/asm"
    colo_asm = args.colo_asm or root / "build/GC6E01/asm"
    for p in (cand_path, map_path, xd_syms_path, xd_asm, colo_asm):
        if not p.exists():
            print(f"missing input: {p}", file=sys.stderr)
            return 2

    confs = set(args.conf.split(","))
    cands = {k: v for k, v in json.loads(cand_path.read_text()).items() if v["conf"] in confs}
    objs, where = parse_map(map_path)
    xd_syms = load_syms(xd_syms_path)
    xd_by_name = defaultdict(list)
    for n, a, s in xd_syms:
        xd_by_name[n].append((a, s))
    colo_syms = load_syms(root / "config/GC6E01/symbols.txt")
    all_colo_names = set(cn.load_symbols(root))
    colo_order = [n for n, a, s in sorted(colo_syms, key=lambda t: t[1])]
    idx = {n: i for i, n in enumerate(colo_order)}
    colo_size = {n: s for n, a, s in colo_syms}
    colo_addr = {n: a for n, a, s in colo_syms}
    print("parsing asm ...", file=sys.stderr)
    cf, cstr = parse_asm(colo_asm)
    xf, xstr = parse_asm(xd_asm)
    xd_fn_at = {f["addr"]: f for f in xf.values() if f["addr"] is not None}

    # String fan-out per game (by text).
    def fanout(funcs, strings):
        c = Counter()
        for f in funcs.values():
            for t in string_refs(f, strings):
                c[t] += 1
        return c
    cfan, xfan = fanout(cf, cstr), fanout(xf, xstr)

    claim = Counter(v["names"][0] for v in cands.values() if len(v["names"]) == 1)
    results = {}
    stage1 = {}
    for fn, v in sorted(cands.items()):
        r = {"fn": fn, "conf": v["conf"], "names": v["names"], "reasons": []}
        results[fn] = r
        if len(v["names"]) != 1:
            r["reasons"].append("several candidate names")
            continue
        name = v["names"][0]
        r["name"] = name
        if fn not in colo_addr:
            r["reasons"].append("fn_ no longer in symbols.txt")
            continue
        if claim[name] > 1:
            r["reasons"].append("name claimed by several candidates")
        if name in all_colo_names:
            r["reasons"].append("name already a Colosseum symbol")
        if base_name(name) not in where:
            r["reasons"].append("name not in NXXJ01.map")
        xs = xd_by_name.get(name, [])
        if len(xs) != 1:
            r["reasons"].append(f"name has {len(xs)} entries in GXXE01 symbols")
            continue
        xa, xsz = xs[0]
        r["xd_addr"] = f"0x{xa:08X}"
        r["size_colo"], r["size_xd"] = colo_size[fn], xsz
        if colo_size[fn] != xsz:
            r["reasons"].append(f"size differs (colo 0x{colo_size[fn]:X}, xd 0x{xsz:X})")
        cfun, xfun = cf.get(fn), xd_fn_at.get(xa)
        if not cfun or not xfun:
            r["reasons"].append("body not found in asm")
            continue
        same = normalise(cfun["ins"]) == normalise(xfun["ins"])
        r["body_identical"] = same
        if not same:
            r["reasons"].append("bodies differ beyond registers/addresses")
        cs, xsr = string_refs(cfun, cstr), string_refs(xfun, xstr)
        rare = sorted(t for t in cs.keys() & xsr.keys() if cfan[t] <= 2 and xfan[t] <= 2)
        if rare:
            r["rare_string"] = rare[0]
            r["str_addr"] = f"0x{cs[rare[0]]:08X}"
        if not r["reasons"]:
            stage1[fn] = r

    # Position check as a fixed point anchored on existing names.
    names_now = {n: n for n in colo_order if not cn.AUTO_NAME.match(n)}
    accepted: dict[str, dict] = {}
    changed = True
    while changed:
        changed = False
        for fn, r in stage1.items():
            if fn in accepted:
                continue
            if "rare_string" in r:
                accepted[fn] = r
                r["via"] = "str"
                names_now[fn] = r["name"]
                changed = True
                continue
            pos = position_ok(fn, r["name"], colo_order, idx, names_now, objs, where)
            if pos:
                key, nb = pos
                r["object"], r["anchor"], r["via"] = short(key), nb, "map"
                accepted[fn] = r
                names_now[fn] = r["name"]
                changed = True
    for fn, r in stage1.items():
        if fn not in accepted:
            r["reasons"].append("no rare shared string and no same-position anchor in the map object")

    today = dt.date.today()
    rows = []
    for fn, r in sorted(accepted.items(), key=lambda kv: colo_addr[kv[0]]):
        addr = colo_addr[fn]
        if r["via"] == "str":
            ref = f"xd:{r['xd_addr']}+str:{r['str_addr']}"
            ev = f"XD {r['name']} @ {r['xd_addr']} ({XD_DECOMP}); NXXJ01.map; identical body; rare str {r['rare_string'][:40]!r}"
        else:
            ref = f"xd:{r['xd_addr']}+map:{r['object']}"
            ev = (f"XD {r['name']} @ {r['xd_addr']} ({XD_DECOMP}); NXXJ01 {r['object']} next to {r['anchor']}; "
                  f"identical body, size 0x{r['size_colo']:X}")
        rows.append({
            "address": f"0x{addr:08X}", "module": "main", "old_name": fn, "new_name": r["name"], "tier": "T1",
            "evidence": ev[:160], "evidence_ref": ref, "unit": "", "source_file": "",
            "batch_id": f"{today:%Y%m%d}-{args.batch_tag}-1", "commit": "", "date": today.isoformat(),
        })
    args.out_batch.parent.mkdir(parents=True, exist_ok=True)
    with args.out_batch.open("w", newline="", encoding="utf-8") as fh:
        w = csv.DictWriter(fh, fieldnames=cn.COLUMNS, lineterminator="\n")
        w.writeheader()
        w.writerows(rows)
    rejected = {fn: r for fn, r in results.items() if fn not in accepted}
    args.out_report.write_text(json.dumps({"accepted": accepted, "rejected": rejected}, indent=1, default=list))
    why = Counter(x for r in rejected.values() for x in r["reasons"])
    print(f"candidates {len(results)}: accepted {len(accepted)} "
          f"(str {sum(1 for r in accepted.values() if r['via'] == 'str')}, "
          f"map {sum(1 for r in accepted.values() if r['via'] == 'map')}), rejected {len(rejected)}")
    for k, n in why.most_common():
        print(f"  {n:4d}  {k}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
