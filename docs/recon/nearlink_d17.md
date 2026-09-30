# Near-link sweep, lane D17 (2026-09-30)

Scope: incomplete units whose inexact functions are all 98%+ (recomputed
from build/GC6E01/report.json), then, after the D17/D18 split, only the
fight_* units, plus the fight_* units whose functions were already all
100% but unlinked.

## Linked

| Commit | Unit | What it took |
|---|---|---|
| ee184fb6 | gbaCommunication_prefix (0x8008C7B0-0x80090720, 16 fns) | fn_8008F524: first wait and the three shadow targets as their own locals, declared first. Functions in address order, unit `-opt nopeephole`, extern const pool literal (RULE-EXCEPTION row) |
| 1236ae5c | fight_range_8021A338 (fn_8021A338, fn_8021A478) | GC/1.3.2 -O4,s; plain source replaces duplicate stores, volatile read and pragmas |
| d811f04f | fight_range_802128D0 (fn_80213270) | four flag clears as plain read-modify-writes of the global |
| df8702f4 | fight_range_80213558 | standalone, -O4,s instead of the optimize_for_size pragma |
| 562d75ed | fight_candidate_8020DA14 (fightKoukaDoFightKoukaJoukenAndKouka) | standalone, accessors external |
| c37f1425 | fight_pokemon_r58_801FEF74_middle (fightOutPokemonGetJoutaiMigawariHp) | standalone, unit `-schedule off` instead of the pragma |
| dd339e9e | fight_candidate_8020DAD0 | owns .sdata2 0x8047E528-0x8047E530 (1.0f, 0.5f as literals; fadeSet prototyped (f32, u32)); sdata2_8047E508 split |
| f0fa6c29 | fight_range_8022D6BC | GC/1.3.2 -O4,s; owns its switch table .data 0x8039A478-0x8039A538; data_8039A220 split; {1, 2, 5} table extern (RULE-EXCEPTION row) |

Lesson: several fight_range candidates imitate a GC/1.3.2 scheduling
difference with duplicate stores, volatile reads and opt_* pragmas. Try the
plain form under GC/1.3.2 first (fn_8021A338/fn_8021A478 went from 98.75/99.33
to exact that way).

## Walls

### fn_8022BE2C (fight_range_8022BE2C, 99.81%)

- Two variables take swapped callee-saved registers: `t4` (retail r29) and
  the first web of `g` (the `(u16)fn_8012640C(e, 0, 0x87, 0)` limit, temp
  r120, retail r25). The mwcc-debugger dump plus D16's simp.py (K=29,
  mincd, pass) reproduces ours. Retail colours t4 before the g webs.
- Also left: `avail[fn_800E0C54() % t3]` has its three volatile temps in a
  different order (retail: rand r4, count r3, base r5), and an `and.`
  operand order.
- Tried: declaration moves of t4 and g (each position), t4 first, g after
  f, g as u16, t4 as u8/s32/(u8) cast, operand order of the `&`, inline and
  separate forms of the modulo. None beat the base.
- Linking would also need .data 0x8039A388-0x8039A3C8 plus 0x8039A3C8's
  table (8-aligned, fine), .rodata 0x80279FF8 and .sdata2 0x8047E604
  (4 mod 8, so only with fn_8022D6BC's 0x8047E600; see below).

### fn_80219354 (fight_range_candidate_802192B4, 99.73%)

- Same modulo idiom: `typeArray[(u16)((u16)random % (s32)(u16)count)]`.
  Debugger: rand r76, count r77, base address r83, coloured in descending
  order, so the base gets r3. Retail's order needs the stack-array base
  created before rand and count.
- Tried: pointer, `*(a + i)`, `i[a]` and byte-offset forms, a named base
  pointer (copy-propagated away), four random-modulo inline helpers, a u16
  prototype for fn_800E0C54, GC/1.3.2 and 2.0. No change.

### fn_80219964 / fn_80219B2C (fight_range_candidate_80219838, 99.78% each)

- `move` (retail r28) and `sVar8` (retail r30) are swapped. The candidate's
  one-member unions make floor and move high-numbered temps. As a plain
  variable, move gets an extra `mr r0,r3; mr r28,r0` copy (115
  instructions).
- Tried: all orders of sVar8, sVar7 and move with a plain move, union temps
  for sVar7 and sVar8, and a Follow Me redirect inline helper for the second
  half, which fn_8022B2CC also has. The helper costs a register (98.75).
- fn_8022B2CC (99.87) has the same side/floorStatus swap in the same
  idiom, so the three should fall together.

### fn_80238060 (fight_range_80238060, 99.49%)

- The u8 slot parameter: retail truncates it in place (`clrlwi r29,r29,24`)
  late in the body and reuses the register for pokemonWazaGetMaxPP's
  result; ours truncates into a new register. The body looks like expanded
  getter inlines (repeated fightFloorGetStatus/fightTrainerGetStatus/
  fightPokemonGetPokemonPtr sequences).
- Tried: u32 parameter with `(u8)` or `& 0xff` at the uses, a u8 slot
  local, `r5 = (u8)r5` and `r5 &= 0xff` before the first use.

### fightTrainerAiWazaValueKiaipanti (fight_trainer_ai_waza_value_candidate_8024DC7C, 98.56%)

- A register rotation. Debugger plus simp.py: rawCount's first web (temp
  r44) is pushed in sweep 1 and coloured after handle and the three
  parameters. wi.py says three more interferences on it give exactly
  retail's allocation (rawCount r31, handle r30, then r29/r28/r27).
- Tried: every declaration order of the five locals, removing the parameter
  copies, and three loop-2 bound forms. The instruction stream never
  changed.

### fn_80238B0C (fight_range_80238B0C, 99.30%)

- Built from repeated getter sequences (tokusei with trainer check, waza
  status with an ally branch). The ally branch truncates the index with
  `clrlwi 16` where retail uses 24. Needs the real inline helpers; not
  attempted beyond the diff.

### fight_range_8022B5C8 (all 100%, unlinked)

- The function is exact standalone (GC/1.3, unit flags). Its switch table
  is at .data 0x8039A314, which is 4 mod 8. MWCC gives every .data and
  .sdata2 section 8-byte alignment, so a carve cannot start there: the
  object lands at 0x8039A318 and the DOL grows by 32 bytes. It has to link
  with fn_8022B2CC, whose table 0x8039A2F4-0x8039A314 precedes it.

### fight_timer candidates 80265A6C / 80265C84 (all 100%, unlinked)

- Both emit a local int-to-float double (@16) that retail keeps at
  0x8047E6F0 (colosseum_battle_sdata2_8047E6E8.c). Merging
  0x80265A6C-0x80265D54 into one carve that owns 0x8047E6F0 makes a
  link-order cycle with fight_timer_exact_80265DB0 (.sdata2 0x8047E6E0) and
  the 0x8047E6E8 data unit.
- The whole-TU route fails on pool order. Retail's pool
  (0.0f, 60.0f, unsigned double, -3600.0f, signed double) is first-use order
  with fightTimerThreadFunc code-generated first. That is what -inline
  deferred (or reversed source) gives, but those also reverse .text. No
  flag combination gave both orders.
