# Current state (2026-09-28)

A snapshot of where the decompilation stands for the native recomp's
title-screen milestone, and how to pick the work back up.

## Title-screen path (recomp checker `boot_status.py`)

- **208/210** path functions accepted; 209 at 100%.
- Title-loop closure (everything the title loop reaches): about 88% of
  functions accepted.
- Whole game: 7,357 of 8,605 functions matched exactly; 48.3% of code is in
  fully linked units; fuzzy match 92.0%.
- `main.dol` and `common_rel.rel` match their SHA1s at every commit on this
  branch.

Not accepted:

| Row | Function | Blocker |
|---|---|---|
| 7: main-thread init | `fn_8013024C` | Byte-exact (neck-mode rule exception), but the whole hero_move TU must link. H1's work is merged; H2's and H3's work is on their branches and needs one integration pass (see the ledger in CAMPAIGN_OPERATIONS.md). |
| 43: title camera | `cameraPlayAnime` | The recomp is porting the camera natively (user decision). The TU can't link: .rodata keeps a stripped function's strings (`cameraDispInfo`), and no build has its body. |

Recomp: the native boot runs calls 6–31 of 55 and stops at call 32
(`fn_800FF828`, row 20). Row 40's title-loop closure is down to about 74 after
the recomp scoped it; row 20's to 10; fsys row 38 to 14.

## Policy changes made in this campaign (docs/CAMPAIGN_OPERATIONS.md)

- Same-engine Pokémon XD functions admit inline helpers (same calls, same order,
  named after XD's function).
- Named computed values (THPPlayer's `src`).
- C++ reference bindings in TUs evidenced as C++ (dbgMenu).
- Authentic hand-written library assembly, per function with documented
  evidence (`docs/asm_evidence/`, enforced by the quality scan). The registry
  is empty so far.
- **Title-path rule exceptions:** byte-exact forms that break the strict policy,
  tagged `RULE-EXCEPTION(title-path)` in the source and listed in
  `docs/RULE_EXCEPTIONS.md` for revisiting.

## Where the pieces are

- `docs/RULE_EXCEPTIONS.md`: every title-path exception, with its clean fix.
- `docs/CAMPAIGN_OPERATIONS.md` → "Paused title-path lanes (2026-09-28)": the
  paused agent lanes, their branches and handoff notes (`.lane-handoff/<LANE>.md`
  on each branch). Branches pushed: `worktree-agent-*`.
- `docs/recon/`: evidence from XD, the XD demo linker map and other builds.
- `docs/LOCAL_LLM_CAMPAIGN.md`: the local-model harness: focused edits, snippet
  repair, the rewrite search (`local_campaign.py rewrite`) and the register
  replay (`local_campaign.py explain`, which needs mwcc-debugger tools in
  build/tools/mwdbg).
- `tools/session_2026_09/`: the coordinator's working scripts from this
  session. They reference session-local paths; update those before reuse.
  - `merge_check.sh`: cherry-pick, policy scan, build, SHA1 and report diff.
  - `link_priority.py` / `title_closure.py`: title-closure measurement and the
    link-blocker list.
  - `promote_local.py`, `restart_dreamworld.sh`, `pause_worker.sh`,
    `resume_worker.sh`: local-model operations.
  - `lane_preamble.md`, `burst_common.md`, `exception_mode.txt`, `wrapup.txt`:
    the instructions given to agent lanes.

## Upstream

PR https://github.com/dougchansan/pkmn-colosseum/pull/635 is open (a strict
pragma cleanup), introducing this fork to the upstream maintainer. Only work
that follows upstream's own rules goes upstream; the exceptions above don't.

## Nothing is running

All agents, the local-model worker, its watchdog and the promotion loop are
stopped. To resume the local model, see `tools/session_2026_09/restart_dreamworld.sh`
and `docs/LOCAL_LLM_CAMPAIGN.md`; its title-loop priority list is in
build/local_llm_campaign/priority.json (not committed).
