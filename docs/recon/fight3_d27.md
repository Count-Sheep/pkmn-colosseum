# Fight near-links, lane D27 (2026-09-30)

Scope: the fight units D24 did not try (fight_range_8023B498, fn_80213E94,
fn_8023A308, fn_80220868, fn_802232F4), then other fight_* near-link units
in the report. I skipped the documented walls. Every linked unit passed:

- configure;
- `ninja all_source report`;
- full ninja, with main.dol and common_rel.rel SHA-1 OK;
- quality_scan;
- check_regression (+1 exact each).

## Linked

| Commit | Unit | What it took |
|---|---|---|
| cfb4c3ae | fight_side_candidate_801F72B0 (fightSideGetFightTrainerGridParam 99.26) | The u16 kind parameter is compared through a named local copy (`kind = param_2`). This numbers the compare temp ahead of the Yrot value, giving retail's r0/r3. Clean. |
| 0ac2a0a9 | fight_pokemon_candidate_801FE3F8 (6 fns, fightOutPokemonSetHensinStatusAfterLevelUp 99.34) | The loop body is an inline, `fightPokemonCopyHensinField(context, fieldId)`. Inside it, the 0xD5/0xD7 slot reads are plain calls and the two slot-to-Pokemon reads use a shared getter inline. Locals are declared destPokemon, srcPokemon, srcSlot, destSlot. The unit is now a standalone six-function file. The loop-body inline is single-use (RULE-EXCEPTION row). Slot-getter inlines broke the two sibling functions, so they stay as plain calls. |
| ee9f41a4 | fight_floor_candidate_801F4354 (fightFloorGetFightOutPokemonPtrToFightTrainerPtr 98.36, 4 bytes short) | The trainer search is the next function's body (fightFloorGetFightPokemonPtrToFightTrainerPtr) as a static inline, and the caller keeps its own null check. RULE-EXCEPTION row: duplicate-body inline. |
| a3eeb1bc | fight_target_801F0134 (fightTargetGetTragetPtrToRelativeHostSideFightTargetId 97.88, 4 bytes short) | The id loop is an inline that returns i or 0. The table count is an inline local assigned in the loop test (`(index = i & 0xFFFF) < (count = lbl_80478D40)`) and declared after index. That gives count r4 and index r5, and replaces the candidate's `register` index. The unit now compiles only this function (FIGHT_TARGET_801F0134_ONLY). Single-use inline (RULE-EXCEPTION row). |
| 9d27446e | fight_range_80218FDC (fn_8021908C 97.89, 4 bytes short) | The user's 0xD5 and 0xD6 slots are read into their own locals (firstSlot, secondSlot, declared right after status) before the two fightPokemonGetPokemonPtr calls. This replaces the candidate's block temp and reused variables. Standalone, GC/1.3, clean. |

## Lessons

- **A retail `bne; b` exit that is 4 bytes longer than a goto form is an
  inline `return` inside a loop.** The candidates wrote the found-exit as
  `goto done`, which MWCC folds into one `beq`. An inline's `return x`
  becomes "temp = x; goto end". The copy keeps the branch from folding and
  gives the join register (`li r31, 0` / `mr r3, r31`). Two units fell to
  this (fightFloor 801F4354, fightTarget 801F0134). A plain `return i` in the
  caller gives the right branches but loses the join.
- **Inline locals are numbered like frontend temps, and CSE temps come
  after them.** A value read through a global (a table count) becomes a CSE
  temp numbered below the inline's locals. To order it, give it a name:
  assign it inside the expression (`(count = lbl)`). The load stays in the
  loop, and the declaration order now counts.
- **Declaration search over structural forms.** For the hensin function I
  tried each of 36 inline/direct form combinations, then each form's
  declaration permutations, all compiled in a loop (scratchpad/d27/hs/gen.py,
  gen2.py). The loop-body inline was the only structure that reached 100%.
  The simulator (simclimb) pointed to it first: the needed order put an
  inline temp between named locals.
- **Named copies from calls.** Reading the two slot values into their own
  locals instead of reusing the result variables (fn_8021908C) gives
  retail's `mr r0,r3; ...; mr rX,r0` copy chains.

## Walls and partial results

### fn_80220868 (fight_range_80220868, 99.33 standalone)

