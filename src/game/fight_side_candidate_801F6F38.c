#include "game/pokemon_fight_types.h"

extern u32 fightSideBiosGetFightTrainerPtr(u32, u32);
extern u32 fightTrainerGetValidFightPokemonPtr(u32, u32);
extern s16 fightTrainerGetHikaeFightPokemonNum(u32, u32, u32);
extern u8 fightTrainerCheckValid(u32);
extern u32 fightPokemonGetPokemonPtr(u32);
extern u8 fightPokemonCheckFightOut(u32);

/* 0x801F6F38 | size: 0x9C */
u16 fightSideGetHikaeFightPokemonNum(u32 side, u32 trainerLimit, u32 param_3, u32 param_4) {
    u32 trainerIndex;
    u16 total = 0;
    u32 trainer;

    for (trainerIndex = 0; (u16)trainerIndex < (u16)trainerLimit; trainerIndex++) {
        if (side == 0) {
            trainer = 0;
        } else {
            trainer = fightSideBiosGetFightTrainerPtr(side, trainerIndex);
        }
        if (fightTrainerCheckValid(trainer)) {
            total += fightTrainerGetHikaeFightPokemonNum(trainer, param_3, param_4);
        }
    }
    return total;
}

/* RULE-EXCEPTION(user-approved): inline helper and local declaration order
 * preserve the retail count loop's register allocation. */
static inline u16 countFightPokemon(u32 side, u32 trainerLimit, u32 pokemonLimit) {
    u32 trainer;
    int trainerIndex;
    int pokemonIndex;
    u32 total = 0;
    u32 fightPokemon;

    for (trainerIndex = 0; (u16)trainerIndex < (u16)trainerLimit; trainerIndex++) {
        if (side == 0) {
            trainer = 0;
        } else {
            trainer = fightSideBiosGetFightTrainerPtr(side, trainerIndex);
        }
        if (fightTrainerCheckValid(trainer)) {
            for (pokemonIndex = 0; (u16)pokemonIndex < (u16)pokemonLimit; pokemonIndex++) {
                fightPokemon = fightTrainerGetValidFightPokemonPtr(trainer, pokemonIndex);

                if (fightPokemon != 0 && fightPokemonCheckFightOut(fightPokemon)) {
                    total++;
                }
            }
        }
    }
    return total;
}

/* 0x801F6FD4 | size: 0xBC */
u16 fightSideGetFightPokemonNum(u32 side, u32 trainerLimit, u32 pokemonLimit) {
    return countFightPokemon(side, trainerLimit, pokemonLimit);
}

/* RULE-EXCEPTION(user-approved): the inline helper, accumulator pointer, and
 * declaration order reproduce the retail HP loops' register allocation. */
static inline void sumFightPokemonHp(s32* total, u32 side, u32 trainerLimit, u32 pokemonLimit, u16 field) {
    extern u32 pokemonGetStatus(u32, u32, u16, u32);
    int pokemonIndex;
    int trainerIndex;
    u32 pokemon;
    u32 fightPokemon;
    u32 trainer;

    for (trainerIndex = 0; (u16)trainerIndex < (u16)trainerLimit; trainerIndex++) {
        if (side == 0) {
            trainer = 0;
        } else {
            trainer = fightSideBiosGetFightTrainerPtr(side, trainerIndex);
        }
        if (fightTrainerCheckValid(trainer)) {
            for (pokemonIndex = 0; (u16)pokemonIndex < (u16)pokemonLimit; pokemonIndex++) {
                fightPokemon = fightTrainerGetValidFightPokemonPtr(trainer, pokemonIndex);

                if (fightPokemon != 0) {
                    pokemon = fightPokemonGetPokemonPtr(fightPokemon);

                    if (pokemon != 0 && fightPokemonCheckFightOut(fightPokemon)) {
                        u16 hp = pokemonGetStatus(pokemon, 0, field, 0);

                        *total += (u32)hp;
                    }
                }
            }
        }
    }
}

/* 0x801F7090 | size: 0xE4 */
s32 fightSideGetFightPokemonMaxHp(u32 side, u32 trainerLimit, u32 pokemonLimit) {
    s32 total = 0;
    sumFightPokemonHp(&total, side, trainerLimit, pokemonLimit, 0x87);
    return total;
}

/* 0x801F7174 | size: 0xE4 */
s32 fightSideGetFightPokemonNokoriHp(u32 side, u32 trainerLimit, u32 pokemonLimit) {
    s32 total = 0;
    sumFightPokemonHp(&total, side, trainerLimit, pokemonLimit, 0x83);
    return total;
}
