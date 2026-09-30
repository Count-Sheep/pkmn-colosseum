# Fight register walls, lane D20 (2026-09-30)

Scope: the fight walls D17 documented in `docs/recon/nearlink_d17.md`.
Tools: D16's simplify simulator (`simp.py`, `trace.py`, `wi.py`) on
mwcc-debugger (GC/2.6) dumps. Every linked unit passed configure, report,
full ninja (main.dol and common_rel.rel SHA-1), quality_scan and
check_regression.

## Linked

| Commit | Unit | What it took |
|---|---|---|
| f5b4f041, 17cdd62d | fight_range_candidate_80219838 (fn_80219838, fn_80219964, fn_80219B2C, fn_80219CF4) | fn_80219964/fn_80219B2C are one inline body, `fightSeqAimStoredTarget(targetStatus, powerStatus)`. Inline locals are numbered in reverse declaration order (last declared = highest vreg = coloured first), which gives retail's floor/power/target id/move/attacker order, and the move result coalesces without the extra `mr r0,r3; mr r28,r0` a plain local gets. The Follow Me redirect is the shared `fightGetFollowMeTarget` inline. fn_80219838 needed `wazaGetStatus(u32, u16, u32, u32)`. GC/1.3 -O4,s |
| aa5158a8 | fight_range_8022B2CC (fn_8022B2CC + fn_8022B5C8, merged) | fn_8022B2CC: `fightGetFollowMeTarget` inline with locals target, userId, side, redirected, floor. Both switch tables are 4 mod 8, so the carve owns .data from fn_802249B8's 8-aligned table at 0x8039A220 (written as a data initializer; RULE-EXCEPTION row) |
| 98a858fd | fight_trainer_ai_waza_value_candidate_8024DC7C (Kiaipanti) | Declaration only: `fightFloorGetFightTrainerFightOutPokemonPtrAry` returns u16; u16 count, plain `for (i = 0; i < count; i++)`. No interference tricks needed |
| b95cf8bb | fight_trainer_ai_waza_value_candidate_80245FC4 (Himitunotikara) | Retail inlines the terrain moves' own AI value functions (TuikaDoku, TuikouMeityuuDaun, TuikouBougyoDaun, Oororabiimu, TuikouSubayasaDaun, TuikouKonran, TuikaMahi); static inline copies (RULE-EXCEPTION row). Case 1 is an inline too. `pokemonGetStatus(...) == pokemonAry[i]` with a void* array fixes the cmplw order. Owns its switch table 0x8039A5D8-0x8039A648 |
| 3d9282f5 | fight_range_candidate_802192B4 (fn_802192B4..fn_802196A8) | fn_80219354: `random = rand(); p = typeArray; p += idx; random = *p;` creates the stack-array base before the modulo operands. fn_802196A8: `fn_80201764(u32, u32, s32)` (its definition's third argument is a full word) replaces opt_common_subs off |
| 6c8fe10a | fight_range_80238060 | The fight AI accessors as inlines (aiGetTrainer, aiGetFightPokemon, PP / max-PP getters taking a u32 slot masked with `& 0xFF`): in-place slot mask, and aiGetTrainer's inline result is numbered ahead of the PP value |
| d349290d | fight_range_8022BE2C (fn_8022BE2C + fn_8022D084) | g (max HP + case-7 loop index) and t4 (PP + 0x15 loop counter) split: u16 g assigned straight from the call keeps its own web; pp declared second, g where t4 was. Same `p +=` modulo fix. `isBitSet(bits, bit)` inline puts the call result on the left of the `and.` (the parser moves a bare call to the right operand). Owns its three switch tables; kinds/avail initializers are extern stand-ins (RULE-EXCEPTION row) |

## Lessons

- Before simulating interferences, check the declarations: Kiaipanti's
  "three more interferences" wall was a u32 prototype for a u16 getter.
- Inline expansion is the main renumbering lever. Inline locals get vregs
  above the caller's locals, last declared highest; an inline's return value
  is a temp that coalesces where a named local would keep an extra copy.
- `p = base; p += offset; *p` fixes the "array base created too late"
  modulo idiom (both fn_80219354 and fn_8022BE2C).
- MWCC's parser swaps `call & expr` so the call is the right operand;
  routing one side through an inline keeps source order.
- A value assigned from a cast temp (`s32 g = (u16)call()`) is copy
  propagated into the temp and loses its declaration-order vreg; assign
  directly to a variable of the narrow type.

## Walls

### fn_80238B0C (fight_range_80238B0C, 99.30% -> 99.90%, candidate)

- Structure: tokusei check (0x1A with move type 4 -> 0x43), a two-slot
  type list (the same loop as exact fn_802389D4, with the
  `pokemonGetStatus(u32, u32, u32, u8)` prototype that fixes D17's
  clrlwi 16/24 difference), fn_8010C650, then a second tokusei check (0x19).
- Committed as a standalone candidate at GC/1.3 -O4,s: aiGetTrainer,
  aiGetTokusei/aiIsTokusei and aiGetTypeList inlines. The type loop is an
  inline whose locals are declared floor, trainer, species, type, count, i
  (last declared = highest vreg gives retail's i r27, count r26, species
  r25, trainer r24, floor r23); the loop's trainer is the explicit
  fightTrainerGetStatus pair, not aiGetTrainer (the inline's return temp
  swaps trainer and floor).
- Left: the first tokusei check's trainer/pokemon pointer (one register,
  retail r26, ours r23). Debugger: the two aiIsTokusei expansions are
  nested inlines and get vregs below the loop inline's; retail needs the
  first expansion's value between count and species. A single-level
  aiIsTokusei moves the pointer to r27 (99.85). Tried local order, explicit
  trainer, and a result variable in the getter.