- Operand order fixed: the first loop is `baseCommand + (u8)bitIndex`, like
  the second (99.28 by score, but structurally right).
- What is left: six first-branch values take a rotated set of r26-r31.
  Retail's order is flags r31, ability r30, flags&8 r29, baseCommand r28,
  (u8)bitIndex r27, current mask r26. The else branch's baseCommand should
  reuse r18.
- `flags` is removed by frontend copy propagation (it becomes CSE temp
  @86). The hoisted loop invariants are @82/@83/@84.
- The simulator, with every temp position free, reaches only 17/19, so the
  cause is structural.
- Tried, none moved it:
  - flags as u32/int;
  - ability as u16;
  - moving flags or ability in the declarations;
  - the load order of mask and flags;
  - a status-id switch inline;
  - a separate second-branch base variable in 6 positions;
  - a ternary base.
- Linking also needs its two jump tables (0x8039A0F0, 0x8039A110) split out
  of data_8039A088.c.

### fn_8023C530 (fight_range_8023B498, 99.61)

- The three copies of the move-validity check are one shape.
  - As an inline, `fightAiCheckWazaHit(trainer, pokemon, move, target)`,
    they compile to the right size once case 1/5/7 keeps its `result`
    form. That scores 99.34.
  - The candidate's per-case block form scores 99.61.
- moveValue and moveType are copy-propagated into call-result temps (vregs
  98/130/131), so declaration order does nothing. The simulator reaches
  15/20.
- Function-scope shared locals: 99.44.

### fn_8023B498 (fight_range_8023B498, 98.26, 4 bytes long)

- 3792 bytes. All 18 callee-saved registers are in use, with stack spills,
  plus a struct-copied rodata initializer. Not attempted.
- The unit would also need .rodata 0x8027A408-0x8027A420 (split
  rodata_80279AE8.c at both 8-aligned ends) and .sdata2
  0x8047E628-0x8047E630. It would also need .data 0x8039A578-0x8039A5D8,
  which is 8-aligned: the jump tables of fn_8023B498, fn_8023C370 and
  fn_8023C530, split from data_8039A538.c.
- fn_8023C370 is 99.87.

### fn_8023A308 (fight_range_8023A308, 98.39, 4 bytes long)

- The candidate's `opt_common_subs off` gives the size-closest form. The
  plain form, which CSEs the `(u16)count` casts, is 96.18.
- Retail recomputes `clrlwi` at each use, keeps
  `fightTargetGetTragetPtrToRelativeHostSideFightTargetId`'s result in a
  new register without the extra `mr r27, r0`, and schedules one `extsb`
  differently. Not solved.

### fn_80224740 / fn_80224158 (fight_range_80224158)

- fn_80224740: the candidate dropped fn_802026E4's arguments. Retail calls
  `fn_802026E4(target, status)` (r3 still holds the target, r4 the table
  status). With the arguments, at -O4,s, it scores 99.31.
- Left in fn_80224740: the tail's script pointer and zero constant are
  swapped (retail r3/r4).
  - Tried every order of the three tail statements.
  - Tried a pc local in each position.
  - Tried a tail inline (six bodies x three parameter types).
- The unit needs -O4,s (retail uses stmw for three registers).
- fn_80224158 (97.98, 1512 bytes) was not attempted: a register rotation
  plus one extra `mr`.

### fn_80213E94 (fight_range_80213A78, 98.66 standalone at GC/1.3.2 and 2.0)

- Register rotation across the whole function. Not attempted beyond the
  survey.
- Linking also needs its jump table at .data 0x80399FF4, which is 4 mod 8
  and so has to share an object with the preceding table's owner. It also
  needs the .sdata2 double at 0x8047E610.

### Not attempted

- fn_802232F4: its sibling fn_802230BC is a D24 wall, so the unit cannot
  link alone.
- fight_floor_candidate_801F2B5C: four functions, all register rotations.
- fight_range_802274F0, fight_range_80230568, fight_range_802331F4,
  fight_range_80234A0C.

Tools, all in scratchpad/d27:

- var2.py: a variant runner that reports the named function. It inserts the
  `pre` text at a `//NULLDEF` marker.
- hs/gen.py and hs/gen2.py: the form x declaration searches.
- cmp.py, ext.py, dbg.sh, vmap.py, simclimb.py: copies of D24's tools.
