/**
 * @file fight_range_80238B0C.c
 * @brief fn_80238B0C, 0x80238B0C - 0x80238E30: the AI's type-effectiveness
 *        lookup for a move type against a pokemon (fn_8010C650), with the
 *        tokusei 0x1A / 0x19 overrides (0x43).
 *
 * Candidate (99.90%): written with the fight AI accessor inlines. Only the
 * first tokusei check's trainer/pokemon register differs (retail r26, here
 * r23); see docs/recon/fightwalls_d20.md.
 */
#include "dolphin/types.h"
extern u32 pokemonGetStatus(u32, u32, u32, u8);
extern u32 fightFloorGetStatus();
extern u32 fightTrainerGetStatus();
extern u32 fightPokemonGetPokemonPtr();
extern u32 pokemonGetTokuseiDataId();
extern u8 fightTrainerIsAllyFightTargetPtr();
extern u32 fn_8010C650();

static inline u16 aiGetTrainer(u32 ctx)
{
    return fightTrainerGetStatus(0, fightTrainerGetStatus(ctx, 0, 0x43, 0) & 0xFFFF, 2, 0);
}

static inline u32 aiGetTokusei(u32 ctx, u32 pokemon)
{
    u16 trainer;
    u32 p;

    aiGetTrainer(ctx);
    fightFloorGetStatus(0, 0, 0x14, 0);
    trainer = aiGetTrainer(ctx);
    p = fightPokemonGetPokemonPtr(pokemon);
    if ((u8)fightTrainerGetStatus(0, trainer, 0x2B, 0) == 1) {
        return pokemonGetTokuseiDataId(p);
    }
    return 0;
}

static inline u8 aiIsTokusei(u32 ctx, u32 pokemon, u16 tokusei)
{
    return (u16)aiGetTokusei(ctx, pokemon) == tokusei;
}

static inline u32 aiGetTypeList(u32 ctx, u32 pokemon, u16* types)
{
    u16 floor;
    u16 trainer;
    u16 species;
    u16 type;
    u32 count;
    u32 i;

    count = 0;
    for (i = count; (u8)i < 2; i++) {
        floor = fightFloorGetStatus(0, 0, 0x14, 0);
        trainer = fightTrainerGetStatus(0, fightTrainerGetStatus(ctx, 0, 0x43, 0) & 0xFFFF, 2, 0);
        species = pokemonGetStatus(fightPokemonGetPokemonPtr(pokemon), 0, 0x6E, 0);
        if ((u8)fightTrainerGetStatus(0, trainer, 0x2A, 0) == 1) {
            if (fightTrainerIsAllyFightTargetPtr(ctx, pokemon, floor) == 0) {
                type = pokemonGetStatus(0, species, 0x16, i);
            } else {
                type = pokemonGetStatus(0, species, 0x16, i);
            }
        } else {
            type = 9;
        }
        if (type != 9) {
            types[(u16)count] = type;
            count++;
        }
    }
    return count;
}

u32 fn_80238B0C(u32 ctx, u32 pokemon, u32 moveType, s16 power)
{
    u16 types[2];
    u32 count;
    u32 result;

    if ((u16)moveType == 9) {
        return 0x3F;
    }
    if (aiIsTokusei(ctx, pokemon, 0x1A) == 1 && (u16)moveType == 4) {
        return 0x43;
    }
    count = aiGetTypeList(ctx, pokemon, types);
    if ((u16)count == 0) {
        return 0x3F;
    }
    result = fn_8010C650(moveType, types, count);
    if (aiIsTokusei(ctx, pokemon, 0x19) == 1 && (u16)result != 0x41 && power > 0) {
        return 0x43;
    }
    return result;
}
