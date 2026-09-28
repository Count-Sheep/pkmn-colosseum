#!/usr/bin/env python3
"""Promote local-model improvements into tracked source so progress compounds.

usage: promote_local.py [--apply] [--min-gain 0.05] [--symbol NAME]
Without --apply it only lists what would be tried. With --apply, each candidate is
spliced into the CURRENT owner source (function-local), then kept only if:
  * the runner's policy() has no rejections AND no flags (flagged ones are held for review),
  * the target function still scores >= the candidate's measured score,
  * no other function in the full report regresses,
  * main.dol and every module hash in build.sha1 still verify.
Each promotion is one commit. Failures are reverted.
"""
import argparse, json, subprocess, sys
from pathlib import Path

ROOT = Path("/Users/cs/Nextcloud/VSCode/Pokemon-GC")
sys.path.insert(0, str(ROOT / "tools"))
import local_campaign as c  # noqa: E402

REPORT = ROOT / "build/GC6E01/report.json"


def scores():
    r = json.loads(REPORT.read_text())
    return {(f["name"], str(f.get("metadata", {}).get("virtual_address", ""))): f.get("fuzzy_match_percent", 0)
            for u in r["units"] for f in u.get("functions", [])}


def build(cmd):
    p = subprocess.run([sys.executable, "tools/local_campaign.py", "build", "--worker", "Claude-promote", "--", *cmd],
                       cwd=ROOT, capture_output=True, text=True)
    return p.returncode, p.stdout + p.stderr


def sha_ok():
    p = subprocess.run(["shasum", "-a", "1", "-c", "config/GC6E01/build.sha1"], cwd=ROOT, capture_output=True, text=True)
    return p.returncode == 0, p.stdout.strip()


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--apply", action="store_true")
    ap.add_argument("--min-gain", type=float, default=0.05)
    ap.add_argument("--symbol")
    ap.add_argument("--allow-flags", action="store_true", help="human-reviewed: accept flagged (not rejected) candidates")
    a = ap.parse_args()
    state = json.loads(c.STATE_FILE.read_text())
    claims = set(c.COORDINATOR.snapshot().get("claims", {}))
    todo = []
    for item in state["items"].values():
        best = item.get("best") or {}
        rep = item.get("last_report") or {}
        if not best and rep.get("pct") is not None and rep["pct"] > item["base_pct"]:
            best = {"pct": rep["pct"], "candidate": rep.get("candidate") or item.get("candidate"), "attempt": item.get("attempts")}
        if not best or best.get("pct") is None or best["pct"] < item["base_pct"] + a.min_gain:
            continue
        if item.get("promoted_pct") and item["promoted_pct"] >= best["pct"]:
            continue
        if a.symbol and item["symbol"] != a.symbol:
            continue
        if item.get("kind") == "cleanup":
            continue  # exact pragma-free rewrites need their pragma blocks restructured: review, not auto-promote
        owner = item.get("owner_source", item["source"])
        todo.append((best["pct"] - item["base_pct"], item, best, owner, owner in claims))
    todo.sort(key=lambda t: -t[0])
    for gain, item, best, owner, claimed in todo:
        print(f"{item['symbol']:40} {item['base_pct']:7.3f} -> {best['pct']:7.3f} (+{gain:.3f}) {owner}{'  [CLAIMED, skip]' if claimed else ''}")
    if not a.apply:
        return
    for gain, item, best, owner, claimed in todo:
        if claimed:
            continue
        try:
            promote_one(a, c, state, item, best, owner)
        except Exception as exc:  # one bad item must never stop the pass
            print(f"ERROR {item['symbol']}: {exc}")


def promote_one(a, c, state, item, best, owner):
        sym, path = item["symbol"], ROOT / owner
        cand = ROOT / best["candidate"]
        if not cand.exists():
            print("SKIP missing candidate", sym); return
        current = path.read_text(encoding="utf-8", errors="replace")
        try:
            cs, ce = c.function_span(current, sym)
            ks, ke = c.function_span(cand.read_text(encoding="utf-8", errors="replace"), sym)
        except ValueError as e:
            print("SKIP span", sym, e); return
        proposal = cand.read_text(encoding="utf-8", errors="replace")[ks:ke]
        rejected, flags = c.policy(current[cs:ce], proposal)
        if rejected or (flags and not a.allow_flags):
            print(f"HOLD {sym}: rejected={rejected} flags={flags}"); return
        before = scores()
        path.write_text(current[:cs] + proposal + current[ce:], encoding="utf-8")
        rc, out = build(["ninja", "all_source", "build/GC6E01/report.json"])
        after = scores() if rc == 0 else {}
        mine = [v for (n, _), v in after.items() if n == sym]
        down = [(k, before[k], v) for k, v in after.items() if k in before and v < before[k] - 1e-9]
        ok = rc == 0 and mine and max(mine) >= best["pct"] - 1e-6 and not down
        if ok:
            rc2, _ = build(["ninja"]); good, shas = sha_ok()
            ok = rc2 == 0 and good
        if not ok:
            path.write_text(current, encoding="utf-8")
            build(["ninja", "all_source", "build/GC6E01/report.json"])
            print(f"REVERT {sym}: build={rc} score={mine} regressions={down[:3]}"); return
        subprocess.run(["git", "add", owner], cwd=ROOT, check=True)
        if subprocess.run(["git", "diff", "--cached", "--quiet", "--", owner], cwd=ROOT).returncode == 0:
            # Already on master (an earlier pass committed it but crashed before recording it).
            st = c.load_state(c.DEFAULT_HOST, c.DEFAULT_MODEL, update_settings=False)
            it = st["items"][item["id"]]; it["promoted_pct"] = max(mine); it["base_pct"] = max(mine)
            c.save_state(st)
            print(f"SKIP {sym}: already committed; recorded as promoted at {max(mine):.3f}")
            return
        msg = (f"Promote local-model improvement to {sym} ({item['base_pct']:.2f} -> {max(mine):.2f})\n\n"
               f"Candidate from the local model ({state['workers'].get(item.get('worker',''),{}).get('ollama_model','?')}, "
               f"attempt {best.get('attempt')}), spliced into the current {owner} and kept only after the runner's "
               "policy check passed, the full report showed no regressions, and main.dol plus module "
               "hashes verified.\n\nCo-Authored-By: Claude Opus 5.5 (1M context) <noreply@anthropic.com>")
        subprocess.run(["git", "commit", "-q", "-m", msg], cwd=ROOT, check=True)
        st = c.load_state(c.DEFAULT_HOST, c.DEFAULT_MODEL, update_settings=False)
        it = st["items"][item["id"]]; it["promoted_pct"] = max(mine); it["base_pct"] = max(mine)
        c.event(st, "local_promoted", task=item["id"], symbol=sym, pct=max(mine)); c.save_state(st)
        print(f"PROMOTED {sym} -> {max(mine):.3f}")


if __name__ == "__main__":
    main()
