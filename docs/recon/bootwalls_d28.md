# Boot-path walls (lane D28, 2026-09-30)

Two row-7/36 walls run before the title screen with no input:

- pokemonSetLevelBasisStatus. Path: fn_80005AAC -> savedataCreate ->
  heroPokemonGetBlacky/Eifie -> pokemonResetBasisStatus.
- fn_801902E0 (GSflagTest). It runs on the title floor.

Both are now linked. _flagSet, fightFloorGetStatus and cbPoison do not
run before the title and were skipped.

## pokemonSetLevelBasisStatus: 93.49% -> 100%, clean

Linked as unit pokemon_range_8012795C (Matching). It needed three changes,
all in plain C, with no rule exception.

1. **The /100 schedule.** The nature adjust's `nature` parameter is
   `u32`, as in XD's pokemonAdjustValueBySeikaku(u32, u16, u32)
   (github.com/TeamOrre/xd-decomp @ 4989794e, pokemon.cpp). The widened
   argument copy moves `mr r3,nature` after the magic-constant `addi`, so
   `lis` takes r3 (93.49 -> 98.62).
2. **obj/oldMaxHp (r30/r31).**
   - The cause: `(u8)level` in the SetLevel call evaluates into an
     argument temp (`rlwinm r111; mr r7,r111`). That temp is coalesced
     into r7, but it stays in obj's and oldMaxHp's neighbour lists as a
     stale entry.
   - Both nodes then stall together at degree 29 (11 physical registers,
     17 stale entries and each other), and cost/degree spills oldMaxHp
     first.
   - `level & 0xFF` writes r7 directly. Both nodes then sit at 28 and
     become colourable in the same sweep. obj (lower vreg) is pushed
     first, which gives retail's r30/r31 (98.62 -> 99.50).
   - The general lesson: stale coalesced neighbours count in MWCC's
     degree. When two long-lived nodes tie, removing a short argument temp
     that interferes with both can break the tie. Copies of obj could not
     do it, because the frontend copy-propagates them all away.
3. **value/rate (r24/r25) in stats 1-5.**
   - XD's separate `result` local in the nature adjust fixes it:
     `result = value * kake; if (waru) result /= waru; return result;`.
   - `result` must be declared after `rate`; declaring it first or second
     leaves the swap. The simulator
     (d24 simclimb on d16 simp) had shown this pair was order-solvable.

## fn_801902E0 (GSflagTest): 96.92% -> 100%, RULE-EXCEPTION

Linked through a one-function carve, gs_flag_test_exact_801902E0.c
(0x801902E0-0x801903B0). The neighbouring functions stay candidates:
fn_8018FE30 is at 70% (size 1084 vs 1200) and the setters at 98.72%. Carving
means none of them has to be exact first.

- Retail keeps `value != 0` as a bne / li 0 / b / li 1 diamond.
- The frontend (IRO, frontend-01) rewrites if/else returns and assignments
  into ECOND. Codegen then lowers ECOND(x==0, 0, 1) straight to
  neg/or/srwi; it is already in backend-00.
- These forms all convert, whatever the compiler version (1.3, 1.3.2, 2.0;
  1.2.5n is worse), return type or arm order:
  - if/else, ?:, `!!`, early return and goto;
  - inline helpers;
  - arms that are constant-valued variables.
- A switch keeps the diamond, but it uses cmpwi.
- `#pragma peephole off` keeps the diamond but loses the +4 displacement
  fold (best 93.7%).
- A two-statement else arm, `result = 0; result++;`, escapes the
  conversion and folds to `li r3,1`, which gives 100%. It is tagged and
  listed in docs/RULE_EXCEPTIONS.md.
- A clean fix needs the original statement shape that IRO leaves alone.
