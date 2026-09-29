"""Read-only bridge to the native recomp's boot-blocker evaluator.

The recomp owns the boot-critical manifest and measures it against this
checkout's report, so its current blocker advances as matching lands here.
Nothing in this module affects queue order, acceptance, or promotion.
"""

from __future__ import annotations

import importlib.util
import json
import os
import time
from datetime import UTC, datetime
from pathlib import Path
from typing import Any

CACHE_SECONDS = 15
_cache: dict[str, Any] = {"key": None, "value": None}


def recomp_root(decomp_root: Path) -> Path:
    return Path(os.environ.get("COLO_RECOMP_ROOT", decomp_root.parent / "Pokemon-Recomp"))


def recomp_status(decomp_root: Path, report_file: Path) -> dict[str, Any]:
    root = recomp_root(decomp_root)
    tool = root / "tools" / "boot_status.py"
    if not tool.exists():
        return {"available": False, "error": f"{tool} not found; set COLO_RECOMP_ROOT to the recomp checkout"}
    progress_file = root / "docs" / "port_progress.json"
    inputs = (tool, tool.with_name("boot_blockers.json"), report_file, progress_file)
    key = (str(tool), tuple(path.stat().st_mtime if path.exists() else 0 for path in inputs),
           int(time.time() // CACHE_SECONDS))
    if _cache["key"] != key:
        try:
            spec = importlib.util.spec_from_file_location("colo_recomp_boot_status", tool)
            module = importlib.util.module_from_spec(spec)
            spec.loader.exec_module(module)
            value = {"available": True, **module.evaluate(decomp_root)}
            try:
                progress = json.loads(progress_file.read_text(encoding="utf-8"))
                value["native_ports"] = {
                    "done": len(progress.get("done", [])), "skipped": len(progress.get("skipped", [])),
                    "updated_at": datetime.fromtimestamp(progress_file.stat().st_mtime, UTC).isoformat(timespec="seconds"),
                }
            except (OSError, ValueError):
                value["native_ports"] = None
            _cache["value"] = value
        except Exception as error:  # the dashboard must stay up if the recomp tool breaks
            _cache["value"] = {"available": False, "error": f"{type(error).__name__}: {error}"}
        _cache["key"] = key
    return _cache["value"]


def boot_index(status: dict[str, Any]) -> dict[str, dict[str, Any]]:
    """Map unaccepted entry functions and callees to their earliest path row."""
    index: dict[str, dict[str, Any]] = {}
    for blocker in status.get("blockers", []):
        rows = [*blocker.get("functions", []), *(blocker.get("dependencies") or {}).get("blocking", [])]
        for row in rows:
            symbol = row.get("symbol")
            if row.get("status") != "accepted" and symbol and symbol not in index:
                index[symbol] = {"index": blocker["index"], "title": blocker["title"],
                                 "current": blocker["index"] == (status.get("current") or {}).get("index")}
    return index
