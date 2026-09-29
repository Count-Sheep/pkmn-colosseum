"""Read-only filesystem timestamps for the live campaign dashboard.

File mtimes describe activity, not successful validation. In particular, a
source edit never changes the canonical objdiff measurements until report.json
is rebuilt. Keep that distinction visible to remote dashboard viewers.
"""

from __future__ import annotations

import os
import hashlib
import re
from datetime import UTC, datetime
from pathlib import Path
from typing import Any, Iterable


_retail_cache: dict[str, Any] = {"key": None, "value": None}

_probe_complete = re.compile(r"Diagnostic fn_800057B0 prefix executed (\d+) of (\d+) source-order calls")
_probe_stop = re.compile(r"fn_800057B0 native port stops at call (\d+) of (\d+): ([A-Za-z0-9_]+)")
_probe_diagnostic_stop = re.compile(
    r"Diagnostic fn_800057B0 prefix next unbound call (\d+) of (\d+): ([A-Za-z0-9_]+)"
)


def _boot_probe(log_dir: Path) -> dict[str, Any] | None:
    """Summarize the newest diagnostic boot log; never expose disc data or log text."""
    if not log_dir.is_dir():
        return None
    try:
        logs = sorted(
            (path for path in log_dir.glob("colosseum-recomp-*.log") if path.is_file()),
            key=lambda path: path.stat().st_mtime_ns, reverse=True,
        )[:20]
    except OSError:
        return None
    for log in logs:
        try:
            stat = log.stat()
            with log.open("rb") as stream:
                stream.seek(max(0, stat.st_size - 262_144))
                tail = stream.read().decode("utf-8", errors="replace")
        except OSError:  # log rotation or a writer replacing the file
            continue
        failed = "Diagnostic fn_800057B0 prefix failed:" in tail
        complete = _probe_complete.search(tail)
        diagnostic_stop = _probe_diagnostic_stop.search(tail)
        stop = diagnostic_stop or _probe_stop.search(tail)
        if not failed and not complete:
            continue
        at = datetime.fromtimestamp(stat.st_mtime_ns / 1_000_000_000, UTC).isoformat(timespec="seconds")
        if failed:
            return {"at": at, "path": "Diagnostic startup prefix failed; inspect app log", "status": "failed",
                    "mtime_ns": stat.st_mtime_ns}
        if not stop:
            continue
        executed, total = map(int, complete.groups())
        next_call, stop_total = map(int, stop.groups()[:2])
        if total != stop_total or next_call != executed + 1:
            continue
        target = stop.group(3)
        path = f"{executed}/{total} calls; next #{next_call} {target} (safe stop)"
        if diagnostic_stop:
            path = f"Diagnostic {path}; production readiness unchanged"
        return {"at": at, "path": path,
                "status": "safe-stop", "executed": executed, "total": total,
                "next_call": next_call, "next_target": target,
                "diagnostic_only": diagnostic_stop is not None, "mtime_ns": stat.st_mtime_ns}
    return None


def _retail_hashes(root: Path) -> dict[str, Any] | None:
    """Check only the two declared retail outputs; cache by file metadata."""
    manifest = root / "config/GC6E01/build.sha1"
    if not manifest.is_file():
        return None
    expected: list[tuple[str, Path]] = []
    for line in manifest.read_text(encoding="utf-8").splitlines():
        parts = line.split(maxsplit=1)
        if len(parts) != 2 or len(parts[0]) != 40:
            continue
        relative = parts[1].lstrip("* ")
        if relative not in ("build/GC6E01/main.dol", "build/GC6E01/common_rel/common_rel.rel"):
            continue
        expected.append((parts[0].lower(), root / relative))
    if len(expected) != 2:
        return None
    key = (str(root), manifest.stat().st_mtime_ns,
           tuple((str(path), path.stat().st_mtime_ns, path.stat().st_size) if path.exists() else (str(path), 0, 0)
                 for _, path in expected))
    if _retail_cache["key"] == key:
        return _retail_cache["value"]
    files = []
    for digest, path in expected:
        if not path.is_file():
            files.append({"path": path.relative_to(root).as_posix(), "status": "missing"})
            continue
        with path.open("rb") as built:
            actual = hashlib.file_digest(built, "sha1").hexdigest()
        files.append({"path": path.relative_to(root).as_posix(), "status": "match" if actual == digest else "mismatch"})
    status = "missing" if any(row["status"] == "missing" for row in files) else (
        "match" if all(row["status"] == "match" for row in files) else "mismatch")
    value = {"status": status, "files": files}
    _retail_cache.update(key=key, value=value)
    return value


