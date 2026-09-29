#!/usr/bin/env python3
"""Compare models on fixed sets of campaign tasks with the campaign's own attempt loop.

    python3 tools/local_campaign_bench.py select                 # once: freeze the 25-task main set
    python3 tools/local_campaign_bench.py select-prelim          # 5 of those, spread by size
    python3 tools/local_campaign_bench.py run NAME --host URL --model ID [--set prelim] [--server-cmd CMD]
    python3 tools/local_campaign_bench.py report [--set prelim]  # prelim: speed and health gate
    python3 tools/local_campaign_bench.py compare A B            # paired wins/losses and a sign test
    python3 tools/local_campaign_bench.py queue CONFIG.json --stage prelim|main [--only-passing]

The preliminary round only screens for speed and answer health (valid, complete, compilable code):
five functions cannot rank models on quality. Quality is decided on the main set, function by function,
and a lead only counts when repeated runs (--seed-salt) agree.

Every run starts each task from its assigned source with no history. By default each task gets its whole
attempt budget (--early-stop restores the campaign's stall and register-only stops), so every model gets
the same number of attempts. State and candidates stay under build/local_llm_campaign/bench/, never in the
campaign's state. Verification still takes the shared build lock and each owner's claim, because it
temporarily replaces source in the shared tree. One run at a time: a lock serialises runs, which share
one model server. Results are review-only, like campaign results.
"""

from __future__ import annotations

import argparse
import copy
import fcntl
import json
import random
import shlex
import shutil
import signal
import statistics
import subprocess
import sys
import time
import urllib.request
from collections import Counter
from math import comb
from pathlib import Path
from typing import Any

import local_campaign as lc
from local_campaign_coordination import ClaimConflict

BENCH_DIR = lc.STATE_DIR / "bench"
SET_FILE = BENCH_DIR / "set.json"
LOCK_FILE = BENCH_DIR / "run.lock"
# (lowest pct, highest pct, tasks): near matches, the middle, and functions still far off.
BANDS = ((98.0, 100.0, 6), (93.0, 98.0, 7), (85.0, 93.0, 6), (0.0, 85.0, 6))
UNUSABLE = {"blocked_source_context", "blocked_no_editable_source", "stale_source", "review_exact", "running"}
# Everything a campaign visit leaves on an item; a bench task starts without any of it.
HISTORY = ("attempts", "best", "tried", "candidate", "last_report", "last_error", "activity", "worker",
           "focus_round", "retry_round", "retry_mode", "last_focused", "last_attempt_at", "promoted_pct",
           "promotion_rejected", "permuted", "rewritten", "updated_at")
# Answers the harness could not use at all: the preliminary round's health measure.
UNUSABLE_ANSWERS = ("error", "policy_rejected", "model_loop", "incomplete_response")
# Preliminary gate: a run passes on speed and answer health, never on quality.
GATE = {"speed_fraction": 0.5, "unusable": 0.25, "no_compile": 0.35}


def set_file(name: str) -> Path:
    return SET_FILE if name == "main" else BENCH_DIR / f"set-{name}.json"


def runs_dir(name: str) -> Path:
    return BENCH_DIR / ("runs" if name == "main" else f"runs-{name}")


def owner_digest(item: dict[str, Any]) -> str | None:
    path = lc.ROOT / item.get("owner_source", item["source"])
    return lc.digest(path.read_text(encoding="utf-8", errors="replace")) if path.is_file() else None


