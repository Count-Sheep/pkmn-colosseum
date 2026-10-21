#!/usr/bin/env python3
"""Apply one rename batch and run the full naming gate (docs/NAMING_SCHEME.md, section 3).

Steps (each must pass; on any failure the batch is rolled back from the
apply_renames journal and the exit status is 1):
  0. base: HEAD sha; base report = --base-report, or the cached
     build/naming/base-report-<sha>.json, or a fresh build of HEAD made now
     (before the batch is applied). With --objects-identical, HEAD is always
     linked first and the hash of every compiled object is recorded.
  1. tools/naming/apply_renames.py BATCH (all-or-nothing, journaled), or, with
     --apply-cmd instead of a batch, an arbitrary edit command (for example
     tools/naming/delete_dead_stubs.py); its changes are journaled from git.
  2. python3 tools/check_naming.py --base HEAD
  3. fresh full link: wait until `pgrep -x ninja` is empty, delete
     build/GC6E01/{main.elf,main.dol,ok}, `python3 configure.py --no-progress`,
     `ninja`; the output must print "<N> files OK" (default 26).
  4. ninja build/GC6E01/report.json
  5. remapped regression check: check_naming --remap-report base -> remapped,
     then .github/scripts/check_regression.py remapped head (with the cleanup
     manifest); plus: every remapped base function is present in head, and the
     number of 100% functions did not drop.
  6. python3 tools/check_object_map_freeze.py
  7. .github/scripts/quality_scan.py HEAD <temporary commit of the batch> (run
     right after step 2, so it fails fast), and .github/scripts/check_asm_wrappers.py src.
  8. tools/verify_decomp_rules.py --no-sha1: result PASS with 0 failures
     (required since the tree is 100% matched; --no-verify-rules skips it for
     debugging only).
  9. with --objects-identical: every compiled object under build/GC6E01/src is
     byte-identical to the base build (same set of files, same SHA-1s).
With --commit, a passing batch is committed with the scheme's message.

Exit status: 0 pass, 1 gate failure (rolled back), 2 usage/setup error.
"""

from __future__ import annotations

import argparse
import csv
import hashlib
import json
import os
import shutil
import subprocess
import sys
import time
from collections import Counter
from pathlib import Path

REPO = Path(__file__).resolve().parents[2]
PY = sys.executable
GUARDED = ("src", "include", "config", "configure.py", "docs/function_renames.csv",
           "docs/RULE_EXCEPTIONS.md", "docs/asm_evidence", ".github/regression_policy_cleanup.json")


class GateFail(Exception):
    pass


class Gate:
    def __init__(self, root: Path, log: Path | None):
        self.root = root
        self.log = log.open("a") if log else None
        self.steps: list[tuple[str, str, float]] = []

    def say(self, msg: str) -> None:
        print(msg, flush=True)
        if self.log:
            self.log.write(msg + "\n")
            self.log.flush()

    def run(self, cmd: list[str], *, check: bool = True, tail: int = 15) -> subprocess.CompletedProcess:
        self.say(f"$ {' '.join(cmd)}")
        p = subprocess.run(cmd, cwd=self.root, capture_output=True, text=True)
        out = (p.stdout + p.stderr).rstrip()
        if self.log:
            self.log.write(out + "\n")
        lines = out.splitlines()
        for line in lines[-tail:]:
            print(f"    {line}")
        if check and p.returncode != 0:
            raise GateFail(f"{cmd[0] if cmd[0] != PY else cmd[1]} exited {p.returncode}")
        return p

    def step(self, name: str, fn) -> None:
        t = time.time()
        self.say(f"== {name}")
        try:
            fn()
        except GateFail as e:
            self.steps.append((name, f"FAIL: {e}", time.time() - t))
            raise
        self.steps.append((name, "ok", time.time() - t))


def git(root: Path, *args: str) -> str:
    return subprocess.run(["git", *args], cwd=root, check=True, capture_output=True, text=True).stdout.strip()


def wait_ninja_idle(gate: Gate, timeout: float = 3600) -> None:
    t = time.time()
    while subprocess.run(["pgrep", "-x", "ninja"], capture_output=True).returncode == 0:
        if time.time() - t > timeout:
            raise GateFail("another ninja is still running")
        if int(time.time() - t) % 60 == 0:
            gate.say("    waiting for another ninja to finish ...")
        time.sleep(5)


