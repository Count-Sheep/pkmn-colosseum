# Goal continuation triage (2026-10-02)

Scope: quick high-value checks while pursuing linked-unit progress. No asm or
policy exceptions were introduced.

## Candidates sampled, no retained source change

### toolentryTaisenSetEtnryPokemonOrder

Owner: `src/game/toolentry.c`, scored by
`main/game/toolentry_candidate_8025D644`.

Baseline report score: 90.703125%, 256 bytes. The raw diff is mostly saved
register allocation:

- retail keeps `order` in `r29`, `ctx` in `r25`, entry count in `r30`;
- current source keeps `order` in `r30`, entry count in `r29`, then rotates the
  live-count/limit/index trio through `r28`/`r27`/`r26`.

Tried and reverted:

- typed `fn_8006B09C(s32)` / `fn_8006B1D4(BattleFieldAccessor*)` plus local
  reordering around `limit`, `liveCount`, and `i`: regressed to 85.46875%.
- explicit `savedOrder = order` used in the duplicate scan and store: no codegen
  change and no score change.

The next useful pass should use a broader original-source shape search; simple
local copies and the obvious typed cleanup do not help.

### fn_8006B420

Owner: `src/game/menu/menu_middle.c`, scored by
`main/game/menu/menu_middle_range_8006B420`.

Baseline report score: 94.71429%, 140 bytes. The only meaningful tail mismatch
is return-address expression scheduling after `savedataGetStatus(0, 0xE)`:
retail forms the staged offset in `r4`, adds it to `r3`, then moves to `r3`.

Tried and reverted:

- collapsed the block-local offset into one return expression:
  `savedataGetStatus(...) + index * 0x54 + 0x10000 - 0x3624`. This regressed to
  93.14286% by introducing an extra `mr r0, r3` and forming the final address in
  a different order.

Keep the staged block-local offset unless a more precise source shape is found.

### __sys_free

Scored by `main/dolphin/sdk_r52_800C4D8C_suffix`. The remaining diff is call
argument scheduling for `fn_8009AB60`, the alignment copy of arena high, and
epilogue load order. No source change was retained.

### TRKDoWriteMemory

Scored by `main/trk/TRKDispatch_range_800BF53C_r41_800BFECC`. The sampled diff
is dominated by standalone string/jumptable relocation names and broad
saved-register rotation, so it was not edited in this pass.

## Current-state recheck while serial validation was active

The exact-but-unlinked etctool cluster remains gated by `fn_801E09E0`:

- `main/game/gs_range_candidate_801DF474` is still 100% on `fn_801DF474`.
- `main/game/field_range_801DF790` and
  `main/game/field_range_801DF790_r41_801E075C_gc125n` are still exact-source
  chunks in the shared etctool pool region.
- `main/game/gs_candidate_801E09E0` is still the blocker. Current fdiff shows
  the same two structural copy/scheduling problems documented in
  `gs_model_shadow_render_and_etctool_walls.md`: the sequence table base is
  built through `r0` before `r30`, and the case-10 `floorOpenObject` result
  copies directly to `r29` instead of retail's `r0` shuttle before
  `GSvecCopy(&objectPosition, &sequencePositions[0])`.

I also scanned all incomplete main units with 100% matched code and no obvious
docs mention. The only code units returned were the two fight timer candidates
(`fightTimerCommandInit` and `fightTimerAllInit`), which are already covered by
the known shared conversion-pool wall in the fight notes. The remaining hits
were zero-code data/auto sections.

## Later continuation samples, no retained source change

### `gs_npc_event_candidate_800324A0`

The report still marks the unit as 100% code / 100% data, and fdiff confirms
`fn_800324A0` and `fn_80032564` exact with `fn_8003258C` differing only by the
jump-table label name (`jumptable_802E4FE0` vs `@337`). Do not simply promote
the object: `tier3_d26.md` documents the real blocker. The unit must own the
switch table at `.data 0x802E4FE0-0x802E502C`, but the following sysvars table
starts 4-aligned; a compiled data object aligns it to 8 and shifts the
continuation, breaking the link.

