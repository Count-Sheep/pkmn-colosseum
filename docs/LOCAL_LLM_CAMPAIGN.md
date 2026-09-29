# Local LLM campaign

The local-LLM harness (campaign runner, model benchmark, dashboard and refresh tool) and the
infrastructure behind it moved to their own private repository on 2026-09-29:
**github.com/Count-Sheep/pokemon-decomp-harness**, checked out as the sibling folder
`Pokemon-Decomp-Harness`. Its `docs/LOCAL_LLM_CAMPAIGN.md` is the full documentation, and its
README covers the machines and common commands.

The harness drives this repository from outside it. Its state (queue, candidates, bench runs)
still lives here, in the ignored `build/local_llm_campaign/`, next to the report it measures.

## What stays the same here

`tools/` keeps a forwarding stub for every former harness module (see
`tools/_harness_forward.py`), so these commands work unchanged from this repository:

```bash
python3 tools/local_campaign.py sync                    # refresh the queue from the report
python3 tools/local_campaign.py claim SYMBOL --worker NAME
python3 tools/local_campaign.py build --worker NAME -- ninja -j2 all_source build/GC6E01/report.json
python3 tools/local_campaign.py release SOURCE --token TOKEN
python3 tools/local_campaign.py claims
python3 tools/local_campaign_server.py --open           # dashboard, http://127.0.0.1:8765
python3 tools/refresh_dashboard.py [--full] [--refreeze-bench]
```

The stubs find the harness through `COLO_HARNESS_ROOT` or the sibling folder, and tell it where
this repository is through `COLO_DECOMP_ROOT`.

## Working alongside the model

- Claim a task before editing. A claim reserves its entire owner file. A conflicting claim
  fails immediately; model workers skip claimed owners.
- Run every build, objdiff read, report refresh and verification script through the shared
  build lock (`tools/local_campaign.py build --worker NAME -- COMMAND`). Verification
  temporarily replaces source under that lock and restores it afterwards.
- Release manual claims with their token after validation.
- Stop the model workers before `sync`, and stop them for shared header or build-configuration
  changes, whose effects reach beyond one owner.
- After accepted source lands, run `python3 tools/refresh_dashboard.py`: it rebuilds the report,
  syncs the queue and progress chart, and lists benchmark tasks the change made stale.

Every model result is review-only: audit the stored prompt, response, candidate and objdiff
output, apply only reviewed changes in a normal branch, and run the standard validation
(`configure.py --no-progress`, the report target, `ninja`, `configure.py progress`).