def fresh_link(gate: Gate, expect_files: int) -> None:
    wait_ninja_idle(gate)
    for name in ("main.elf", "main.dol", "ok"):
        p = gate.root / "build" / "GC6E01" / name
        if p.exists():
            p.unlink()
    gate.run([PY, "configure.py", "--no-progress"])
    p = gate.run(["ninja"], check=False)
    if p.returncode != 0:
        raise GateFail("ninja failed")
    if f"{expect_files} files OK" not in p.stdout + p.stderr:
        raise GateFail(f"link did not print '{expect_files} files OK'")


def fmap(path: Path) -> dict[str, float]:
    r = json.loads(path.read_text())
    m = {}
    for u in r.get("units", []):
        fns = list(u.get("functions", []))
        for s in u.get("sections", []):
            fns += s.get("functions", [])
        for f in fns:
            if f.get("name") and f.get("fuzzy_match_percent") is not None:
                m[f["name"]] = float(f["fuzzy_match_percent"])
    return m


def object_hashes(root: Path) -> dict[str, str]:
    base = root / "build" / "GC6E01" / "src"
    return {str(p.relative_to(base)): hashlib.sha1(p.read_bytes()).hexdigest()
            for p in sorted(base.rglob("*.o"))}


def changed_files(root: Path) -> list[str]:
    out = subprocess.run(["git", "status", "--porcelain", "--untracked-files=no", "--", *GUARDED],
                         cwd=root, check=True, capture_output=True, text=True).stdout
    return [line[3:] for line in out.splitlines() if line.strip()]


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("batch", type=Path, nargs="?", help="batch CSV (ledger columns)")
    ap.add_argument("--apply-cmd", help="instead of a batch: shell command that edits the tree")
    ap.add_argument("--objects-identical", action="store_true",
                    help="require every compiled object to be byte-identical to the base build")
    ap.add_argument("--root", type=Path, default=REPO)
    ap.add_argument("--base-report", type=Path, help="report.json of HEAD before the batch")
    ap.add_argument("--expect-files", type=int, default=26)
    ap.add_argument("--drop-refused", type=Path, metavar="CSV", help="passed to apply_renames.py")
    ap.add_argument("--commit", action="store_true", help="commit the batch when the gate passes")
    ap.add_argument("--message", help="commit message (default: the scheme's Names: ... line)")
    ap.add_argument("--allow-dirty", action="store_true", help="skip the clean-tree check")
    ap.add_argument("--keep-on-fail", action="store_true", help="do not roll back on failure (debugging)")
    ap.add_argument("--no-verify-rules", action="store_true", help="skip tools/verify_decomp_rules.py")
    ap.add_argument("--log", type=Path, help="append full command output here")
    ap.add_argument("--summary-json", type=Path, help="write the step results here")
    args = ap.parse_args()

    root = args.root.resolve()
    naming = root / "build" / "naming"
    naming.mkdir(parents=True, exist_ok=True)
    gate = Gate(root, args.log)
    if bool(args.batch) == bool(args.apply_cmd):
        print("gate: give exactly one of BATCH or --apply-cmd", file=sys.stderr)
        return 2
    if args.apply_cmd and args.commit and not args.message:
        print("gate: --apply-cmd with --commit needs --message", file=sys.stderr)
        return 2
    batch = args.batch.resolve() if args.batch else root / "build" / "naming" / "apply-cmd.csv"

    dirty = git(root, "status", "--porcelain", "--untracked-files=no", "--", *GUARDED)
    if dirty and not args.allow_dirty:
        print(f"gate: tracked changes under the live reference set; commit or stash first:\n{dirty}", file=sys.stderr)
        return 2
    head = git(root, "rev-parse", "HEAD")
    journal = naming / f"journal-{batch.stem}.json"
    verify_tool = root / "tools" / "verify_decomp_rules.py"
    use_verify = not args.no_verify_rules
    if use_verify and not verify_tool.exists():
        print("gate: tools/verify_decomp_rules.py is missing (use --no-verify-rules to skip)", file=sys.stderr)
        return 2
    applied = False
    base_objects: dict[str, str] = {}
    result = "FAIL"

    try:
        # 0. base report (and base rules run) for HEAD.
        base_report = args.base_report or naming / f"base-report-{head[:12]}.json"

        def base():
            nonlocal base_objects
            if not base_report.exists() or args.objects_identical:
                gate.say("    building HEAD first")
                fresh_link(gate, args.expect_files)
                gate.run(["ninja", "build/GC6E01/report.json"])
                if not base_report.exists():
                    shutil.copy(root / "build/GC6E01/report.json", base_report)
            if args.objects_identical:
                base_objects = object_hashes(root)
                if not base_objects:
                    raise GateFail("no compiled objects under build/GC6E01/src")
                gate.say(f"    {len(base_objects)} base objects hashed")
        gate.step("base report", base)

        # 1. apply.
        def apply():
            nonlocal applied
            if args.apply_cmd:
                gate.say(f"$ {args.apply_cmd}")
                p = subprocess.run(args.apply_cmd, shell=True, cwd=root, capture_output=True, text=True)
                for line in (p.stdout + p.stderr).rstrip().splitlines()[-8:]:
                    gate.say(f"    {line}")
                files = changed_files(root)
                journal.write_text(json.dumps({"files": {f: None for f in files}, "git": head}, indent=1))
                applied = bool(files)
                if p.returncode != 0:
                    raise GateFail(f"apply command exited {p.returncode}")
                return
            cmd = [PY, "tools/naming/apply_renames.py", str(batch), "--journal", str(journal)]
            if args.drop_refused:
                cmd += ["--drop-refused", str(args.drop_refused)]
            gate.run(cmd, tail=8)
            applied = journal.exists()
        gate.step("apply renames", apply)
        if not applied:
            gate.say("nothing applied (batch already applied?)")
            result = "NOOP"
            return 0

        # 2. checker.
        gate.step("check_naming --base HEAD", lambda: gate.run([PY, "tools/check_naming.py", "--base", head]))
        # 2b. quality scan: it diffs two revisions, so scan a temporary commit
        # of the batch and drop the commit again (the files stay changed).
        def quality():
            files = list(json.loads(journal.read_text())["files"].keys())
            gate.run(["git", "add", "--", *files])
            gate.run(["git", "commit", "-q", "--no-verify", "-m", "naming gate: temporary quality-scan commit"])
            tmp = git(root, "rev-parse", "HEAD")
            try:
                gate.run([PY, ".github/scripts/quality_scan.py", head, tmp], tail=6)
            finally:
                subprocess.run(["git", "reset", "-q", "--mixed", head], cwd=root, check=True)
        gate.step("quality scan", quality)
        # 3. fresh link.
        gate.step(f"fresh link ({args.expect_files} files OK)", lambda: fresh_link(gate, args.expect_files))
        # 4. report.
        gate.step("report", lambda: gate.run(["ninja", "build/GC6E01/report.json"]))

        # 5. remapped regression.
        def regression():
            remapped = naming / f"base-remapped-{batch.stem}.json"
            head_report = root / "build/GC6E01/report.json"
            gate.run([PY, "tools/check_naming.py", "--remap-report", str(base_report), str(remapped)])
            gate.run([PY, ".github/scripts/check_regression.py", str(remapped), str(head_report),
                      "--cleanup-manifest", ".github/regression_policy_cleanup.json", "--base-sha", head])
            b, h = fmap(remapped), fmap(head_report)
            missing = sorted(set(b) - set(h))
            if missing:
                raise GateFail(f"{len(missing)} base function(s) missing from head after remap: {missing[:5]}")
            nb, nh = sum(v >= 100 for v in b.values()), sum(v >= 100 for v in h.values())
            gate.say(f"    100% functions: base {nb}, head {nh}")
            if nh < nb:
                raise GateFail(f"100% functions dropped {nb} -> {nh}")
        gate.step("remapped regression check", regression)

        # 6. freeze.
        gate.step("object-map freeze", lambda: gate.run([PY, "tools/check_object_map_freeze.py"]))
        # 7. scans.
        gate.step("asm-wrapper scan", lambda: gate.run([PY, ".github/scripts/check_asm_wrappers.py", "src"]))

        # 8. rules verifier.
        if use_verify:
            def verify():
                out = naming / f"head-verify-{batch.stem}.json"
                gate.run([PY, str(verify_tool), "--no-sha1", "--json", str(out)], check=False, tail=4)
                if not out.exists():
                    raise GateFail("verify_decomp_rules.py wrote no JSON")
                data = json.loads(out.read_text())
                gate.say(f"    verify_decomp_rules: {data.get('result')}, {data.get('failures')} failure(s), "
                         f"{data.get('warnings')} warning(s)")
                if data.get("result") != "PASS" or data.get("failures"):
                    bad = {k: v["count"] for k, v in data.get("classes", {}).items()
                           if v.get("severity") == "fail" and v.get("count")}
                    raise GateFail(f"verify_decomp_rules failed: {bad}")
            gate.step("verify_decomp_rules (PASS, 0 failures)", verify)
        else:
            gate.say("== verify_decomp_rules: SKIPPED (--no-verify-rules); this run does not count as a pass")

        # 9. objects byte-identical.
        if args.objects_identical:
            def objects():
                now = object_hashes(root)
                diff = sorted(k for k in set(base_objects) | set(now) if base_objects.get(k) != now.get(k))
                if diff:
                    raise GateFail(f"{len(diff)} object(s) differ from the base build: {diff[:5]}")
                gate.say(f"    {len(now)} objects byte-identical")
            gate.step("objects byte-identical", objects)

        if args.commit:
            def commit():
                files = json.loads(journal.read_text())["files"].keys()
                gate.run(["git", "add", "--", *files])
                msg = args.message or default_message(root, batch)
                gate.run(["git", "commit", "-q", "-m", msg])
            gate.step("commit", commit)
        result = "PASS"
        return 0
    except GateFail as e:
        gate.say(f"GATE FAIL: {e}")
        if args.commit and git(root, "rev-parse", "HEAD") != head:
            gate.say(f"    resetting the batch commit (back to {head[:12]})")
            subprocess.run(["git", "reset", "-q", "--mixed", head], cwd=root)
        if applied and not args.keep_on_fail:
            if args.apply_cmd:
                files = list(json.loads(journal.read_text())["files"])
                gate.run(["git", "checkout", head, "--", *files], check=False)
            else:
                gate.run([PY, "tools/naming/apply_renames.py", "--rollback", str(journal)], check=False)
        return 1
    finally:
        gate.say(f"gate {result}: " + "; ".join(f"{n}: {s} ({t:.0f}s)" for n, s, t in gate.steps))
        if args.summary_json:
            args.summary_json.write_text(json.dumps({"result": result, "head": head, "batch": str(batch),
                                                     "steps": gate.steps}, indent=1))


