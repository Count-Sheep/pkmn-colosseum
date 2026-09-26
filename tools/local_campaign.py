#!/usr/bin/env python3
"""Local-LLM queue runner for strict, review-only decompilation proposals.

The runner asks Ollama for a single replacement function and retains it in
ignored campaign state. Verification temporarily substitutes source under the
shared build lock and restores it afterward. A 100% result remains a review
candidate; the runner never promotes a source change.
"""

from __future__ import annotations

import argparse
import fcntl
import hashlib
import json
import os
import re
import signal
import subprocess
import sys
import tempfile
import time
import urllib.error
import urllib.request
from collections import Counter
from datetime import UTC, datetime
from pathlib import Path
from typing import Any

from local_campaign_coordination import ClaimConflict, Coordinator
from local_campaign_priority import rank, reference_graph
from local_campaign_recomp import boot_index, recomp_status

ROOT = Path(__file__).resolve().parents[1]
STATE_DIR = ROOT / "build" / "local_llm_campaign"
STATE_FILE = STATE_DIR / "state.json"
REPORT_FILE = ROOT / "build" / "GC6E01" / "report.json"
BRIEF_TOOL = ROOT / "tools" / "decomp_work" / "handoff" / "gen_brief.py"
VERIFY_TOOL = ROOT / "tools" / "decomp_work" / "handoff" / "verify.py"
DEFAULT_HOST = "http://dreamworld:11434"
DEFAULT_MODEL = "qwen2.5-coder:14b"
DEFAULT_NUM_PREDICT = int(os.environ.get("OLLAMA_NUM_PREDICT", "4096"))
DEFAULT_NUM_CTX = int(os.environ.get("OLLAMA_NUM_CTX", "32768"))
COORDINATOR = Coordinator(STATE_DIR)
MODEL_WORKER = "Local LLM"
DEFAULT_WORKER = os.environ.get("LOCAL_CAMPAIGN_WORKER", MODEL_WORKER)
RUNNER_LOCKS: dict[str, Any] = {}
STOP_REQUESTED = False
NUM_CTX = DEFAULT_NUM_CTX
HOST_BACKOFF = (30, 600)


class StaleSource(RuntimeError):
    pass


class HostUnavailable(RuntimeError):
    """The Ollama host could not be reached; the task was not attempted."""


class RepeatedResponse(RuntimeError):
    pass


class IncompleteResponse(RuntimeError):
    pass


class RestorationError(RuntimeError):
    pass


def check_baseline(item: dict[str, Any]) -> None:
    path = ROOT / item.get("owner_source", item["source"])
    if digest(path.read_text(encoding="utf-8", errors="replace")) != item["source_sha256"]:
        raise StaleSource("source changed since this task was queued; run sync")


def timestamp() -> str:
    return datetime.now(UTC).isoformat(timespec="microseconds")


def digest(value: str) -> str:
    return hashlib.sha256(value.encode("utf-8", errors="replace")).hexdigest()


def read_json(path: Path, default: Any) -> Any:
    return json.loads(path.read_text(encoding="utf-8")) if path.exists() else default


def write_json(path: Path, value: Any) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    temporary = path.with_suffix(path.suffix + ".tmp")
    temporary.write_text(json.dumps(value, indent=2, sort_keys=True) + "\n", encoding="utf-8")
    temporary.replace(path)


def blank_state(host: str, model: str, num_predict: int = DEFAULT_NUM_PREDICT) -> dict[str, Any]:
    return {
        "schema": 1,
        "created_at": timestamp(),
        "settings": {
            "ollama_host": host.rstrip("/"),
            "ollama_model": model,
            "ollama_num_predict": num_predict,
        },
        "workers": {},
        "items": {},
        "events": [],
        "snapshots": [],
    }


def load_state(host: str, model: str, num_predict: int = DEFAULT_NUM_PREDICT, *, update_settings: bool = True) -> dict[str, Any]:
    state = read_json(STATE_FILE, blank_state(host, model, num_predict))
    if state.get("schema") != 1:
        raise RuntimeError(f"unsupported state schema: {STATE_FILE}")
    state.setdefault("items", {})
    state.setdefault("events", [])
    state.setdefault("snapshots", [])
    state.setdefault("settings", {})
    state.setdefault("workers", {})
    defaults = {
        "ollama_host": host.rstrip("/"),
        "ollama_model": model,
        "ollama_num_predict": num_predict,
    }
    for key, value in defaults.items():
        state["settings"].setdefault(key, value)
    if update_settings:
        state["settings"].update(defaults)
    return state


def event_key(row: dict[str, Any]) -> str:
    return json.dumps(row, sort_keys=True, separators=(",", ":"))


def item_stamp(item: dict[str, Any]) -> str:
    activity_row = item.get("activity") or {}
    return max(str(item.get("updated_at", "")), str(activity_row.get("updated_at", "")), str(item.get("last_attempt_at", "")))


def save_state(state: dict[str, Any]) -> None:
    state["updated_at"] = timestamp()
    with COORDINATOR.state_file():
        current = read_json(STATE_FILE, None)
        replace_items = bool(state.pop("_replace_items", False))
        if not isinstance(current, dict) or current.get("schema") != 1:
            merged = state
        else:
            merged = current
            merged.setdefault("settings", {}).update(state.get("settings", {}))
            for name, worker in state.get("workers", {}).items():
                old_worker = merged.setdefault("workers", {}).get(name, {})
                if worker.get("updated_at", "") >= old_worker.get("updated_at", ""):
                    merged["workers"][name] = worker
            if replace_items:
                merged["items"] = state.get("items", {})
                merged["priority_analysis"] = state.get("priority_analysis", {})
            else:
                merged.setdefault("items", {})
                for task, item in state.get("items", {}).items():
                    old = merged["items"].get(task)
                    if old is None or item_stamp(item) > item_stamp(old):
                        merged["items"][task] = item
            existing = {event_key(row) for row in merged.setdefault("events", [])}
            for row in state.get("events", []):
                key = event_key(row)
                if key not in existing:
                    merged["events"].append(row)
                    existing.add(key)
            existing_snapshots = {event_key(row) for row in merged.setdefault("snapshots", [])}
            for row in state.get("snapshots", []):
                key = event_key(row)
                if key not in existing_snapshots:
                    merged["snapshots"].append(row)
                    existing_snapshots.add(key)
            merged["updated_at"] = state["updated_at"]
        merged["events"] = merged.get("events", [])[-500:]
        merged["snapshots"] = merged.get("snapshots", [])[-500:]
        merged.pop("_replace_items", None)
        write_json(STATE_FILE, merged)


def event(state: dict[str, Any], kind: str, **details: Any) -> None:
    state["events"].append({"at": timestamp(), "kind": kind, **details})


