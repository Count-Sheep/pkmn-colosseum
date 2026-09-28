/**
 * @file fight_exact_8020E124.c
 * @brief fightTypeGetFightSideFightOutPokemonMax, 0x8020E124 - 0x8020E1A4.
 *
 * The fight-type table's trainer count times its fight-out Pokemon count
 * (0 for an index past the table). Retail expands the table lookup twice,
 * with its "index > count" bound. Carved out of the fight.c range at its
 * function boundary with fight.c's body; built with the fight unit's flags
 * (GC/1.3 -O4,s -use_lmw_stmw on -sdata 8 -sdata2 8), no pragmas.
 * Text-only: the table and its count are .sbss pointers of an
 * auto-generated data unit and stay extern.
 */
#include "dolphin/types.h"

typedef struct FightTypeData {
    u8 trainerNum;
    u8 entryPokemonNum;
    u8 fightoutPokemonNum;
    u8 pad_03;
    u32 name;
} FightTypeData;

extern u32* lbl_80478F00;
extern FightTypeData* lbl_80478F04;

/* The bounds-check index is a separate local, not the u16 parameter: retail
 * keeps the widened index in r7 (the last-coloured slot), which only happens
 * when it is a declared local rather than a compiler temp.  It is spelled
 * `unsigned int`, not `u32` -- both are 32-bit unsigned here, but MWCC ranks
 * int- and long-typed locals separately when it colours them, and only the
 * int spelling puts trainerNum/count/idx in r5/r6/r7 the way retail has it.
 * Same lever as fightTrainerAiWazaValueHimitunotikara's param2. */
u16 fightTypeGetFightSideFightOutPokemonMax(u16 index) {
    FightTypeData* type;
    u8 trainerNum;
    u8 fightoutPokemonNum;
    u32 count;
    unsigned int idx;

    idx = index;
    count = *lbl_80478F00;
    if (idx > count) {
        type = NULL;
    } else {
        type = &lbl_80478F04[index];
    }
    if (type == NULL) {
        trainerNum = 0;
    } else {
        trainerNum = type->trainerNum;
    }
    if (idx > count) {
        type = NULL;
    } else {
        type = &lbl_80478F04[index];
    }
    if (type == NULL) {
        fightoutPokemonNum = 0;
    } else {
        fightoutPokemonNum = type->fightoutPokemonNum;
    }
    return trainerNum * (u8)fightoutPokemonNum;
}
