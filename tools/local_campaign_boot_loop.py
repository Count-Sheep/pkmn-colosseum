#!/usr/bin/env python3
"""Keep the campaign pointed at the native recomp's boot frontier.

Each tick re-measures the recomp's boot-critical inventory against the current
report (see local_campaign_recomp.py) and then:

1. writes boot_focus.json, which local_campaign.py workers read to take
   frontier functions ahead of the ordinary value-score order;
2. launches at most --max-agents isolated Codex lanes, in frontier order, for
   frontier functions. These include 100% functions the Ollama queue never
   holds, such as ones blocked by prohibited pragmas or unlinked candidates.

Lanes commit only to their own branch. Nothing here merges, pushes, or promotes
source: a blocker clears only after a human accepts a lane and the report is
rebuilt, at which point the next tick moves the frontier and continues.
"""

from __future__ import annotations

import argparse
import fcntl
import json
import os
import re
import shutil
import signal
import subprocess
import sys
import time
import uuid
from datetime import UTC, datetime, timedelta
from pathlib import Path
from typing import Any

import local_campaign_agents as agents
from local_campaign_coordination import Coordinator, now
from local_campaign_recomp import recomp_status

ROOT = Path(__file__).resolve().parents[1]
STATE_DIR = ROOT / "build" / "local_llm_campaign"
REPORT_FILE = ROOT / "build" / "GC6E01" / "report.json"
FOCUS_FILE = STATE_DIR / "boot_focus.json"
LOOP_FILE = STATE_DIR / "boot_loop.json"
LOCK_FILE = STATE_DIR / "boot_loop.lock"
WORKTREE_ROOT = Path(os.environ.get("BOOT_LOOP_WORKTREES", "/private/tmp"))
COORDINATOR = Coordinator(STATE_DIR)
ACTIONABLE = ("decomp-blocked", "upstream-verification")
RETRY_AT = re.compile(r"try again at (\w{3} \d{1,2})(?:st|nd|rd|th)?(, \d{4} \d{1,2}:\d{2} [AP]M)")
STOP = False


def read_json(path: Path, default: Any) -> Any:
    try:
        return json.loads(path.read_text(encoding="utf-8"))
    except (FileNotFoundError, json.JSONDecodeError):
        return default


def write_json(path: Path, value: Any) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    temporary = path.with_suffix(path.suffix + ".tmp")
    temporary.write_text(json.dumps(value, indent=2) + "\n", encoding="utf-8")
    temporary.replace(path)


def frontier(status: dict[str, Any], depth: int) -> tuple[list[dict[str, Any]], list[dict[str, Any]]]:
    """Unaccepted functions of the first `depth` blockers that still need decomp work."""
    blockers = [row for row in status["blockers"] if row["status"] in ACTIONABLE][:depth]
    order = {"needs-verification": 0, "unmatched": 1, "missing": 2}
    targets = []
    for blocker in blockers:
        pending = [row for row in blocker["functions"] if row["status"] != "accepted"]
        pending.sort(key=lambda row: (order[row["status"]], -row["fuzzy"], row["size"]))
        targets += [{
            "symbol": row["symbol"], "unit": row.get("unit"), "source": row.get("source"),
            "fuzzy": row["fuzzy"], "size": row["size"], "status": row["status"], "issues": row["issues"],
            "blocker": blocker["index"], "blocker_title": blocker["title"],
        } for row in pending]
    return blockers, targets


def source_digest(relative: str | None) -> str | None:
    if not relative or not (ROOT / relative).exists():
        return None
    return subprocess.run(["git", "hash-object", relative], cwd=ROOT, capture_output=True, text=True).stdout.strip() or None


def goal(target: dict[str, Any]) -> str:
    issues = target["issues"]
    if target["status"] == "unmatched":
        return (f"Raise `{target['symbol']}` from {target['fuzzy']:.5f}% to an exact, strict match. "
                "Partial improvements are useful evidence but are not a strict win.")
    steps = []
    if any("pragma" in issue for issue in issues):
        steps.append("remove the prohibited compiler-control pragma(s) that currently cover it and recover "
                     "natural source that is still 100% without them")
    if any("inline asm" in issue for issue in issues):
        steps.append("replace the inline asm with natural C that is still 100%")
    if any("not linked" in issue for issue in issues):
        steps.append("make its owning object linkable: it sits in an incomplete CodeCandidate object, so the object "
                     "must become fully exact and strict before it can be admitted as Matching. You may change only "
                     "that object's status in configure.py for the promotion; if promotion needs split, symbol, or "
                     "other object changes, stop and report exactly what is required instead of editing them")
    return (f"`{target['symbol']}` already reports 100%, but it is not strictly accepted: {'; '.join(issues)}. "
            f"Your task: {'; then '.join(steps)}.")