def activity(state: dict[str, Any], item: dict[str, Any], phase: str, detail: str, **metrics: Any) -> None:
    """Persist the exact worker phase so the dashboard survives a refresh."""
    previous = item.get("activity") or {}
    item["activity"] = {
        **(previous if previous.get("phase") == phase else {}),
        "started_at": previous.get("started_at") or timestamp(),
        "updated_at": timestamp(),
        "phase": phase,
        "detail": detail,
        **metrics,
    }
    item["updated_at"] = item["activity"]["updated_at"]
    save_state(state)


def functions_in_report(report: dict[str, Any]):
    for unit in report.get("units", []):
        functions = list(unit.get("functions") or [])
        for section in unit.get("sections") or []:
            functions.extend(section.get("functions") or [])
        for function in functions:
            yield unit, function


def identifier(source: str, unit: str, symbol: str) -> str:
    hash_part = hashlib.sha256(f"{source}\0{unit}\0{symbol}".encode()).hexdigest()[:16]
    clean = re.sub(r"[^A-Za-z0-9_.-]+", "_", symbol)[:48]
    return f"{clean}-{hash_part}"


def category(unit: dict[str, Any]) -> str:
    categories = (unit.get("metadata") or {}).get("progress_categories") or []
    return categories[0] if categories else "other"


def owner_source(source: str) -> str:
    """Resolve chained score shims without reading or modifying assembly."""
    visited = set()
    while source not in visited:
        visited.add(source)
        path = ROOT / source
        if not path.is_file():
            return source
        includes = re.findall(r'^\s*#\s*include\s+"(src/[^"\n]+\.c)"', path.read_text(encoding="utf-8", errors="replace"), flags=re.MULTILINE)
        if len(includes) != 1 or not (ROOT / includes[0]).is_file():
            return source
        source = includes[0]
    raise ValueError(f"cyclic source-owner includes: {source}")


def sync(state: dict[str, Any], reset: bool = False) -> dict[str, int]:
    refreshed = subprocess.run(
        ["ninja", "-j2", "all_source", "build/GC6E01/report.json"],
        cwd=ROOT, capture_output=True, text=True, timeout=1800,
    )
    if refreshed.returncode:
        raise RuntimeError("authoritative report rebuild failed: " + (refreshed.stderr or refreshed.stdout)[-1600:])
    if not REPORT_FILE.exists():
        raise RuntimeError("report target completed without creating report.json")
    report = read_json(REPORT_FILE, {})
    grouped = list(functions_in_report(report))
    sources, hashes, functions = {}, {}, {}
    for unit, function in grouped:
        metadata = unit.get("metadata") or {}
        source = metadata.get("source_path")
        owner = None
        if source and not metadata.get("auto_generated"):
            if source not in sources:
                sources[source] = owner_source(source)
            owner = sources[source]
            if owner not in hashes and (ROOT / owner).is_file():
                hashes[owner] = digest((ROOT / owner).read_text(encoding="utf-8", errors="replace"))
        functions[(unit["name"], function["name"])] = {
            "unit": unit["name"], "symbol": function["name"],
            "owner_source": owner if owner in hashes else None,
            "source_sha256": hashes.get(owner),
            "base_pct": float(function.get("fuzzy_match_percent") or 0),
            "size": int(function.get("size") or 0),
        }
    graph, covered, missing = reference_graph(ROOT, read_json(ROOT / "objdiff.json", {}).get("units", []), functions)
    priorities = rank(functions, graph, covered)
    state["priority_analysis"] = {
        "version": 1, "at": timestamp(), "report_sha256": digest(REPORT_FILE.read_text()),
        "covered_functions": len(covered), "total_functions": len(functions),
        "reference_edges": sum(len(targets) for targets in graph.values()), "missing_units": missing,
    }
    unit_totals = Counter(unit["name"] for unit, _ in grouped)
    unit_exacts = Counter(
        unit["name"] for unit, function in grouped
        if float(function.get("fuzzy_match_percent") or 0) >= 100.0
    )
    unit_stats = {
        name: {"total": unit_totals[name], "exact": unit_exacts[name]}
        for name in unit_totals
    }
    residuals = Counter(
        unit["name"] for unit, function in grouped
        if float(function.get("fuzzy_match_percent") or 0) < 100.0
    )
    owner_residuals = Counter()
    for unit, function in grouped:
        if float(function.get("fuzzy_match_percent") or 0) >= 100.0:
            continue
        metadata = unit.get("metadata") or {}
        source = metadata.get("source_path")
        if source and not metadata.get("auto_generated"):
            owner_residuals[sources[source]] += 1
    claimed_sources = set((COORDINATOR.snapshot().get("claims") or {}).keys())
    refreshed: dict[str, dict[str, Any]] = {}
    for unit, function in grouped:
        pct = float(function.get("fuzzy_match_percent") or 0)
        if pct >= 100.0:
            continue
        metadata = unit.get("metadata") or {}
        source = metadata.get("source_path")
        symbol = function["name"]
        task_source = source or "[no declared source]"
        task = identifier(task_source, unit["name"], symbol)
        if not source or metadata.get("auto_generated"):
            refreshed[task] = {
                "id": task, "source": task_source, "unit": unit["name"], "symbol": symbol,
                "category": category(unit), "size": int(function.get("size") or 0), "base_pct": pct,
                "residual_functions": residuals[unit["name"]], "status": "blocked_no_editable_source",
                "attempts": 0, "updated_at": timestamp(),
                "last_error": "no source-backed owner is declared in the current object map",
            }
            continue
        owner = sources[source]
        owner_path = ROOT / owner
        if not owner_path.is_file():
            refreshed[task] = {
                "id": task, "source": source, "unit": unit["name"], "symbol": symbol,
                "category": category(unit), "size": int(function.get("size") or 0), "base_pct": pct,
                "residual_functions": residuals[unit["name"]], "status": "blocked_no_editable_source",
                "attempts": 0, "updated_at": timestamp(),
                "last_error": "declared source file is absent from this checkout",
            }
            continue
        record = {
            "id": task,
            "source": source,
            "unit": unit["name"],
            "symbol": symbol,
            "category": category(unit),
            "owner_source": owner,
            "size": int(function.get("size") or 0),
            "base_pct": pct,
            "residual_functions": residuals[unit["name"]],
            "exact_siblings": unit_stats.get(unit["name"], {}).get("exact", 0),
            "owner_residual_functions": owner_residuals.get(owner, 0),
            "source_sha256": hashes[owner],
            "updated_at": timestamp(),
        }
        record.update(priorities[(unit["name"], symbol)])
        old = state["items"].get(task)
        if old and not reset:
            for key in ("status", "attempts", "last_attempt_at", "last_report", "last_error", "candidate", "worker", "activity"):
                if key in old:
                    record[key] = old[key]
            if record.get("status") == "running" and owner not in claimed_sources:
                record["status"] = "pending"
                record["last_error"] = "recovered after interrupted runner"
            if old.get("source_sha256") != record["source_sha256"] and record.get("status") not in {"review_exact", "running"}:
                record["status"] = "stale_source"
        else:
            record.update({"status": "pending", "attempts": 0})
        try:
            start, end = function_span(owner_path.read_text(encoding="utf-8", errors="replace"), symbol)
            record["function_chars"] = end - start
            if record["status"] == "blocked_source_context":
                record["status"] = "pending"
        except ValueError as exc:
            record.update(status="blocked_source_context", last_error=str(exc))
        refreshed[task] = record
    removed = len(set(state["items"]) - set(refreshed))
    state["items"] = refreshed
    state["_replace_items"] = True
    counts = Counter(item["status"] for item in refreshed.values())
    measures = report.get("measures") or {}
    state["snapshots"].append({
        "at": timestamp(),
        "fuzzy_match_percent": measures.get("fuzzy_match_percent", 0),
        "matched_functions": measures.get("matched_functions", 0),
        "worklist": len(refreshed),
        "review_exact": counts["review_exact"],
        "attempted": sum(int(item.get("attempts", 0)) > 0 for item in refreshed.values()),
    })
    event(state, "queue_synced", pending=counts["pending"], removed=removed, report_rebuilt=True)
    save_state(state)
    return {"open": len(refreshed), "pending": counts["pending"], "removed": removed}


