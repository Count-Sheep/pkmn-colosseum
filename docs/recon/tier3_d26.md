# Near-link tier 3 (lane D26, 2026-09-30)

Worktree `/private/tmp/pkmn-decomp-d12`, branch `claude/decomp-d26-tier3`.
Stopped early: the coordinator moved all lanes to the title-screen boot path.

## How the list was built

The list comes from `build/GC6E01/report.json`: incomplete non-fight units
with up to 6 inexact functions at 95% or better, or 1-2 at 85% or better.
It is ranked by unit bytes per missing byte. These were skipped:
- the walls in tier2_d25.md, carves2_d22.md, nearlink_d18.md and
  walls_d16.md;
- fight_*, gs_msg*, gs_title*, GScolsys2Thru, bytecode, THPDec and camera.

## Acceptance

Every link passed all of these:
- configure;
- `ninja -j2 all_source build/GC6E01/report.json`;
- a full `ninja -j2`, with main.dol and common_rel.rel SHA-1 OK;
- `quality_scan` (clean);
- `check_regression` (no regressions);
- the unit Matching.

## Linked

| Commit | Unit | What it took |
|---|---|---|
| e49b08ca | game/floor_event_exact_80115CB4 (floorEventGetTresureList, floorEventSetTresureDisp, floorEventChangeTresure) | GetTresureList compares with `target == found++`. entry and found are cleared before the type test, and the locals are declared type, entry, index, found, target (found by an init/declaration search). It is a data-free carve at the TU's -opt nopeephole. The unit was renamed from floor_event.c in splits (same range), so floor_event.c stays the whole-TU candidate. The same fix in its floorEventFindTresureEntry inline lifts floorEventCtrlTresure from 92.9 to 97.9. The log strings are extern stand-ins: RULE-EXCEPTION(user-approved), row added |

## Candidate-only improvements

| Commit | Unit | What changed |
|---|---|---|
| d0ed99e0 | game/wazaViewer_candidate_801D5464 | fn_801D624C 95.4 -> 100: see the notes below. fn_801D5A94 98.74 -> 98.77: the 0x664 buffer is stored straight from the call |
| cc9acfdc | game/pokemon_range_80121C18 | pokemonCreateSequence 97.6 -> 99.6: see the notes below |
| cc9acfdc | game/effect/effect_visual_r51_8013CA48_suffix | fn_8013CBF0 96.8 -> 100 (report): the alpha is a single expression, `(u8)((f32)color[3] * (1 - ix*x*x) * (1 - iz*z*z))` |
| cc9acfdc | game/field_range_80114AE0 | floorReadMapPostFunc 92.1 -> 97.2: see the notes below |
| fd419341 | game/gs_npc_event_candidate_800324A0 | fn_800324A0 93.8 -> 100. fn_8003258C 97.8 -> 100 (report). See the notes below |

### wazaViewer: fn_801D624C

- The function keeps `u8* work = lbl_804673F8` in r31 through its calls.
- The spawn inlines take `work` for their staged reads.
- The defender store (0x620) and the index loops address the global.
- An `else` arm replaces the early return, which gives retail's second `b`.
- The defender id test is `if ((id = work[0x648]) != 0)`: a caller local
  numbers the id below the CSE temporary.
- The advance loop is `for (...; (i = work->idx) < 2; ...)`, so the body
  uses the value the test loaded and the step reloads.

### pokemonCreateSequence

- Rebuilt from inline helpers:
  - fn_80121ADC's body is `pokemonSeqCheckSlot`, which returns s32 and is
    compared as `(u8)x == 1`;
  - a species-valid check;
  - a rare (shiny) check;
  - a slot 3..8 scan.
- fn_801DE190 has a u16 first parameter.
- The personality and the sequence share one variable.

### floorReadMapPostFunc

- The lights keep their own index, cleared at the top.
- One variable holds the archive, then its public block.
- Peephole is off, as in the rest of the file.

### gs_npc_event

- fn_800324A0:
  - GSresGetResource takes (group, id);
  - the id is passed as read back from lbl_8047A418 (write through the
    global).
- fn_8003258C:
  - the counter test is `x < 0xFFFFFFFF`;
  - the coupon switch has cases 6/12/18/24 and a 600 default.

