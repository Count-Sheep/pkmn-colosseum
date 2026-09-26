#!/usr/bin/env python3
"""Launch bounded Codex review agents for completed local-model attempts.

Each lane must use an isolated Git worktree. The main checkout remains reserved
for the Ollama campaign, so agents can inspect and build in parallel without
sharing mutable source or object files. Agent branches are review-only: this
tool never cherry-picks, pushes, or merges their work.
"""

from __future__ import annotations

import argparse
import json
import os
import re
import shutil
import subprocess
import sys
import uuid
from pathlib import Path
from typing import Any

from local_campaign_coordination import Coordinator, now


ROOT = Path(__file__).resolve().parents[1]
STATE_DIR = ROOT / "build" / "local_llm_campaign"
STATE_FILE = STATE_DIR / "state.json"
COORDINATOR = Coordinator(STATE_DIR)
CODEx = os.environ.get("CODEX_BIN") or shutil.which("codex")
DEFAULT_BUILD_JOBS = int(os.environ.get("LOCAL_CAMPAIGN_AGENT_JOBS", "1"))
DEFAULT_NICENESS = int(os.environ.get("LOCAL_CAMPAIGN_AGENT_NICENESS", "10"))


def read_state() -> dict[str, Any]:
    if not STATE_FILE.exists():
        raise RuntimeError("local campaign state is missing; run local_campaign.py sync first")
    return json.loads(STATE_FILE.read_text(encoding="utf-8"))


def resolve_tasks(state: dict[str, Any], selectors: list[str]) -> list[dict[str, Any]]:
    selected: list[dict[str, Any]] = []
    for selector in selectors:
        matches = [
            item for item in state["items"].values()
            if selector in (item.get("id"), item.get("symbol"))
        ]
        if len(matches) != 1:
            raise RuntimeError(f"{selector!r} must identify exactly one queue entry")
        item = matches[0]
        if not item.get("owner_source"):
            raise RuntimeError(f"{selector!r} has no editable source owner")
        selected.append(item)
    owners = {item["owner_source"] for item in selected}
    if len(owners) != 1:
        raise RuntimeError("one agent lane must cover one source owner; use separate lanes")
    return selected


def checked_worktree(path: Path) -> None:
    if not (path / ".git").exists():
        raise RuntimeError(f"not a Git worktree: {path}")
    original = path / "orig" / "GC6E01" / "sys" / "main.dol"
    if not original.is_file():
        raise RuntimeError(f"missing original DOL in worktree: {original}")


def task_evidence(item: dict[str, Any]) -> str:
    report = item.get("last_report") or {}
    measurement = "no retained measurement"
    if report.get("pct") is not None:
        measurement = f"candidate {report['pct']:.5f}% ({report.get('deltas', '?')} displayed deltas)"
    error = item.get("last_error")
    return (
        f"- `{item['symbol']}` in `{item['unit']}`: baseline {item.get('base_pct', 0):.5f}%; "
        f"{measurement}" + (f"; prior result: {error}" if error else "")
    )


def prompt(items: list[dict[str, Any]], build_jobs: int) -> str:
    owner = items[0]["owner_source"]
    symbols = ", ".join(f"`{item['symbol']}`" for item in items)
    evidence = "\n".join(task_evidence(item) for item in items)
    return f"""You are a bounded Pokémon Colosseum GC6E01 decompilation scout.

You are working in an isolated Git worktree. The main checkout has a local
Ollama campaign running independently. Do not access, edit, or rely on that
checkout. Do not push, merge, rebase, reset, delete a worktree, or use an
assembly fallback.

Read `AGENTS.md` and `docs/CAMPAIGN_OPERATIONS.md` before editing. Your only
editable owner is `{owner}`. Work only on {symbols}. Do not edit headers,
`configure.py`, symbols, splits, `.inc` files, generated products, or unrelated
functions. The local model has already tried these functions:

{evidence}

Treat its output as evidence, not authority. Recover natural, source-faithful
C89 from the target assembly and known callsite/data evidence. The project bans
inline asm, included asm, new pragmas, fake volatility, dummy assignments,
compiler-shaping aliases, and invented helpers. Do not replace one semantic
mistake with a match-shaped expression.

Use the project toolchain and objdiff to measure each change. If a function
becomes exact, verify the owning object and run the standard report/link checks:

    python3 configure.py --no-progress
    ninja -j {build_jobs} all_source build/GC6E01/report.json
    ninja -j {build_jobs}
    python3 configure.py progress

This fleet shares an interactive machine with the campaign dashboard. Keep
every Ninja invocation at `-j {build_jobs}` so parallel review lanes remain
responsive and do not crowd out the remote-model workflow.

Only if the result is strict and every applicable gate passes, set the Git
author to `Codex <codex@users.noreply.github.com>` and commit only the owner
source file to this lane's branch. Never stage generated files. If no strict
match survives, leave the branch uncommitted and report the best measurement,
the remaining target difference, and the reason it cannot yet be promoted.
"""


