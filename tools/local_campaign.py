#!/usr/bin/env python3
"""Local-LLM queue runner for strict, review-only decompilation proposals.

The runner never writes a proposal into tracked source. It asks Ollama for a
single replacement function, writes that to ignored campaign state, and calls
the repository's verifier. A 100% result remains a human-review candidate.
"""

from __future__ import annotations

import argparse
import hashlib
import json
import os
import re
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

ROOT = Path(__file__).resolve().parents[1]
STATE_DIR = ROOT / "build" / "local_llm_campaign"
STATE_FILE = STATE_DIR / "state.json"
LOCK_FILE = STATE_DIR / "runner.lock"
REPORT_FILE = ROOT / "build" / "GC6E01" / "report.json"
BRIEF_TOOL = ROOT / "tools" / "decomp_work" / "handoff" / "gen_brief.py"
VERIFY_TOOL = ROOT / "tools" / "decomp_work" / "handoff" / "verify.py"
DEFAULT_HOST = "http://dreamworld:11434"
DEFAULT_MODEL = "qwen2.5-coder:14b"


def timestamp() -> str:
    return datetime.now(UTC).isoformat(timespec="seconds")


def digest(value: str) -> str:
    return hashlib.sha256(value.encode("utf-8", errors="replace")).hexdigest()


def read_json(path: Path, default: Any) -> Any:
    return json.loads(path.read_text(encoding="utf-8")) if path.exists() else default


def write_json(path: Path, value: Any) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    temporary = path.with_suffix(path.suffix + ".tmp")
    temporary.write_text(json.dumps(value, indent=2, sort_keys=True) + "\n", encoding="utf-8")
    temporary.replace(path)


def blank_state(host: str, model: str) -> dict[str, Any]:
    return {
        "schema": 1,
        "created_at": timestamp(),
        "settings": {"ollama_host": host.rstrip("/"), "ollama_model": model},
        "items": {},
        "events": [],
        "snapshots": [],
    }


def load_state(host: str, model: str) -> dict[str, Any]:
    state = read_json(STATE_FILE, blank_state(host, model))
    if state.get("schema") != 1:
        raise RuntimeError(f"unsupported state schema: {STATE_FILE}")
    state.setdefault("items", {})
    state.setdefault("events", [])
    state.setdefault("snapshots", [])
    state.setdefault("settings", {})
    state["settings"].update({"ollama_host": host.rstrip("/"), "ollama_model": model})
    return state


def save_state(state: dict[str, Any]) -> None:
    state["updated_at"] = timestamp()
    state["events"] = state["events"][-500:]
    state["snapshots"] = state["snapshots"][-500:]
    write_json(STATE_FILE, state)


def event(state: dict[str, Any], kind: str, **details: Any) -> None:
    state["events"].append({"at": timestamp(), "kind": kind, **details})


def activity(state: dict[str, Any], item: dict[str, Any], phase: str, detail: str, **metrics: Any) -> None:
    """Persist the exact worker phase so the dashboard survives a refresh."""
    previous = item.get("activity") or {}
    item["activity"] = {
        "started_at": previous.get("started_at") or timestamp(),
        "updated_at": timestamp(),
        "phase": phase,
        "detail": detail,
        **metrics,
    }
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
    """Resolve a current score-only .c include shim to its existing owner."""
    path = ROOT / source
    if not path.is_file():
        return source
    includes = re.findall(r'^\s*#\s*include\s+"(src/[^"\n]+\.c)"', path.read_text(encoding="utf-8", errors="replace"), flags=re.MULTILINE)
    if len(includes) == 1 and (ROOT / includes[0]).is_file():
        return includes[0]
    return source


