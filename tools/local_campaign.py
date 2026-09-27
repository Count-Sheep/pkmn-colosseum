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
import shlex
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
import local_campaign_corpus as corpus
import local_campaign_permute as permute

ROOT = Path(__file__).resolve().parents[1]
STATE_DIR = ROOT / "build" / "local_llm_campaign"
STATE_FILE = STATE_DIR / "state.json"
REPORT_FILE = ROOT / "build" / "GC6E01" / "report.json"
BOOT_FOCUS_FILE = STATE_DIR / "boot_focus.json"
BOOT_FOCUS_MAX_AGE = 30 * 60
BRIEF_TOOL = ROOT / "tools" / "decomp_work" / "handoff" / "gen_brief.py"
VERIFY_TOOL = ROOT / "tools" / "decomp_work" / "handoff" / "verify.py"
DEFAULT_HOST = "http://dreamworld:11434"
DEFAULT_MODEL = "qwen3.6:27b"
DEFAULT_NUM_PREDICT = int(os.environ.get("OLLAMA_NUM_PREDICT", "4096"))
DEFAULT_NUM_CTX = int(os.environ.get("OLLAMA_NUM_CTX", "32768"))
COORDINATOR = Coordinator(STATE_DIR)
MODEL_WORKER = "Local LLM"
DEFAULT_WORKER = os.environ.get("LOCAL_CAMPAIGN_WORKER", MODEL_WORKER)
RUNNER_LOCKS: dict[str, Any] = {}
STOP_REQUESTED = False
NUM_CTX = DEFAULT_NUM_CTX
HOST_BACKOFF = (30, 600)
STALL_SECONDS = 600  # longest silence tolerated on an Ollama stream before the attempt is abandoned
THINK: bool | None = None  # None: let the model decide; thinking models reason unless told otherwise
RETRIES = 1  # correction attempts per task after the first, each fed the latest diff
RUN_OPTIONS: dict[str, Any] = {}  # run settings shown on the dashboard's worker card
STALL_ROUNDS = 4  # stop a task's correction rounds after this many rounds without a new best
PERMUTE = False  # run the declaration-order search on near-exact tasks after the model rounds
PERMUTE_CAP = 720
PERMUTE_SECONDS = 600
RECYCLE = False  # when the queue runs dry, retry attempts made before the latest harness improvement


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
    # Units carry the binary ("dol"/"modules") first and the subsystem after; group by subsystem.
    specific = [name for name in categories if name not in ("dol", "modules")]
    return (specific or categories or ["other"])[0]


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


FORBIDDEN_PRAGMAS = ("optimization_level", "optimize_for_size", "scheduling", "peephole", "opt_propagation")
PRAGMA_LINE = re.compile(r"^[ \t]*#[ \t]*pragma[ \t]+(\w+)(?:[ \t]+([^\n/]*))?", re.M)


def pragmas_active_at(text: str, offset: int) -> dict[str, str]:
    """Forbidden compiler-control pragmas in effect at `offset`, honouring push/pop scoping."""
    state: dict[str, str] = {}
    stack: list[dict[str, str]] = []
    live: list[bool] = []  # conditional-compilation stack; `#if 0` regions don't count
    for line in text[:offset].splitlines():
        directive = re.match(r"[ \t]*#[ \t]*(\w+)[ \t]*(.*)", line)
        if not directive:
            continue
        word, rest = directive.group(1), directive.group(2).strip()
        if word in ("if", "ifdef", "ifndef"):
            live.append(not (word == "if" and rest.split("//")[0].strip() == "0"))
            continue
        if word in ("else", "elif"):
            if live:
                live[-1] = not live[-1] if word == "else" else True
            continue
        if word == "endif":
            if live:
                live.pop()
            continue
        if word != "pragma" or not all(live):
            continue
        match = PRAGMA_LINE.match(line)
        if not match:
            continue
        name, value = match.group(1), (match.group(2) or "").strip()
        if name == "push":
            stack.append(dict(state))
        elif name == "pop":
            state = stack.pop() if stack else {}
        elif name in FORBIDDEN_PRAGMAS:
            if value == "reset":
                state.pop(name, None)
            else:
                state[name] = value
    return state


def unit_pragma_defaults(cflags: str) -> dict[str, str]:
    """The pragma values a unit's own command-line flags imply, so one function can be
    compiled as if no local pragma applied without disturbing its neighbours."""
    level = re.search(r"-O(\d)(,([ps]))?", cflags)
    return {
        "optimization_level": level.group(1) if level else "4",
        "optimize_for_size": "on" if level and level.group(3) == "s" else "off",
        "peephole": "off" if "nopeephole" in cflags else "on",
        "scheduling": "off" if re.search(r"noschedule|-schedule off", cflags) else "on",
        "opt_propagation": "off" if "noprop" in cflags else "on",
    }


def neutralize(text: str, symbol: str, cflags: str) -> str:
    """Wrap one function so it compiles under the unit's flags instead of its local pragmas."""
    start, end = function_span(text, symbol)
    defaults = unit_pragma_defaults(cflags)
    head = "#pragma push\n" + "".join(f"#pragma {name} {defaults[name]}\n" for name in FORBIDDEN_PRAGMAS)
    return text[:start] + head + text[start:end] + "\n#pragma pop\n" + text[end:]


def unit_cflags(item: dict[str, Any]) -> str:
    compiled = Path("build") / "GC6E01" / Path(item["source"]).with_suffix(".o")
    out = subprocess.run(["ninja", "-t", "commands", str(compiled)], cwd=ROOT, capture_output=True, text=True, timeout=60)
    lines = [line for line in out.stdout.splitlines() if "mwcceppc" in line]
    return lines[-1] if lines else ""


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
    owner_texts: dict[str, str] = {}
    for unit, function in grouped:
        pct = float(function.get("fuzzy_match_percent") or 0)
        cleanup = None
        if pct >= 100.0:
            # Exact only under a forbidden local pragma: the recomp can't accept it, so the
            # model gets a policy-cleanup task to find pragma-free C that still matches.
            metadata = unit.get("metadata") or {}
            source = metadata.get("source_path")
            if metadata.get("complete") or not source or metadata.get("auto_generated") or source not in sources:
                continue
            owner = sources[source]
            if owner not in owner_texts:
                path = ROOT / owner
                owner_texts[owner] = path.read_text(encoding="utf-8", errors="replace") if path.is_file() else ""
            try:
                start, _ = function_span(owner_texts[owner], function["name"])
            except ValueError:
                continue
            cleanup = pragmas_active_at(owner_texts[owner], start)
            if not cleanup:
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
        record.update(priorities.get((unit["name"], symbol), {}))
        if cleanup:
            record.update(kind="cleanup", pragmas=cleanup, base_pct=0.0)
        old = state["items"].get(task)
        if old and not reset:
            for key in ("status", "attempts", "last_attempt_at", "last_report", "last_error", "candidate", "worker", "activity",
                        "best", "promoted_pct", "promotion_rejected", "structural_hunks", "clean_base_pct"):
                if key in old:
                    record[key] = old[key]
            if record.get("kind") == "cleanup" and record.get("clean_base_pct") is not None:
                record["base_pct"] = record["clean_base_pct"]
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
        if item.get("kind") != "cleanup":
            return build_prompt_unlocked(item, source)
        owner = ROOT / item.get("owner_source", item["source"])
        original = owner.read_bytes()
        try:
            owner.write_text(source, encoding="utf-8")  # the neutralized text process() passes in
            if item.get("clean_base_pct") is None:
                with tempfile.TemporaryDirectory() as temporary:
                    item["clean_base_pct"] = scratch_score(item, Path(temporary))
                if item["clean_base_pct"] is not None:
                    item["base_pct"] = item["clean_base_pct"]
            brief, prompt = build_prompt_unlocked(item, source)
        finally:
            owner.write_bytes(original)
        note = ("## Policy cleanup task\n\nThis function already matches, but only under forbidden local "
                f"compiler pragmas ({', '.join(f'{k} {v}' for k, v in (item.get('pragmas') or {}).items())}). "
                "The recomp cannot accept pragma-shaped code. The diff above is the function compiled under "
                "its unit's normal flags. Write natural, pragma-free C that matches the target under those flags.\n\n")
        return brief, note + prompt