def prompt(target: dict[str, Any], build_jobs: int) -> str:
    return f"""You are a bounded Pokémon Colosseum GC6E01 decompilation agent working on the
native recomp's boot frontier.

The separate native recomp cannot boot until `{target['symbol']}` is strictly
accepted here. It belongs to boot blocker #{target['blocker']} ({target['blocker_title']}).
Authoritative report: {target['fuzzy']:.5f}% in unit `{target['unit']}`, source
`{target['source']}`, {target['size']} bytes.

{goal(target)}

You are working in an isolated Git worktree. The main checkout runs a local
Ollama campaign independently. Do not access, edit, or rely on that checkout.
Do not push, merge, rebase, reset, delete a worktree, or use an assembly
fallback.

Read `AGENTS.md` and `docs/CAMPAIGN_OPERATIONS.md` before editing. Edit only the
source file that defines `{target['symbol']}` (candidate wrappers may `#include`
it) plus the single configure.py status change allowed above. Do not edit
headers, symbols, splits, `.inc` files, generated products, or unrelated
functions. The project bans inline asm, included asm, compiler-control pragmas,
fake volatility, dummy assignments, compiler-shaping aliases, and invented
helpers. Do not replace one semantic mistake with a match-shaped expression.

Use the project toolchain and objdiff to measure each change. Before committing,
run the standard report and link checks:

    python3 configure.py --no-progress
    ninja -j {build_jobs} all_source build/GC6E01/report.json
    ninja -j {build_jobs}
    python3 configure.py progress

Keep every Ninja invocation at `-j {build_jobs}`; this machine is shared.

Only if the result is strict and every applicable gate passes, set the Git
author to `Codex <codex@users.noreply.github.com>` and commit only the files
allowed above to this lane's branch. Never stage generated files. If no strict
result survives, leave the branch uncommitted and report the best measurement,
the remaining difference, and why it cannot yet be promoted.
"""


def codex_binary() -> str | None:
    """CODEX_BIN, PATH, then the newest binary bundled with the VS Code extension."""
    if agents.CODEx:
        return agents.CODEx
    bundled = sorted(Path.home().glob(".vscode/extensions/openai.chatgpt-*/bin/*/codex"),
                     key=lambda path: path.stat().st_mtime)
    return str(bundled[-1]) if bundled else None


def launch(target: dict[str, Any], args: argparse.Namespace) -> dict[str, Any]:
    codex = codex_binary()
    if not codex:
        raise RuntimeError("codex executable not found; set CODEX_BIN")
    stamp = datetime.now(UTC).strftime("%Y%m%d%H%M%S")
    slug = re.sub(r"[^a-z0-9]+", "-", target["symbol"].lower()).strip("-")
    lane_id = f"boot-{slug}-{stamp}"
    branch = f"codex/boot-{slug}-{stamp}"
    worktree = WORKTREE_ROOT / f"pkmn-colosseum-{lane_id}"
    base = subprocess.run(["git", "rev-parse", "HEAD"], cwd=ROOT, check=True, capture_output=True, text=True).stdout.strip()
    subprocess.run([sys.executable, str(ROOT / "tools" / "setup_worker_worktree.py"), str(worktree),
                    "--branch", branch, "--base", base], cwd=ROOT, check=True, capture_output=True, text=True)
    agents.checked_worktree(worktree)
    lane_dir = STATE_DIR / "agents" / lane_id
    lane_dir.mkdir(parents=True, exist_ok=True)
    log_path, message_path = lane_dir / "agent.jsonl", lane_dir / "last-message.md"
    command = [codex, "exec", "--ephemeral", "--json", "--color", "never", "--cd", str(worktree),
               "--approve-for-me", "--output-last-message", str(message_path)]
    if args.model:
        command += ["--model", args.model]
    command.append(prompt(target, args.build_jobs))
    if args.niceness and shutil.which("nice"):
        command = [shutil.which("nice"), "-n", str(args.niceness), *command]
    with log_path.open("ab") as log:
        process = subprocess.Popen(command, cwd=worktree, stdin=subprocess.DEVNULL, stdout=log,
                                   stderr=subprocess.STDOUT, start_new_session=True)
    return COORDINATOR.register_agent({
        "id": lane_id, "worker": "Codex boot agent", "status": "running", "pid": process.pid,
        "source": target["source"], "symbols": [target["symbol"]], "branch": branch, "base": base,
        "worktree": str(worktree), "log": str(log_path), "last_message": str(message_path),
        "started_at": now(), "token": uuid.uuid4().hex, "boot_blocker": target["blocker"],
        "detail": f"Boot blocker #{target['blocker']}: {target['status'].replace('-', ' ')} (Ninja -j{args.build_jobs}).",
    })


