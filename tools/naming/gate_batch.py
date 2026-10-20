#!/usr/bin/env python3
"""Apply one rename batch and run the full naming gate (docs/NAMING_SCHEME.md, section 3).

Steps (each must pass; on any failure the batch is rolled back from the
apply_renames journal and the exit status is 1):
  0. base: HEAD sha; base report = --base-report, or the cached
     build/naming/base-report-<sha>.json, or a fresh build of HEAD made now
     (before the batch is applied). The same for the optional
     tools/verify_decomp_rules.py base result.
  1. tools/naming/apply_renames.py BATCH (all-or-nothing, journaled).
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
  8. tools/verify_decomp_rules.py --no-sha1 if the file exists: no fail-class
     count may grow relative to the base run (the tree is not 100% matched yet,
     so the absolute result is not required to be PASS).
With --commit, a passing batch is committed with the scheme's message.

Exit status: 0 pass, 1 gate failure (rolled back), 2 usage/setup error.
"""

from __future__ import annotations

import argparse
import csv
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


def verify_counts(path: Path) -> Counter:
    data = json.loads(path.read_text())
    return Counter({k: v["count"] for k, v in data.get("classes", {}).items() if v.get("severity") == "fail"})


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("batch", type=Path, help="batch CSV (ledger columns)")
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
    batch = args.batch.resolve()

    dirty = git(root, "status", "--porcelain", "--untracked-files=no", "--", *GUARDED)
    if dirty and not args.allow_dirty:
        print(f"gate: tracked changes under the live reference set; commit or stash first:\n{dirty}", file=sys.stderr)
        return 2
    head = git(root, "rev-parse", "HEAD")
    journal = naming / f"journal-{batch.stem}.json"
    verify_tool = root / "tools" / "verify_decomp_rules.py"
    use_verify = verify_tool.exists() and not args.no_verify_rules
    applied = False
    result = "FAIL"

    try:
        # 0. base report (and base rules run) for HEAD.
        base_report = args.base_report or naming / f"base-report-{head[:12]}.json"
        base_verify = naming / f"base-verify-{head[:12]}.json"

        def base():
            if not base_report.exists():
                gate.say("    no cached base report; building HEAD first")
                fresh_link(gate, args.expect_files)
                gate.run(["ninja", "build/GC6E01/report.json"])
                shutil.copy(root / "build/GC6E01/report.json", base_report)
            if use_verify and not base_verify.exists():
                gate.run([PY, str(verify_tool), "--no-sha1", "--json", str(base_verify)], check=False)
        gate.step("base report", base)

        # 1. apply.
        def apply():
            nonlocal applied
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

        # 8. rules verifier (optional).
        if use_verify:
            def verify():
                out = naming / f"head-verify-{batch.stem}.json"
                gate.run([PY, str(verify_tool), "--no-sha1", "--json", str(out)], check=False)
                if not out.exists():
                    raise GateFail("verify_decomp_rules.py wrote no JSON")
                bc = verify_counts(base_verify) if base_verify.exists() else Counter()
                hc = verify_counts(out)
                grew = {k: (bc.get(k, 0), n) for k, n in hc.items() if n > bc.get(k, 0)}
                if grew:
                    raise GateFail(f"verify_decomp_rules fail classes grew: {grew}")
            gate.step("verify_decomp_rules (no new failures)", verify)
        else:
            gate.say("== verify_decomp_rules: skipped (tools/verify_decomp_rules.py absent or disabled)")

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
            gate.run([PY, "tools/naming/apply_renames.py", "--rollback", str(journal)], check=False)
        return 1
    finally:
        gate.say(f"gate {result}: " + "; ".join(f"{n}: {s} ({t:.0f}s)" for n, s, t in gate.steps))
        if args.summary_json:
            args.summary_json.write_text(json.dumps({"result": result, "head": head, "batch": str(batch),
                                                     "steps": gate.steps}, indent=1))


def default_message(root: Path, batch: Path) -> str:
    led = list(csv.DictReader((root / "docs/function_renames.csv").open(encoding="utf-8")))
    rows = [r for r in csv.DictReader(batch.open(encoding="utf-8"))]
    names = {(r["old_name"], r["new_name"]) for r in rows}
    applied = [r for r in led if (r["old_name"], r["new_name"]) in names]
    t = Counter(r["tier"] for r in applied)
    units = sorted({r["unit"] for r in applied})
    bids = sorted({r["batch_id"] for r in applied})
    unit = units[0] if len(units) == 1 else f"{len(units)} units"
    head = (f"Names: {unit} — {len(applied)} renames (T1 {t['T1']} / T2 {t['T2']} / T3 {t['T3']}) "
            + " ".join(f"[batch {b}]" for b in bids))
    body = "\n".join(f"{r['old_name']} -> {r['new_name']} ({r['tier']}, {r['evidence_ref']})" for r in applied)
    return f"{head}\n\n{body}\n\nCo-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>\n"


if __name__ == "__main__":
    sys.exit(main())