def select(count_scale: float, seed: int, max_bytes: int, force: bool, name: str = "main") -> int:
    target = set_file(name)
    if target.exists() and not force:
        print(f"{target} exists; pass --force to replace it (earlier runs then stop being comparable)")
        return 1
    state = lc.load_state(lc.DEFAULT_HOST, lc.DEFAULT_MODEL, update_settings=False)
    claimed = set(lc.COORDINATOR.snapshot().get("claims", {}))
    # A second set never reuses a task or owner of the main set, so it tests generalisation.
    taken = {row.get("owner_source") for row in (lc.read_json(SET_FILE, {}) or {}).get("tasks", [])} if name != "main" else set()
    pool = [item for item in state["items"].values()
            if item.get("status") not in UNUSABLE and item.get("kind") != "cleanup"
            and 0 < int(item.get("size") or 0) <= max_bytes
            and item.get("owner_source", item["source"]) not in claimed | taken
            and owner_digest(item) == item.get("source_sha256")]
    rng, chosen, owners = random.Random(seed), [], set()
    for low, high, count in BANDS:
        band = sorted((item for item in pool if low <= float(item["base_pct"]) < high), key=lambda item: item["id"])
        rng.shuffle(band)
        picked = 0
        for item in band:
            owner = item.get("owner_source", item["source"])
            if picked >= round(count * count_scale) or owner in owners:
                continue  # one task per owner keeps the set from leaning on one file
            owners.add(owner)
            chosen.append({"id": item["id"], "symbol": item["symbol"], "base_pct": item["base_pct"], "size": item["size"],
                           "owner_source": owner, "source_sha256": item["source_sha256"], "band": f"{low:g}-{high:g}"})
            picked += 1
    BENCH_DIR.mkdir(parents=True, exist_ok=True)
    lc.write_json(target, {"created_at": lc.timestamp(), "seed": seed, "max_bytes": max_bytes, "tasks": chosen})
    for row in chosen:
        print(f"{row['band']:>8}  {row['base_pct']:>9.4f}%  {row['size']:>5} B  {row['symbol']}")
    print(f"{len(chosen)} tasks written to {target}")
    return 0


def select_prelim(count: int, force: bool) -> int:
    """`count` main-set tasks spread across function size, smallest and largest included.

    Taken from the main set, so a run that already did the main set also has preliminary results.
    Spread by size because a model can look fast on short prompts and slow on long ones."""
    target = set_file("prelim")
    if target.exists() and not force:
        print(f"{target} exists; pass --force to replace it")
        return 1
    main = lc.read_json(SET_FILE, None)
    if not main:
        print("no main set: run `select` first")
        return 1
    ordered = sorted(main["tasks"], key=lambda row: (row["size"], row["id"]))
    picks = sorted({round(i * (len(ordered) - 1) / max(1, count - 1)) for i in range(count)})
    chosen = [ordered[i] for i in picks]
    lc.write_json(target, {"created_at": lc.timestamp(), "from": str(SET_FILE.name), "main_created_at": main["created_at"],
                           "tasks": chosen})
    for row in chosen:
        print(f"{row['size']:>5} B  {row['base_pct']:>9.4f}%  {row['symbol']}")
    print(f"{len(chosen)} tasks written to {target}")
    return 0


def busy_workers() -> list[str]:
    """Campaign workers holding a runner lock: they would share the model server with the bench."""
    busy = []
    for path in sorted((lc.STATE_DIR / "runners").glob("*.lock")):
        with path.open("a+") as handle:
            try:
                fcntl.flock(handle, fcntl.LOCK_EX | fcntl.LOCK_NB)
            except BlockingIOError:
                busy.append(path.stem)
    return busy


def server_root(host: str) -> str:
    root = host.rstrip("/")
    return root[:-3] if root.endswith("/v1") else root


def wait_for_restart(host: str, seconds: int) -> None:
    """Return once the server has stopped answering healthy, or after `seconds` if it never does."""
    url = server_root(host) + ("/health" if host.rstrip("/").endswith("/v1") else "/api/version")
    deadline = time.monotonic() + seconds
    while time.monotonic() < deadline:
        try:
            with urllib.request.urlopen(url, timeout=3) as response:
                if response.status != 200:
                    return
        except (OSError, ValueError):
            return
        time.sleep(1)