def lane_failure(lane: dict[str, Any]) -> tuple[str, datetime | None] | None:
    """Classify a lane whose Codex turn failed: ("quota", reset time) or ("failed", None)."""
    log = Path(lane.get("log") or "")
    text = log.read_text(encoding="utf-8", errors="replace") if log.is_file() else ""
    if '"turn.failed"' not in text:
        return None
    if "usage limit" not in text.lower():
        return "failed", None
    match = RETRY_AT.search(text)
    try:
        until = datetime.strptime("".join(match.groups()), "%b %d, %Y %I:%M %p").astimezone(UTC) if match else None
    except ValueError:
        until = None
    return "quota", until or datetime.now(UTC) + timedelta(hours=1)


def discard(lane: dict[str, Any]) -> None:
    """Remove a lane that never ran: `worktree remove` refuses dirty trees and `branch -d` unmerged ones."""
    worktree, branch = lane.get("worktree"), lane.get("branch")
    if worktree and Path(worktree).exists():
        subprocess.run(["git", "worktree", "remove", worktree], cwd=ROOT, capture_output=True)
    if branch and not (worktree and Path(worktree).exists()):
        subprocess.run(["git", "branch", "-d", branch], cwd=ROOT, capture_output=True)


def reap_failures(loop: dict[str, Any], lanes: dict[str, Any]) -> None:
    attempts = loop.setdefault("attempts", {})
    for symbol, attempt in list(attempts.items()):
        lane = lanes.get(attempt["lane"])
        if not lane or lane.get("status") == "failed":
            continue
        failure = lane_failure(lane)
        if not failure:
            continue
        kind, until = failure
        if lane.get("pid"):
            try:
                os.killpg(lane["pid"], signal.SIGTERM)
            except (ProcessLookupError, PermissionError):
                pass
        if kind == "quota":
            paused = loop.get("agents_paused_until")
            loop["agents_paused_until"] = max(filter(None, [paused, until.isoformat(timespec="seconds")]))
            loop["agents_paused_reason"] = "Codex usage limit"
            discard(lane)
            del attempts[symbol]  # never ran; retry as soon as the quota resets
            detail = f"Codex usage limit; lane discarded, retry after {loop['agents_paused_until']}."
        else:
            attempt["lane_status"] = "failed"
            detail = "Codex turn failed; see the lane log. Retried after the retry window."
        COORDINATOR.update_agent(lane["id"], status="failed", finished_at=now(), detail=detail)
        print(f"{now()} {lane['id']}: {detail}", flush=True)


def eligible(target: dict[str, Any], attempts: dict[str, Any], lanes: dict[str, Any], retry_after: timedelta) -> bool:
    """One lane per symbol until its source changes, its lane is rejected, or the retry window passes."""
    attempt = attempts.get(target["symbol"])
    if not attempt:
        return True
    lane = lanes.get(attempt["lane"], {})
    if lane.get("status") in {"running", "review_ready", "accepted"}:
        return False  # in progress, or waiting at the human gate / for the report to catch up
    if attempt.get("source_digest") != source_digest(target["source"]):
        return True
    launched = datetime.fromisoformat(attempt["launched_at"])
    return lane.get("status") != "rejected" and datetime.now(UTC) - launched > retry_after


