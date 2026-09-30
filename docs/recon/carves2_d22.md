# Carve sweep, second pass (lane D22, 2026-09-30)

Worktree `/private/tmp/pkmn-decomp-d12`, branch `claude/decomp-d22-carves2`.
This lane follows `nearlink_d18.md` and `rangecarve_d19.md`. Each link passed
all of these:

- configure;
- `ninja all_source build/GC6E01/report.json`;
- a full `ninja`, with main.dol and common_rel.rel SHA-1 OK;
- `quality_scan` (clean);
- `check_regression` (no regressions);
- the unit reported complete.

None of these functions has a `local_campaign` queue entry, so the claims
list was checked before each edit instead.

## menuOffScreen (for the recomp)

de801226: menuOffScreenCreate, menuOffScreenCheckEnable and
menuOffScreenFadeSync no longer use `#pragma peephole off`.

- **The flag.** game/menu_offscreen.c is built with a unit-wide
  `-opt nopeephole`. With it, all 12 functions are 100%. With the peephole
  pass on, FadeSync, CheckEnable and Create fall to 88.9%, 84.2% and 52.3%.
- **The loops.** The wait loops are plain
  `while (wait) { if (flag) break; _threadSwitch(); }`.
- **The flag variable.** The capture-done flag lbl_8047AD20 is written by
  the capture callback and polled by these functions. It is declared
  `extern volatile u8`. That gives retail's lbz/clrlwi return and replaces
  the old volatile pointer-cast read.
- **Create** ends with `return menuOffScreenCheckEnable(wait);`, which
  auto-inlines as it does in retail.
- **FadeSet** no longer has dead conditions or a comma expression.

There is no rule exception.

## Linked

| Commit | Unit | What it took |
|---|---|---|
| f4e8e267 | dolphin/sdk_candidate_800B671C.c (__GXInitGX) and new dolphin/sdk_exact_800B6FE0.c (GXCPInterruptHandler, GXInitFifoBase, GXInitFifoPtrs) | The old unit spanned two TUs built with different flags. __GXInitGX (end of GXInit.c) matches only with the peephole pass off, so its unit is built with `-opt nopeephole`. The GXFifo.c head matches only with the pass on. Both are guarded carves of sdk_range_800AE3F0.c: `SDK_EXACT_800B671C_800B6FE0` and `SDK_EXACT_800B6FE0_800B71F0`. Only __GXInitGX reads the pool at 0x8047C2E4-0x8047C308, which starts at 4 mod 8. So the unit owns 0x8047C2E0-0x8047C308, from `gx` onward, and sdata2_8047C2A0_prefix.c now ends at 0x8047C2E0. The inlined FIFO handlers' static copies are dead-stripped. Clean |
| 9bdd93a7 | game/hero_move_r49_8012B5E4_o4s.c (heroMoveChkHinderClear) | Text-only carve with the TU's static inlines and pool stand-ins. sqrtf copies 0.5 and 3.0 into `const f64` locals. Read in place, the stand-ins move the argument from retail's f4 to f2 (99.59%). RULE-EXCEPTION(user-approved) |
| c2dabebf | game/hero_move_r46_8012C540.c (heroMoveCheckEvent) | Text-only carve with plain pool stand-ins. RULE-EXCEPTION(user-approved) |
| e43aa2db | game/msgctrl.c (head, 4 functions), msgctrl_candidate_80131714.c (4), msgctrl_r49_80131A34_suffix.c (7) | The local pragmas are gone (peephole, scheduling, optimization_level 2). All three units use the unit-wide `-opt nopeephole` of msgctrl_exact_80132A38. msgctrl.c emits only its head. The rest of the file is behind `MSGCTRL_WHOLE_TU`, which the three remaining candidate chunks define. The message object's bytes at +0x42 and +0x43 (line space, baseline bias) are now `s8`, which gives lbz/extsb/stb. Clean |
| 4bd4d53e | game/people/people_item_getters_exact_80143C50.c now runs to 0x80144064 | itemDataBiosCheckExportable inlines fn_80143FCC (the important-flag getter), which is defined in that unit. So the function joins the unit (split change) and people_data_candidate_8014402C is removed. Clean |
| 6f3f92b3 | game/people/people_data_r49_80140A9C_suffix.c (fn_80140A9C) | Clean struct swap of the 4-byte {itemDataId, num} records. It replaces a volatile temporary |
| b3b388ea | game/menu/pkjb_candidate_80072D58.c, pkjb_candidate_800733D0.c | Already exact. Each has a single-use static inline for the send loop. RULE-EXCEPTION(user-approved), rows added |

## Walled or left