def sync(state: dict[str, Any], reset: bool = False) -> dict[str, int]:
    refreshed = subprocess.run(
        ["ninja", "all_source", "build/GC6E01/report.json"],
        cwd=ROOT, capture_output=True, text=True, timeout=1800,
    )
    if refreshed.returncode:
        raise RuntimeError("authoritative report rebuild failed: " + (refreshed.stderr or refreshed.stdout)[-1600:])
    if not REPORT_FILE.exists():
        raise RuntimeError("report target completed without creating report.json")
    report = read_json(REPORT_FILE, {})
    grouped = list(functions_in_report(report))
    residuals = Counter(
        unit["name"] for unit, function in grouped
        if float(function.get("fuzzy_match_percent") or 0) < 100.0
    )
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
        owner = owner_source(source)
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
            "source_sha256": digest(owner_path.read_text(encoding="utf-8", errors="replace")),
            "updated_at": timestamp(),
        }
        old = state["items"].get(task)
        if old and not reset:
            for key in ("status", "attempts", "last_attempt_at", "last_report", "last_error", "candidate"):
                if key in old:
                    record[key] = old[key]
            if old.get("source_sha256") != record["source_sha256"] and record.get("status") not in {"review_exact", "running"}:
                record["status"] = "stale_source"
        else:
            record.update({"status": "pending", "attempts": 0})
        refreshed[task] = record
    removed = len(set(state["items"]) - set(refreshed))
    state["items"] = refreshed
    counts = Counter(item["status"] for item in refreshed.values())
    measures = report.get("measures") or {}
    state["snapshots"].append({
        "at": timestamp(),
        "fuzzy_match_percent": measures.get("fuzzy_match_percent", 0),
        "matched_functions": measures.get("matched_functions", 0),
        "worklist": len(refreshed),
        "review_exact": counts["review_exact"],
        "attempted": sum(value for key, value in counts.items() if key not in {"pending", "running"}),
    })
    event(state, "queue_synced", pending=counts["pending"], removed=removed, report_rebuilt=True)
    save_state(state)
    return {"open": len(refreshed), "pending": counts["pending"], "removed": removed}


def function_span(text: str, symbol: str) -> tuple[int, int]:
    """Find a definition while ignoring declarations and balanced literals."""
    pattern = re.compile(rf"\b{re.escape(symbol)}\s*\(")
    for match in pattern.finditer(text):
        open_paren = text.find("(", match.start())
        try:
            after_params = balanced_end(text, open_paren, "(", ")")
        except ValueError:
            continue
        cursor = skip_space_and_comments(text, after_params)
        if cursor >= len(text) or text[cursor] != "{":
            continue
        start = text.rfind("\n", 0, match.start()) + 1
        return start, balanced_end(text, cursor, "{", "}")
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


