# Near-link tier 2 (lane D25, 2026-09-30)

Worktree `/private/tmp/pkmn-decomp-d12`, branch `claude/decomp-d25-tier2`.

## How the list was built

The list comes from `build/GC6E01/report.json`: incomplete non-fight units
with at most 4 inexact functions, all at 93% or better. It is ranked by unit
bytes per missing byte.

These were skipped, as the brief asked:
- fight_* (D24);
- GScolsys2Thru* (D23);
- gs_msg* and gs_title*;
- hsd/bytecode;
- the camera, cbPoison and _flagSet walls;
- pkjb_candidate_80072A00;
- the gs_candidate_801E09E0 trio;
- menu_candidate_80077ED4;
- effect_visual 8013AB60/8013C670;
- fsys_file_r48.

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
| 8fa2dc2a | dolphin/sdk_range_800AF8A0 (CARDBios middle, 7 functions) | Standalone carve of sdk_range_800AE3F0.c behind `SDK_EXACT_800AF8A0_800B016C`, functions in address order. CARDInit uses XD's form (TeamOrre/xd-decomp@4989794e). It keeps the block base in r30 only when `__CARDBlock` (lbl_803FC620) is *defined* in the TU, as CARDBios.c does. As an extern it is 96.2%, at every compiler version and flag. So the unit owns .bss 0x803FC620-0x803FC840 (split change). Clean |
| d4d12707 | dolphin/sdk_range_800B771C_r40_800B7D3C (GXAttr: GXClearVtxDesc .. GXSetArray) | Standalone carve. `SDK_RANGE_800B771C_DECLS_ONLY` takes only the range file's types, and the five functions moved out of the range file. GXClearVtxDesc drops its pragmas and volatile re-read. The unit owns the two switch tables at .data 0x80312B48-0x80312BD0, so data_803127F0_prefix now ends at 0x80312B48. __GXSetVAT: see the lessons below. RULE-EXCEPTION(user-approved), row added |
| 4bbc6a7b | game/pokemon_range_801226D0 (pokemonGetEffortFromPokemon) | Found with the GC/2.6 regalloc dump and simulator (what-if permutation of vreg order). First-loop colouring: entry declared first. Held-item conversion hoisted: read `(u16)item_id` inside the loop, with no `original_item` local. Second loop uses its own index `j`, declared before `total`, so the entry split is numbered above it. The clamps are written `gain -= a + gain - cap`, with gain first in the per-stat sum. Clean |
| f636f5cf | game/gbaCommunication_candidate_80091DA4 (9 functions) | Standalone carve with the unit-wide `-opt nopeephole` of gbaCommunication_prefix. The functions moved out of gbaCommunication.c, which keeps its peephole-off state for the code that follows. fn_80091F48's shadow-receiver setup is an inline expanded twice there and five times in fn_80092140 (repeated expansion). With retail's declaration order it is exact. Clean |
| 2a5d29cb | game/dbgMenu_r61_middle_80133510 (3 callbacks) | Standalone carve at the TU's flags. fn_8013356C's `id = -1; switch (arg2) { case 0: id = 1; break; }` gives retail's `li; beq; b; li`. The ternary and if/else forms give 95.8-96.1%. All three callbacks were added to FORCEACTIVE. Clean |
| 597cc531 | game/gs_range_candidate_801DF1D0 (waza weather update/clear/start) | fn_801DF1D0's bit test and random-threshold test were inverted. The fix uses retail's branch order and `threshold = t * (2 - t); threshold *= 1/60`. The unit owns .sdata2 0x8047E3D8-0x8047E3F0, which only fn_801DF1D0 reads; MWCC emits them in order. sdata2_8047E390 is split, and the rest moved to sdata2_8047E3F0.c. fn_801DF474 moved to a new candidate, gs_range_candidate_801DF474.c (split change). 0.5f/0.0f are shared stand-ins: RULE-EXCEPTION(user-approved), row added |
| 031eab63 | game/window.c (windowAllocMemory, coordinator priority) | Already linked; now policy-clean. The window TU is built with `-opt nopeephole`, so the push/peephole-off/pop pragma was redundant. The source now uses a WindowWork view (0xAC handle, 0xB0 block) |

