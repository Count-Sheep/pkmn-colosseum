#!/usr/bin/env python3
"""Report registered (evidenced) assembly separately from decompiled C.

Counts the functions in docs/asm_evidence/registry.json by tier (library or
first-party) and, when a report is available, their code bytes and how much of
the overall matched code they make up, so progress figures can show
"decompiled C" without the transcribed assembly.

    python3 .github/scripts/asm_evidence_summary.py [build/GC6E01/report.json]
"""

import json
import sys
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parents[2]
REGISTRY = REPO_ROOT / "docs" / "asm_evidence" / "registry.json"


def main() -> int:
    report_path = Path(sys.argv[1]) if len(sys.argv) > 1 else REPO_ROOT / "build" / "GC6E01" / "report.json"
    entries = json.loads(REGISTRY.read_text(encoding="utf-8")).get("entries", [])
    sizes: dict[str, int] = {}
    measures: dict = {}
    if report_path.is_file():
        report = json.loads(report_path.read_text(encoding="utf-8"))
        measures = report.get("measures", {})
        for unit in report.get("units", []):
            for fn in unit.get("functions", []):
                sizes.setdefault(fn["name"], int(fn.get("size", 0)))
    tiers: dict[str, list[str]] = {}
    for entry in entries:
        tiers.setdefault(entry.get("tier", "library"), []).append(entry["function"])
    total_bytes = 0
    for tier, funcs in sorted(tiers.items()):
        size = sum(sizes.get(f, 0) for f in funcs)
        total_bytes += size
        print(f"{tier:12} {len(funcs):4} functions  {size:7} bytes")
    if measures:
        matched = int(measures.get("matched_code", 0))
        total = int(measures.get("total_code", 0)) or 1
        print(f"{'all asm':12} {sum(map(len, tiers.values())):4} functions  {total_bytes:7} bytes")
        print(f"matched code {matched} bytes ({matched / total:.2%}); "
              f"of which evidenced asm {total_bytes} bytes; "
              f"decompiled C ≈ {(matched - total_bytes) / total:.2%} of all code")
    return 0


if __name__ == "__main__":
    sys.exit(main())