def wait_for_model(host: str, model: str, seconds: int) -> None:
    """Block until the server is healthy and serving `model` (a llama-server alias or an Ollama tag)."""
    deadline, last = time.monotonic() + seconds, ""
    compatible = host.rstrip("/").endswith("/v1")
    while time.monotonic() < deadline:
        try:
            if compatible:
                with urllib.request.urlopen(server_root(host) + "/health", timeout=5) as response:
                    healthy = json.load(response).get("status") == "ok"
                with urllib.request.urlopen(host.rstrip("/") + "/models", timeout=5) as response:
                    served = [row.get("id") for row in json.load(response).get("data", [])]
            else:
                with urllib.request.urlopen(host.rstrip("/") + "/api/tags", timeout=5) as response:
                    served = [row.get("name") for row in json.load(response).get("models", [])]
                healthy = True
            if healthy and model in served:
                return
            last = f"serving {served}"
        except (OSError, ValueError) as exc:
            last = str(exc)
        time.sleep(5)
    raise RuntimeError(f"{host} is not serving {model} after {seconds} s ({last})")


def fresh_item(item: dict[str, Any]) -> dict[str, Any]:
    clean = copy.deepcopy(item)
    for key in HISTORY:
        clean.pop(key, None)
    clean.update(status="pending", attempts=0)
    return clean


def attempt_rows(run_dir: Path, task: str) -> list[dict[str, Any]]:
    rows = []
    for path in sorted((run_dir / "candidates" / task).glob("attempt-*.report.json")):
        record = lc.read_json(path, {})
        report = record.get("report") or {}
        timings = record.get("timings") or {}
        rows.append({"status": record.get("status"), "pct": report.get("pct"),
                     "variants": len(report.get("variant_pcts") or []) or (1 if report.get("pct") is not None else 0),
                     "generation_seconds": record.get("generation_seconds") or 0.0,
                     "prompt_tps": timings.get("prompt_per_second"), "gen_tps": timings.get("predicted_per_second"),
                     "gen_tokens": timings.get("predicted_n")})
    return rows


def median(values: list[float]) -> float | None:
    values = [value for value in values if value]
    return round(statistics.median(values), 1) if values else None


def summarise(meta: dict[str, Any], tasks: list[dict[str, Any]]) -> dict[str, Any]:
    measured = [task for task in tasks if task["outcome"] == "done"]
    attempts = [row for task in measured for row in task.get("attempt_rows", [])]
    statuses = Counter(row["status"] for row in attempts)
    wall = sum(task["seconds"] for task in measured)
    gains = sorted((task["best_pct"] - task["base_pct"] for task in measured), reverse=True)
    improved = [gain for gain in gains if gain > 1e-9]
    return {
        **meta,
        "tasks": len(tasks), "measured": len(measured), "skipped": len(tasks) - len(measured),
        "improved": len(improved), "exact": sum(task["best_pct"] >= 100.0 for task in measured),
        "mean_gain_pp": round(sum(gains) / max(1, len(measured)), 4),
        # One lucky function can carry a mean: report it without the largest gain too.
        "mean_gain_without_top_pp": round(sum(gains[1:]) / max(1, len(measured) - 1), 4) if len(measured) > 1 else None,
        "attempts": len(attempts), "variants_built": sum(row["variants"] for row in attempts),
        "wall_seconds": round(wall, 1),
        "generation_seconds": round(sum(row["generation_seconds"] for row in attempts), 1),
        "seconds_per_attempt": round(wall / len(attempts), 1) if attempts else None,
        "attempts_per_hour": round(len(attempts) / (wall / 3600), 1) if wall else None,
        "improved_per_hour": round(len(improved) / (wall / 3600), 2) if wall else None,
        "prompt_tps": median([row.get("prompt_tps") for row in attempts]),
        "gen_tps": median([row.get("gen_tps") for row in attempts]),
        "statuses": dict(statuses),
        "rates": {name: round(statuses[name] / max(1, len(attempts)), 3)
                  for name in ("no_change", "duplicate", "verification_error", "invalid_edit", "incomplete_response")},
        "unusable_rate": round(sum(statuses[name] for name in UNUSABLE_ANSWERS) / max(1, len(attempts)), 3),
    }


