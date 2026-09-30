# Range carves (lane D19, 2026-09-30)

Worktree `/private/tmp/pkmn-decomp-d12`, branch `claude/decomp-d19-rangecarve`.
This lane follows up `nearlink_d18.md`. The targets were incomplete units whose
functions were all 100% but which compiled a whole range file. Each link passed
all of these:

- configure;
- `ninja all_source build/GC6E01/report.json`;
- a full `ninja`, with main.dol and common_rel.rel SHA-1 OK;
- `quality_scan` (clean);
- `check_regression` (no regressions);
- the unit reported complete.

None of these functions has a `local_campaign` queue entry, so `claim` refuses
them. The claims list was checked before each edit instead.

## Linked

| Commit | Unit | What it took |
|---|---|---|
| ab1abcec | dolphin/card_exact_800B0694.c (new; replaces sdk_r51_800B0694_prefix and sdk_r51_800B1464_inline_noauto) | CARDUnlock.c body: ReadArrayUnlock, DummyLen, __CARDUnlock, InitCallback, DoneCallback. One unit, because it is one TU body (split change). It uses a new `SDK_EXACT_800B0694_800B1788` guard in sdk_range_800AE3F0.c and XD's forms, built with `-inline noauto` and explicit inline helpers. It keeps the volatile frame pads (RULE-EXCEPTION, 45bd17ae) |
| 78cb56e8 | dolphin/sdk_candidate_800B3978.c | CARDUnmount + FormatCallback, built from card_dsp_private.h with a static inline DoUnmount. Clean |
| 5d8bd40e | dolphin/sdk_range_800B8AE8.c | GXAbortFrame, GXSetDrawSync, GXSetDrawDone, with static inline GXFlush, __GXAbort and the wait helpers. Clean |
| fc2091b0 | dolphin/sdk_r52_800BD7A0_prefix.c | GXTransform tail + GXSetGPMetric. The functions are put into address order. The unit owns GXPerf .data 0x80313628-0x80313770, so data_80313590.c now ends at 0x80313628. fn_800BE164's table at 0x80313714 (4 mod 8) is a named array (RULE-EXCEPTION, 45bd17ae) |
| 8573f649 | dolphin/os/OSThread_r51_800A1404_prefix.c | OSDisableScheduler, OSEnableScheduler, UnsetRun, __OSGetEffectivePriority. The scheduler state is read through the address-suffixed names the data units export (`RunQueueBits_8047A760` etc.) |
| f622d25d | dolphin/os/OSThread_r51_800A1528_inline_noauto.c | SetEffectivePriority, the same way. It is global because the unlinked suffix calls it |
| 5908a8b2 | game/gba/gba_conv_r49_80088428_prefix.c | gba_conv.c declarations + fn_80088428 |
| 5cf2228d | game/gba/gba_conv_r59_80088964_middle.c | fn_80088964, fn_800889A4 |
| a973f81a | game/gba/gba_conv_candidate_80088F58.c | Five debug callbacks. FORCEACTIVE: without it the DOL shifted by 0xD0 from fn_80089028 on |
| b71dd129 | game/gba/gba_misc_r58_8008C6FC_suffix.c | gba_misc.c declarations + fn_8008C6FC, fn_8008C700, fn_8008C78C |

## Walled

- **GXProject** (sdk_candidate_800BD16C, with GXSetProjection).
  GXTransform.c's .sdata2 pool, 0x8047C3E0-0x8047C400, is also read by
  sdk_candidate_800BD454 and 800BD58C, which are unlinked. So the pool can
  only be reached through stand-ins.
  - `lbl_8047C3E8` is 0.5f: MWCC turned retail's `/ 2.0f` into `* 0.5f`.
  - With `* lbl_8047C3E8` the two inner products come out as
    `fmuls fD, fK, fX` instead of `fX, fK`. The canonicalisation swaps the
    operands; temporaries, pointer casts, volatile and reordering were all
    tried. The best score is 99.78%, and that also needs a reversed compare
    (`lbl_8047C3E0 == pm[0]`).
  - It links with the whole GXTransform pool: 800BD16C, 800BD454 and
    800BD58C in one unit.
- **CARDUnlock frames.** DummyLen and __CARDUnlock have 8 more frame bytes
  than XD's forms produce under GC/1.2.5n. These were ruled out: other
  compiler versions (1.0-1.3), -O2/-O3/-O4,s, `-opt` sub-flags, -sym on,
  -fp_contract/-char, `-inline deferred`, unused and u64/f64 locals, and the
  manual rand form. Only a volatile local reproduces the frames.
  InitCallback's 8 bytes come from auto-inlining being on: `-inline noauto`
  or `#pragma dont_inline on` fixes it.

## Still to do (all functions 100%, unit incomplete)

Ranked by size. fight_*, gs_msg*, THPDec, OSCache, camera and mtx.c are
excluded, for the reasons in D18's notes.

- **sdk_candidate_800B671C** (__GXInitGX, GXCPInterruptHandler,
  GXInitFifoBase, GXInitFifoPtrs). It spans the GXInit.c end and the
  GXFifo.c head. __GXInitGX reads GXInit's .data (0x80312AB4 ..
  0x80313294) and .sdata2 (0x8047C2E4-0x8047C300). Check that pool's other
  readers before choosing stand-ins over ownership.
- **hero_move** r49_8012B5E4_o4s, r49_8012BBA8_suffix and r46_8012C540.
  D18's hero_move carves are the pattern to follow.
- **msgctrl** (0x80131588), msgctrl_candidate_80131714 and
  msgctrl_r49_80131A34_suffix. The msgctrl TU is `-O4,p` with the peephole
  off (see msgctrl_exact_80132A38.c). The digit functions may inline
  _msgctrlMakeDigit, which is not exact.
- **people_data** candidate_8014402C and r49_80140A9C_suffix. They are tiny;
  check which people.c flags apply.
- **Others:** pkjb (80072D58, 800733D0), GScolsys2Thru_8011163C,
  math_range_800CE378, effect_visual (8013AB60, 8013C670), cardesavedata
  suffix, memo suffix, field_candidate_801CB834, gs_light_800DC878,
  menu_middle, dbgMenu_r61_middle, menuPokemon suffix,
  gs_pokemon_summary prefix, toolentry prefix and menu_candidate_80075390.
- **field_range_801DF790 + r41_801E075C**: the fn_801E09E0 trio (not
  attempted).

## Lessons

- The report's objdiff pairs jump tables and dtk's `R_PPC_NONE`
  annotations more leniently than `objdiff-cli diff`. The old
  sdk_r52_800BD7A0_prefix object scored 97.2% in the CLI but 100% in the
  report. Trust report.json.
- A carve that defines data which dtk annotated as an `R_PPC_NONE` addend
  (CardData's `addis 0x8000`) loses that pairing and scores 99.9%. Leaving
  the data extern kept it at 100%.
- For range files whose declarations sit outside any `#if` guard
  (sdk_range_800BB30C.c, gba_conv.c, gba_misc.c), a carve can include or
  copy only the declaration block and then the bodies in address order.
- Cross-unit statics resolve under dtk's address-suffixed names
  (`Reschedule_8047A768`), which the target objects already use.