def launch(args: argparse.Namespace) -> int:
    if not CODEx:
        raise RuntimeError("codex executable is not on PATH; set CODEX_BIN")
    worktree = Path(args.worktree).resolve()
    checked_worktree(worktree)
    items = resolve_tasks(read_state(), args.task)
    lane_id = args.lane or re.sub(r"[^a-z0-9-]+", "-", items[0]["owner_source"].lower()).strip("-")
    lane_dir = STATE_DIR / "agents" / lane_id
    lane_dir.mkdir(parents=True, exist_ok=True)
    log_path = lane_dir / "agent.jsonl"
    message_path = lane_dir / "last-message.md"
    command = [
        CODEx, "exec", "--ephemeral", "--json", "--color", "never",
        "--cd", str(worktree), "--approve-for-me",
        "--output-last-message", str(message_path),
    ]
    if args.model:
        command.extend(["--model", args.model])
    command.append(prompt(items, args.build_jobs))
    if args.niceness:
        nice = shutil.which("nice")
        if not nice:
            raise RuntimeError("nice executable is unavailable; pass --niceness 0 to opt out")
        command = [nice, "-n", str(args.niceness), *command]
    with log_path.open("ab") as log:
        process = subprocess.Popen(
            command, cwd=worktree, stdin=subprocess.DEVNULL, stdout=log,
            stderr=subprocess.STDOUT, start_new_session=True,
        )
    lane = COORDINATOR.register_agent({
        "id": lane_id,
        "worker": "Codex agent",
        "status": "running",
        "pid": process.pid,
        "source": items[0]["owner_source"],
        "symbols": [item["symbol"] for item in items],
        "branch": args.branch,
        "worktree": str(worktree),
        "log": str(log_path),
        "last_message": str(message_path),
        "started_at": now(),
        "detail": f"Recovering and measuring local-model follow-up work (Ninja -j{args.build_jobs}).",
        "token": uuid.uuid4().hex,
    })
    print(json.dumps(lane, indent=2))
    return 0


def show(_: argparse.Namespace) -> int:
    print(json.dumps(COORDINATOR.snapshot().get("agents", {}), indent=2))
    return 0


def review(args: argparse.Namespace) -> int:
    lane = COORDINATOR.snapshot().get("agents", {}).get(args.lane)
    if lane is None:
        raise RuntimeError(f"unknown agent lane: {args.lane}")
    if lane.get("status") == "running":
        raise RuntimeError("cannot review an agent lane that is still running")
    changes: dict[str, Any] = {
        "status": args.outcome,
        "detail": args.detail,
        "reviewed_at": now(),
    }
    if args.commit:
        changes["commit"] = args.commit
    COORDINATOR.update_agent(args.lane, **changes)
    print(json.dumps(COORDINATOR.snapshot()["agents"][args.lane], indent=2))
    return 0


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    commands = parser.add_subparsers(dest="command", required=True)
    launch_parser = commands.add_parser("launch", help="start one isolated Codex review lane")
    launch_parser.add_argument("--lane", help="stable dashboard ID for this lane")
    launch_parser.add_argument("--worktree", required=True)
    launch_parser.add_argument("--branch", required=True)
    launch_parser.add_argument("--task", action="append", required=True, help="queue ID or unambiguous symbol")
    launch_parser.add_argument("--model", help="optional Codex model override")
    launch_parser.add_argument("--build-jobs", type=int, default=DEFAULT_BUILD_JOBS,
                               help=f"maximum Ninja jobs per review lane (default: {DEFAULT_BUILD_JOBS})")
    launch_parser.add_argument("--niceness", type=int, default=DEFAULT_NICENESS,
                               help=f"positive scheduling niceness for review lanes (default: {DEFAULT_NICENESS})")
    commands.add_parser("status", help="show dashboard agent lanes")
    review_parser = commands.add_parser("review", help="record the outcome of a completed review lane")
    review_parser.add_argument("--lane", required=True)
    review_parser.add_argument("--outcome", choices=("accepted", "rejected"), required=True)
    review_parser.add_argument("--detail", required=True, help="concise review conclusion shown in the dashboard")
    review_parser.add_argument("--commit", help="strict survivor commit hash, only for an accepted lane")
    args = parser.parse_args()
    if args.command == "launch":
        if args.build_jobs < 1:
            parser.error("--build-jobs must be at least 1")
        if args.niceness < 0:
            parser.error("--niceness cannot be negative")
        return launch(args)
    if args.command == "review":
        return review(args)
    return show(args)


if __name__ == "__main__":
    raise SystemExit(main())