## Walled

- **wazaViewer_candidate_801D5464.** Four functions are open.
  - wazaViewerThread and fn_801D6A64 share the PollCommand expansion:
    - retail colours getsize.file/block r22, record r23 and loadfile.file r24;
    - the d16 simulator cannot reach this by any vreg order (climbs over all
      nodes), so the interference graph differs;
    - these don't work: declaration permutations, and PollCommand written
      in place.
  - fn_801D603C and fn_801D6A64 have retail's `li r4,0; mr r5,r4` for
    `arg2 = arg1 = 0`:
    - GC/2.6 dumps show constant propagation turning our `mr` into `li`;
    - these don't work: chains, separate assignments, and u16/s16/u8/int
      types.
  - fn_801D5A94: retail has slot r31, modelPath r30 and viewerData r29. No
    vreg order reaches that, so the graph differs here too.
- **effect_visual_r51_8013CA48_suffix.** Both functions match in the
  report, but the carve cannot link.
  - Its pool (.sdata2 0x8047D240-0x8047D260) is ownable, but 0.0f
    (0x8047D23C) is pooled by seaEffectStart (the C718 unit, 67%).
  - An `extern const f32` stand-in reschedules fn_8013CBF0's
    rowStep/columnStep blocks (89.4%). These were tried: non-const,
    volatile, array, noauto, nopeephole.
  - With a literal, 0.0f lands at 0x8047D25C (the pad), which is the
    same bytes but at a different address.
  - A clean link needs the C718..CE58 range with its pool from 0x8047D228.
- **gs_npc_event_candidate_800324A0.** All three functions are exact as a
  carve (scratch file ne_carve_blocked.c), at -opt nopeephole.
  - It must own the switch table at .data 0x802E4FE0-0x802E502C.
  - The next table (sysvars, 0x802E502C) starts 4-aligned. A compiled data
    object aligns .data to 8, so the continuation moves to 0x802E5030 and
    main.dol fails. This is the same limit as the gs_gfx dl note in
    configure.py.
  - A link needs the following data owned by its real TU, or an
    alignment-4 data object.
- **pokemon_range_80121C18** (99.6). Retail copies fn_801DE190's result
  r3 -> r0 -> r30.
  - In our code the copy coalesces straight into the shared variable, and
    a separate `sequence` variable swaps pokemon/personality.
  - What-if: pokemon's vreg must sort after personality, which a parameter
    cannot do.
- **field_range_80114AE0 floorReadMapPostFunc** (97.2, colouring). Retail
  has lightIndex r31, param r30, owner r29, group r28, strings r27, public
  r26, size r25 and index r24.
  - What-if reaches this only by moving codegen temporaries (the strings
    address and the unload pointer).
  - Declaration permutations top out at 97.46.
- **field_r55_801CBA90_prefix fn_801CBAB8** (97.1). Retail's `li r28,0; mr
  r30,r28; mr r31,r28` copies survive only at -O2, which gives 99.0 with a
  done/input swap and an epilogue order difference. Declaration and chain
  orders don't help at either level.
- pokemon_range_80128524 was not tried: pokemonEvolution is on the wall
  list.

## Lessons

- **A cached work pointer.** When retail keeps a global's base in a saved
  register through calls but rematerializes it in a few places, the
  source has a local pointer (`work = lbl`). The rematerialized places
  address the global directly.
- **Loop-test values.** MWCC's load deletion reuses a value only in blocks
  numbered after the load. A `for` body cannot reuse its test's load;
  `(i = x) < n` in the test gives retail's form (body uses the test's
  value, the step reloads).
- **Chained compares.** `if (target == found++)` gives retail's `mr r0,rX;
  addi rX,rX,1; cmplw`.
- **Implicit versus explicit conversions.** A u16 prototype parameter
  converts at the call, after the other arguments' moves. An explicit
  `(u16)` cast converts before them. This decides the `mr`/`clrlwi` order.
- **Pool literals versus stand-ins.** An extern stand-in for a literal is
  not scheduling-neutral: MWCC orders a real pool literal's load
  differently from an extern global's.
- **Compiled data alignment.** Data objects align .data to 8. A carve that
  owns a switch table followed by another TU's 4-aligned table cannot
  link.