def build_prompt(item: dict[str, Any], source: str) -> tuple[str, str]:
    result = subprocess.run(
        [sys.executable, str(BRIEF_TOOL), "--source", item["source"], "--symbol", item["symbol"]],
        cwd=ROOT, capture_output=True, text=True, timeout=120,
    )
    if result.returncode:
        raise RuntimeError((result.stderr or result.stdout).strip()[-1600:])
    brief = result.stdout.split("## Current C source", 1)[0].rstrip()
    start, end = function_span(source, item["symbol"])
    prompt = f"""{brief}

## Assigned function source

```c
{source[start:end].strip()}
```

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


def ollama(host: str, model: str, prompt: str, timeout: int, progress) -> str:
    request = urllib.request.Request(
        host.rstrip("/") + "/api/generate",
        data=json.dumps({
            "model": model, "prompt": prompt, "stream": True, "keep_alive": "20m",
            "options": {"temperature": 0.15, "num_ctx": 32768},
        }).encode(),
        headers={"Content-Type": "application/json"},
    )
    try:
        with urllib.request.urlopen(request, timeout=timeout) as response:
            chunks, last_update = [], 0.0
            for line in response:
                if not line.strip():
                    continue
                packet = json.loads(line)
                piece = packet.get("response")
                if isinstance(piece, str):
                    chunks.append(piece)
                moment = time.monotonic()
                if moment - last_update >= 1.0 or packet.get("done"):
                    answer_so_far = "".join(chunks)
                    progress(
                        response_chars=len(answer_so_far),
                        response_chunks=len(chunks),
                        response_preview=answer_so_far[-320:],
                        eval_count=packet.get("eval_count"),
                        eval_duration_ns=packet.get("eval_duration"),
                    )
                    last_update = moment
            answer = "".join(chunks)
    except urllib.error.URLError as exc:
        raise RuntimeError(f"Ollama request failed: {exc}") from exc
    if not isinstance(answer, str) or not answer.strip():
        raise RuntimeError("Ollama returned no candidate")
    return answer


def verify(item: dict[str, Any], candidate: Path, link: bool = False) -> dict[str, Any]:
    if item.get("owner_source", item["source"]) != item["source"]:
        return verify_shim_owner(item, candidate)
    command = [sys.executable, str(VERIFY_TOOL), "--source", item["source"], "--symbol", item["symbol"], "--candidate", str(candidate)]
    if link:
        command.append("--link")
    result = subprocess.run(command, cwd=ROOT, capture_output=True, text=True, timeout=900)
    output = (result.stdout + "\n" + result.stderr).strip()
    match = re.search(rf"^{re.escape(item['symbol'])}:\s+([0-9.]+)%.*?deltas=(\d+)", output, re.MULTILINE)
    return {
        "returncode": result.returncode,
        "pct": float(match.group(1)) if match else None,
        "deltas": int(match.group(2)) if match else None,
        "linked": "main.dol: OK" in output,
        "output_tail": output[-6000:],
    }


def objdiff_cli() -> Path:
    for name in ("objdiff-cli.exe", "objdiff-cli"):
        path = ROOT / "build" / "tools" / name
        if path.exists():
            return path
    raise RuntimeError("objdiff-cli is missing; run ninja once")


def verify_shim_owner(item: dict[str, Any], candidate: Path) -> dict[str, Any]:
    """Measure an ignored owner candidate through its declared shim unit."""
    owner = ROOT / item["owner_source"]
    compiled = ROOT / "build" / "GC6E01" / Path(item["source"]).with_suffix(".o")
    original = owner.read_text(encoding="utf-8", errors="replace")
    output = ""
    try:
        owner.write_text(candidate.read_text(encoding="utf-8"), encoding="utf-8")
        compiled.unlink(missing_ok=True)
        build = subprocess.run(["ninja", str(compiled.relative_to(ROOT))], cwd=ROOT, capture_output=True, text=True, timeout=900)
        output = (build.stdout + "\n" + build.stderr).strip()
        if build.returncode or not compiled.exists():
            return {"returncode": 1, "pct": None, "deltas": None, "linked": False, "output_tail": output[-6000:]}
        with tempfile.TemporaryDirectory() as temporary:
            diff_path = Path(temporary) / "diff.json"
            diff = subprocess.run(
                [str(objdiff_cli()), "diff", "-p", str(ROOT), "-u", item["unit"], "-o", str(diff_path), "--format", "json", item["symbol"]],
                cwd=ROOT, capture_output=True, text=True, timeout=120,
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
            return {"returncode": 0 if pct == 100.0 else 1, "pct": pct, "deltas": deltas, "linked": False, "output_tail": output[-6000:]}
    finally:
        owner.write_text(original, encoding="utf-8")
        compiled.unlink(missing_ok=True)
        subprocess.run(["ninja", str(compiled.relative_to(ROOT))], cwd=ROOT, capture_output=True, text=True, timeout=900)


def process(state: dict[str, Any], item: dict[str, Any], host: str, model: str, timeout: int) -> None:
    source_path = ROOT / item.get("owner_source", item["source"])
    source = source_path.read_text(encoding="utf-8", errors="replace")
    if digest(source) != item["source_sha256"]:
        item.update(status="stale_source", last_error="source changed; run sync")
        save_state(state)
        return
    item["status"] = "running"
    item["attempts"] = int(item.get("attempts", 0)) + 1
    item["last_attempt_at"] = timestamp()
    activity(state, item, "Checking source baseline", "Confirming the queued owner source has not changed.")
    attempt = item["attempts"]
    folder = STATE_DIR / "candidates" / item["id"]
    folder.mkdir(parents=True, exist_ok=True)
    try:
        activity(state, item, "Generating objdiff brief", "Reading the target diff and active compiler flags.")
        brief, prompt = build_prompt(item, source)
        (folder / f"attempt-{attempt:03d}.brief.md").write_text(brief, encoding="utf-8")
        (folder / f"attempt-{attempt:03d}.prompt.md").write_text(prompt, encoding="utf-8")
        activity(
            state, item, "Generating candidate with local model", "Streaming the Ollama response from dreamworld.",
            prompt_chars=len(prompt), response_chars=0, response_chunks=0,
        )
        response = ollama(
            host, model, prompt, timeout,
            lambda **metrics: activity(
                state, item, "Generating candidate with local model", "Streaming the Ollama response from dreamworld.", **metrics,
            ),
        )
        (folder / f"attempt-{attempt:03d}.response.md").write_text(response, encoding="utf-8")
        activity(state, item, "Extracting one replacement function", "Rejecting any response that is not a complete definition for this symbol.", response_chars=len(response))
        proposal = extract_function(response, item["symbol"])
        start, end = function_span(source, item["symbol"])
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
        measured = verify(item, candidate)
        measured.update(candidate=str(candidate.relative_to(ROOT)), review_flags=flags)
        item["candidate"] = measured["candidate"]
        item["last_report"] = measured
        if measured["pct"] == 100.0:
            if item.get("owner_source", item["source"]) != item["source"]:
                measured["link_gate"] = {"skipped": "owner is shared through an existing score shim; require full-TU review before promotion"}
            else:
                activity(state, item, "Running full link gate", "Text is exact; checking the retail DOL gate before review.")
                measured["link_gate"] = verify(item, candidate, link=True)
            item["status"] = "review_exact"
            activity(state, item, "Exact candidate awaiting human review", "All automatic gates completed; no source was promoted.", completed_at=timestamp())
            event(state, "review_exact", task=item["id"], symbol=item["symbol"], linked=measured["link_gate"].get("linked", False))
        elif measured["pct"] is None:
            item.update(status="verification_error", last_error=measured["output_tail"][-1200:])
            activity(state, item, "Verification failed", "The candidate could not be measured; inspect the retained report.", completed_at=timestamp())
            event(state, "verification_error", task=item["id"])
        else:
            item["status"] = "non_exact"
            activity(state, item, "Candidate measured non-exact", f"objdiff measured {measured['pct']:.5f}% with {measured['deltas']} instruction deltas.", completed_at=timestamp())
            event(state, "non_exact", task=item["id"], symbol=item["symbol"], pct=measured["pct"])
    except (OSError, RuntimeError, subprocess.SubprocessError, TimeoutError, ValueError) as exc:
        item.update(status="error", last_error=str(exc)[-1800:])
        activity(state, item, "Runner error", item["last_error"], completed_at=timestamp())
        event(state, "error", task=item["id"], detail=item["last_error"])
    finally:
        save_state(state)


def acquire_lock() -> None:
    STATE_DIR.mkdir(parents=True, exist_ok=True)
    if LOCK_FILE.exists():
        try:
            pid = int(LOCK_FILE.read_text().strip())
            os.kill(pid, 0)
        except (ValueError, ProcessLookupError):
            LOCK_FILE.unlink(missing_ok=True)
        else:
            raise RuntimeError(f"runner already active: pid {pid}")
    LOCK_FILE.write_text(f"{os.getpid()}\n", encoding="ascii")


def release_lock() -> None:
    if LOCK_FILE.exists() and LOCK_FILE.read_text().strip() == str(os.getpid()):
        LOCK_FILE.unlink()


def run(state: dict[str, Any], host: str, model: str, limit: int, timeout: int, delay: float) -> int:
    acquire_lock()
    try:
        recovered = 0
        for item in state["items"].values():
            if item.get("status") == "running":
                item.update(status="pending", last_error="recovered after interrupted runner")
                recovered += 1
        if recovered:
            event(state, "runner_recovered", tasks=recovered)
            save_state(state)
        processed = 0
        while not limit or processed < limit:
            pending = [item for item in state["items"].values() if item.get("status") == "pending"]
            if not pending:
                break
            pending.sort(key=lambda item: (-float(item["base_pct"]), item["residual_functions"], -item["size"], item["id"]))
            process(state, pending[0], host, model, timeout)
            processed += 1
            if delay:
                time.sleep(delay)
        print(json.dumps({"processed": processed, "remaining": sum(item.get("status") == "pending" for item in state["items"].values())}, indent=2))
        return 0
    finally:
        release_lock()


def dashboard() -> dict[str, Any]:
    state = load_state(DEFAULT_HOST, DEFAULT_MODEL)
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
    return {
        "generated_at": timestamp(), "report_measures": report.get("measures") or {}, "categories": report.get("categories") or [], "maps": maps,
        "queue": {
            "total": len(items), "status": Counter(item.get("status", "pending") for item in items),
            "active": [item for item in items if item.get("status") == "running"],
            "review": sorted((item for item in items if item.get("status") == "review_exact"), key=lambda item: item.get("last_attempt_at", ""), reverse=True)[:100],
        },
        "events": list(reversed(state.get("events", [])[-80:])), "snapshots": state.get("snapshots", []), "settings": state.get("settings", {}),
    }


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--ollama-host", default=os.environ.get("OLLAMA_HOST", DEFAULT_HOST))
    parser.add_argument("--model", default=os.environ.get("OLLAMA_MODEL", DEFAULT_MODEL))
    commands = parser.add_subparsers(dest="command", required=True)
    sync_parser = commands.add_parser("sync", help="queue every source-backed, non-exact report function")
    sync_parser.add_argument("--reset", action="store_true", help="restart existing tasks from pending")
    run_parser = commands.add_parser("run", help="ask the local model to process pending tasks")
    run_parser.add_argument("--limit", type=int, default=0, help="maximum tasks; 0 means all pending")
    run_parser.add_argument("--timeout", type=int, default=900)
    run_parser.add_argument("--delay", type=float, default=0)
    commands.add_parser("status", help="show campaign state")
    args = parser.parse_args()
    state = load_state(args.ollama_host, args.model)
    if args.command == "sync":
        print(json.dumps(sync(state, args.reset), indent=2))
    elif args.command == "run":
        if not state["items"]:
            sync(state)
        return run(state, args.ollama_host, args.model, args.limit, args.timeout, args.delay)
    else:
        print(json.dumps({"state": str(STATE_FILE.relative_to(ROOT)), "counts": Counter(item.get("status", "pending") for item in state["items"].values()), "settings": state["settings"]}, indent=2))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
