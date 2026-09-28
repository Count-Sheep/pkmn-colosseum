"""Link-blocker priority list: non-exact functions in units that hold exact-but-unlinked title-closure functions."""
import json, sys
from collections import defaultdict
from pathlib import Path
sys.path.insert(0, "/Users/cs/Nextcloud/VSCode/pkmn-colosseum-recomp/tools")
import boot_status as bs
ROOT = Path("/Users/cs/Nextcloud/VSCode/Pokemon-GC")
d = bs.Decomp(ROOT)
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
waiting = defaultdict(list)  # unit -> exact-but-unlinked closure functions
for s in seen:
    r = d.best(s)
    if r and not r["unit"].startswith(bs.HOST_UNIT_PREFIXES) and r["status"] == "needs-verification":
        waiting[r["unit"]].append(s)
claimed = set(json.load(open(ROOT / "build/local_llm_campaign/coordination.json")).get("claims", {}))
state = json.load(open(ROOT / "build/local_llm_campaign/state.json"))
queued = {i["symbol"]: i for i in state["items"].values()}
plan = []
for unit, exact in waiting.items():
    rows = d.units.get(unit, [])
    below = [r for r in rows if r["fuzzy"] < 100]
    if not below: continue
    owners = {queued.get(r["symbol"], {}).get("owner_source") for r in below}
    if owners & claimed: continue
    plan.append((len(below), -len(exact), unit, sorted(below, key=lambda r: -r["fuzzy"]), len(exact)))
plan.sort()
symbols, lines = [], []
for n, _, unit, below, exact in plan:
    lines.append(f"{unit}: {exact} exact closure fn(s) waiting; blockers " + ", ".join(f"{r['symbol']} {r['fuzzy']:.2f}" for r in below[:6]))
    symbols += [r["symbol"] for r in below if r["symbol"] not in symbols]
print(len(plan), "units,", len(symbols), "blocking functions")
for l in lines[:30]: print(" ", l)
json.dump({"symbols": symbols, "units": [p[2] for p in plan]}, open(sys.argv[1], "w"), indent=1)