def _newest(root: Path, paths: Iterable[str], suffixes: set[str] | None = None) -> dict[str, Any] | None:
    latest: tuple[int, Path] | None = None
    for name in paths:
        path = root / name
        if path.is_file():
            candidates = [path]
        elif path.is_dir():
            candidates = (
                Path(folder) / filename
                for folder, _, files in os.walk(path)
                for filename in files
            )
        else:
            continue
        for candidate in candidates:
            if suffixes is not None and candidate.suffix.lower() not in suffixes:
                continue
            try:
                changed = candidate.stat().st_mtime_ns
            except OSError:  # a build or editor may replace a file mid-scan
                continue
            if latest is None or changed > latest[0]:
                latest = changed, candidate
    if latest is None:
        return None
    changed, path = latest
    return {
        "at": datetime.fromtimestamp(changed / 1_000_000_000, UTC).isoformat(timespec="seconds"),
        "path": path.relative_to(root).as_posix(),
        "mtime_ns": changed,
    }


def freshness(decomp_root: Path, recomp_root: Path, log_dir: Path | None = None) -> dict[str, Any]:
    """Summarize observable edits and build outputs without starting a build."""
    decomp_source = _newest(decomp_root, ("src", "include", "config", "configure.py"))
    decomp_work = _newest(decomp_root, ("src", "include", "config", "docs", "configure.py"))
    decomp_report = _newest(decomp_root, ("build/GC6E01/report.json",))
    decomp_build = _newest(decomp_root, (
        "build/GC6E01/main.dol", "build/GC6E01/common_rel/common_rel.rel",
    ))
    recomp_source = _newest(recomp_root, (
        "src", "include", "CMakeLists.txt", "CMakePresets.json",
    ))
    recomp_work = _newest(recomp_root, (
        "src", "include", "tests", "docs", "tools", "CMakeLists.txt", "CMakePresets.json",
    ))
    recomp_build = _newest(recomp_root, (
        "build/macos-debug/Pokemon Colosseum Recomp.app/Contents/MacOS/Pokemon Colosseum Recomp",
        "build/macos-debug/colosseum-target-tests",
        "build/macos-debug/colosseum-runtime-tests",
        "build/macos-debug/colosseum-hsd-tests",
        "build/macos-debug/colosseum-card-transport-tests",
    ))
    recomp_app = _newest(recomp_root, (
        "build/macos-debug/Pokemon Colosseum Recomp.app/Contents/MacOS/Pokemon Colosseum Recomp",
    ))
    recomp_test = _newest(recomp_root, ("build/macos-debug/Testing/Temporary/LastTest.log",))
    if log_dir is None:
        log_dir = Path.home() / "Library/Application Support/Count Sheep/Pokemon Colosseum Recomp/logs"
    recomp_probe = _boot_probe(log_dir)
    records = (
        ("decomp", decomp_work), ("decomp", decomp_build),
        ("recomp", recomp_work), ("recomp", recomp_build), ("recomp", recomp_test),
        ("recomp", recomp_probe),
    )
    latest = max(((project, row) for project, row in records if row), key=lambda pair: pair[1]["mtime_ns"], default=None)
    latest_work = {"project": latest[0], **latest[1]} if latest else None
    if latest_work:
        latest_work.pop("mtime_ns", None)
    for _, row in records:
        if row:
            row.pop("mtime_ns")
    result = {
        "latest_work": latest_work,
        "decomp": {
            "source": decomp_source,
            "work": decomp_work,
            "report": decomp_report,
            "build": decomp_build,
            "retail_hashes": _retail_hashes(decomp_root),
            "report_behind_source": bool(decomp_source and decomp_report and decomp_source["mtime_ns"] > decomp_report["mtime_ns"]),
        },
        "recomp": {
            "source": recomp_source,
            "work": recomp_work,
            "build": recomp_build,
            "app": recomp_app,
            "test": recomp_test,
            "boot_probe": recomp_probe,
            "app_behind_source": bool(recomp_source and recomp_app and recomp_source["mtime_ns"] > recomp_app["mtime_ns"]),
        },
    }
    for project in (result["decomp"], result["recomp"]):
        for row in project.values():
            if isinstance(row, dict):
                row.pop("mtime_ns", None)
    return result