## Walled

- **menuNameEntry_r56_80026370_prefix** (fn_800263B0 95.6, fn_8002641C 94.8).
  - Retail's invalid-index branch is `li r0,0xff; mr r5,r0; mr r6,r0`. Every
    form tried gets constant-propagated to three `li`:
    - separate stores;
    - chained `a = b = c = 0xff` in both orders;
    - `r5 = r0; r6 = r0`.
  - The file's `#pragma optimization_level 4` lines are sticky. With them
    removed, -O3/-O2/-O1 are no better.
  - The unit would also need a carve out of the whole menuNameEntry.c.
- **floor_character** (fn_80116D30, 98.6).
  - Only colouring differs. What-if on the 2.6 dump:
    - retail needs the vreg order kind < i < rec < skind < arg < offset <
      text < floor;
    - so `arg` must number above two locals, which a parameter cannot.
  - These don't work:
    - declaration climbs;
    - an arg copy local;
    - the event loop as an inline (92.9-93.5).
- **pkjb_candidate_80073700** (fn_80073700, 98.4).
  - The file header describes retail's data-pointer copy in the loop
    preheader.
  - A receive-block inline taking `data` as a parameter does make that copy
    (`mr`). But it renumbers the response/status stack slots and the call
    registers (93.5).
- **pokemonEvolution** (pokemon_range_80128524, 98.5).
  - What-if found an exact vreg order: dst, species, evolution, src, level,
    move, waza, nickname, count, then the two temps.
  - That puts src and waza after other values, so parameter order cannot
    produce it. They are probably reached through inline parameter copies.
  - Not pursued further. The unit's other two functions (99.0, 97.2) are
    also open.
- **win_msg** (winMsgDraw, 97.7).
  - These don't reproduce the `mr r29,r3; mr r28,r0` copies of the else
    branch:
    - accessor inlines for width/height;
    - a pointer to the size pair;
    - s16 locals.
- **field_candidate_801CB61C** (fn_801CB61C, 96.4).
  - Retail reuses r31 for resource, model and the part.
    `GSmodelGetPart`'s result goes `mr r0,r3; mr r3,r31; mr r31,r0`.
  - Without its local pragmas it drops to 85% (93.8 with -opt nopeephole).
    Not pursued.
- **gs_npc_event_candidate_800301B0** (fn_800301B0, 97.2). Retail loads
  `*(s16*)(r4+6)` through the saved copy (r30) and sets model = 0 after it.
  Declaration and statement order, nopeephole and noschedule change nothing.
- Looked at and left, being larger jobs:
  - wazaSequenceCamera_candidate_801D4DA0: C++ TU sharing its pool;
  - field_range_801ECFE0: several distinct differences.

## Lessons

- **Defined versus extern arrays.** An array defined in the TU and one
  declared `extern` get different address CSE. CARDInit's base register
  exists only with the definition. When an SDK carve needs the TU's own
  .bss object, owning the .bss range is the clean fix.
- **Retail's TU flags.** When a TU's candidate source is full of local
  `#pragma peephole off` (gbaCommunication, window), check the linked
  siblings' unit flags first. The pragma usually just restates a unit-wide
  `-opt nopeephole`.
- **Frontend temporaries.** GC/2.6's frontend dump (`frontend-01-ast-after-optimizations`) shows
  IRO temporaries (`@63` etc.) for split loop variables and hoisted
  int conversions. They are numbered in reverse creation order, after all
  declared locals, and locals are numbered in reverse declaration order. A
  second loop's reuse of a variable becomes a split temporary. A fresh
  declared variable (pokemonGetEffortFromPokemon's `j`) puts it back in
  declaration order.
- **What-if search.** The d16 simulator reproduces GC/1.3's colouring
  whenever GC/2.6 emits the same code, and it did for both pokemon
  functions. A permutation or pairwise climb over node positions then says
  which order retail needs, before any source is edited.
- **Owning a conversion bias.** A carve can own an int-to-float bias when no
  other code references the symbol. Grep `build/GC6E01/asm` for the label
  first.