def _clean_body(symbol: str, source_path: str | None, budget: int) -> str:
    """A matched function's source body, only if it is policy-clean and within budget."""
    if not source_path:
        return ""
    try:
        owner = owner_source(source_path)
        text = (ROOT / owner).read_text(encoding="utf-8", errors="replace")
        start, end = function_span(text, symbol)
    except (OSError, ValueError):
        return ""
    body = text[start:end].strip()
    if re.search(r"#\s*pragma\b|\.inc\b|\b(?:asm|__asm|__asm__)\b", body) or len(body) > budget:
        return ""
    rejected, flags = policy("", body)
    return "" if rejected or flags else body


def corpus_context(item: dict[str, Any], owner_text: str) -> str:
    """Similar already-matched code and the declarations of every callee, from the corpus index."""
    try:
        index = corpus.load_index(ROOT, REPORT_FILE, STATE_DIR / "corpus_index.json")
    except (OSError, ValueError):
        return reference_context(item)
    owner = item.get("owner_source", item["source"])
    parts, budget = [], 4000
    for name in corpus.similar_matched(index, item["symbol"], owner, limit=5):
        body = _clean_body(name, index["functions"][name].get("source"), budget)
        if body:
            parts.append(f"### {name} ({index['functions'][name]['source']}, matched 100%)\n```c\n{body}\n```")
            budget -= len(body)
        if len(parts) == 3 or budget < 400:
            break
    out = []
    if parts:
        out.append("## Matched code in this project that calls the same functions or uses the same data\n"
                   "These compile byte-exactly with the same compiler. Reuse their idioms, types and field names.\n\n"
                   + "\n\n".join(parts))
    else:
        fallback = reference_context(item)
        if fallback:
            out.append(fallback)
    decls = corpus.callee_declarations(index, item["symbol"], owner_text, _headers_text(), ROOT)
    if decls:
        out.append("## Declarations of the functions it calls (match these parameter and return types exactly)\n\n```c\n"
                   + "\n".join(decls) + "\n```")
    return "\n\n".join(out)


MELEE_ROOT = ROOT / "build" / "reference" / "melee" / "src"
MELEE_DEF = re.compile(r"(?m)^[A-Za-z_][^\n;#{}()]*?\b([A-Za-z_]\w*)\s*\([^;{}]*\)\s*\{")
_MELEE_INDEX: dict[str, Path] = {}


def melee_reference(symbol: str, budget: int = 3500) -> str:
    """The same HAL function from the Melee decompilation, when one exists under this name.

    Melee and Colosseum ship the same HAL library (HSD), at different versions. Melee's copy
    matches its own retail build, so it shows the original structure, types and field names;
    it is reference only and is filtered by the same policy as project examples.
    """
    if not MELEE_ROOT.is_dir():
        return ""
    if not _MELEE_INDEX:
        for path in sorted(MELEE_ROOT.rglob("*.c")):
            try:
                text = path.read_text(encoding="utf-8", errors="replace")
            except OSError:
                continue
            for match in MELEE_DEF.finditer(text):
                _MELEE_INDEX.setdefault(match.group(1), path)
        _MELEE_INDEX.setdefault("\0", MELEE_ROOT)
    path = _MELEE_INDEX.get(symbol)
    if not path:
        return ""
    try:
        text = path.read_text(encoding="utf-8", errors="replace")
        start, end = function_span(text, symbol)
    except (OSError, ValueError):
        return ""
    body = text[start:end].strip()
    if len(body) > budget or re.search(r"#\s*pragma\b|\.inc\b|\b(?:asm|__asm|__asm__)\b", body):
        return ""
    rejected, _ = policy("", body)
    if rejected:
        return ""
    return (f"## The same HAL function in the Melee decompilation ({path.relative_to(MELEE_ROOT.parent.parent)})\n"
            "Melee ships an earlier build of the same HAL library; this version matches Melee's retail code.\n"
            "Use it for structure, types and field meanings, but our target is authoritative where they differ.\n\n"
            f"```c\n{body}\n```")


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


BRANCH_ROW = re.compile(r"^(?P<mark>>>|  )\s*(?P<k>\d+)\s{2}(?P<rest>.*)$")
BRANCH_OP = re.compile(r"^(b[a-z]*)\s+(0x[0-9a-fA-F]+)$")


def _diff_cells(rest: str) -> tuple[str, str]:
    """Split a brief diff row into its (target, ours) instruction cells."""
    parts = re.split(r"\s{2,}", rest.strip(), maxsplit=1)
    if len(rest) - len(rest.lstrip()) >= 30:  # target cell empty: only ours printed
        return "", rest.strip()
    return parts[0], (parts[1] if len(parts) > 1 else "")


REGISTER = re.compile(r"\b[rf]\d{1,2}\b")
JUMPTABLE = re.compile(r"\bjumptable_[0-9A-Fa-f]+@(?:ha|l)\b")
LOCAL_ADDRESS = re.compile(r"^(?:lis|addi)\s+r\d+,\s*(?:r\d+,\s*)?@\d+@(?:ha|l)$")
LITERAL_RELOC = re.compile(r"@\d+@(?:sda21|sda2|ha|l)\b|\blbl_[0-9A-F]{8}@(?:sda21|sda2|ha|l)\b")


def diff_class(want: str, have: str) -> str:
    """Why a flagged row differs: branch displacement, data-symbol naming, registers, or shape."""
    a, b = BRANCH_OP.match(want), BRANCH_OP.match(have)
    if a and b and a.group(1) == b.group(1):
        return "branch"
    if want and have and "@" in want + have and LITERAL_RELOC.sub("SYM", want) == LITERAL_RELOC.sub("SYM", have) \
            and re.search(r"@\d+@", want + have):
        # A compiler literal (@NNN) against a named constant: the unit's data layout decides
        # this, not the function body, so the model cannot fix it here.
        return "data"
    if JUMPTABLE.search(want + " " + have) or (not (want and have) and LOCAL_ADDRESS.match(want or have)):
        # A switch table the target keeps as a named data symbol against our compiler-local
        # table: the address is formed differently, but which unit owns the table decides it.
        return "data"
    if want and have and want.split()[0] == have.split()[0] and REGISTER.sub("R", want) == REGISTER.sub("R", have):
        return "register"
    return "structural"


def sharpen_diff(brief: str) -> tuple[str, list[str]]:
    """Unflag rows the function body cannot fix and summarise the real hunks, shape first.

    Once code before a branch changes size, every later branch target shifts, so the raw
    diff marks most branches as mismatches even when the branch itself is identical; a
    compiler literal against a named constant is unit data layout. Both are noise that
    hides the few real differences. Hunks with a structural difference (an extra, missing
    or different instruction) come before register-only ones, which usually disappear
    once the structure matches.
    """
    lines, real, kinds = brief.splitlines(), [], {}
    for index, line in enumerate(lines):
        row = BRANCH_ROW.match(line)
        if not row or row["mark"] != ">>":
            continue
        want, have = _diff_cells(row["rest"])
        kind = diff_class(want, have)
        if kind in ("branch", "data"):
            lines[index] = "  " + line[2:]
        else:
            real.append(int(row["k"]))
            kinds[int(row["k"])] = kind
    rows = {int(m["k"]): m for m in (BRANCH_ROW.match(l) for l in lines) if m}
    hunks, seen = [], set()
    for k in real:
        if k in seen:
            continue
        span = [j for j in range(k - 2, k + 3) if j in rows]
        seen.update(span)
        label = "structural" if any(kinds.get(j) == "structural" for j in span) else "register-only"
        body = "\n".join(("-> " if j in real else "   ") + rows[j]["rest"].rstrip() for j in span)
        hunks.append((label, f"[{label}]\n{body}"))
    hunks.sort(key=lambda pair: pair[0] != "structural")
    return "\n".join(lines), [text for _, text in hunks]


