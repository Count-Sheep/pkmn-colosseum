# Row 7 residual walls after D11's pass (2026-09-30)

## fightFloorGetStatus (fight_floor_candidate_801F54A4, 99.73%, -O4,s)

- Retail masks the case-0x5D index once, after the three count reads and
  the counter initialisation (`clrlwi r31,r23,16` just before the loop), and
  keeps `arg` in r23 for the whole dispatcher. -O4,s runs no loop motion, so
  that mask is a source-level temp. Writing it as a u16 local assigned right
  before the outer loop of fightFloorSearchFightOutPokemon
  (`target = index;` ... `(u16)n == target`) reproduces the instruction
  stream exactly: arg in r23, the clrlwi in place, same length.
- That form moves pkm to r31 (retail r30) and permutes target/c16/c18/n
  (retail r31/r27/r28/r29), so the raw score drops to 99.29 and it was not
  applied. 300 random orders of the helper's locals, mask forms for the
  counts, `target = index & 0xFFFF`, a local copy of pkm and the reversed
  compare changed nothing. In the regalloc replay pkm is coloured first (the
  highest-degree parameter); retail colours the index temp before it.
- Linking also needs .data jumptable_803754AC (data_803754AC.c,
  0x803754AC - 0x80375624) moved into the unit, as done for
  fightTrainerGetStatus.

## _flagSet / fn_801903B0 / fn_80190528 (98.72%)

- All 16 operand orders of the two masked-word stores (or and andc operands,
  table term first or last) score at most the current 98.72; swapping the
  `|` operands (retail's `or r0,shift,masked` order) makes the volatile
  registers worse (97.9 - 98.2). Pointer-arithmetic table access is neutral;
  a `(u32*)(u32)` cast of the table is 91.5.

## cbPoison (hero_move_candidate_8012AD50, 99.48%)

- A standalone carve form (static inline copies of heroMoveInitEvent and
  heroMoveTermEvent, pool stand-ins as in hero_move_exact_8012C660.c) has the
  retail structure. With its best local order and `livingPoisoned = 0`
  first it scores 99.57 (cmp.py). Left: `changed` and the inlined InitEvent
  member counter take r28 (retail r30), and the two inlined resource IDs
  swap r28/r29 and r29/r30. The {100, 101} table as a struct stand-in also
  gives a `lbl_8047D030+4` relocation where retail names lbl_8047D034.
