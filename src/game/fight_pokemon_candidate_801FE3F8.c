/**
 * @file fight_pokemon_candidate_801FE3F8.c
 * @brief fightPokemon menu and transform (hensin) helpers,
 *        0x801FE3F8 - 0x801FE91C (6 fns).
 *
 * Standalone source at GC/1.3 -O4,s. fightOutPokemonSetHensinStatusAfterLevelUp
 * copies 18 status fields from the 0xD5 slot's Pokemon to the 0xD7 slot's;
 * its loop body is the fightPokemonCopyHensinField inline, whose locals (and
 * the two fightPokemonSlotGetPokemon expansions) give retail's register order.
 */
#include "dolphin/types.h"

extern void* pokemonGetStatus(void* context, u32 slot, u16 tableId, u32 flags);
extern u32 pokemonSetStatus(void* context, u32 slot, u16 tableId, u32 flags, u32 value);

/* 0x801FE3F8 | size: 0x70 */
void fightOutPokemonToMenuWazaStatus(void* context, u8* dest) {
    extern void pokemonToMenuWazaStatus(void* pokemon, u8* dest);
    void* partyList;
    void* pokemon;

    partyList = pokemonGetStatus(context, 0, 0xD6, 0);
    if (partyList == NULL) {
        pokemon = NULL;
    } else {
        pokemon = pokemonGetStatus(partyList, 0, 0xCC, 0);
    }
    if (pokemon != NULL) {
        pokemonToMenuWazaStatus(pokemon, dest);
    }
    *(u32*)(dest + 0x40) = (u32)context;
}

/* 0x801FE468 | size: 0xF4 */
void fightPokemonToMenuLvupStatus(void* context, u8* dest) {
    void* pokemon;

    if (context != NULL && dest != NULL) {
        if (context == NULL) {
            pokemon = NULL;
        } else {
            pokemon = pokemonGetStatus(context, 0, 0xCC, 0);
        }
        *(u16*)(dest + 0x2) = (u16)(u32)pokemonGetStatus(pokemon, 0, 0x87, 0);
        *(u16*)(dest + 0x4) = (u16)(u32)pokemonGetStatus(pokemon, 0, 0x88, 0);
        *(u16*)(dest + 0x6) = (u16)(u32)pokemonGetStatus(pokemon, 0, 0x89, 0);
        *(u16*)(dest + 0xA) = (u16)(u32)pokemonGetStatus(pokemon, 0, 0x8A, 0);
        *(u16*)(dest + 0xC) = (u16)(u32)pokemonGetStatus(pokemon, 0, 0x8B, 0);
        *(u16*)(dest + 0x8) = (u16)(u32)pokemonGetStatus(pokemon, 0, 0x8C, 0);
        *(u8*)(dest + 0x0) = 0;
    }
}

/* 0x801FE55C | size: 0x78 */
void fightOutPokemonAddFightOutPokemonEnemyDamage(void* self, void* other, u32 offset) {
    extern void* fightOutPokemonEnemySearchAry(void* data, u32 mode, void* key);
    extern u32 fightOutPokemonEnemyBiosGetDamage(void* ptr);
    extern void fightOutPokemonEnemyBiosSetDamage(void* ptr, u32 val);
    void* data;
    void* result;
    u32 val;

    if (self != NULL) {
        if (other != NULL) {
            if (self != other) {
                data = pokemonGetStatus(other, 0, 0x122, 0);
                if ((result = fightOutPokemonEnemySearchAry(data, 4, self)) != NULL) {
                    val = fightOutPokemonEnemyBiosGetDamage(result);
                    val += offset;
                    fightOutPokemonEnemyBiosSetDamage(result, val);
                }
            }
        }
    }
}

/* 0x801FE5D4 | size: 0x13C */
typedef struct { u16 fields[18]; } FieldTable18;

static inline void* fightPokemonSlotGetPokemon(void* slot)
{
    return pokemonGetStatus(slot, 0, 0xCC, 0);
}

/* RULE-EXCEPTION(user-approved): single-use inline helper whose only
 * evidence is register order -- see docs/RULE_EXCEPTIONS.md. */
static inline void fightPokemonCopyHensinField(void* context, u16 fieldId)
{
    void* destPokemon;
    void* srcPokemon;
    void* srcSlot;
    void* destSlot;

    srcSlot = pokemonGetStatus(context, 0, 0xD5, 0);
    destSlot = pokemonGetStatus(context, 0, 0xD7, 0);
    if (srcSlot != NULL && destSlot != NULL) {
        if (srcSlot == NULL) {
            srcPokemon = NULL;
        } else {
            srcPokemon = fightPokemonSlotGetPokemon(srcSlot);
        }
        if (destSlot == NULL) {
            destPokemon = NULL;
        } else {
            destPokemon = fightPokemonSlotGetPokemon(destSlot);
        }
        pokemonSetStatus(destPokemon, 0, fieldId, 0,
                         (u32)pokemonGetStatus(srcPokemon, 0, fieldId, 0));
    }
}

void fightOutPokemonSetHensinStatusAfterLevelUp(void* context) {
    extern FieldTable18 lbl_80279CE4;
    FieldTable18 table;
    u8 i;

    table = lbl_80279CE4;
    for (i = 0; i < 18; i++) {
        fightPokemonCopyHensinField(context, table.fields[i]);
    }
}

/* 0x801FE710 | size: 0xDC | ClearTrainerEventState */
void fightOutPokemonSetHensinFightPokemonStatusId(void* trainer, u16 eventId, u32 param) {
    void* src;
    void* dst;
    u32 val;

    if ((u8)param == 1) {
        src = pokemonGetStatus(trainer, 0, 0xD5, 0);
        dst = pokemonGetStatus(trainer, 0, 0xD7, 0);
    } else {
        src = pokemonGetStatus(trainer, 0, 0xD7, 0);
        dst = pokemonGetStatus(trainer, 0, 0xD5, 0);
    }
    if (src == NULL) {
        return;
    }
    if (dst == NULL) {
        return;
    }
    val = (u32)pokemonGetStatus(src, 0, eventId, 0);
    pokemonSetStatus(dst, 0, eventId, 0, val);
}

/* 0x801FE7EC | size: 0x130 */
void fightOutPokemonSetHensinPokemonStatusId(void* trainer, u16 eventId, u32 param1, u32 param2) {
    void* srcSlot;
    void* srcPokemon;
    void* dst;
    u32 value;

    if ((u8)param2 == 1) {
        srcSlot = pokemonGetStatus(trainer, 0, 0xD5, 0);
        srcPokemon = pokemonGetStatus(trainer, 0, 0xD7, 0);
        dst = srcPokemon;
    } else {
        srcSlot = pokemonGetStatus(trainer, 0, 0xD7, 0);
        dst = pokemonGetStatus(trainer, 0, 0xD5, 0);
    }
    if (srcSlot != NULL && dst != NULL) {
        if (srcSlot == NULL) {
            srcPokemon = NULL;
        } else {
            void* temp = pokemonGetStatus(srcSlot, 0, 0xCC, 0);
            srcPokemon = temp;
        }
        if (dst == NULL) {
            dst = NULL;
        } else {
            dst = fightPokemonSlotGetPokemon(dst);
        }
        value = (u32)pokemonGetStatus(srcPokemon, 0, eventId, param1);
        pokemonSetStatus(dst, 0, eventId, param1, value);
    }
}