### `fight_side_candidate_801F6F38`

The four accessors remain a broad register-coloring rotation. For
`fightSideGetHikaeFightPokemonNum`, retail keeps the outer limit in `r31` and
the current trainer pointer in `r30`; current source keeps them as `r30`/`r31`.
Swapping the local declaration order around `uVar4`/`uVar5` had no assembly
effect and was reverted. The other three functions show the same family of
outer/inner limit, total, and pointer rotations.

### `_fightFloorCheckHuuinWazaFightOutPokemonSub__FPvUsPv`

Baseline fdiff for `main/game/fight_floor_candidate_801F1F7C` is dominated by
local-copy coloring: retail keeps `data` in `r28`, `obj` in `r31`, `mon` in
`r30`, and `waza` in `r29`; current source keeps `data` in `r29`, `obj` in
`r28`, and `waza` in `r31`. Adding explicit `s = data` and `target = obj`
locals regressed aligned-same from 33/42 to 31/42 and was reverted.

### `hero_move_r49_8012BBA8_suffix`

The unit is exact at the instruction level except for shared hero-move pool
symbol names (`lbl_8047D030`/`lbl_8047D034` versus an anonymous `@20` pair).
This is not a standalone-link target; the rule-exception rows and hero-move
recon notes require whole-TU linkage with the real literal pool.

### `effect_visual_r49_80138BBC_prefix`

`fn_80138BBC` has a tempting inner/outer loop register swap: retail uses
`r28` for the outer model loop and `r29` for the inner material loop; current
source uses them in the opposite order. Moving the `i` declaration before `j`
regressed the whole function (aligned-same 60/68 to 52/68) and was reverted.
`fn_80138CCC` remains a larger saved-register rotation.

### `TRKNub_candidate_800BE844`

`TRKReadBuffer_ui8` differs in the inlined read helper's size comparison:
retail keeps the requested length in a register (`cmplw r30,r0`), while current
source folds the constant (`cmplwi r0,1`). Introducing a `requested = n` local
inside `TRKReadBufferInline` regressed the unit: `TRKReadBuffer_ui8`
aligned-same dropped from 36/38 to 29/38 and the wider typed-buffer functions
also rotated further. The helper change was reverted.

### Exact SDK asm candidates

`OSCache_privileged_prefix` and `OSCache_privileged_suffix` remain 100% in the
report but are hand-written SDK assembly (`DCEnable`, `ICEnable`, `LC*`, etc.).
They should stay unlinked unless the hand-written asm policy path is extended
with evidence/registry coverage for these exact functions.

## Additional continuation samples

### `fn_80222BD8`

Baseline fdiff is 26/27 aligned-same. The missing instruction is the first
store of `lbl_8047B618` after clearing bit `0x40`, before clearing bit
`0x4000`. Rewriting the tail as:

- `lbl_8047B618 = first;`
- `state = lbl_8047B618;`
- `result[6] = zero;`
- `state &= 0xffffbfff;`

kept the first store, but MWCC scheduled the second `rlwinm` before that store
and produced 26/28 aligned-same (92.9%), so the edit was reverted. This
confirms the old volatile-write note: the issue is not merely source statement
order; it needs a shape that preserves the intermediate store without moving
the second mask.

### `fightTrainerAiWazaValueTedasuke`

Sampled `main/game/fight_trainer_ai_waza_value_candidate_80241660`. The diff is
not localized: parameters `ctx`/`param1`/`param2`, the `handle`, enemy-array
base, enemy count, loop indices, and temporary action/waza locals are all
rotated through the whole function. No narrow edit was attempted.

### `effect_visual_candidate_8013B268`

Sampled `fn_8013B268`. The report score is pulled down by both shared
conversion-pool labels (`lbl_8047D1E8` versus anonymous pool entries) and broad
register/float-register rotation in the per-channel color interpolation. This
also touches the pool called out by `effect_visual_candidate_8013AB60`; no
standalone edit was retained.