def acquire_run_lock(wait: bool):
    BENCH_DIR.mkdir(parents=True, exist_ok=True)
    handle = LOCK_FILE.open("a+")
    try:
        fcntl.flock(handle, fcntl.LOCK_EX | fcntl.LOCK_NB)
    except BlockingIOError:
        if not wait:
            handle.close()
            return None
        print("another bench run holds the model server; waiting for it to finish", flush=True)
        fcntl.flock(handle, fcntl.LOCK_EX)
    return handle


def run(name: str, host: str, model: str, args: argparse.Namespace) -> int:
    bench_set = lc.read_json(set_file(args.set), None)
    if not bench_set:
        print(f"no '{args.set}' task set: run `select`{'' if args.set == 'main' else ' / select-prelim'} first")
        return 1
    lock = acquire_run_lock(args.wait)
    if lock is None:
        print("another bench run holds the model server; pass --wait to queue behind it")
        return 1
    run_dir = runs_dir(args.set) / name
    if run_dir.exists():
        if not args.force:
            print(f"{run_dir} exists; pass --force to discard it and run again")
            return 1
        shutil.rmtree(run_dir)
    busy = busy_workers()
    if busy and not args.allow_busy:
        print(f"campaign workers are running ({', '.join(busy)}): they share the model server and would skew the "
              "timing. Stop them, or pass --allow-busy if they use a different server.")
        return 1
    if args.server_cmd:
        print(f"switching model server: {args.server_cmd}", flush=True)
        subprocess.run(args.server_cmd, shell=True, check=True)
        # The command may return before the old server exits. When it already serves the same model,
        # waiting for the name alone starts the run against a server about to restart.
        wait_for_restart(host, 90)
    print(f"waiting for {host} to serve {model}", flush=True)
    wait_for_model(host, model, args.server_wait)

    campaign = lc.load_state(lc.DEFAULT_HOST, lc.DEFAULT_MODEL, update_settings=False)
    # Point the campaign's state and candidates at this run; claims and the build lock stay shared.
    run_dir.mkdir(parents=True)
    lc.STATE_FILE, lc.CANDIDATES_DIR = run_dir / "state.json", run_dir / "candidates"
    lc.RETRIES, lc.STALL_ROUNDS, lc.THINK = args.retries, args.stall_rounds, {"auto": None, "on": True, "off": False}[args.think]
    lc.NUM_CTX, lc.PERMUTE, lc.REWRITE, lc.REASONING_TOKENS = args.num_ctx, False, False, args.reasoning_tokens
    lc.FIXED_TEMPERATURE, lc.SAMPLING, lc.TEMPLATE_KWARGS = args.temperature, args.sampling, args.template_kwargs
    lc.EARLY_STOP, lc.SEED_SALT = args.early_stop, args.seed_salt
    state = lc.blank_state(host, model, args.num_predict)
    worker = f"bench-{name}"
    meta = {"name": name, "set": args.set, "host": host, "model": model, "think": args.think,
            "reasoning_tokens": args.reasoning_tokens, "temperature": args.temperature, "sampling": args.sampling,
            "template_kwargs": args.template_kwargs, "retries": args.retries, "stall_rounds": args.stall_rounds,
            "early_stop": args.early_stop, "seed_salt": args.seed_salt, "num_predict": args.num_predict,
            "server_cmd": args.server_cmd, "set_created_at": bench_set["created_at"], "started_at": lc.timestamp()}
    tasks = []
    for row in bench_set["tasks"]:
        source = campaign["items"].get(row["id"])
        if source is None or owner_digest(source) != row["source_sha256"]:
            tasks.append({**row, "outcome": "stale", "best_pct": row["base_pct"], "seconds": 0})
            continue
        state["items"][row["id"]] = fresh_item(source)
    lc.save_state(state)

    def stop(signum, frame):
        lc.STOP_REQUESTED = True
    signal.signal(signal.SIGTERM, stop)
    signal.signal(signal.SIGINT, stop)
    for row in bench_set["tasks"]:
        if row["id"] not in state["items"] or lc.STOP_REQUESTED:
            continue
        if (lc.STATE_DIR / "halt.json").exists():
            print("campaign halted (halt.json): stopping the bench")
            break
        item = state["items"][row["id"]]
        try:
            claim = lc.COORDINATOR.claim(item.get("owner_source", item["source"]), worker, item["symbol"],
                                         automatic=True, detail="Bench attempt")
        except ClaimConflict:
            tasks.append({**row, "outcome": "claimed", "best_pct": row["base_pct"], "seconds": 0})
            continue
        started = time.monotonic()
        try:
            item["worker"] = worker
            lc.work_task(state, item, host, model, args.timeout, worker, args.num_predict)
        except lc.HostUnavailable as exc:
            print(f"model server unreachable: {exc}")
            lc.STOP_REQUESTED = True
        finally:
            lc.COORDINATOR.release(claim["source"], claim["token"])
        seconds = time.monotonic() - started
        attempts = attempt_rows(run_dir, row["id"])
        best = max([row["base_pct"]] + [a["pct"] for a in attempts if a["pct"] is not None])
        tasks.append({**row, "outcome": "done" if not lc.STOP_REQUESTED else "interrupted", "best_pct": best,
                      "seconds": round(seconds, 1), "attempt_rows": attempts})
        print(f"{row['symbol']:<48} {row['base_pct']:>9.4f}% -> {best:>9.4f}%  {len(attempts):>2} attempts  {seconds:6.0f} s",
              flush=True)
        lc.write_json(run_dir / "results.json", {"summary": summarise(meta, tasks), "tasks": tasks})
    meta["finished_at"] = lc.timestamp()
    summary = summarise(meta, tasks)
    lc.write_json(run_dir / "results.json", {"summary": summary, "tasks": tasks})
    print(json.dumps({key: summary[key] for key in ("measured", "improved", "exact", "mean_gain_pp", "attempts_per_hour",
                                                     "improved_per_hour", "gen_tps", "unusable_rate", "rates")}, indent=2))
    return 0