def source_file_key(root: Path, unit: str) -> str:
    sys.path.insert(0, str(root / "tools"))
    import check_naming  # noqa: PLC0415
    return check_naming.source_file_of_unit(root, unit)


def default_message(root: Path, batch: Path) -> str:
    led = list(csv.DictReader((root / "docs/function_renames.csv").open(encoding="utf-8")))
    rows = [r for r in csv.DictReader(batch.open(encoding="utf-8"))]
    names = {(r["old_name"], r["new_name"]) for r in rows}
    applied = [r for r in led if (r["old_name"], r["new_name"]) in names]
    t = Counter(r["tier"] for r in applied)
    units = sorted({r["unit"] for r in applied})
    bids = sorted({r["batch_id"] for r in applied})
    files = sorted({source_file_key(root, u) for u in units})
    if len(units) == 1:
        unit = units[0]
    elif len(files) == 1:  # a per-source-file batch (NAMING_SCHEME section 4)
        unit = f"{files[0]} ({len(units)} units)"
    else:
        unit = f"{len(units)} units"
    head = (f"Names: {unit} — {len(applied)} renames (T1 {t['T1']} / T2 {t['T2']} / T3 {t['T3']}) "
            + " ".join(f"[batch {b}]" for b in bids))
    body = "\n".join(f"{r['old_name']} -> {r['new_name']} ({r['tier']}, {r['evidence_ref']})" for r in applied)
    return f"{head}\n\n{body}\n\nCo-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>\n"


if __name__ == "__main__":
    sys.exit(main())