def _scratch_symbols(item: dict[str, Any], directory: Path, sym_on: bool = True):
    """Compile the unit to a scratch object and objdiff the symbol: (target, ours) or None.

    MWCC's `-sym on` only adds debug sections (.text is byte-identical), and with them
    objdiff reports the source line behind each of our instructions. The scratch object
    never replaces the build's own, so it cannot disturb verification.
    """
    compiled = Path("build") / "GC6E01" / Path(item["source"]).with_suffix(".o")
    commands = subprocess.run(["ninja", "-t", "commands", str(compiled)], cwd=ROOT,
                              capture_output=True, text=True, timeout=60)
    lines = [line for line in commands.stdout.splitlines() if "mwcceppc" in line and f" {item['source']} " in line + " "]
    project = read_json(ROOT / "objdiff.json", {})
    unit = next((u for u in project.get("units", []) if u.get("name") == item["unit"]), None)
    if commands.returncode or not lines or not unit or not unit.get("target_path"):
        return None
    compile_line = lines[-1].split(" && ", 1)[0]
    args = [arg for arg in shlex.split(compile_line) if arg != "-MMD"]
    if "-o" not in args:
        return None
    args[args.index("-o") + 1] = str(directory)
    if sym_on:
        args[args.index("-o"):args.index("-o")] = ["-sym", "on"]
    built = subprocess.run(args, cwd=ROOT, capture_output=True, text=True, timeout=300)
    scratch = directory / compiled.name
    if built.returncode or not scratch.exists():
        return None
    output = directory / "lines.json"
    diff = subprocess.run([str(objdiff_cli()), "diff", "-1", str(ROOT / unit["target_path"]), "-2", str(scratch),
                           "-o", str(output), "--format", "json", item["symbol"]],
                          cwd=ROOT, capture_output=True, text=True, timeout=120)
    data = read_json(output, {}) if not diff.returncode else {}
    pick = lambda side: next((s for s in data.get(side, {}).get("symbols", []) if s.get("name") == item["symbol"]), None)
    left, right = pick("left"), pick("right")
    return (left, right) if left and right else None


def scratch_score(item: dict[str, Any], directory: Path) -> float | None:
    """The symbol's objdiff match for whatever the owner source currently holds."""
    pair = _scratch_symbols(item, directory, sym_on=False)
    return None if pair is None else pair[0].get("match_percent")


def _objdiff_rows(item: dict[str, Any], directory: Path) -> list[tuple[str, str, int | None]] | None:
    """(target, ours, our source line) per aligned row, from a scratch `-sym on` compile."""
    pair = _scratch_symbols(item, directory)
    if pair is None:
        return None
    left, right = pair
    rows = []
    for index in range(max(len(left.get("instructions") or []), len(right.get("instructions") or []))):
        cell = lambda symbol: ((symbol.get("instructions") or [])[index].get("instruction") or {}) \
            if index < len(symbol.get("instructions") or []) else {}
        want, have = cell(left), cell(right)
        rows.append((want.get("formatted", ""), have.get("formatted", ""), have.get("line_number")))
    return rows


def line_annotated_hunks(item: dict[str, Any], source_text: str, limit: int = 10) -> list[str] | None:
    """Real-difference hunks, shape first, each naming the C statements that produced it."""
    try:
        with tempfile.TemporaryDirectory() as temporary:
            rows = _objdiff_rows(item, Path(temporary))
    except (OSError, subprocess.SubprocessError, ValueError):
        return None
    if not rows:
        return None
    source_lines = source_text.splitlines()
    kinds = {k: diff_class(want, have) for k, (want, have, _) in enumerate(rows) if want != have}
    real = [k for k, kind in kinds.items() if kind in ("structural", "register")]
    hunks, seen = [], set()
    for k in real:
        if k in seen:
            continue
        span = [j for j in range(k - 2, k + 3) if 0 <= j < len(rows)]
        seen.update(span)
        label = "structural" if any(kinds.get(j) == "structural" for j in span) else "register-only"
        body = "\n".join(f"{'-> ' if j in real else '   '}{rows[j][0]:<38} {rows[j][1]}" for j in span)
        # A target-only row has no line of ours; its neighbours' lines are the place to look.
        numbers = sorted({rows[j][2] for j in span if rows[j][2]})
        statements = "\n".join(f"    {source_lines[n - 1].strip()}" for n in numbers if 0 < n <= len(source_lines))
        text = f"[{label}]\n{body}"
        if statements:
            text += f"\nproduced by these lines of the assigned function:\n{statements}"
        hunks.append((label, text, numbers))
    hunks.sort(key=lambda pair: pair[0] != "structural")
    # Every structural hunk's lines, in order, so focused retries can move past the first one.
    item["focus_hunks"] = [numbers for label, _, numbers in hunks if label == "structural" and numbers]
    if item["focus_hunks"]:
        item["focus_lines"] = item["focus_hunks"][0]
    return [text for _, text, _ in hunks[:limit]]


TYPE_WORD = re.compile(r"\b([A-Z][A-Za-z0-9_]*[a-z][A-Za-z0-9_]*)\b")
_HEADER_CACHE: dict[str, str] = {}


def _headers_text() -> str:
    if "all" not in _HEADER_CACHE:
        parts = []
        for path in sorted((ROOT / "include").rglob("*.h")):
            try:
                parts.append(path.read_text(encoding="utf-8", errors="replace"))
            except OSError:
                continue
        _HEADER_CACHE["all"] = "\n".join(parts)
    return _HEADER_CACHE["all"]


def _type_definition(name: str, text: str) -> str:
    """Return the typedef/struct/union/enum block that defines `name`, if present."""
    patterns = (rf"typedef\s+(?:struct|union|enum)\s*\w*\s*\{{", rf"(?:struct|union|enum)\s+{name}\s*\{{")
    for pattern in patterns:
        for match in re.finditer(pattern, text):
            depth, index = 0, match.end() - 1
            while index < len(text):
                depth += {"{": 1, "}": -1}.get(text[index], 0)
                index += 1
                if depth == 0:
                    break
            tail = re.match(r"\s*(\w+)?\s*;", text[index:index + 80])
            block = text[match.start():index + (tail.end() if tail else 0)]
            if pattern.startswith("typedef") and not (tail and tail.group(1) == name):
                continue
            return block.strip()
    alias = re.search(rf"typedef\s+[^;{{}}]*\b{name}\s*;", text)
    return alias.group(0) if alias else ""


def type_context(body: str, owner_source: str, budget: int = 3500) -> str:
    """Definitions of the struct/typedef names the function uses, so the model sees real fields."""
    names = [n for n in dict.fromkeys(TYPE_WORD.findall(body)) if not n.startswith(("GX", "OS", "DVD", "PAD"))]
    found = []
    for name in names:
        block = _type_definition(name, owner_source) or _type_definition(name, _headers_text())
        if block and len(block) <= budget:
            found.append(f"```c\n{block}\n```")
            budget -= len(block)
    return ("## Types used by this function (from the repo; use these real fields)\n\n" + "\n\n".join(found)) if found else ""


