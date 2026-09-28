import json, sys
sys.path.insert(0, "/Users/cs/Nextcloud/VSCode/pkmn-colosseum-recomp/tools")
import boot_status as bs
from pathlib import Path
d = bs.Decomp(Path("/Users/cs/Nextcloud/VSCode/Pokemon-GC"))
m = json.loads(bs.MANIFEST.read_text())
entries = m["blockers"] + m.get("title_path", [])
native = {n["symbol"] for e in entries for n in e.get("native_functions", [])}
boundary = {b for e in entries for b in e.get("boundary", [])}
graph = d.call_graph()
roots = set()
for e in entries:
    roots.update(e.get("functions", []))
    for u in e.get("units", []): roots.update(r["symbol"] for r in d.units.get(u, []))
    for p in e.get("unit_prefixes", []):
        roots.update(r["symbol"] for n, rs in d.units.items() if n.startswith(p) for r in rs)
tables = {t for e in entries for t in e.get("indirect_tables", [])}
stack = list(roots) + [t for tb in tables for t in d.pointer_tables.get(tb, [])]
seen = set()
while stack:
    s = stack.pop()
    if s in seen or s in native: continue
    seen.add(s)
    if s in boundary and s not in roots: continue
    stack.extend(graph.get(s, []))
from collections import Counter
st = Counter(); bytes_ = Counter(); fuzzy_bytes = 0; total_bytes = 0
for s in seen:
    r = d.best(s)
    if r is None: st["unmapped"] += 1; continue
    if r["unit"].startswith(bs.HOST_UNIT_PREFIXES): st["host"] += 1; continue
    st[r["status"]] += 1; bytes_[r["status"]] += r["size"]
    total_bytes += r["size"]; fuzzy_bytes += r["size"] * r["fuzzy"] / 100
game = sum(v for k, v in st.items() if k not in ("host", "unmapped"))
print("closure functions (game-side):", game, dict(st))
print("accepted: %d/%d = %.1f%% by count, %.1f%% by bytes" % (st["accepted"], game, 100*st["accepted"]/game, 100*bytes_["accepted"]/total_bytes))
print("byte-weighted fuzzy match: %.1f%%" % (100*fuzzy_bytes/total_bytes))
print("native-only excluded:", len(native))

# group non-accepted closure functions by unit
from collections import defaultdict
byunit = defaultdict(lambda: {"nv": [], "un": []})
for s in seen:
    r = d.best(s)
    if r is None or r["unit"].startswith(bs.HOST_UNIT_PREFIXES) or r["status"] == "accepted": continue
    byunit[r["unit"]]["nv" if r["status"] == "needs-verification" else "un"].append((s, r["fuzzy"], r["size"]))
rows = []
for u, v in byunit.items():
    allrows = d.units.get(u, [])
    below = [x for x in allrows if x["fuzzy"] < 100]
    src = allrows[0].get("source") if allrows else None
    rows.append((u, src, len(v["nv"]), len(v["un"]), len(allrows), len(below)))
print("\nunits where every function is already 100% (link-only):")
lo = sorted([r for r in rows if r[5] == 0], key=lambda r: -r[2])
print(len(lo), "units,", sum(r[2] for r in lo), "closure functions")
for r in lo[:25]: print("  ", r)
print("\nunits with closure fns and few functions below 100%:")
for r in sorted([r for r in rows if 0 < r[5] <= 3], key=lambda r: -(r[2]+r[3]))[:25]: print("  ", r)

skip = ("main/hsd/hsd_jobj", "main/hsd/hsd_range_801920E4")
with open(sys.argv[1] if len(sys.argv) > 1 else "/dev/null", "w") as f:
    for r in lo:
        if r[0].startswith(skip): continue
        syms = [s for s, _, _ in byunit[r[0]]["nv"]]
        f.write(f"{r[0]}\t{r[1]}\t{','.join(sorted(syms))}\n")