def source_mask(text: str) -> str:
    """Keep offsets while masking comments, literals, directives and dead #if 0.

    Unknown conditions retain both branches; ambiguous definitions fail closed.
    This is a source locator, not a replacement for the build preprocessor.
    """
    blank = lambda value: re.sub(r"[^\n]", " ", value)
    clean = re.sub(r'/\*.*?\*/|//[^\n]*|"(?:\\.|[^"\\])*"|\'(?:\\.|[^\'\\])*\'',
                   lambda match: blank(match.group()), text, flags=re.DOTALL)
    active, stack, lines = True, [], []
    for line in clean.splitlines(keepends=True):
        directive = re.match(r"\s*#\s*(\w+)\b(.*)", line)
        if directive:
            kind, expression = directive.groups()
            expression = expression.strip().strip("() ")
            value = expression == "1" if expression in {"0", "1"} else None
            if kind in {"if", "ifdef", "ifndef"}:
                stack.append((active, value))
                active = active and value is not False
            elif kind in {"else", "elif"} and stack:
                parent, taken = stack[-1]
                branch = (not taken) if kind == "else" and taken is not None else value
                active = parent and taken is not True and branch is not False
                stack[-1] = (parent, True if taken is True or branch is True else None if taken is None or branch is None else False)
            elif kind == "endif" and stack:
                active, _ = stack.pop()
            lines.append(blank(line))
        else:
            lines.append(line if active else blank(line))
    return "".join(lines)


def function_span(text: str, symbol: str) -> tuple[int, int]:
    """Find one C definition; never select a disabled reference-asm stub."""
    clean = source_mask(text)
    definitions = []
    pattern = re.compile(rf"\b{re.escape(symbol)}\s*\(")
    for match in pattern.finditer(clean):
        open_paren = clean.find("(", match.start())
        try:
            after_params = balanced_end(clean, open_paren, "(", ")")
        except ValueError:
            continue
        cursor = skip_space_and_comments(clean, after_params)
        if cursor >= len(clean) or clean[cursor] != "{":
            continue
        start = max(clean.rfind(delimiter, 0, match.start()) for delimiter in ";{}") + 1
        while start < match.start() and clean[start].isspace():
            start += 1
        prefix = clean[start:match.start()]
        if not prefix.strip() or re.search(r"\b(?:asm|__asm|__asm__|if|while|switch)\b", prefix):
            continue
        definitions.append((start, balanced_end(clean, cursor, "{", "}")))
    if len(definitions) == 1:
        return definitions[0]
    if definitions:
        raise ValueError(f"ambiguous C definitions: {symbol}")
    raise ValueError(f"function definition not found: {symbol}")


def skip_space_and_comments(text: str, cursor: int) -> int:
    while cursor < len(text):
        if text[cursor].isspace():
            cursor += 1
        elif text.startswith("/*", cursor):
            end = text.find("*/", cursor + 2)
            if end < 0:
                raise ValueError("unterminated comment")
            cursor = end + 2
        elif text.startswith("//", cursor):
            end = text.find("\n", cursor + 2)
            cursor = len(text) if end < 0 else end + 1
        else:
            break
    return cursor


def balanced_end(text: str, start: int, opening: str, closing: str) -> int:
    depth, cursor, quote = 0, start, ""
    while cursor < len(text):
        char = text[cursor]
        if quote:
            if char == "\\":
                cursor += 2
                continue
            if char == quote:
                quote = ""
        elif text.startswith("/*", cursor):
            end = text.find("*/", cursor + 2)
            if end < 0:
                raise ValueError("unterminated comment")
            cursor = end + 2
            continue
        elif text.startswith("//", cursor):
            end = text.find("\n", cursor + 2)
            cursor = len(text) if end < 0 else end + 1
            continue
        elif char in {'"', "'"}:
            quote = char
        elif char == opening:
            depth += 1
        elif char == closing:
            depth -= 1
            if depth == 0:
                return cursor + 1
        cursor += 1
    raise ValueError(f"unbalanced {opening}{closing}")


def function_signature(text: str, start: int, end: int) -> str:
    return " ".join(text[start:text.index("{", start, end)].split())


def extract_function(response: str, symbol: str) -> str:
    blocks = re.findall(r"```(?:c|C)?\s*\n(.*?)```", response, flags=re.DOTALL)
    for block in blocks or [response]:
        try:
            start, end = function_span(block, symbol)
        except ValueError:
            continue
        candidate = block[start:end].strip() + "\n"
        if re.search(r"#\s*include\b|\b(?:__asm__|__asm|asm)\b|\.inc\b", candidate, flags=re.IGNORECASE):
            raise ValueError("candidate contains forbidden assembly or include")
        return candidate
    raise ValueError("model response contains no complete target function")


def splice(base: str, symbol: str, proposal: str) -> str:
    base_start, base_end = function_span(base, symbol)
    proposal_start, proposal_end = function_span(proposal, symbol)
    if function_signature(base, base_start, base_end) != function_signature(proposal, proposal_start, proposal_end):
        raise ValueError("candidate changed the target function signature")
    return base[:base_start] + proposal[proposal_start:proposal_end].strip() + "\n" + base[base_end:]


