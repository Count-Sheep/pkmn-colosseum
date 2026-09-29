"""Forward a harness module from this repository to the Pokemon-Decomp-Harness repository.

The local-LLM harness (campaign runner, model benchmark, dashboard, refresh tool) lives in
its own private repository, github.com/Count-Sheep/pokemon-decomp-harness. The stubs in
tools/ keep every former module name working, whether run as a script or imported:
`python3 tools/local_campaign.py ...` still works as AGENTS.md documents.

The harness is found through COLO_HARNESS_ROOT, or as the sibling folder
Pokemon-Decomp-Harness. It is told where this decomp is through COLO_DECOMP_ROOT.
"""

from __future__ import annotations

import importlib
import os
import runpy
import sys
from pathlib import Path

DECOMP_ROOT = Path(__file__).resolve().parents[1]
HARNESS_DIR = Path(os.environ.get("COLO_HARNESS_ROOT") or DECOMP_ROOT.parent / "Pokemon-Decomp-Harness").expanduser() / "harness"


def forward(name: str, path: str) -> None:
    stem = Path(path).stem
    target = HARNESS_DIR / f"{stem}.py"
    if not target.is_file():
        raise SystemExit(f"{stem} moved to the harness repository, which was not found at {HARNESS_DIR.parent}. "
                         "Clone github.com/Count-Sheep/pokemon-decomp-harness next to this repository "
                         "or set COLO_HARNESS_ROOT.")
    os.environ.setdefault("COLO_DECOMP_ROOT", str(DECOMP_ROOT))
    if str(HARNESS_DIR) not in sys.path:
        sys.path.insert(0, str(HARNESS_DIR))  # the harness's modules win over these stubs
    if name == "__main__":
        sys.argv[0] = str(target)
        runpy.run_path(str(target), run_name="__main__")
        return
    # Imported: replace this stub with the harness module of the same name.
    stub = sys.modules.pop(name)
    try:
        sys.modules[name] = importlib.import_module(stem)
    except BaseException:
        sys.modules[name] = stub
        raise