def tick(args: argparse.Namespace, loop: dict[str, Any]) -> None:
    status = recomp_status(ROOT, REPORT_FILE)
    loop["updated_at"] = now()
    if not status.get("available"):
        loop["last_error"] = status.get("error")
        return
    loop["last_error"] = None
    blockers, targets = frontier(status, args.depth)
    current = status["current"]
    write_json(FOCUS_FILE, {"generated_at": now(), "current": current and current["index"], "targets": targets})
    snapshot = {"current": current and current["index"], "title": current and current["title"],
                "status": current and current["status"],
                "accepted": status["summary"]["accepted_functions"], "functions": status["summary"]["functions"]}
    previous = loop.get("frontier")
    if previous != snapshot:
        loop.setdefault("history", []).append({"at": now(), "from": previous, "to": snapshot})
        loop["history"] = loop["history"][-50:]
        print(f"{now()} frontier: {previous} -> {snapshot}", flush=True)
    loop["frontier"] = snapshot
    loop["focus"] = {"blockers": [row["index"] for row in blockers], "targets": len(targets)}

    lanes = COORDINATOR.snapshot().get("agents", {})
    reap_failures(loop, lanes)
    lanes = COORDINATOR.snapshot().get("agents", {})
    attempts = loop.setdefault("attempts", {})
    for attempt in attempts.values():
        attempt["lane_status"] = lanes.get(attempt["lane"], {}).get("status", "unknown")
    running = [lane for lane in lanes.values() if lane.get("worker") == "Codex boot agent" and lane.get("status") == "running"]
    if args.max_agents <= len(running):
        return
    paused = loop.get("agents_paused_until")
    if paused and datetime.now(UTC) < datetime.fromisoformat(paused):
        return
    loop.pop("agents_paused_until", None)
    loop.pop("agents_paused_reason", None)
    retry_after = timedelta(hours=args.retry_hours)
    busy = {lane.get("source") for lane in lanes.values() if lane.get("status") == "running"}
    for target in targets:
        if len(running) >= args.max_agents:
            break
        if target["source"] in busy or not eligible(target, attempts, lanes, retry_after):
            continue
        if args.dry_run:
            print(f"{now()} would launch lane for {target['symbol']} (blocker #{target['blocker']})", flush=True)
            break
        lane = launch(target, args)
        attempts[target["symbol"]] = {"lane": lane["id"], "launched_at": now(), "branch": lane["branch"],
                                      "source_digest": source_digest(target["source"]), "lane_status": "running"}
        running.append(lane)
        busy.add(target["source"])
        print(f"{now()} launched {lane['id']} for {target['symbol']} (blocker #{target['blocker']})", flush=True)


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--interval", type=int, default=60, help="seconds between frontier checks")
    parser.add_argument("--depth", type=int, default=3, help="actionable blockers included in the focus")
    parser.add_argument("--max-agents", type=int, default=1, help="concurrent Codex boot lanes (0 disables lanes)")
    parser.add_argument("--retry-hours", type=float, default=24, help="retry an unsuccessful, unrejected symbol after this long")
    parser.add_argument("--build-jobs", type=int, default=agents.DEFAULT_BUILD_JOBS)
    parser.add_argument("--niceness", type=int, default=agents.DEFAULT_NICENESS)
    parser.add_argument("--model", help="optional Codex model override")
    parser.add_argument("--once", action="store_true", help="run a single tick and exit")
    parser.add_argument("--dry-run", action="store_true", help="write the focus file but never launch lanes")
    args = parser.parse_args()

    STATE_DIR.mkdir(parents=True, exist_ok=True)
    lock = LOCK_FILE.open("a+")
    try:
        fcntl.flock(lock, fcntl.LOCK_EX | fcntl.LOCK_NB)
    except BlockingIOError:
        print("boot loop already running", file=sys.stderr)
        return 1

    def stop(signum, frame):
        global STOP
        STOP = True
    signal.signal(signal.SIGTERM, stop)
    signal.signal(signal.SIGINT, stop)

    loop = read_json(LOOP_FILE, {})
    loop.update(pid=os.getpid(), started_at=now(), running=True, settings={
        key: getattr(args, key) for key in ("interval", "depth", "max_agents", "retry_hours", "build_jobs", "dry_run")})
    while True:
        try:
            tick(args, loop)
        except Exception as error:  # keep watching; the next tick may succeed
            loop["last_error"] = f"{type(error).__name__}: {error}"
            print(f"{now()} error: {loop['last_error']}", flush=True)
        write_json(LOOP_FILE, loop)
        if args.once or STOP:
            break
        deadline = time.monotonic() + args.interval
        while not STOP and time.monotonic() < deadline:
            time.sleep(1)
        if STOP:
            break
    loop["running"] = False
    write_json(LOOP_FILE, loop)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