MWCC_GUIDE = """## How MWCC source shape maps to these differences

Structure (fix these first):
- An extra or missing `b` next to a conditional branch, or `beq` where the target has `bne`,
  means the if/else, loop or early-return structure differs: invert the test, swap the
  branches, or turn a loop into the target's shape (for/while/do-while, test at top vs bottom).
  Separate early returns and one `||` condition compile differently; so do a switch and an
  if-chain, and `if (a->next == NULL) x = y; else x = a->next;` vs `x = a->next; if (!x) x = y;`.
- An extra `clrlwi`/`extsb`/`extsh`/`rlwinm` (or a missing one), or `cmpwi` vs `cmplwi`,
  means an integer width or signedness differs: fix the variable, field, parameter or return
  type (s8/u8/s16/u16/s32/u32). A value printed with %d, or compared signed, is signed.
- A load the target repeats but we hoist (or the reverse) means the source re-reads memory:
  read the field again instead of caching it in a local, pass a `const T*` (a const pointer
  parameter makes MWCC re-read through it), or the reverse.
- An extra `mr` copy of a value into another register, or a copy the target has and we
  lack, usually marks the boundary of an inlined helper: the target may have expanded a small
  function (one that also exists, or is expanded at other sites, in the same file) there.
- A literal the target loads from a named constant while we emit `@NNN` (or the reverse) is
  data layout, not something the function body can fix.

Registers (fix only after the structure matches):
- MWCC assigns callee-saved registers r31 downwards. Locals of inlined helpers are coloured
  first: the first inlined expansion colours its locals in reverse declaration order, then
  its parameters; later expansions colour parameters first, then locals in order. After that,
  the function's own locals are coloured in declaration order (top level, then inner blocks).
  A value reuses the lowest already-used register that doesn't overlap it, otherwise it
  takes the next new one. So to move a variable to a higher register, declare it earlier.
- Only register numbers differ: change declaration order and variable lifetimes, and keep
  every instruction that already matches.
- Never use dummy locals, self-assignments, volatile, pragmas or asm to force registers.
"""


FILE_SCOPE_DECL = r"(?m)^(?:extern\s+|static\s+)?[A-Za-z_][^\n;(){{}}=]*\b{name}\b[^\n;(){{}}]*(?:=[^\n;]*)?;"


def same_file_context(item: dict[str, Any], source: str, budget: int = 4500) -> str:
    """What the rest of the translation unit contributes to this function's code.

    MWCC inlines same-file definitions (deferred inlining even ones defined later) and
    pools the file's constants, so the callees defined in this file and the file-scope
    declarations of the data it touches often decide the instructions the diff shows.
    """
    try:
        index = corpus.load_index(ROOT, REPORT_FILE, STATE_DIR / "corpus_index.json")
    except (OSError, ValueError):
        return ""
    row = index["functions"].get(item["symbol"]) or {}
    parts, used = [], 0
    for callee in row.get("callees", []):
        try:
            start, end = function_span(source, callee)
        except ValueError:
            continue
        body = source[start:end].strip()
        if len(body) > 1800 or used + len(body) > budget:
            continue
        if re.search(r"#\s*pragma\b|\b(?:asm|__asm|__asm__)\b", body):
            continue
        parts.append(f"### {callee} (defined in this file; MWCC may inline it here)\n```c\n{body}\n```")
        used += len(body)
    decls = []
    for name in row.get("data", [])[:24]:
        match = re.search(FILE_SCOPE_DECL.format(name=re.escape(name)), source)
        if match and len(match.group(0)) < 200:
            decls.append(match.group(0).strip())
    if decls:
        parts.append("### File-scope declarations of the data this function uses\n```c\n" + "\n".join(decls) + "\n```")
    if not parts:
        return ""
    return "## Same-file context (this translation unit)\n\n" + "\n\n".join(parts)


DIFF_TABLE_BUDGET = 9000


