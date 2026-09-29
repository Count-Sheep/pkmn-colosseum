# Row 7 (main-thread init): remaining register-colouring walls (lane D12, 2026-09-30)

Every function below matches retail instruction-for-instruction except for
one swapped register pair or a scheduling slot. What was tried is listed so
the next lane does not repeat it.

## fightOutPokemonSetMeetEnemyFightPokemonEnemySideAll — 99.74% (fight_out_pokemon.c)

The last function keeping `fight_out_pokemon` (34 functions, including the
row 7 quick wins fightOutPokemonGetMotoWazaDataId and GetUseWazaDataId) from
linking.

- Form: XD's structure (GXXE01 0x802045E8 calls fightOutPokemonCheckFightOut
  and fightOutPokemonSetMeetEnemyFightPokemon 0x802046B4). Colosseum expands
  both, with SetMeet expanding CheckMeetEnemyFightPokemon. The fightSide and
  fightTrainer getters are declared with a u16 index.
- Left: retail keeps the trainer count in r24 and the enemy pointer (also the
  0xD5 fightPokemon and the entry id) in r25. We get these swapped.
- The mwcc-debugger dump shows the outer values colored by descending cost
  (fo, p, t, trainer, side, pokemons, trainers). The enemy node (cost 6,
  degree 46) is colored after them, so in retail the enemy must be a
  low-cost, high-degree node.
- Tried, with no effect:
  - all 720 declaration orders;
  - u16/u32/s32 for the counts and loop counters;
  - reusing `enemy` for the 0xD5 pointer;
  - `enemy =` inside the call, or a cast argument;
  - fightFloor inline wrappers;
  - an unused `count` from status 0x14;
  - swapping the local order inside the inlines (99.68 to 99.74).
- Wrapping the inner loop or the check+set in one more inline exceeds MWCC's
  inline depth, and the helper is then called instead of expanded.

## pokemonSetWazaStatus — 99.59% (pokemon_range_801237B8, with pokemonSearchWazaDataId now exact)

- Left: the `(u16)waza` value in the pokemonSearchWazaDataId expansion is r29,
  and the search counter is r28. Retail has them swapped. The same pair
  repeats for the PP slot and max-PP id.
- Debugger: our `(u16)id` is a backend CSE temp (the highest vreg, colored
  first). In retail the counter is colored first.
- Tried:
  - `u32 target` with a `(u16)` compare;
  - `id & 0xFFFF` / `(u16)(id + 0)` arguments;
  - a local `u16 waza` (98.3);
  - a u16 `id` parameter, which fixes the search part but frees r31 and drops
    one saved register (97.1).

## fightFloorGetStatus — 99.75% (fight_floor_candidate_801F54A4)

- Case 0x5D is a static inline search (the triple loop of
  fightFloorLoopValidFightOutPokemon). That puts the case 0xA/0xB masks back
  in arg's register and gives retail's `i = n` copy.
- Left: retail masks `(u16)arg` once into r31 before the loop and keeps pkm
  in r30. We keep arg or the mask in r30/r31 swapped. The count, Pokemon-count
  and trainer-count registers are also permuted.
- The XD-shaped form (fightFloorLoopValidFightOutPokemonSub copies, u16
  counters) gets the counts right through declaration order, but the index
  still colors after pkm.
- -O4,s runs no loop passes on this function, so retail's single mask before
  the loop is a real temp, not a hoist.
- Linking also needs `.data` jumptable_803754AC (0x5E words) in the unit.

## GS flag setters (_flagSet, fn_801903B0, fn_80190528) — 98.72% each

- They expand XD's _flagSet(buffer, defs, flagId, value) as a static inline.
- Left: volatile-register choice in the multi-bit write block:
  - retail: bitPosition r6, mask table r4, word index r5;
  - ours: r5, r6, r4.
- Retail's table address is colored before the position and index. A
  CSE-created temp would do that. Ours reuses the first materialisation.
- Tried:
  - operand orders;
  - pointer, array and `buffer +=` lvalues;
  - a mask variable;
  - a table-pointer local (91.5);
  - all declaration orders.

## fn_801902E0 (GSflagTest) — 96.92%

- Left: retail keeps the final `value != 0` as an if/else with `li r3,0/1`.
- MWCC's peephole turns every if/else, `?:` and `!=` form into neg/or/srwi.
  `result = 0; if (value) result = 1;` keeps the branch, but in beq-skip form.
- `#pragma peephole off` keeps retail's branch but loses the rlwinm fusion of
  the type index.

## pokemonSetLevelBasisStatus — 93.49% (98.62% at -O3)

- Under -O3 only register pairs differ: obj and oldMaxHp, and value and rate
  in stats 1-5.
- At -O4 the magic-divide constant is scheduled into r4, not r3.
- XD's _pokemonGetLevelOneStatus structure gives the same code.
- Tried:
  - declaration orders;
  - parameter orders;
  - u16 pokemonGetStatus prototypes.