- **hero_move_r49_8012BBA8_suffix** (heroMoveTermEvent 100%, heroMoveInitEvent).
  - In InitEvent the {100, 101} table is copied twice, once for each inlined
    getResID. Retail keeps a temporary copy at 0x8(r1) and fills the second
    table from it.
  - Only a struct copy of an extern stand-in reproduces that:
    `ids = *(const HeroMoveResIDTable*)&lbl_8047D030`. But it relocates the
    second word as `lbl_8047D030+4` instead of `lbl_8047D034`, which scores
    99.94% and is not accepted by the report.
  - These don't work:
    - element stores, including D18's `p[-1]/p[0]` form: 95.8%;
    - an explicit shared temporary: 97.4%, with an extra stack slot;
    - a compound literal: 85.5%.
  - A clean fix would need either the whole hero_move TU linked with its
    pool, or lbl_8047D030 made one 8-byte symbol. The second option touches
    the ~15 sources that name lbl_8047D034.
- **fn_801E09E0 + field_range_801DF790 + r41_801E075C**: not attempted.
- **Pool-sharing small units** (need stand-ins or pool ownership):
  - GScolsys2Thru_candidate_8011163C (lbl_8047CF58/5C);
  - effect_visual_candidate_8013AB60 (lbl_8047D1E0/E8);
  - field_candidate_801CB834 (lbl_8047E150/58);
  - gs_light_candidate_800DC878 (lbl_8047CA78/88);
  - dbgMenu_r61_middle_8013327C (.rodata lbl_80272AE0/F0);
  - gs_pokemon_summary_r57b_8001501C_prefix (.rodata lbl_80266918).
- **Pool-free small units** (next easy targets): these read none of the
  TU's pool literals. Each still compiles a whole range file today, so each
  needs a standalone carve:
  - menu/cardesavedata_r51_80083AF4_suffix;
  - memo_r57b_8025FD34_suffix;
  - menu/menu_middle_r50_8006EE7C_prefix;
  - menuPokemon_r50_8001D624_suffix;
  - toolentry_r55_8025D364_prefix;
  - menu/menu_candidate_80075390_r46_8007C23C;
  - effect/effect_visual_r51_8013C670_prefix, which is exact only under
    `#pragma global_optimizer off` and so needs a unit flag or an exception.
- **crt/math_range_800CE378** was not looked at.

## Lessons

- A unit-wide `-opt nopeephole` lets a TU drop its local `#pragma peephole`
  blocks. It is clean when every function in the unit matches with the flag.
  Both GXInit.c and msgctrl.c were built that way. GXFifo.c, right after
  GXInit.c, was not, so a range unit that crosses a TU boundary may have to
  be split by flags.
- When a range unit crosses a TU boundary, check where the split falls
  before choosing flags. __GXInitGX and GXCPInterruptHandler were one unit.
- A redundant clrlwi before a byte store (the value is converted back from
  `(u8)(s8)`) means the field is signed. Declaring it `s8` makes
  optimization_level pragmas unnecessary.