def cap_diff_table(brief: str, budget: int = DIFF_TABLE_BUDGET) -> str:
    """Keep the full instruction table only when it fits; large functions get the hunks alone.

    Whole-corpus runs reach multi-kilobyte functions whose table alone would overflow the
    context window. The ranked, line-annotated hunks carry what the model needs to change.
    """
    start = brief.find("## Instruction diff")
    if start < 0:
        return brief
    fence = brief.find("```", start)
    close = brief.find("```", fence + 3) if fence >= 0 else -1
    if fence < 0 or close < 0 or close - fence <= budget:
        return brief
    rows = brief[fence + 3:close].count("\n")
    note = (f"(The full {rows}-row instruction table is omitted for length; the real differences "
            "below list every mismatched region with the source lines that produced it.)\n")
    return brief[:fence] + note + brief[close + 3:]


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
    brief, hunks = sharpen_diff(brief)
    brief = cap_diff_table(brief)
    annotated = line_annotated_hunks(item, source)
    if annotated:
        hunks = annotated
    # Register-only residue is an allocation problem the model rarely reasons its way out of;
    # the runner uses this count to spend its attempts on shape differences first.
    item["structural_hunks"] = sum(hunk.startswith("[structural]") for hunk in hunks)
    start, end = function_span(source, item["symbol"])
    real = ("## Real differences to fix (`->` marks the mismatched rows)\n\n"
            "Rows that differ only by branch displacement or by a compiler literal (`@NNN`) versus a named\n"
            "constant are removed: the function body cannot change them. Fix the [structural] hunks first\n"
            "(an instruction the target has and we lack, or the reverse); [register-only] hunks usually\n"
            "resolve once the structure matches. Where shown, \"produced by\" lists the statements of the\n"
            "assigned function that compiled to that hunk: that is where to change the source.\n\n"
            + "\n\n".join(f"```\n{h}\n```" for h in hunks[:12])) if hunks else ""
    types = type_context(source[start:end], source)
    prompt = f"""{brief}

{real}

{MWCC_GUIDE}
{types}

## Assigned function source

The code below does NOT yet compile to the target. Returning it unchanged is a failed attempt.
Make the smallest source changes that fix the real differences listed above.

```c
{source[start:end].strip()}
```

{same_file_context(item, source)}

{corpus_context(item, source)}

{melee_reference(item["symbol"])}

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


def ollama_payload(model: str, prompt: str, num_predict: int, temperature: float = 0.15) -> dict[str, Any]:
    payload: dict[str, Any] = {
        "model": model, "prompt": prompt, "stream": True, "keep_alive": "20m",
        "options": {"temperature": temperature, "num_ctx": context_window(prompt, num_predict), "num_predict": num_predict},
    }
    if THINK is not None:
        payload["think"] = THINK
    return payload


def ollama(host: str, model: str, prompt: str, timeout: int, num_predict: int, progress, response_path: Path | None = None,
           temperature: float = 0.15) -> str:
    request = urllib.request.Request(
        host.rstrip("/") + "/api/generate",
        data=json.dumps(ollama_payload(model, prompt, num_predict, temperature)).encode(),
        headers={"Content-Type": "application/json"},
    )
    chunks, completed, started = [], False, time.monotonic()
    thinking_chars = 0
    try:
        # A silent stream must fail fast: cap each read, while the loop below enforces the total deadline.
        with urllib.request.urlopen(request, timeout=min(timeout, STALL_SECONDS)) as response:
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
                # Thinking models stream their reasoning separately; count it so a long
                # reasoning phase shows progress instead of looking stalled.
                if isinstance(packet.get("thinking"), str):
                    thinking_chars += len(packet["thinking"])
                moment = time.monotonic()
                if moment - last_update >= 1.0 or packet.get("done"):
                    answer_so_far = "".join(chunks)
                    progress(
                        thinking_chars=thinking_chars,
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
            rows = []
            for index in range(max(len(left_rows), len(right_rows))):
                want, have = formatted(left_rows, index), formatted(right_rows, index)
                kind = diff_class(want, have) if want != have else "same"
                if kind in ("structural", "register"):
                    rows.append((kind != "structural", f"[{kind}] target: {want or '(none)'} | candidate: {have or '(none)'}"))
            # Shape differences first; branch-offset and literal-naming rows are noise here too.
            feedback = [text for _, text in sorted(rows, key=lambda row: row[0])]
            if pct != 100.0:
                # The candidate is still in place: name the statements behind each remaining hunk.
                annotated = line_annotated_hunks(item, owner.read_text(encoding="utf-8", errors="replace"), limit=6)
                if annotated:
                    feedback = annotated
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


RETRY_TEMPERATURES = (0.15, 0.45, 0.7, 0.9)


def _unfinished(line: str) -> bool:
    """A code line that continues on the next line (a statement or call cut mid-way)."""
    code = re.sub(r"//.*$", "", line).rstrip()
    return bool(code) and not code.lstrip().startswith("#") \
        and not code.endswith((";", "{", "}", ":", "*/"))


def focus_region(source: str, symbol: str, lines: list[int], context: int = 1, cap: int = 14) -> tuple[int, int] | None:
    """Owner-file line range (inclusive) to rewrite: the statements behind one hunk, inside the function.

    The range never includes the function's opening brace, always covers whole statements
    (a call split over lines is taken whole), and is widened to close a block it opens
    when that stays within `cap + 6` lines. Cut statements and stray braces were the most
    common reason a focused answer failed to compile."""
    start, end = function_span(source, symbol)
    first = source.count("\n", 0, start) + 1
    last = source.count("\n", 0, end) + 1
    body = source.count("\n", 0, source.find("{", start)) + 2  # first line after the opening brace
    inside = [n for n in lines if first < n < last]
    if not inside:
        return None
    text = source.splitlines()
    low, high = max(body, min(inside) - context), min(last - 1, max(inside) + context)
    if low > high:
        return None

    def depth(opening: str, closing: str) -> int:
        chunk = "\n".join(re.sub(r"//.*$", "", line) for line in text[low - 1:high])
        return chunk.count(opening) - chunk.count(closing)

    def complete() -> None:
        nonlocal low, high
        while low > body and _unfinished(text[low - 2]):
            low -= 1
        while high < last - 1 and (_unfinished(text[high - 1]) or depth("(", ")") > 0):
            high += 1

    complete()
    if high - low >= cap:
        return None
    wide = (low, high)
    while depth("{", "}") > 0 and high < last - 1 and high - low < cap + 6:
        high += 1
        complete()
    while depth("{", "}") < 0 and low > body and high - low < cap + 6:
        low -= 1
        complete()
    if depth("{", "}") != 0 or high - low >= cap + 6:
        low, high = wide  # could not close the block within reach: keep the statement-complete range
    return low, high


CHARS_PER_TOKEN = 3.5


def exceeds_budget(function_text: str, num_predict: int) -> bool:
    """A whole-function answer would not fit the output cap (with room for the fence and slack)."""
    return len(function_text) / CHARS_PER_TOKEN > 0.75 * num_predict


FOCUS_REASONS = {
    "no_change": "Your previous answer left the function unchanged.",
    "incomplete_response": "Your previous answer ran out of output tokens before the function ended.",
    "too_large": "This function is too long to return whole within the output limit.",
    "plateau": "Whole-function rewrites have stopped raising the score.",
}


def focused_prompt(prompt: str, source: str, region: tuple[int, int], reason: str = "no_change",
                   current: str | None = None) -> str:
    """Ask for one hunk's lines only. `current` is the function being edited when it is not the
    assigned source (the best candidate so far), shown whole so the model sees what it keeps."""
    lines = source.splitlines()
    excerpt = "\n".join(lines[region[0] - 1:region[1]])
    where = "\"Assigned function source\""
    shown = ""
    if current is not None:
        where = "the current function below"
        shown = ("The function currently reads as follows. This is your best candidate so far, which the\n"
                 "harness builds on:\n\n```c\n" + current.strip() + "\n```\n\n")
    return (prompt + "\n\n## Focused edit (this attempt)\n"
            + FOCUS_REASONS[reason] + " This time rewrite ONLY the lines below, which\n"
            "compile to one [structural] hunk. Return exactly one ```c block with their replacement\n"
            "(it may have more or fewer lines, and it must change something). Do not return the rest of the\n"
            "function: the harness splices your lines back in place of these. Every other line of the function\n"
            f"stays exactly as shown in {where}, so keep any declaration or variable those\n"
            "other lines still use. If these lines open or close a block that continues outside them, keep\n"
            "that brace. Declare any new local at the top of a block, as C89 requires.\n\n"
            + shown + "Lines to rewrite:\n\n```c\n" + excerpt + "\n```\n")


LOCAL_DECL = re.compile(
    r"^(?P<indent>[ \t]*)(?:(?:const|volatile|unsigned|signed|struct|union|enum|register)\s+)*"
    r"(?P<type>[A-Za-z_]\w*)(?:\s+[A-Za-z_]\w*)*[\s\*]+(?P<name>[A-Za-z_]\w*)\s*(?P<array>\[[^\]]*\])?"
    r"\s*(?:=\s*(?P<init>[^;]+))?;[ \t]*(?://.*)?$"
)
NOT_A_TYPE = {"return", "goto", "case", "else", "do", "sizeof", "extern", "static", "typedef"}
MULTI_DECL = re.compile(
    r"^(?P<indent>[ \t]*)(?P<type>(?:(?:const|volatile|unsigned|signed|struct|union|enum|register)\s+)*[A-Za-z_]\w*)\s+"
    r"(?P<rest>[\*\s]*[A-Za-z_]\w*\s*(?:\[[^\]]*\])?\s*(?:=\s*[^,;()]+)?"
    r"(?:\s*,\s*[\*\s]*[A-Za-z_]\w*\s*(?:\[[^\]]*\])?\s*(?:=\s*[^,;()]+)?)+)\s*;[ \t]*(?://.*)?$"
)
DECLARATOR = re.compile(r"^(?P<ptr>[\*\s]*)(?P<name>[A-Za-z_]\w*)\s*(?P<array>\[[^\]]*\])?\s*(?:=\s*(?P<init>.+))?$")


def parse_declaration(line: str) -> tuple[str, str, list[re.Match]] | None:
    """(indent, base type, declarators) for a local declaration line, else None.

    Lists (`s32 i, j;`) are split only when no parentheses are present, so a call in an
    initializer is never cut at its argument commas."""
    match = MULTI_DECL.match(line) if "(" not in line else None
    if match and match["type"].split()[-1] not in NOT_A_TYPE:
        parts = [DECLARATOR.match(part.strip()) for part in match["rest"].split(",")]
        return (match["indent"], match["type"], parts) if all(parts) else None
    match = LOCAL_DECL.match(line)
    if match and match["type"] not in NOT_A_TYPE:
        base = line[:match.start("name")].strip().rstrip("*").strip()
        declarator = DECLARATOR.match(line[match.start("name"):].rstrip().rstrip(";").split("//")[0].strip())
        return (match["indent"], base, [declarator]) if declarator else None
    return None


def local_declarations(lines: list[str]) -> dict[str, int]:
    """Local declarations, one name per declarator: name -> index of the declaring line."""
    found = {}
    for index, line in enumerate(lines):
        parsed = parse_declaration(line)
        for declarator in parsed[2] if parsed else []:
            found.setdefault(declarator["name"], index)
    return found


def repair_snippet(function_lines: list[str], first: int, last: int, snippet: list[str]) -> tuple[list[str], set[int]]:
    """Fix a focused snippet's local bookkeeping; the compiler and objdiff still judge the result.

    `function_lines[first:last]` is the region the snippet replaces. Returns the repaired
    snippet and the indices of function lines outside the region to delete.
    - A snippet declaration identical to one outside the region was moved into the region:
      the outside copy is deleted.
    - Any other snippet declaration of a name the rest of the function declares becomes an
      assignment, or is dropped when it has no initializer. An initializer with a call is left
      alone, because turning it into an assignment could move the call's side effects.
    - A region declaration the snippet dropped while other lines still use the name is
      restored at the snippet's top.
    """
    outside_index = list(range(first)) + list(range(last, len(function_lines)))
    outside = [function_lines[i] for i in outside_index]
    declared_outside = local_declarations(outside)
    declared_inside = local_declarations(function_lines[first:last])
    repaired, remove = [], set()
    for line in snippet:
        parsed = parse_declaration(line)
        # A name the region itself declared lives in its own block scope: leave that declaration be.
        clash = [d for d in parsed[2] if d["name"] in declared_outside and d["name"] not in declared_inside] if parsed else []
        if not clash:
            repaired.append(line)
            continue
        if len(parsed[2]) == 1:
            origin = outside_index[declared_outside[clash[0]["name"]]]
            if " ".join(function_lines[origin].split()) == " ".join(line.split()):
                remove.add(origin)
                repaired.append(line)
                continue
        if any(d["init"] and "(" in d["init"] for d in clash):
            repaired.append(line)
            continue
        indent, base, declarators = parsed
        keep = [d for d in declarators if d not in clash]
        if keep:
            repaired.append(f"{indent}{base} " + ", ".join(d.group(0).strip() for d in keep) + ";")
        repaired.extend(f"{indent}{d['name']} = {d['init'].strip()};" for d in clash if d["init"] and not d["array"])
    kept = local_declarations(repaired)
    region = function_lines[first:last]
    remaining = [line for i, line in zip(outside_index, outside) if i not in remove]
    restored = [region[index] for name, index in sorted(local_declarations(region).items(), key=lambda pair: pair[1])
                if name not in kept and name not in declared_outside
                and any(re.search(rf"\b{re.escape(name)}\b", line) for line in remaining + repaired)]
    return restored + repaired, remove


def apply_focused(source: str, symbol: str, region: tuple[int, int], response: str) -> str:
    """The whole function with the region replaced by the model's snippet."""
    blocks = re.findall(r"```(?:c|C)?\s*\n(.*?)```", response, flags=re.DOTALL)
    if not blocks:
        raise ValueError("focused response has no code block")
    snippet = blocks[0].rstrip("\n")
    if re.search(r"#\s*include\b|\b(?:__asm__|__asm|asm)\b|\.inc\b", snippet, flags=re.IGNORECASE):
        raise ValueError("candidate contains forbidden assembly or include")
    if re.search(rf"\b{re.escape(symbol)}\s*\([^;]*\)\s*\{{", snippet):
        # The model returned the whole definition after all: use it as a whole-function answer.
        return extract_function(response, symbol)
    lines = source.splitlines()
    start, end = function_span(source, symbol)
    first_line, last_line = source.count("\n", 0, start), source.count("\n", 0, end) + 1
    outside_text = "\n".join(lines[:region[0] - 1] + lines[region[1]:])
    kept_lines = []
    for line in snippet.splitlines():
        declared = re.match(r"^\s*extern\b[^;(]*?\b([A-Za-z_]\w*)\s*(?:\[[^\]]*\])?\s*;\s*$", line) \
            or re.match(r"^\s*extern\b[^;]*?\b([A-Za-z_]\w*)\s*\([^;]*\)\s*;\s*$", line)
        # An extern the file already declares elsewhere is a duplicate (often with a clashing type).
        if declared and re.search(rf"\b{re.escape(declared.group(1))}\b", outside_text):
            continue
        kept_lines.append(line)
    snippet = "\n".join(kept_lines)
    function_lines = lines[first_line:last_line]
    fixed, remove = repair_snippet(function_lines, region[0] - 1 - first_line, region[1] - first_line, snippet.splitlines())
    # `remove` holds function-relative indices of declarations the snippet moved into the region.
    drop = {first_line + index for index in remove}
    before = [line for index, line in enumerate(lines[:region[0] - 1]) if index not in drop]
    after = [line for index, line in enumerate(lines[region[1]:], region[1]) if index not in drop]
    edited = "\n".join(before + fixed + after) + "\n"
    start, end = function_span(edited, symbol)
    return edited[start:end].strip() + "\n"


