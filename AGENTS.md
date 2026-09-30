# AGENTS.md — Pokémon Colosseum Decompilation Guardrails

This repository is being reset around the standard dtk-template workflow. The
active build/report path is `configure.py` -> `ninja` ->
`build/GC6E01/report.json`.

## Active Project

- Target: Pokémon Colosseum `GC6E01`.
- Source candidates: `src/`.
- Headers: `include/`.
- Canonical config: `config/GC6E01/config.yml`, `symbols.txt`, `splits.txt`,
  `build.sha1`.
- Archived old campaign material: `archive/previous_campaign/`.

## Hard Rules

- Do not add, edit, stage, or commit `.inc` files.
- Do not count asm wrappers, inline asm, or included assembly as decompilation
  progress, except authentic Dolphin SDK paired-single math admitted by the
  path-, symbol-, and instruction-scoped quality allowlist, and authentic
  hand-written library assembly (user decision, 2026-09-28): a function the
  original developers wrote in assembly, registered one at a time in
  `docs/asm_evidence/registry.json`, with an evidence document proving it
  cannot be C (instructions MWCC never emits, another decompilation that keeps
  it as asm, cited by GitHub URL and commit, and its origin). The source cites
  that document beside the asm. The quality scan enforces all of it; nothing is
  assumed. Do not broaden either exception without a dedicated policy and CI
  change.
- Do not commit extracted game assets, target objects, compiler binaries, or
  generated build products.
- Do not move archived campaign material back into the active tree unless it is
  reintroduced through the dtk-template pipeline.
- Do not rename symbols or change splits without a concrete build/report reason.
- Keep source changes scoped to objects declared in `configure.py`.

## Validation

Use the smallest check that proves the change:

1. `python configure.py --no-progress`
2. `ninja all_source build/GC6E01/report.json`
3. `ninja`
4. `python configure.py progress`

If a command cannot run in the environment, report the exact command and why.

## Campaign Operations

When `tools/local_campaign.py` is running, claim the assigned function with
`python3 tools/local_campaign.py claim SYMBOL --worker Codex` before editing.
The claim covers its entire source owner. Run builds, objdiff, and verification
through `python3 tools/local_campaign.py build --worker Codex -- COMMAND`.
Release the owner with the returned claim token after validation. Read
`docs/LOCAL_LLM_CAMPAIGN.md` for the coordination and queue-refresh workflow.
The harness itself lives in its own repository (sibling folder `Pokemon-Decomp-Harness`,
github.com/Count-Sheep/pokemon-decomp-harness); the `tools/local_campaign*.py` files here are
forwarding stubs, so these commands work unchanged.

Before resuming fleet, farm, worktree-reconciliation, or batch-integration work,
read `docs/CAMPAIGN_OPERATIONS.md`. It is the current restart/cleanup playbook
and contains the handoff ledger. Keep exact-source and newly linked progress
separate.

## Cloud Sessions (no retail disc)

A cloud checkout has no `orig/`, so it cannot build the report or run objdiff.
Measure through the maintainer's diff bridge instead:

- `python3 tools/cloud_diff.py [files] [--symbol NAME] [--task "what you are trying"]`
  sends your changed `src/`/`include/` files. The bridge builds them on the
  maintainer's machine and prints the unit's match and an aligned objdiff
  (retail on the left) for each function that is not exact.
- It needs `DECOMP_BRIDGE_URL` and `DECOMP_BRIDGE_TOKEN` in the environment.
  If they are missing or the bridge is unreachable, say so. Do not claim a
  match you could not measure.
- The base is HEAD when it is pushed, otherwise its merge-base with
  `origin/master`. To change `configure.py` or `config/`, commit and push a
  branch, then pass `--ref <branch>`.
- `--link` also links `main.dol` and checks the SHA-1s. It is slow, so run it
  before you push a result, not on every iteration.
- Always pass `--task`. The maintainer watches the requests on a dashboard.
- Requests run one at a time on one machine. Don't run more than one at once.
- Never commit the bridge's output: it contains retail disassembly.

## Naming

Rename conservatively and preserve address traceability. A nontrivial rename
needs evidence from callsites, strings, known SDK patterns, data tables, or
confirmed assembly/decompiler analysis.
