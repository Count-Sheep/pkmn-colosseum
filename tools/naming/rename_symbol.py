#!/usr/bin/env python3
"""Rename one function safely: a one-row front end to tools/naming/apply_renames.py.

The old implementation replaced whole words everywhere, including inside string
literals, and missed configure.py, docs/RULE_EXCEPTIONS.md, the asm-evidence
registry and the regression cleanup file. This version builds a one-row batch
and hands it to apply_renames.py, which is literal-aware, covers the whole live
reference set, adds the NAME-T tag and the ledger row, and is all-or-nothing.

Example:
  tools/naming/rename_symbol.py fn_801AA6D0 PObjAmnesia --tier T3 \\
      --evidence-ref "ctx:calls:HSD_PObjFree" --evidence "frees the pobj list"
"""

from __future__ import annotations

import argparse
import csv
import datetime as dt
import re
import subprocess
import sys
import tempfile
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parents[2]
COLUMNS = ["address", "module", "old_name", "new_name", "tier", "evidence", "evidence_ref", "unit",
           "source_file", "batch_id", "commit", "date"]


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("old", help="current name, e.g. fn_801AA6D0")
    ap.add_argument("new", help="new name")
    ap.add_argument("--tier", required=True, choices=["T1", "T2", "T3"])
    ap.add_argument("--evidence-ref", required=True)
    ap.add_argument("--evidence", required=True)
    ap.add_argument("--batch-id", help="default: YYYYMMDD-single-1")
    ap.add_argument("--dry-run", action="store_true")
    args = ap.parse_args()

    m = re.fullmatch(r"fn_([0-9A-F]{8})", args.old)
    if not m:
        print("rename_symbol: only fn_XXXXXXXX names may be renamed (docs/NAMING_SCHEME.md)", file=sys.stderr)
        return 2
    today = dt.date.today()
    row = {"address": f"0x{m.group(1)}", "module": "main", "old_name": args.old, "new_name": args.new,
           "tier": args.tier, "evidence": args.evidence, "evidence_ref": args.evidence_ref, "unit": "",
           "source_file": "", "batch_id": args.batch_id or f"{today:%Y%m%d}-single-1", "commit": "",
           "date": today.isoformat()}
    with tempfile.NamedTemporaryFile("w", suffix=".csv", delete=False, newline="") as fh:
        w = csv.DictWriter(fh, fieldnames=COLUMNS, lineterminator="\n")
        w.writeheader()
        w.writerow(row)
    cmd = [sys.executable, str(REPO_ROOT / "tools/naming/apply_renames.py"), fh.name]
    if args.dry_run:
        cmd.append("--dry-run")
    return subprocess.run(cmd).returncode


if __name__ == "__main__":
    raise SystemExit(main())