def process(state: dict[str, Any], item: dict[str, Any], host: str, model: str, timeout: int, worker: str, feedback: str = "") -> None:
    source_path = ROOT / item.get("owner_source", item["source"])
    if not source_path.is_file():
        # A merge reshaped the units since the last sync; never let one stale task stop the worker.
        item.update(status="stale_source", last_error="owner source no longer exists; run sync")
        item["updated_at"] = timestamp()
        save_state(state)
        return
    source = source_path.read_text(encoding="utf-8", errors="replace")
    if digest(source) != item["source_sha256"]:
        item.update(status="stale_source", last_error="source changed; run sync")
        item["updated_at"] = timestamp()
        save_state(state)
        return
    if item.get("kind") == "cleanup":
        try:
            source = neutralize(source, item["symbol"], unit_cflags(item))
        except ValueError as exc:
            item.update(status="blocked_source_context", last_error=str(exc), updated_at=timestamp())
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
        retry = int(item.pop("retry_round", 0) or 0)
        temperature = RETRY_TEMPERATURES[0] if retry == 0 else \
            RETRY_TEMPERATURES[1 + (retry - 1) % (len(RETRY_TEMPERATURES) - 1)]
        region = None
        mode = item.pop("retry_mode", None) if feedback else None
        working, hunk_lines, current = source, item.get("focus_hunks") or [], None
        start, end = function_span(source, item["symbol"])
        best = item.get("best") or {}
        if exceeds_budget(source[start:end], int(state["settings"].get("ollama_num_predict", DEFAULT_NUM_PREDICT))):
            # Too long to return whole, and too long to quote twice: edit the assigned source.
            mode = "too_large"
        elif mode and best.get("focus_hunks") and best.get("candidate") and (ROOT / best["candidate"]).is_file():
            # Edit the best candidate, not the assigned source, so a focused fix keeps earlier gains.
            working = (ROOT / best["candidate"]).read_text(encoding="utf-8", errors="replace")
            hunk_lines = best["focus_hunks"]
            ws, we = function_span(working, item["symbol"])
            current = working[ws:we]
        if mode and hunk_lines:
            first = int(item.get("focus_round", 0))
            # Start at this round's hunk; skip hunks whose lines are too spread out to quote.
            for step in range(len(hunk_lines)):
                region = focus_region(working, item["symbol"], hunk_lines[(first + step) % len(hunk_lines)])
                if region:
                    item["focus_round"] = first + step
                    prompt = focused_prompt(prompt, working, region, mode, current)
                    break
        item["last_focused"] = bool(region)
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
            temperature=temperature,
        )
        (folder / f"attempt-{attempt:03d}.response.md").write_text(response, encoding="utf-8")
        activity(state, item, "Extracting one replacement function", "Rejecting any response that is not a complete definition for this symbol.", response_chars=len(response))
        proposal = apply_focused(working, item["symbol"], region, response) if region \
            else extract_function(response, item["symbol"])
        start, end = function_span(source, item["symbol"])
        ws, we = function_span(working, item["symbol"])
        if proposal.strip() in (source[start:end].strip(), working[ws:we].strip()):
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
        # verify() re-annotates the candidate in place: keep its hunk lines apart from the assigned source's.
        assigned_hunks = item.pop("focus_hunks", None)
        measured = verify(item, candidate, worker)
        candidate_hunks = item.pop("focus_hunks", None) or []
        if assigned_hunks is not None:
            item["focus_hunks"] = assigned_hunks
        if re.search(r"#\s*pragma\s+(?:optimization_level|optimize_for_size|scheduling|peephole|opt_propagation)\b", source):
            flags.append("owner contains legacy compiler controls; audit active target settings before promotion")
        measured.update(candidate=str(candidate.relative_to(ROOT)), review_flags=flags)
        item["candidate"] = measured["candidate"]
        item["last_report"] = measured
        # Keep the best-scoring attempt so later corrections build on it, not on a worse retry.
        if measured["pct"] is not None and measured["pct"] > float((item.get("best") or {}).get("pct") or item["base_pct"]):
            item["best"] = {"pct": measured["pct"], "attempt": attempt, "candidate": measured["candidate"],
                            "diff_feedback": measured.get("diff_feedback", []), "focus_hunks": candidate_hunks,
                            "at": timestamp()}
            item["focus_round"] = 0  # a new best has its own hunk list; start from its first hunk
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
    attempt = item["attempts"]
    best = item.get("best") or {}
    if best.get("pct") is not None and best["pct"] > float(report.get("pct") or 0):
        # The last attempt regressed: correct from the best candidate so far instead.
        attempt, report = best["attempt"], {**report, "pct": best["pct"], "diff_feedback": best.get("diff_feedback", [])}
    if item.get("status") == "no_change":
        # Echoing the unchanged function back only primes the model to copy it again.
        return ("Your previous answer was the assigned function unchanged, so it cannot match. "
                "This time change the source: take the first [structural] hunk in \"Real differences to fix\", "
                "find the C statement that produces those instructions, and rewrite that statement (its "
                "types, operand order, loop or branch shape, or which values live in locals) so the "
                "compiler emits the target's instructions. Keep everything else as it is. "
                "Preserve semantics and the exact signature; no compiler shaping.\n")
    response = STATE_DIR / "candidates" / item["id"] / f"attempt-{attempt:03d}.response.md"
    previous = response.read_text(encoding="utf-8") if response.exists() else ""
    return ("Correct your previous attempt. Preserve semantics and the exact signature. "
            "Do not repeat unchanged code or use compiler shaping.\n"
            f"Previous result: {item['status']}; baseline: {item['base_pct']}%; candidate: {report.get('pct')}.\n"
            + str(item.get("last_error", ""))[-1000:] + "\n"
            + "Remaining differences of your previous answer (\"produced by\" quotes your own lines):\n"
            + "\n\n".join(report.get("diff_feedback", []))[:3000] + "\n"
            + report.get("output_tail", "")[-1500:] + "\nPrevious response:\n" + previous[:5000])


