#!/usr/bin/env python3
"""Measure local source changes against retail through the maintainer's diff bridge.

For cloud sessions, which have no retail disc and so cannot build a report or run objdiff. The
bridge (Pokemon-Decomp-Harness, harness/cloud_bridge.py) checks out a commit that is on GitHub,
lays the files you send over it, rebuilds, and answers with the unit's match and an aligned
objdiff of each function that is not exact.

Setup: DECOMP_BRIDGE_URL (https://...ts.net) and DECOMP_BRIDGE_TOKEN in the environment.

  python3 tools/cloud_diff.py                        # every changed file under src/ and include/
  python3 tools/cloud_diff.py src/hsd/bytecode.c     # just these files
  python3 tools/cloud_diff.py src/x.c --symbol fn_80123456 --task "fn_80123456 loop shape"
  python3 tools/cloud_diff.py --unit main/hsd/jobj include/hsd/jobj.h
  python3 tools/cloud_diff.py --link                 # also link main.dol and check SHA-1s

The base commit is HEAD when HEAD is on origin, otherwise its merge-base with origin/master;
files that differ from the base are sent. Changes to configure.py or config/ can't be sent as
files: commit and push them to a branch, then pass --ref <branch>.
"""

from __future__ import annotations

import argparse
import json
import os
import socket
import subprocess
import sys
import urllib.error
import urllib.request
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
SOURCE_SUFFIXES = (".c", ".h", ".cp", ".cpp", ".hpp")


def git(*args: str) -> str:
    return subprocess.run(["git", *args], cwd=ROOT, capture_output=True, text=True, check=True).stdout.strip()


def git_ok(*args: str) -> bool:
    return subprocess.run(["git", *args], cwd=ROOT, capture_output=True).returncode == 0


def base_commit() -> str:
    git_ok("fetch", "--quiet", "origin", "master")
    if git("branch", "-r", "--contains", "HEAD"):
        return git("rev-parse", "HEAD")
    return git("merge-base", "HEAD", "origin/master")


def changed_paths(base: str) -> list[str]:
    tracked = git("diff", "--name-only", base, "--", "src", "include", "configure.py", "config").splitlines()
    untracked = git("ls-files", "--others", "--exclude-standard", "--", "src", "include").splitlines()
    return sorted(set(tracked) | set(untracked))


def session_name() -> str:
    if os.environ.get("DECOMP_BRIDGE_SESSION"):
        return os.environ["DECOMP_BRIDGE_SESSION"]
    try:
        branch = git("rev-parse", "--abbrev-ref", "HEAD")
    except subprocess.CalledProcessError:
        branch = "detached"
    return f"{branch}@{socket.gethostname()}"


def print_result(result: dict) -> None:
    status = result.get("status") or ("ok" if result.get("ok") else "failed")
    if result.get("id"):
        print(f"[bridge] {status} in {result.get('seconds')}s at {str(result.get('sha') or '?')[:10]} (request {result['id']})")
    else:
        print(f"[bridge] {status}")
    if result.get("error"):
        print(result["error"])
    if result.get("build_output"):
        print(result["build_output"])
    for unit in result.get("units", []):
        flag = ("exact" if unit["exact"] else "not exact") + (", linked" if unit["linked"] else ", not linked")
        print(f"\n{unit['name']}: {unit['fuzzy']:.4f}% ({unit['matched_functions']}/{unit['total_functions']} functions, {flag})")
        for section in unit["sections"]:
            print(f"  {section['name']:<10} {section['size']:>7} B  {section['fuzzy'] if section['fuzzy'] is not None else '-'}")
        for function in unit["functions"]:
            if (function["fuzzy"] or 0) < 100:
                print(f"  {function['fuzzy'] or 0:8.4f}%  {function['size']:>6} B  {function['name']}")
    for diff in result.get("diffs", []):
        print(f"\n== {diff['symbol']}: " + (diff.get("error") or
              f"{diff.get('match')}% · {diff.get('differing_lines')} differing lines "
              f"({diff.get('target_instructions')} target / {diff.get('our_instructions')} ours instructions)"))
        if diff.get("diff"):
            print(diff["diff"])
    link = result.get("link")
    if link:
        print("\nlink:", "OK" if link["ok"] else "FAILED")
        for check in link["sha1"]:
            print(f"  {check['file']}: {check['result']}")
        if link.get("output"):
            print(link["output"])


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__.split("\n\n")[0])
    parser.add_argument("paths", nargs="*", help="files to send (default: everything changed under src/ and include/)")
    parser.add_argument("--ref", help="branch or commit on origin to build on (default: see above)")
    parser.add_argument("--unit", action="append", default=[], help="scoring unit, e.g. main/hsd/bytecode")
    parser.add_argument("--symbol", action="append", default=[], help="function to diff (default: the unit's non-exact ones)")
    parser.add_argument("--task", default="", help="one line on what you are trying; shown on the maintainer's dashboard")
    parser.add_argument("--context", type=int, default=3, help="matching lines shown around each difference")
    parser.add_argument("--link", action="store_true", help="also link and check the SHA-1s (slow)")
    parser.add_argument("--json", action="store_true", help="print the raw JSON answer")
    args = parser.parse_args()

    url, token = os.environ.get("DECOMP_BRIDGE_URL"), os.environ.get("DECOMP_BRIDGE_TOKEN")
    if not url or not token:
        print("DECOMP_BRIDGE_URL and DECOMP_BRIDGE_TOKEN must be set", file=sys.stderr)
        return 2

    ref = args.ref or base_commit()
    paths = args.paths or changed_paths(ref)
    blocked = [p for p in paths if p == "configure.py" or p.startswith("config/")]
    if blocked:
        print(f"{', '.join(blocked)} changed: commit and push them to a branch, then pass --ref <branch>", file=sys.stderr)
        return 2
    files = {}
    for path in paths:
        rel = Path(path).resolve().relative_to(ROOT).as_posix() if Path(path).is_absolute() else path
        if not rel.endswith(SOURCE_SUFFIXES):
            print(f"skipping {rel} (only C/C++ sources and headers are sent)", file=sys.stderr)
            continue
        if (ROOT / rel).exists():
            files[rel] = (ROOT / rel).read_text()
    body = {"ref": ref, "files": files, "units": args.unit, "symbols": args.symbol, "context": args.context,
            "link": args.link, "session": session_name(), "task": args.task}
    request = urllib.request.Request(url.rstrip("/") + "/diff", data=json.dumps(body).encode(), method="POST",
                                     headers={"Authorization": f"Bearer {token}", "Content-Type": "application/json",
                                              "User-Agent": "cloud_diff.py"})
    try:
        with urllib.request.urlopen(request, timeout=2400) as response:
            result = json.loads(response.read())
    except urllib.error.HTTPError as error:
        result = json.loads(error.read() or b"{}") or {"status": f"HTTP {error.code}"}
    except urllib.error.URLError as error:
        print(f"bridge unreachable ({error.reason}); the maintainer's machine may be asleep", file=sys.stderr)
        return 3
    if args.json:
        print(json.dumps(result, indent=1))
    else:
        print_result(result)
    return 0 if result.get("ok") else 1


if __name__ == "__main__":
    raise SystemExit(main())