def load_runs(set_name: str) -> list[dict[str, Any]]:
    """Runs on a set. For the preliminary set, main-set runs count too, restricted to its tasks."""
    found = []
    for folder in sorted(path for path in runs_dir(set_name).glob("*") if path.is_dir()) if runs_dir(set_name).exists() else []:
        results = lc.read_json(folder / "results.json", None)
        if results is None:
            # Started but no task finished yet (results.json is written after each task): list it anyway.
            settings = (lc.read_json(folder / "state.json", {}) or {}).get("settings") or {}
            summary = summarise({"name": folder.name, "set": set_name, "model": settings.get("ollama_model"),
                                 "in_progress": True}, [])
            summary["tasks"] = len((lc.read_json(set_file(set_name), {}) or {}).get("tasks", []))
            results = {"summary": summary, "tasks": []}
        found.append({**results, "source": set_name})
    if set_name != "main":
        wanted = {row["id"] for row in (lc.read_json(set_file(set_name), {}) or {}).get("tasks", [])}
        have = {run["summary"].get("name") for run in found}
        for path in sorted(runs_dir("main").glob("*/results.json")) if runs_dir("main").exists() else []:
            results = lc.read_json(path, {})
            summary = results.get("summary") or {}
            if summary.get("name") in have:
                continue
            tasks = [task for task in results.get("tasks", []) if task["id"] in wanted]
            if tasks:
                found.append({"summary": summarise({**summary, "set": set_name}, tasks), "tasks": tasks, "source": "main"})
    return found