def pause(seconds: float) -> None:
    """Sleep in short steps so SIGTERM still stops a backing-off worker promptly."""
    deadline = time.monotonic() + seconds
    while not STOP_REQUESTED and time.monotonic() < deadline:
        time.sleep(min(1.0, deadline - time.monotonic()))


def worker_slug(worker: str) -> str:
    clean = re.sub(r"[^A-Za-z0-9_.-]+", "-", worker.strip()).strip("-")
    return clean or "worker"


PRIORITY_FILE = STATE_DIR / "priority.json"


def best_pct(item: dict[str, Any]) -> float:
    return max(float(item.get("base_pct") or 0), float((item.get("best") or {}).get("pct") or 0))


def permute_eligible(item: dict[str, Any]) -> bool:
    """Near-exact tasks not yet searched for this exact source."""
    return (item.get("kind") != "cleanup" and item.get("status") != "review_exact" and best_pct(item) >= 97.0
            and (item.get("permuted") or {}).get("sha") != item.get("source_sha256"))


def permute_search(state: dict[str, Any], item: dict[str, Any], worker: str) -> float | None:
    """Try reorderings of the function's declaration block; keep the best as a candidate.

    Runs under the build lock because each try rewrites the owner source in place (always
    restored). Only declarations move, so semantics are unchanged; the result still goes
    through the normal promotion gates (policy, full report, SHA1).
    """
    owner = ROOT / item.get("owner_source", item["source"])
    record = {"sha": item.get("source_sha256"), "at": timestamp(), "tried": 0}
    with COORDINATOR.build(worker, f"Declaration-order search: {item['symbol']}"):
        original = owner.read_bytes()
        text = original.decode("utf-8", errors="replace")
        try:
            start, end = function_span(text, item["symbol"])
        except ValueError:
            return None
        function = text[start:end]
        block = permute.declaration_block(function)
        if digest(text) != item.get("source_sha256") or not block:
            item["permuted"] = {**record, "skipped": "no declaration block" if not block else "source changed"}
            save_state(state)
            return None
        begin, finish, lines = block
        activity(state, item, "Declaration-order search", f"Trying up to {PERMUTE_CAP} orders of {len(lines)} declarations.")
        best_text, deadline = None, time.monotonic() + PERMUTE_SECONDS
        try:
            with tempfile.TemporaryDirectory() as temporary:
                base = scratch_score(item, Path(temporary))
                if base is None:
                    return None
                top = base
                for order in permute.orderings(lines, PERMUTE_CAP):
                    if time.monotonic() > deadline or STOP_REQUESTED:
                        break
                    candidate = text[:start] + function[:begin] + "".join(order) + function[finish:] + text[end:]
                    owner.write_text(candidate, encoding="utf-8")
                    pct = scratch_score(item, Path(temporary))
                    record["tried"] += 1
                    if pct is not None and pct > top + 1e-9:
                        top, best_text = pct, candidate
                        if pct >= 100.0:
                            break
        finally:
            owner.write_bytes(original)
    record.update(base=base, best=top)
    item["permuted"] = record
    if best_text is not None and top > best_pct(item) + 1e-9:
        folder = STATE_DIR / "candidates" / item["id"]
        folder.mkdir(parents=True, exist_ok=True)
        path = folder / f"permute-{record['tried']:04d}.c"
        path.write_text(best_text, encoding="utf-8")
        item["best"] = {"pct": top, "attempt": "declaration-order", "candidate": str(path.relative_to(ROOT)),
                        "diff_feedback": [], "at": timestamp()}
        if top >= 100.0:
            item["status"] = "review_exact"
        event(state, "permute_improved", task=item["id"], symbol=item["symbol"], worker=worker,
              base=base, pct=top, tried=record["tried"])
    item["updated_at"] = timestamp()
    save_state(state)
    return top


def register_only(item: dict[str, Any]) -> bool:
    """True once a prompt has shown that only register-allocation differences remain."""
    return item.get("structural_hunks") == 0


def recycle_attempted(state: dict[str, Any], max_function_bytes: int = 0, batch: int = 40, min_pct: float = 0) -> int:
    """Re-queue the oldest finished attempts that predate the latest harness improvement.

    Lets a worker keep cycling through the whole unmatched corpus instead of stopping once
    every task has had one pass. Rejected candidates, source-less tasks and claimed owners stay out.
    """
    marks = [row["at"] for row in state.get("events", []) if row.get("kind") in ("harness_improved", "model_tuned")]
    since = max(marks) if marks else ""
    claims = set(COORDINATOR.snapshot().get("claims", {}))
    retryable = {"no_change", "non_exact", "incomplete_response", "verification_error", "error", "interrupted", "model_loop"}
    candidates = sorted(
        (item for item in state["items"].values()
         if item.get("status") in retryable and not item.get("promotion_rejected")
         and (item.get("last_attempt_at") or "") < since
         and (not max_function_bytes or item["size"] <= max_function_bytes)
         and float(item.get("base_pct") or 0) >= min_pct
         and item.get("owner_source", item["source"]) not in claims),
        key=lambda item: item.get("last_attempt_at") or "")[:batch]
    for item in candidates:
        item.update(status="pending", updated_at=timestamp())
    if candidates:
        event(state, "recycled", tasks=len(candidates), detail=f"re-queued attempts older than {since[:19]}")
        save_state(state)
    return len(candidates)