- When stand-ins take different FPRs than the literals would, copying them
  into `const` locals at the point of use restored retail's colouring
  (heroMoveChkHinderClear's sqrtf).
- If a function inlines a global helper defined in a linked neighbour unit,
  extending that unit (split change) avoids a static inline copy.
- `config/GC6E01/object_map.freeze.json` has been stale for a long time
  (2287 expected units against 2082). It is not part of link acceptance, and
  it was not updated.

## Second pass (coordinator follow-up)

Each link passed the same acceptance as above. When several units were
validated in one run, they were then committed one per unit from the
validated tree.

| Commit | Unit | What it took |
|---|---|---|
| bab04b6b | menu/cardesavedata_r51_80083AF4_suffix.c (fn_80083AF4, fn_80083BF8) | TU's NULL-checked out-pointer setters kept: retail's result lives in that stack slot. RULE-EXCEPTION(user-approved), single-use inlines. Plain locals: 81.4%/90.6% |
| 86ac3f65 | memo_r57b_8025FD34_suffix.c (3 functions, -O4,s) | memo.c's statement shapes with readable names. Clean |
| 82d1be78 | menu/menu_middle_r50_8006EE7C_prefix.c (2) | `switch (state) { case 2: ... break; }` gives retail's `beq; b` pair. Unit-wide nopeephole replaces local pragmas and dead `(!menu && !menu)` conditions. Clean |
| dcbce7cf | menuPokemon_r50_8001D624_suffix.c (fn_8001D624) | Clean body indexing the u16 tables in data_802E4DB0.c. Unit-wide nopeephole (81.1% with the pass on) |
| 1ecb910e | toolentry_r55_8025D364_prefix.c | toolentry.c's body (fn_8006B09C on both arms). Clean |
| b6486423 | menu/menu_candidate_80075390_r46_8007C23C.c (GBA alarm handler) | Unit-wide nopeephole replaces `scheduling off` (either gives retail). No FORCEACTIVE needed |
| e66b2b25 | GScolsys2Thru_candidate_8011163C.c | Stand-ins for 0.0f/1.0f. Tried a plain or a pointer-cast read at each use: the loop's starting 0.0f read through a cast gives 100% (plain reads everywhere: 98.4%). RULE-EXCEPTION(user-approved) |
| e62cd143 | field_candidate_801CB834.c (fn_801CB834, fn_801CB954) | Owns .sdata2 0x8047E150-0x8047E160 (0.5f + int-to-float bias, read by nothing else). battle_sdata2_8047E0A8 now ends at 0x8047E150. Unit-wide nopeephole, as field_range_801CB180. Clean |
| 5806fa32 | gs_light_candidate_800DC878.c (GSlightPopState) | Stand-ins the linked gs_light carves already use, plus a static inline copy of GSlightSetAnimIndex (retail expands it here). RULE-EXCEPTION(user-approved) |
| da1d1330 | dbgMenu_r61_middle_8013327C.c (5 functions) | Clean at the TU's flags. The check's strings are real literals: retail keeps dbgMenu strings in .rodata, so the unit is built `-str reuse,readonly` and owns .rodata 0x80272AE0-0x80272B08. The DataCandidate blob rodata_802729C0.c is split around it, with the remainder moved to a new rodata_80272B08.c. The five entries are added to FORCEACTIVE: they are menu callbacks referenced only from unlinked data, and without it 0x130 bytes were stripped |
| a46dc09f | gs_pokemon_summary_r57b_8001501C_prefix.c (2) | Clean. The page table lbl_80266918 is a named six-entry table; advancing a SummaryPageEntry pointer gives retail's base/offset registers. Unit-wide nopeephole (fn_8001501C is 38% with the pass on) |
| e989926b | crt/math_range_800CE378.c (__ieee754_sqrt) | fdlibm source as written. The `one +- tiny` tests fold, leaving one 1.0 literal that nothing else reads, so the unit owns .sdata2 0x8047C968-0x8047C970. sdata2_math_8047C8A0 ends at 0x8047C968 and the four constants after it moved to a new sdata2_math_8047C970.c. Clean |

### Still walled

- **fn_801E09E0 trio.** The two open diffs are copies, not colouring order,
  so simp.py's what-if does not apply to them:
  - Our `sequencePositions` is `addi r0,lbl@l; mr r30,r0`, where retail has
    `addi r30,...` directly.
  - In case 10, retail has `bl floorOpenObject; mr r0,r3; ...; mr r29,r0`
    and `addi r4,r30,0` for `&seq[0]`.

  Tried, all at or below 98.6% in objdiff-cli (the report counts literal
  names and scores the function 99.17%):
  - inline helpers of four shapes (open-only, open plus copy, table lookup
    inside, ball lookup inside);
  - a constant index variable;
  - a return temporary assigned after the copy;
  - a struct of three GSvecs, and `&seq[0].x`;
  - the global read directly (94%: MWCC hoists +0xC/+0x18 into their own
    registers);
  - const and non-const variants, and moving the declaration or assignment
    (c1-c6).

  `-opt nopeephole` is wrong for this function: retail has `clrlslwi`. The
  trio has to link together, because MWCC emits its own int-to-float biases
  and the pool 0x8047E3F0-0x8047E428 is read by name from fn_801DF474 to
  fn_801E09E0. The unit would span fn_801DF474 to etctoolSetPokemonNakigoe
  (0x801DF474-0x801E0FB4), which starts inside gs_range_candidate_801DF1D0.
  That unit's fn_801DF1D0 (93.8%) reads only 0x8047E3C8-0x8047E3E8, so the
  cut at 0x801DF474 and 0x8047E3F0 is clean.
- **effect_visual_candidate_8013AB60.** Its int-to-float bias 0x8047D1E8 is
  also read by the unlinked 8013AD9C and 8013B268 chunks. This is D18's
  conversion-bias wall.
- **effect_visual_r51_8013C670.** It is exact only with
  `#pragma global_optimizer off`. A clean source form scores 76.9-84.6% at
  every unit flag tried (default, nopeephole, noschedule, -O4,s, -O3).
- **hero_move_r49_8012BBA8_suffix** (heroMoveInitEvent): see above.
- The rest of the all-100% list is on the brief's skip list: mtx.c
  partitions, GXProject, fight_*, gs_msg*, THPDec, OSCache and camera.

### More lessons

- A string literal's section is evidence of the TU's string flag. dbgMenu's
  strings are in .rodata, so its carve is built `-str reuse,readonly`. C++
  mode alone still put them in .data.
- When a carve's literal sits in the middle of a data unit, split the data
  unit in three. The part after the carve must start 8-aligned.
- For a stand-in that CSE merges, trying a plain or a pointer-cast read at
  each use (2^n variants, cheap with cmp.py) found the one retail needed in
  a few seconds.
- A `switch` with a single `case N:` and a `break` reproduces MWCC's
  `beq over b` pair without local pragmas.