def policy(base_function: str, proposal: str) -> tuple[list[str], list[str]]:
    """Hard-reject the explicit project bans; flag ambiguity for human audit."""
    clean = re.sub(r"/\*.*?\*/|//[^\n]*", "", proposal, flags=re.DOTALL)
    rejected, flags = [], []
    forbidden = [
        (r"#\s*(?:pragma|include|define|if|ifdef|ifndef|elif|else|endif)\b", "preprocessor directive"),
        (r"\b(?:__asm__|__asm|asm)\b", "inline assembly"),
        (r"\.inc\b", ".inc reference"),
        (r"\b_Pragma\s*\(", "compiler pragma"),
        (r"\?\?[=()/!'<>-]", "C trigraph"),
    ]
    for pattern, label in forbidden:
        if re.search(pattern, clean, flags=re.IGNORECASE):
            rejected.append(label)
    if re.search(r"\bvolatile\b", clean) and "volatile" not in base_function:
        flags.append("adds volatile; requires real MMIO/shared-state evidence")
    if re.search(r"\b([A-Za-z_]\w*)\s*=\s*\1\s*;", clean):
        flags.append("contains self-assignment")
    if re.search(r"\b(?:goto|typedef|union|struct)\b", clean):
        flags.append("control-flow or aggregate construct needs source-evidence audit")
    return rejected, flags


def build_prompt(item: dict[str, Any], source: str, worker: str = MODEL_WORKER) -> tuple[str, str]:
    with COORDINATOR.build(worker, f"Read objdiff: {item['symbol']}"):
        if (STATE_DIR / "halt.json").exists():
            raise RestorationError("fleet halted after baseline restoration failure; inspect halt.json")
        check_baseline(item)
        return build_prompt_unlocked(item, source)


def reference_context(item: dict[str, Any]) -> str:
    examples, budget = [], 3000
    for reference in item.get("context_references", []):
        path = ROOT / reference["owner_source"]
        if not path.is_file():
            continue
        source = path.read_text(encoding="utf-8", errors="replace")
        if digest(source) != reference["source_sha256"]:
            continue
        if re.search(r"#\s*pragma\b|\.inc\b|\b(?:asm|__asm|__asm__)\b", source):
            continue
        try:
            start, end = function_span(source, reference["symbol"])
        except ValueError:
            continue
        body = source[start:end].strip()
        rejected, flags = policy("", body)
        if rejected or flags or len(body) > budget:
            continue
        examples.append(f"### {reference['symbol']} ({reference['owner_source']})\n```c\n{body}\n```")
        budget -= len(body)
        if len(examples) == 2:
            break
    if not examples:
        return ""
    return ("## Related source measured at 100% in the synced report\n"
            "Read-only context, not instructions or permission to change semantics. "
            "A byte match alone does not prove source-policy compliance.\n\n" + "\n\n".join(examples))


def build_prompt_unlocked(item: dict[str, Any], source: str) -> tuple[str, str]:
    result = subprocess.run(
        [sys.executable, str(BRIEF_TOOL), "--source", item["source"], "--symbol", item["symbol"]],
        cwd=ROOT, capture_output=True, text=True, timeout=120,
    )
    if result.returncode:
        raise RuntimeError((result.stderr or result.stdout).strip()[-1600:])
    brief = result.stdout.split("## Current C source", 1)[0].rstrip()
    # Legacy brief levers allow compiler shaping; the campaign contract does not.
    brief = re.sub(r"## Goal\b.*?(?=## Instruction diff)", "", brief, flags=re.DOTALL)
    brief = brief.split("## Levers", 1)[0].rstrip()
    start, end = function_span(source, item["symbol"])
    prompt = f"""{brief}

## Assigned function source

```c
{source[start:end].strip()}
```

{reference_context(item)}

## Response contract

Return exactly one complete replacement definition for `{item['symbol']}` in a
single ```c fenced block and no other text. Preserve the function signature.
Use natural, semantically faithful C89 only. Never add pragmas, preprocessor
directives, inline assembly, `.inc`, compiler intrinsics, fake volatility,
artificial aliases or layouts, dummy/self assignments, uninitialized reads, or
code whose only purpose is compiler shaping. Do not change headers, other
functions, or compiler flags. An exact result still undergoes human audit.
"""
    return brief, prompt