def gate(runs: list[dict[str, Any]]) -> dict[str, tuple[bool, str]]:
    """Speed and answer health only. Speed is attempts per hour against the fastest complete run."""
    complete = [run["summary"] for run in runs if run["summary"].get("measured") and not run["summary"].get("skipped")]
    fastest = max((summary.get("attempts_per_hour") or 0 for summary in complete), default=0)
    verdicts = {}
    for run in runs:
        summary = run["summary"]
        reasons = []
        if summary.get("in_progress"):
            verdicts[summary.get("name")] = (False, "in progress")
            continue
        if not summary.get("measured") or summary.get("skipped"):
            reasons.append(f"incomplete ({summary.get('measured', 0)}/{summary.get('tasks', 0)} tasks)")
        if fastest and (summary.get("attempts_per_hour") or 0) < GATE["speed_fraction"] * fastest:
            reasons.append(f"slow ({summary.get('attempts_per_hour')} attempts/h, fastest {fastest})")
        if (summary.get("unusable_rate") or 0) > GATE["unusable"]:
            reasons.append(f"{summary['unusable_rate']:.0%} unusable answers")
        if (summary.get("rates", {}).get("verification_error") or 0) > GATE["no_compile"]:
            reasons.append(f"{summary['rates']['verification_error']:.0%} did not compile")
        verdicts[summary.get("name")] = (not reasons, "; ".join(reasons) or "pass")
    return verdicts


def report(names: list[str], set_name: str, as_json: bool = False) -> int:
    runs = [run for run in load_runs(set_name) if not names or run["summary"].get("name") in names]
    if not runs:
        print("no bench results yet")
        return 1
    verdicts = gate(runs) if set_name == "prelim" else {}
    if as_json:
        print(json.dumps({"passing": [name for name, (ok, _) in verdicts.items() if ok], "verdicts": verdicts}))
        return 0
    if set_name == "prelim":
        print("| Run | Tasks | Attempts/h | s/attempt | Gen tok/s | Prompt tok/s | Unusable | No compile | Gate |")
        print("|---|---|---|---|---|---|---|---|---|")
        for run in runs:
            s = run["summary"]
            ok, why = verdicts[s.get("name")]
            label = s["name"] + (" (from main run)" if run["source"] == "main" else "")
            print(f"| {label} | {s['measured']}/{s['tasks']} | {s['attempts_per_hour']} | {s['seconds_per_attempt']} | "
                  f"{s.get('gen_tps') or '-'} | {s.get('prompt_tps') or '-'} | {s['unusable_rate']:.0%} | "
                  f"{s['rates'].get('verification_error', 0):.0%} | {'PASS' if ok else 'FAIL: ' + why} |")
        return 0
    print("| Run | Model | Tasks | Improved | Exact | Mean gain (pp) | Without top (pp) | Attempts/h | Improved/h | Unchanged | Repeats | No compile |")
    print("|---|---|---|---|---|---|---|---|---|---|---|---|")
    for run in runs:
        s = run["summary"]
        rates = s.get("rates", {})
        without = s.get("mean_gain_without_top_pp")
        print(f"| {s['name']} | {s['model']} | {s['measured']}/{s['tasks']} | {s['improved']} | {s['exact']} | "
              f"{s['mean_gain_pp']:.3f} | {without if without is not None else '-'} | {s['attempts_per_hour']} | "
              f"{s['improved_per_hour']} | {rates.get('no_change', 0):.0%} | {rates.get('duplicate', 0):.0%} | "
              f"{rates.get('verification_error', 0):.0%} |")
    return 0


def sign_test(wins: int, losses: int) -> float:
    """Two-sided exact sign test on the non-tied pairs."""
    n = wins + losses
    if not n:
        return 1.0
    tail = sum(comb(n, i) for i in range(max(wins, losses), n + 1)) / 2 ** n
    return min(1.0, 2 * tail)