def local_priority(items: list[dict[str, Any]]) -> dict[str, Any]:
    """The explicit priority list with each target's live status, for the dashboard."""
    spec = read_json(PRIORITY_FILE, {})
    by_symbol = {item["symbol"]: item for item in items}
    rows = []
    for symbol in spec.get("symbols", []):
        item = by_symbol.get(symbol, {})
        best = (item.get("best") or {}).get("pct")
        rows.append({"symbol": symbol, "status": item.get("status", "not queued"), "base_pct": item.get("base_pct"),
                     "best_pct": best, "worker": item.get("worker"), "attempts": item.get("attempts", 0),
                     "promoted_pct": item.get("promoted_pct")})
    promoted = [item for item in items if item.get("promoted_pct")]
    return {"why": spec.get("why", ""), "at": spec.get("at"), "rows": rows,
            "promoted": sorted(({"symbol": item["symbol"], "pct": item["promoted_pct"]} for item in promoted),
                               key=lambda row: row["symbol"])}


def boot_focus_rank() -> dict[str, int]:
    """Explicit priority list first, then the boot loop's frontier (ignored once it stops refreshing)."""
    manual = read_json(PRIORITY_FILE, {}).get("symbols", [])
    rank = {symbol: index for index, symbol in enumerate(manual)}
    focus = read_json(BOOT_FOCUS_FILE, {})
    try:
        age = (datetime.now(UTC) - datetime.fromisoformat(focus["generated_at"])).total_seconds()
    except (KeyError, TypeError, ValueError):
        return rank
    if age > BOOT_FOCUS_MAX_AGE:
        return rank
    for index, target in enumerate(focus.get("targets", [])):
        rank.setdefault(target["symbol"], len(manual) + index)
    return rank


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
        "think": {None: "auto", True: "on", False: "off"}[THINK],
        **RUN_OPTIONS,
        "pid": os.getpid(),
        "updated_at": timestamp(),
    }
    save_state(state)


def run(state: dict[str, Any], host: str, model: str, limit: int, timeout: int, delay: float, worker: str, num_predict: int, max_function_bytes: int = 0, min_pct: float = 0) -> int:
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
            # The explicit priority list outranks a worker's size/closeness filters: those
            # targets were chosen by hand (usually recomp gates) and are worth the longer runs.
            prioritised = set(read_json(PRIORITY_FILE, {}).get("symbols", []))
            pending = [item for item in state["items"].values() if item.get("status") == "pending"
                       and (item["symbol"] in prioritised
                            or ((not max_function_bytes or item["size"] <= max_function_bytes)
                                and float(item.get("base_pct") or 0) >= min_pct))]
            if not pending:
                if RECYCLE and recycle_attempted(state, max_function_bytes, min_pct=min_pct):
                    continue
                break
            focus = boot_focus_rank()
            pending.sort(key=lambda item: (focus.get(item["symbol"], len(focus)), register_only(item), -int(item.get("value_score") or 0), -float(item["base_pct"]), item["residual_functions"], -item["size"], item["id"]))
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
                        best_seen, stalled = best_pct(item), 0
                        # Whole-function retries that echo the input or leave the score flat rarely
                        # recover, so once either happens the rest of the task uses focused edits.
                        improved, sticky = True, False
                        for retry in range(RETRIES):
                            feedback = retry_feedback(item)
                            if not feedback or STOP_REQUESTED:
                                break
                            if item["status"] == "no_change" and register_only(item):
                                # Nothing structural to fix and the model saw nothing to change.
                                break
                            if item.pop("last_focused", False) and not improved:
                                # That hunk did not give way: try the next one.
                                item["focus_round"] = int(item.get("focus_round", 0)) + 1
                            item["retry_round"] = retry + 1
                            if item["status"] == "no_change":
                                # A whole-function prompt produced a copy; ask for the hunk's lines only.
                                item["retry_mode"], sticky = "no_change", True
                            elif item["status"] == "incomplete_response":
                                # Double the cap for the retry, never below the worker's own setting, and
                                # ask for the hunk's lines only so the answer fits either way.
                                state["settings"]["ollama_num_predict"] = max(num_predict, min(16384, num_predict * 2))
                                item["retry_mode"] = "incomplete_response"
                            elif sticky or (item["status"] == "non_exact" and not improved):
                                item["retry_mode"], sticky = "plateau", True
                            process(state, item, host, model, timeout, worker, feedback)
                            improved = best_pct(item) > best_seen + 1e-9
                            if improved:
                                best_seen, stalled = best_pct(item), 0
                            else:
                                stalled += 1
                                if stalled >= STALL_ROUNDS:
                                    break
                        if PERMUTE and not STOP_REQUESTED and permute_eligible(item):
                            permute_search(state, item, worker)
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
            "local_priority": local_priority(items),
        },
        "recomp": recomp, "boot_loop": read_json(STATE_DIR / "boot_loop.json", {}),
        # Merged writers append out of order; show the newest events, not the last appended.
        "events": sorted(state.get("events", []), key=lambda row: row.get("at", ""), reverse=True)[:80], "snapshots": state.get("snapshots", []), "settings": state.get("settings", {}),
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
    parser.add_argument("--think", choices=("auto", "on", "off"), default="auto",
                        help="reasoning for thinking models: auto sends nothing (the model decides), "
                             "on/off set Ollama's think flag (default: auto)")
    commands =parser.add_subparsers(dest="command", required=True)
    sync_parser = commands.add_parser("sync", help="queue every source-backed, non-exact report function")
    sync_parser.add_argument("--reset", action="store_true", help="restart existing tasks from pending")
    run_parser = commands.add_parser("run", help="ask the local model to process pending tasks")
    run_parser.add_argument("--limit", type=int, default=0, help="maximum tasks; 0 means all pending")
    run_parser.add_argument("--timeout", type=int, default=900)
    run_parser.add_argument("--recycle", action="store_true",
                            help="when no task is pending, re-queue attempts made before the latest harness improvement")
    run_parser.add_argument("--retries", type=int, default=1,
                            help="correction attempts per task after the first, each given the latest diff (default: 1)")
    run_parser.add_argument("--delay", type=float, default=0)
    run_parser.add_argument("--max-function-bytes", type=int, default=0,
                            help="fast-worker size ceiling; 0 includes the whole queue")
    run_parser.add_argument("--stall-rounds", type=int, default=4,
                            help="stop a task's correction rounds after this many without a new best (default: 4)")
    run_parser.add_argument("--permute", action="store_true",
                            help="after the model rounds, search declaration orders for near-exact tasks")
    run_parser.add_argument("--permute-cap", type=int, default=720, help="maximum orders tried per task (default: 720)")
    run_parser.add_argument("--permute-seconds", type=int, default=600, help="time budget per task (default: 600)")
    run_parser.add_argument("--min-pct", type=float, default=0,
                            help="only take functions already at least this close to exact (default: 0)")
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
    global NUM_CTX, THINK, RETRIES, RECYCLE, STALL_ROUNDS, PERMUTE, PERMUTE_CAP, PERMUTE_SECONDS
    NUM_CTX = args.num_ctx
    RETRIES = max(0, getattr(args, "retries", 1))
    if args.command == "run":
        RECYCLE = args.recycle
        STALL_ROUNDS, PERMUTE = max(1, args.stall_rounds), args.permute
        PERMUTE_CAP, PERMUTE_SECONDS = max(1, args.permute_cap), max(10, args.permute_seconds)
        RUN_OPTIONS.update(stall_rounds=STALL_ROUNDS, permute=PERMUTE, retries=RETRIES, timeout=args.timeout, max_function_bytes=args.max_function_bytes, min_pct=args.min_pct, recycle=RECYCLE)
    THINK = {"auto": None, "on": True, "off": False}[args.think]
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
                return run(state, args.ollama_host, args.model, args.limit, args.timeout, args.delay, args.worker, args.num_predict, args.max_function_bytes, args.min_pct)
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
