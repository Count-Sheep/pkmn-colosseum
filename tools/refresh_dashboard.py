#!/usr/bin/env python3
"""Bring every campaign-dashboard source up to date after source changes (merges, promotions).

    python3 tools/refresh_dashboard.py                    # report + campaign queue
    python3 tools/refresh_dashboard.py --full             # also the full build and DOL/REL SHA-1 check
    python3 tools/refresh_dashboard.py --refreeze-bench   # also re-freeze stale benchmark task sets
    python3 tools/refresh_dashboard.py --readme           # also the README progress table (a tracked file)

The dashboard reads some sources live and others from stored state:
- live from the report: overall measures, category maps, and the recomp checker's rows (which also
  read the sibling Pokemon-Recomp checkout);
- stored: the campaign queue (worklist counts, high-value and priority lists, coverage) and the
  progress-over-time chart. These only change on `local_campaign.py sync`, which this tool runs.
Builds take the shared build lock, so a running benchmark or worker waits instead of seeing a
half-updated tree. The queue sync is skipped, with a note, while a campaign worker is running.
"""

from __future__ import annotations

import argparse
import json
import shutil
import subprocess
import sys
from pathlib import Path
from typing import Any

import local_campaign as lc
import local_campaign_bench as bench
from local_campaign_recomp import recomp_status

ROOT = lc.ROOT


def run(command: list[str], description: str) -> bool:
    print(f"--- {description}", flush=True)
    result = subprocess.run(command, cwd=ROOT, capture_output=True, text=True)
    tail = (result.stdout + result.stderr).strip().splitlines()[-6:]
    for line in tail:
        print(f"    {line}")
    if result.returncode:
        print(f"    FAILED (exit {result.returncode})")
    return result.returncode == 0


def numbers() -> dict[str, Any]:
    """The figures the dashboard shows most prominently."""
    report = lc.read_json(lc.REPORT_FILE, {}) or {}
    measures = report.get("measures") or {}
    state = lc.read_json(lc.STATE_FILE, {}) or {}
    items = (state.get("items") or {}).values()
    recomp = recomp_status(ROOT, lc.REPORT_FILE)
    current = (recomp.get("current") or {}) if recomp.get("available") else {}
    snapshots = state.get("snapshots") or [{}]
    return {
        "matched functions": measures.get("matched_functions"),
        "linked code %": round(float(measures.get("complete_code_percent") or 0), 3),
        "queue: pending": sum(item.get("status") == "pending" for item in items),
        "queue: size": len(state.get("items") or {}),
        "chart: latest point": snapshots[-1].get("matched_functions"),
        "recomp current row": current.get("id"),
        "recomp current row blocking": (current.get("dependencies") or {}).get("blocking_count"),
        "recomp accepted": f"{(recomp.get('summary') or {}).get('accepted_functions')}/"
                           f"{(recomp.get('summary') or {}).get('functions')}" if recomp.get("available") else None,
    }


def stale_bench_tasks() -> dict[str, list[str]]:
    """Benchmark tasks whose owner source no longer matches the frozen hash."""
    stale = {}
    for name in ("main", "prelim"):
        rows = (lc.read_json(bench.set_file(name), {}) or {}).get("tasks", [])
        stale[name] = [row["symbol"] for row in rows
                       if bench.owner_digest({"source": row["owner_source"]}) != row["source_sha256"]]
    return stale


def refreeze_bench() -> None:
    """Archive the runs made against the old sets, then freeze new ones on the current source."""
    stamp = lc.timestamp()[:19].replace(":", "").replace("-", "")
    archive = bench.BENCH_DIR / f"archive-{stamp}-refreeze"
    for folder in [bench.runs_dir("main"), bench.runs_dir("prelim")]:
        if folder.exists() and any(folder.iterdir()):
            archive.mkdir(parents=True, exist_ok=True)
            shutil.move(str(folder), str(archive / folder.name))
    for name in ("main", "prelim"):
        if bench.set_file(name).exists():
            archive.mkdir(parents=True, exist_ok=True)
            shutil.copy2(bench.set_file(name), archive / bench.set_file(name).name)
    if archive.exists():
        (archive / "README.txt").write_text("Benchmark runs and task sets archived by refresh_dashboard.py "
                                            "--refreeze-bench: their tasks' source changed, so they are not "
                                            "comparable with runs on the re-frozen sets.\n")
        print(f"    archived earlier runs and sets to {archive.relative_to(ROOT)}")
    bench.select(1.0, 20260929, 2048, True)
    bench.select_prelim(5, True)


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--full", action="store_true", help="also run the full build and the DOL/REL SHA-1 check")
    parser.add_argument("--refreeze-bench", action="store_true",
                        help="re-freeze benchmark task sets whose source changed (archives earlier runs)")
    parser.add_argument("--readme", action="store_true", help="also update the README progress table")
    args = parser.parse_args()

    before = numbers()
    ok = True
    build = [sys.executable, "tools/local_campaign.py", "build", "--worker", "refresh", "--"]
    ok &= run(build + [sys.executable, "configure.py", "--no-progress"], "configure (regenerates build.ninja after config changes)")
    ok &= run(build + ["ninja", "all_source", "build/GC6E01/report.json"], "report")
    if args.full:
        ok &= run(build + ["ninja"], "full build and SHA-1 check")
    if not ok:
        print("build failed: nothing else refreshed")
        return 1

    busy = bench.busy_workers()
    if busy:
        print(f"--- campaign queue: NOT synced, workers are running ({', '.join(busy)}); stop them and rerun")
    else:
        ok &= run([sys.executable, "tools/local_campaign.py", "sync"], "campaign queue sync (queue panels and progress chart)")

    stale = stale_bench_tasks()
    print(f"--- benchmark: stale tasks main {len(stale['main'])} {stale['main'][:6]}, "
          f"prelim {len(stale['prelim'])} {stale['prelim']}")
    if args.refreeze_bench and (stale["main"] or stale["prelim"]):
        refreeze_bench()
    elif stale["main"] or stale["prelim"]:
        print("    pass --refreeze-bench to re-freeze them (earlier runs are archived)")

    if args.readme:
        ok &= run([sys.executable, "tools/update_readme_progress.py"], "README progress table")

    after = numbers()
    print("\n| Figure | Before | After |\n|---|---|---|")
    for key in after:
        print(f"| {key} | {before.get(key)} | {after.get(key)} |")
    print("\nThe dashboard server picks these up on its next refresh (every 2 seconds).")
    return 0 if ok else 1


if __name__ == "__main__":
    sys.exit(main())