def compare(left: str, right: str, set_name: str) -> int:
    """Function-by-function comparison. Comma-separated names pair replicate runs in order."""
    runs = {run["summary"].get("name"): run for run in load_runs(set_name)}
    sides = [name.split(",") for name in (left, right)]
    if len(sides[0]) != len(sides[1]) or any(name not in runs for side in sides for name in side):
        print(f"need the same number of existing runs on each side; have: {', '.join(sorted(runs))}")
        return 1
    wins = losses = ties = exact_left = exact_right = 0
    for a_name, b_name in zip(*sides):
        a = {task["id"]: task for task in runs[a_name]["tasks"] if task["outcome"] == "done"}
        b = {task["id"]: task for task in runs[b_name]["tasks"] if task["outcome"] == "done"}
        for task in sorted(set(a) & set(b)):
            gain_a, gain_b = a[task]["best_pct"] - a[task]["base_pct"], b[task]["best_pct"] - b[task]["base_pct"]
            exact_left += a[task]["best_pct"] >= 100.0
            exact_right += b[task]["best_pct"] >= 100.0
            if abs(gain_a - gain_b) < 1e-6:
                ties += 1
            elif gain_a > gain_b:
                wins += 1
                print(f"  {left} better   {a[task]['symbol'][:40]:<40} +{gain_a:.3f} vs +{gain_b:.3f}")
            else:
                losses += 1
                print(f"  {right} better  {a[task]['symbol'][:40]:<40} +{gain_a:.3f} vs +{gain_b:.3f}")
    p = sign_test(wins, losses)
    print(f"{left} better on {wins}, {right} better on {losses}, ties {ties}; exact {exact_left} vs {exact_right}; "
          f"sign test p = {p:.2f}")
    verdict = "no clear leader" if p >= 0.1 else f"{left if wins > losses else right} leads"
    print(f"verdict (p < 0.1 over non-tied functions): {verdict}")
    return 0