def repeated_tail(answer: str) -> bool:
    """Detect degenerate decoding without mistaking ordinary short C for a loop."""
    compact = re.sub(r"\s+", " ", answer[-4096:]).strip()
    if len(compact) < 768:
        return False
    for width in range(24, min(256, len(compact) // 4) + 1):
        tail = compact[-width:]
        if compact.endswith(tail * 4):
            return True
    return False


def context_window(prompt: str, num_predict: int) -> int:
    """Use the worker's context when the prompt fits; never let Ollama truncate a large prompt."""
    needed = len(prompt) // 3 + num_predict + 512
    return NUM_CTX if needed <= NUM_CTX else max(NUM_CTX, DEFAULT_NUM_CTX)


def ollama(host: str, model: str, prompt: str, timeout: int, num_predict: int, progress, response_path: Path | None = None) -> str:
    request = urllib.request.Request(
        host.rstrip("/") + "/api/generate",
        data=json.dumps({
            "model": model, "prompt": prompt, "stream": True, "keep_alive": "20m",
            "options": {"temperature": 0.15, "num_ctx": context_window(prompt, num_predict), "num_predict": num_predict},
        }).encode(),
        headers={"Content-Type": "application/json"},
    )
    chunks, completed, started = [], False, time.monotonic()
    try:
        with urllib.request.urlopen(request, timeout=timeout) as response:
            last_update = 0.0
            for line in response:
                if STOP_REQUESTED:
                    raise InterruptedError("worker stopped; partial response retained")
                if time.monotonic() - started > timeout:
                    raise TimeoutError("model exceeded total generation deadline")
                if not line.strip():
                    continue
                packet = json.loads(line)
                if packet.get("error"):
                    raise RuntimeError(f"Ollama error: {packet['error']}")
                piece = packet.get("response")
                if isinstance(piece, str):
                    chunks.append(piece)
                    if repeated_tail("".join(chunks)):
                        raise RepeatedResponse("Ollama response entered a repeated-output loop")
                moment = time.monotonic()
                if moment - last_update >= 1.0 or packet.get("done"):
                    answer_so_far = "".join(chunks)
                    progress(
                        response_chars=len(answer_so_far),
                        response_chunks=len(chunks),
                        response_preview=answer_so_far[-320:],
                        eval_count=packet.get("eval_count"),
                        eval_duration_ns=packet.get("eval_duration"),
                        done_reason=packet.get("done_reason"),
                    )
                    last_update = moment
                if packet.get("done"):
                    if packet.get("done_reason") == "length":
                        raise IncompleteResponse("model reached output cap; partial response retained")
                    completed = True
                    break
            answer = "".join(chunks)
    except urllib.error.URLError as exc:
        if not chunks:
            raise HostUnavailable(f"Ollama request failed: {exc}") from exc
        raise RuntimeError(f"Ollama request failed: {exc}") from exc
    finally:
        if response_path is not None:
            response_path.write_text("".join(chunks), encoding="utf-8")
    if not completed:
        raise IncompleteResponse("model stream ended without completion; partial response retained")
    if not isinstance(answer, str) or not answer.strip():
        raise RuntimeError("Ollama returned no candidate")
    return answer


def verify(item: dict[str, Any], candidate: Path, worker: str = MODEL_WORKER, link: bool = False) -> dict[str, Any]:
    with COORDINATOR.build(worker, f"Verify: {item['symbol']}"):
        if (STATE_DIR / "halt.json").exists():
            raise RestorationError("fleet halted after baseline restoration failure; inspect halt.json")
        check_baseline(item)
        try:
            return verify_unlocked(item, candidate, link)
        except RestorationError as exc:
            write_json(STATE_DIR / "halt.json", {"at": timestamp(), "worker": worker, "error": str(exc)})
            raise


def verify_unlocked(item: dict[str, Any], candidate: Path, link: bool = False) -> dict[str, Any]:
    if link:
        raise RuntimeError("full-unit policy and relocation review must precede promotion/link testing")
    return verify_shim_owner(item, candidate)


def checked_command(command: list[str], timeout: int):
    """Reap the entire tool process group before restoring shared source."""
    process = subprocess.Popen(command, cwd=ROOT, stdout=subprocess.PIPE, stderr=subprocess.PIPE,
                               text=True, start_new_session=True)
    try:
        stdout, stderr = process.communicate(timeout=timeout)
    except BaseException:
        try:
            os.killpg(process.pid, signal.SIGKILL)
        except ProcessLookupError:
            pass
        process.communicate()
        raise
    return subprocess.CompletedProcess(command, process.returncode, stdout, stderr)


def objdiff_cli() -> Path:
    for name in ("objdiff-cli.exe", "objdiff-cli"):
        path = ROOT / "build" / "tools" / name
        if path.exists():
            return path
    raise RuntimeError("objdiff-cli is missing; run ninja once")


def verify_shim_owner(item: dict[str, Any], candidate: Path) -> dict[str, Any]:
    """Measure an ignored owner candidate through its declared shim unit."""
    owner = ROOT / item.get("owner_source", item["source"])
    compiled = ROOT / "build" / "GC6E01" / Path(item["source"]).with_suffix(".o")
    original = owner.read_bytes()
    output = ""
    try:
        owner.write_bytes(candidate.read_bytes())
        compiled.unlink(missing_ok=True)
        build = checked_command(["ninja", "-j2", str(compiled.relative_to(ROOT))], timeout=900)
        output = (build.stdout + "\n" + build.stderr).strip()
        if build.returncode or not compiled.exists():
            return {"returncode": 1, "pct": None, "deltas": None, "linked": False, "output_tail": output[-6000:]}
        with tempfile.TemporaryDirectory() as temporary:
            diff_path = Path(temporary) / "diff.json"
            diff = checked_command(
                [str(objdiff_cli()), "diff", "-p", str(ROOT), "-u", item["unit"], "-o", str(diff_path), "--format", "json", item["symbol"]],
                timeout=120,
            )
            output = (output + "\n" + diff.stdout + "\n" + diff.stderr).strip()
            if diff.returncode or not diff_path.exists():
                return {"returncode": 1, "pct": None, "deltas": None, "linked": False, "output_tail": output[-6000:]}
            data = read_json(diff_path, {})
            left = next((symbol for symbol in data.get("left", {}).get("symbols", []) if symbol.get("name") == item["symbol"]), None)
            right = next((symbol for symbol in data.get("right", {}).get("symbols", []) if symbol.get("name") == item["symbol"]), None)
            if not left or not right:
                return {"returncode": 1, "pct": None, "deltas": None, "linked": False, "output_tail": "target symbol not found in objdiff output"}
            left_rows, right_rows = left.get("instructions") or [], right.get("instructions") or []
            def formatted(rows: list[dict[str, Any]], index: int) -> str:
                return "" if index >= len(rows) else (rows[index].get("instruction") or {}).get("formatted", "")
            deltas = sum(formatted(left_rows, index) != formatted(right_rows, index) for index in range(max(len(left_rows), len(right_rows))))
            pct = left.get("match_percent")
            feedback = [f"target: {formatted(left_rows, index)} | candidate: {formatted(right_rows, index)}"
                        for index in range(max(len(left_rows), len(right_rows)))
                        if formatted(left_rows, index) != formatted(right_rows, index)]
            return {"returncode": 0 if pct == 100.0 else 1, "pct": pct, "deltas": deltas, "linked": False,
                    "diff_feedback": feedback[:24], "output_tail": output[-6000:]}
    finally:
        owner.write_bytes(original)
        compiled.unlink(missing_ok=True)
        try:
            restored = checked_command(["ninja", "-j2", str(compiled.relative_to(ROOT))], timeout=900)
            if restored.returncode or not compiled.exists():
                raise RestorationError("baseline rebuild failed: " + (restored.stdout + restored.stderr)[-1200:])
        except (OSError, subprocess.SubprocessError) as exc:
            raise RestorationError(f"baseline rebuild failed: {exc}") from exc


def process(state: dict[str, Any], item: dict[str, Any], host: str, model: str, timeout: int, worker: str, feedback: str = "") -> None:
    source_path = ROOT / item.get("owner_source", item["source"])
    source = source_path.read_text(encoding="utf-8", errors="replace")
    if digest(source) != item["source_sha256"]:
        item.update(status="stale_source", last_error="source changed; run sync")
        item["updated_at"] = timestamp()
        save_state(state)
        return
    item["status"] = "running"
    for key in ("last_report", "last_error", "candidate", "activity"):
        item.pop(key, None)
    item["attempts"] = int(item.get("attempts", 0)) + 1
    item["last_attempt_at"] = timestamp()
    activity(state, item, "Checking source baseline", "Confirming the queued owner source has not changed.")
    attempt = item["attempts"]
    folder = STATE_DIR / "candidates" / item["id"]
    folder.mkdir(parents=True, exist_ok=True)
    try:
        activity(state, item, "Generating objdiff brief", "Reading the target diff and active compiler flags.")
        brief, prompt = build_prompt(item, source, worker)
        if feedback:
            prompt += "\n\n## Previous attempt feedback\n" + feedback
        (folder / f"attempt-{attempt:03d}.brief.md").write_text(brief, encoding="utf-8")
        (folder / f"attempt-{attempt:03d}.prompt.md").write_text(prompt, encoding="utf-8")
        activity(
            state, item, "Generating candidate with local model", f"Streaming the Ollama response from {host.rstrip('/')}.",
            prompt_chars=len(prompt), response_chars=0, response_chunks=0,
            num_predict=int(state["settings"].get("ollama_num_predict", DEFAULT_NUM_PREDICT)),
        )
        response = ollama(
            host, model, prompt, timeout,
            int(state["settings"].get("ollama_num_predict", DEFAULT_NUM_PREDICT)),
            lambda **metrics: activity(
                state, item, "Generating candidate with local model", f"Streaming the Ollama response from {host.rstrip('/')}.", **metrics,
            ),
            response_path=folder / f"attempt-{attempt:03d}.response.md",
        )
        (folder / f"attempt-{attempt:03d}.response.md").write_text(response, encoding="utf-8")
        activity(state, item, "Extracting one replacement function", "Rejecting any response that is not a complete definition for this symbol.", response_chars=len(response))
        proposal = extract_function(response, item["symbol"])
        start, end = function_span(source, item["symbol"])
        if proposal.strip() == source[start:end].strip():
            item.update(status="no_change", last_error="model repeated the existing function unchanged")
            activity(state, item, "Unchanged proposal", "No compilation needed; source is identical.", completed_at=timestamp())
            event(state, "no_change", task=item["id"], worker=worker)
            return
        activity(state, item, "Applying strict source policy", "Checking the proposed definition before any compiler runs.")
        rejected, flags = policy(source[start:end], proposal)
        if rejected:
            item.update(status="policy_rejected", last_error="; ".join(rejected), last_report={"policy_rejected": rejected, "review_flags": flags})
            activity(state, item, "Rejected by source policy", "; ".join(rejected), completed_at=timestamp())
            event(state, "policy_rejected", task=item["id"], reasons=rejected)
            return
        candidate = folder / f"attempt-{attempt:03d}.c"
        candidate.write_text(splice(source, item["symbol"], proposal), encoding="utf-8")
        activity(
            state, item, "Compiling ignored candidate", "Building only the assigned objdiff unit; tracked source will be restored afterward.",
            candidate=str(candidate.relative_to(ROOT)), review_flags=flags,
        )
        measured = verify(item, candidate, worker)
        if re.search(r"#\s*pragma\s+(?:optimization_level|optimize_for_size|scheduling|peephole|opt_propagation)\b", source):
            flags.append("owner contains legacy compiler controls; audit active target settings before promotion")
        measured.update(candidate=str(candidate.relative_to(ROOT)), review_flags=flags)
        item["candidate"] = measured["candidate"]
        item["last_report"] = measured
        if measured["pct"] == 100.0:
            measured["link_gate"] = {"skipped": "full-owner source policy, sibling/data and relocation audit required before link testing"}
            item["status"] = "review_exact"
            activity(state, item, "Exact text candidate awaiting review", "Raw objdiff is 100%; source policy, full-owner and link gates remain pending.", completed_at=timestamp())
            event(state, "review_exact", task=item["id"], symbol=item["symbol"], linked=measured["link_gate"].get("linked", False))
        elif measured["pct"] is None:
            item.update(status="verification_error", last_error=measured["output_tail"][-1200:])
            activity(state, item, "Verification failed", "The candidate could not be measured; inspect the retained report.", completed_at=timestamp())
            event(state, "verification_error", task=item["id"])
        else:
            item["status"] = "non_exact"
            measured["improvement_pp"] = measured["pct"] - item["base_pct"]
            activity(state, item, "Candidate measured non-exact", f"objdiff measured {measured['pct']:.5f}% with {measured['deltas']} instruction deltas.", completed_at=timestamp())
            event(state, "non_exact", task=item["id"], symbol=item["symbol"], pct=measured["pct"])
    except RestorationError as exc:
        item.update(status="restoration_error", last_error=str(exc))
        activity(state, item, "Worker halted", str(exc), completed_at=timestamp())
        raise
    except InterruptedError as exc:
        item.update(status="interrupted", last_error=str(exc))
        activity(state, item, "Worker stopped", str(exc), completed_at=timestamp())
    except IncompleteResponse as exc:
        item.update(status="incomplete_response", last_error=str(exc))
        activity(state, item, "Incomplete model output", str(exc), completed_at=timestamp())
        event(state, "incomplete_response", task=item["id"], worker=worker)
    except StaleSource as exc:
        item.update(status="stale_source", last_error=str(exc))
        activity(state, item, "Source changed", str(exc), completed_at=timestamp())
        event(state, "stale_source", task=item["id"], symbol=item["symbol"])
    except RepeatedResponse as exc:
        item.update(status="model_loop", last_error=str(exc))
        activity(state, item, "Looping model response stopped", str(exc), completed_at=timestamp())
        event(state, "model_loop", task=item["id"], symbol=item["symbol"])
    except HostUnavailable as exc:
        item.update(status="pending", last_error=str(exc), attempts=attempt - 1)
        activity(state, item, "Model host unreachable", "Requeued without consuming an attempt.", completed_at=timestamp())
        event(state, "host_unavailable", task=item["id"], worker=worker, detail=str(exc))
        raise
    except (OSError, RuntimeError, subprocess.SubprocessError, TimeoutError, ValueError) as exc:
        item.update(status="error", last_error=str(exc)[-1800:])
        activity(state, item, "Runner error", item["last_error"], completed_at=timestamp())
        event(state, "error", task=item["id"], detail=item["last_error"])
    finally:
        write_json(folder / f"attempt-{attempt:03d}.report.json", {
            "task": item["id"], "symbol": item["symbol"], "worker": worker, "model": model,
            "source_sha256": item["source_sha256"], "status": item["status"],
            "base_pct": item["base_pct"], "at": timestamp(),
            "report": item.get("last_report"), "error": item.get("last_error"),
            "activity": item.get("activity"),
        })
        save_state(state)


def retry_feedback(item: dict[str, Any]) -> str:
    if item.get("status") not in {"no_change", "non_exact", "verification_error", "incomplete_response"}:
        return ""
    report = item.get("last_report") or {}
    if not report and not item.get("last_error"):
        return ""
    response = STATE_DIR / "candidates" / item["id"] / f"attempt-{item['attempts']:03d}.response.md"
    previous = response.read_text(encoding="utf-8") if response.exists() else ""
    return ("One correction attempt remains before moving to the next task. Preserve semantics and the exact signature. "
            "Do not repeat unchanged code or use compiler shaping.\n"
            f"Previous result: {item['status']}; baseline: {item['base_pct']}%; candidate: {report.get('pct')}.\n"
            + str(item.get("last_error", ""))[-1000:] + "\n"
            + "\n".join(report.get("diff_feedback", []))[:2000] + "\n"
            + report.get("output_tail", "")[-1500:] + "\nPrevious response:\n" + previous[:5000])


def pause(seconds: float) -> None:
    """Sleep in short steps so SIGTERM still stops a backing-off worker promptly."""
    deadline = time.monotonic() + seconds
    while not STOP_REQUESTED and time.monotonic() < deadline:
        time.sleep(min(1.0, deadline - time.monotonic()))


def worker_slug(worker: str) -> str:
    clean = re.sub(r"[^A-Za-z0-9_.-]+", "-", worker.strip()).strip("-")
    return clean or "worker"


def runner_lock_file(worker: str) -> Path:
    return STATE_DIR / "runners" / f"{worker_slug(worker)}.lock"


def acquire_lock(worker: str) -> None:
    STATE_DIR.mkdir(parents=True, exist_ok=True)
    lock_file = runner_lock_file(worker)
    lock_file.parent.mkdir(parents=True, exist_ok=True)
    handle = lock_file.open("a+")
    try:
        fcntl.flock(handle, fcntl.LOCK_EX | fcntl.LOCK_NB)
    except BlockingIOError as exc:
        handle.close()
        raise RuntimeError(f"runner already active for {worker}") from exc
    handle.seek(0)
    handle.truncate()
    handle.write(f"{os.getpid()}\n")
    handle.flush()
    RUNNER_LOCKS[worker_slug(worker)] = handle


def release_lock(worker: str) -> None:
    handle = RUNNER_LOCKS.pop(worker_slug(worker), None)
    if handle:
        handle.close()


def register_worker(state: dict[str, Any], worker: str, host: str, model: str, num_predict: int) -> None:
    state.setdefault("workers", {})[worker] = {
        "worker": worker,
        "ollama_host": host.rstrip("/"),
        "ollama_model": model,
        "ollama_num_predict": num_predict,
        "ollama_num_ctx": NUM_CTX,
        "pid": os.getpid(),
        "updated_at": timestamp(),
    }
    save_state(state)


def run(state: dict[str, Any], host: str, model: str, limit: int, timeout: int, delay: float, worker: str, num_predict: int, max_function_bytes: int = 0) -> int:
    acquire_lock(worker)
    try:
        register_worker(state, worker, host, model, num_predict)
        recovered = 0
        live_claims = COORDINATOR.snapshot().get("claims", {})
        claimed_sources = set(live_claims)
        for item in state["items"].values():
            if item.get("status") == "running" and item.get("owner_source", item["source"]) not in claimed_sources:
                item.update(status="pending", last_error="recovered after interrupted runner")
                item["updated_at"] = timestamp()
                recovered += 1
        if recovered:
            event(state, "runner_recovered", tasks=recovered)
            save_state(state)
        processed, backoff, waiting = 0, 0, None
        while not STOP_REQUESTED and (not limit or processed < limit):
            if (STATE_DIR / "halt.json").exists():
                raise RestorationError("fleet halted; inspect halt.json and rebuild the baseline before resuming")
            state = load_state(host, model, num_predict, update_settings=False)
            register_worker(state, worker, host, model, num_predict)
            pending = [item for item in state["items"].values() if item.get("status") == "pending"
                       and (not max_function_bytes or item["size"] <= max_function_bytes)]
            if not pending:
                break
            pending.sort(key=lambda item: (-int(item.get("value_score") or 0), -float(item["base_pct"]), item["residual_functions"], -item["size"], item["id"]))
            blocked = set()
            for item in pending:
                try:
                    claim = COORDINATOR.claim(
                        item.get("owner_source", item["source"]), worker,
                        item["symbol"], automatic=True, detail="Generating and verifying candidate",
                    )
                except ClaimConflict:
                    blocked.add(item.get("owner_source", item["source"]))
                    continue
                try:
                    state = load_state(host, model, num_predict, update_settings=False)
                    item = state["items"][item["id"]]
                    if item.get("status") != "pending":
                        continue
                    state["settings"]["ollama_num_predict"] = num_predict
                    item["worker"] = worker
                    try:
                        process(state, item, host, model, timeout, worker)
                        feedback = retry_feedback(item)
                        if feedback and not STOP_REQUESTED:
                            if item["status"] == "incomplete_response":
                                state["settings"]["ollama_num_predict"] = min(4096, num_predict * 2)
                            process(state, item, host, model, timeout, worker, feedback)
                        processed += 1
                        backoff = 0
                    except HostUnavailable:
                        backoff = min(max(backoff * 2, HOST_BACKOFF[0]), HOST_BACKOFF[1])
                finally:
                    COORDINATOR.release(claim["source"], claim["token"])
                waiting = None
                break
            else:
                # Every pending owner is claimed elsewhere; say so once instead of idling silently.
                if blocked != waiting:
                    event(state, "waiting_on_claims", worker=worker, sources=sorted(blocked))
                    save_state(state)
                    waiting = blocked
                pause(15)
            pause(backoff or delay)
        print(json.dumps({"processed": processed, "remaining": sum(item.get("status") == "pending" for item in state["items"].values())}, indent=2))
        return 0
    finally:
        release_lock(worker)


def dashboard() -> dict[str, Any]:
    state = load_state(DEFAULT_HOST, DEFAULT_MODEL, update_settings=False)
    report = read_json(REPORT_FILE, {"measures": {}, "categories": [], "units": []})
    maps: dict[str, list[dict[str, Any]]] = {}
    for unit in report.get("units", []):
        metadata = unit.get("metadata") or {}
        if metadata.get("auto_generated") or not metadata.get("source_path"):
            continue
        measures = unit.get("measures") or {}
        maps.setdefault(category(unit), []).append({
            "name": unit["name"], "source": metadata["source_path"], "fuzzy": measures.get("fuzzy_match_percent", 0),
            "matched": measures.get("matched_functions", 0), "total": measures.get("total_functions", 0),
            "complete": bool(metadata.get("complete")), "code": int(measures.get("total_code") or 0),
        })
    for entries in maps.values():
        entries.sort(key=lambda entry: (-entry["code"], entry["name"]))
    items = list(state["items"].values())
    high_value = sorted(
        (item for item in items if item.get("status") == "pending"),
        key=lambda item: (-int(item.get("value_score") or 0), -float(item.get("base_pct") or 0), item.get("id", "")),
    )[:20]
    recomp = recomp_status(ROOT, REPORT_FILE)
    boot = boot_index(recomp)
    boot_queue = sorted(
        ({**item, "boot_blocker": boot[item["symbol"]]} for item in items
         if item.get("symbol") in boot and item.get("status") in {"pending", "running", "review_exact"}),
        key=lambda item: (item["boot_blocker"]["index"], -float(item.get("base_pct") or 0), item.get("id", "")),
    )[:24]
    high_value = [{**item, "boot_blocker": boot.get(item.get("symbol"))} for item in high_value]
    return {
        "generated_at": timestamp(), "report_measures": report.get("measures") or {}, "categories": report.get("categories") or [], "maps": maps,
        "queue": {
            "total": len(items), "status": Counter(item.get("status", "pending") for item in items),
            "active": [{**item, "boot_blocker": boot.get(item.get("symbol"))} for item in items if item.get("status") == "running"],
            "review": sorted((item for item in items if item.get("status") == "review_exact"), key=lambda item: item.get("last_attempt_at", ""), reverse=True)[:100],
            "high_value": high_value, "boot_critical": boot_queue,
        },
        "recomp": recomp,
        "events": list(reversed(state.get("events", [])[-80:])), "snapshots": state.get("snapshots", []), "settings": state.get("settings", {}),
        "workers": state.get("workers", {}),
        "worker_outcomes": {
            name: {
                "attempted": sum(item.get("worker") == name and int(item.get("attempts", 0)) > 0 for item in items),
                "improved": sum(item.get("worker") == name and (item.get("last_report") or {}).get("pct") is not None
                                and item["last_report"]["pct"] > item["base_pct"] for item in items),
                "status": Counter(item["status"] for item in items if item.get("worker") == name),
            } for name in state.get("workers", {})
        },
        "priority_analysis": state.get("priority_analysis", {}),
        "coordination": COORDINATOR.snapshot(),
    }


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--ollama-host", default=os.environ.get("OLLAMA_HOST", DEFAULT_HOST))
    parser.add_argument("--model", default=os.environ.get("OLLAMA_MODEL", DEFAULT_MODEL))
    parser.add_argument("--worker", default=DEFAULT_WORKER,
                        help=f"fleet worker name for claims, locks, and dashboard rows (default: {DEFAULT_WORKER})")
    parser.add_argument("--num-predict", type=int, default=DEFAULT_NUM_PREDICT,
                        help=f"maximum generated tokens per Ollama request (default: {DEFAULT_NUM_PREDICT})")
    parser.add_argument("--num-ctx", type=int, default=DEFAULT_NUM_CTX,
                        help="context window for prompts that fit; larger prompts fall back to "
                             f"{DEFAULT_NUM_CTX} rather than being truncated (default: {DEFAULT_NUM_CTX})")
    commands =parser.add_subparsers(dest="command", required=True)
    sync_parser = commands.add_parser("sync", help="queue every source-backed, non-exact report function")
    sync_parser.add_argument("--reset", action="store_true", help="restart existing tasks from pending")
    run_parser = commands.add_parser("run", help="ask the local model to process pending tasks")
    run_parser.add_argument("--limit", type=int, default=0, help="maximum tasks; 0 means all pending")
    run_parser.add_argument("--timeout", type=int, default=900)
    run_parser.add_argument("--delay", type=float, default=0)
    run_parser.add_argument("--max-function-bytes", type=int, default=0,
                            help="fast-worker size ceiling; 0 includes the whole queue")
    commands.add_parser("status", help="show campaign state")
    claim_parser = commands.add_parser("claim", help="reserve a function's entire source owner")
    claim_parser.add_argument("task", help="queue ID or unambiguous symbol")
    claim_parser.add_argument("--worker", default="Codex")
    claim_parser.add_argument("--detail", default="Reviewing and matching source")
    release_parser = commands.add_parser("release", help="release a manual source claim")
    release_parser.add_argument("source", help="owner source path returned by claim")
    release_parser.add_argument("--token", required=True)
    build_parser = commands.add_parser("build", help="run a command under the shared build lock")
    build_parser.add_argument("--worker", default="Codex")
    build_parser.add_argument("build_command", nargs=argparse.REMAINDER)
    commands.add_parser("claims", help="show source claims and waiting builds")
    args = parser.parse_args()
    if args.command == "run":
        def stop_worker(signum, frame):
            global STOP_REQUESTED
            STOP_REQUESTED = True
        signal.signal(signal.SIGTERM, stop_worker)
        signal.signal(signal.SIGINT, stop_worker)
    if args.num_predict < 1:
        parser.error("--num-predict must be at least 1")
    if args.num_ctx < 2048:
        parser.error("--num-ctx must be at least 2048")
    global NUM_CTX
    NUM_CTX = args.num_ctx
    if args.command in {"sync", "run"}:
        with COORDINATOR.writer(shared=args.command == "run"):
            state = load_state(args.ollama_host, args.model, args.num_predict)
            if args.command == "run" and not state["items"]:
                parser.error("queue is empty; run sync before starting workers")
            if args.command == "sync":
                with COORDINATOR.build("Queue sync", "Refresh authoritative report"):
                    result = sync(state, getattr(args, "reset", False))
                print(json.dumps(result, indent=2))
            if args.command == "run":
                return run(state, args.ollama_host, args.model, args.limit, args.timeout, args.delay, args.worker, args.num_predict, args.max_function_bytes)
    elif args.command == "claim":
        state = load_state(args.ollama_host, args.model, args.num_predict, update_settings=False)
        matches = [item for item in state["items"].values() if args.task in (item["id"], item["symbol"])]
        if len(matches) != 1 or not matches[0].get("owner_source"):
            parser.error("task must identify one source-backed queue entry; use its full ID if ambiguous")
        item = matches[0]
        claim = COORDINATOR.claim(item["owner_source"], args.worker, item["symbol"], detail=args.detail)
        print(json.dumps(claim, indent=2))
    elif args.command == "release":
        COORDINATOR.release(args.source, args.token)
    elif args.command == "build":
        command = args.build_command
        if command[:1] == ["--"]:
            command = command[1:]
        if not command:
            parser.error("build requires a command after --")
        with COORDINATOR.build(args.worker, " ".join(command)):
            return subprocess.run(command, cwd=ROOT).returncode
    elif args.command == "claims":
        print(json.dumps(COORDINATOR.snapshot(), indent=2))
    else:
        state = load_state(args.ollama_host, args.model, args.num_predict, update_settings=False)
        print(json.dumps({"state": str(STATE_FILE.relative_to(ROOT)), "counts": Counter(item.get("status", "pending") for item in state["items"].values()), "settings": state["settings"]}, indent=2))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
