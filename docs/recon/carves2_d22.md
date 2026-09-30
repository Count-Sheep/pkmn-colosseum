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