def queue(config_path: str, stage: str, only_passing: bool, dry_run: bool) -> int:
    """Run each configuration in a JSON file on one set, one after another (see bench/queue.json)."""
    config = json.loads(Path(config_path).read_text())
    passing = None
    if only_passing:
        passing = {name for name, (ok, _) in gate(load_runs("prelim")).items() if ok}
        print(f"passed the preliminary gate: {', '.join(sorted(passing)) or 'none'}")
    for entry in config["configs"]:
        if passing is not None and entry["name"] not in passing:
            print(f"=== {entry['name']}: skipped (did not pass the preliminary gate)")
            continue
        if stage in entry.get("skip_stages", []):
            continue
        extra = entry.get("server_extra", "")
        server = config["server_cmd"].format(model=entry["model"], extra=(f' -Extra "{extra}"' if extra else ""))
        command = [sys.executable, str(Path(__file__).resolve()), "run", entry["name"], "--set", stage, "--wait",
                   "--host", config["host"], "--model", entry["model"], "--server-cmd", server,
                   "--server-wait", str(config.get("server_wait", 1200)), *entry.get("args", []),
                   *(["--force"] if config.get("force") else [])]
        print(f"=== {lc.timestamp()[11:19]} {entry['name']} on {stage}: {shlex.join(command[2:])}", flush=True)
        if dry_run:
            continue
        if config.get("pre_cmd"):
            subprocess.run(config["pre_cmd"], shell=True)
        subprocess.run(command)
    return 0


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    commands = parser.add_subparsers(dest="command", required=True)
    chooser = commands.add_parser("select", help="freeze a benchmark task set")
    chooser.add_argument("--name", default="main", help="set name; any other than 'main' avoids the main set's owners")
    chooser.add_argument("--scale", type=float, default=1.0, help="multiply the per-band task counts (default 1: 25 tasks)")
    chooser.add_argument("--seed", type=int, default=20260929)
    chooser.add_argument("--max-bytes", type=int, default=2048, help="largest function in the set (default 2048)")
    chooser.add_argument("--force", action="store_true")
    prelim = commands.add_parser("select-prelim", help="freeze the preliminary set: main-set tasks spread by size")
    prelim.add_argument("--count", type=int, default=5)
    prelim.add_argument("--force", action="store_true")
    runner = commands.add_parser("run", help="run one model over a task set")
    runner.add_argument("name", help="run name, e.g. qwen3-coder-30b-rpc")
    runner.add_argument("--set", default="main", help="task set: main, prelim, or a named set")
    runner.add_argument("--host", required=True, help="model server; a URL ending in /v1 is OpenAI-compatible")
    runner.add_argument("--model", required=True, help="model id the server must be serving")
    runner.add_argument("--think", choices=("auto", "on", "off"), default="auto")
    runner.add_argument("--temperature", type=float, default=None,
                        help="one sampling temperature for every attempt instead of the retry schedule")
    runner.add_argument("--sampling", type=json.loads, default={}, help="extra sampling fields as JSON")
    runner.add_argument("--template-kwargs", type=json.loads, default={}, help="chat-template options as JSON")
    runner.add_argument("--reasoning-tokens", type=int, default=lc.REASONING_TOKENS,
                        help="output allowance for thinking under --think on")
    runner.add_argument("--retries", type=int, default=7, help="correction rounds per task (default 7: 8 attempts)")
    runner.add_argument("--stall-rounds", type=int, default=4, help="only with --early-stop")
    runner.add_argument("--early-stop", action="store_true",
                        help="stop a task like the campaign does (stalls, unchanged register-only answers)")
    runner.add_argument("--seed-salt", default="", help="vary the per-attempt seeds, for a repeated run")
    runner.add_argument("--num-predict", type=int, default=4096)
    runner.add_argument("--num-ctx", type=int, default=16384)
    runner.add_argument("--timeout", type=int, default=900)
    runner.add_argument("--server-cmd", help="shell command that (re)starts the server on this model first")
    runner.add_argument("--server-wait", type=int, default=900, help="seconds to wait for the model to be served")
    runner.add_argument("--wait", action="store_true", help="queue behind another bench run instead of refusing")
    runner.add_argument("--allow-busy", action="store_true")
    runner.add_argument("--force", action="store_true")
    reporter = commands.add_parser("report", help="one row per run; on the prelim set, the speed and health gate")
    reporter.add_argument("names", nargs="*")
    reporter.add_argument("--set", default="main")
    reporter.add_argument("--json", action="store_true", help="prelim: print the gate verdicts as JSON")
    comparer = commands.add_parser("compare", help="function-by-function comparison of two runs (or replicate lists)")
    comparer.add_argument("left")
    comparer.add_argument("right")
    comparer.add_argument("--set", default="main")
    queuer = commands.add_parser("queue", help="run the configurations in a JSON file on one set")
    queuer.add_argument("config")
    queuer.add_argument("--stage", default="prelim", help="task set to run them on (default prelim)")
    queuer.add_argument("--only-passing", action="store_true", help="skip configurations that failed the prelim gate")
    queuer.add_argument("--dry-run", action="store_true")
    args = parser.parse_args()
    if args.command == "select":
        return select(args.scale, args.seed, args.max_bytes, args.force, args.name)
    if args.command == "select-prelim":
        return select_prelim(args.count, args.force)
    if args.command == "run":
        return run(args.name, args.host, args.model, args)
    if args.command == "compare":
        return compare(args.left, args.right, args.set)
    if args.command == "queue":
        return queue(args.config, args.stage, args.only_passing, args.dry_run)
    return report(args.names, args.set, args.json)


if __name__ == "__main__":
    sys.exit(main())
