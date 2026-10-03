/**
 * @file fight_pokemon_candidate_801FE3F8_r40_801FE91C_gc125n.c
 * @brief fightOutPokemonCopyHensinStatus carve, 0x801FE91C - 0x801FEC10.
 *
 * Transform (hensin) copy: the target's Pokemon record takes the source's
 * fourteen battle fields (lbl_80279CB8), its four moves with their PP
 * capped at 5, and the fight Pokemon takes the seven fields listed in
 * lbl_80279CD4 plus fields 0x100 and 0xFF.
 */
#include "dolphin/types.h"

extern void* pokemonGetStatus(void* context, u32 slot, u16 tableId, u32 flags);
extern u32 pokemonSetStatus(void* context, u32 slot, u16 tableId, u32 flags, u32 value);
extern u8 wazaGetStatus(u32 context, u16 wazaId, u16 field, u32 flags);

typedef struct {
    u16 ids[14];
} HensinPokemonFields;

typedef struct {
    u16 ids[7];
} HensinFightFields;

/* The Pokemon record held in `pokemon`'s 0xD6 slot, or NULL. */
static inline void* fightPokemonGetSlotPokemon(void* pokemon) {
    void* slot = pokemonGetStatus(pokemon, 0, 0xD6, 0);

    if (slot == NULL) {
        return NULL;
    }
    return pokemonGetStatus(slot, 0, 0xCC, 0);
}

void fightOutPokemonCopyHensinStatus(void* src, void* dst) {
    extern const HensinPokemonFields lbl_80279CB8;
    extern const HensinFightFields lbl_80279CD4;
    HensinFightFields fightFields;
    HensinPokemonFields pokemonFields;
    void* srcPokemon;
    void* dstPokemon;
    u8 k;
    u8 j;
    u8 m;
    u16 id;
    u32 n2;
    u16 id2;
    u8 i;
    u16 waza;
    u32 value;
    u32 n;

    pokemonFields = lbl_80279CB8;
    fightFields = lbl_80279CD4;
    if (src == NULL) {
        return;
    }
    if (dst == NULL) {
        return;
    }
    if (src == NULL) {
        srcPokemon = NULL;
    } else {
        srcPokemon = fightPokemonGetSlotPokemon(src);
    }
    if (dst == NULL) {
        dstPokemon = NULL;
    } else {
        dstPokemon = fightPokemonGetSlotPokemon(dst);
    }
    for (i = 0; i < 14; i++) {
        id = pokemonFields.ids[i];
        value = (u32)pokemonGetStatus(srcPokemon, 0, id, 0);
        pokemonSetStatus(dstPokemon, 0, id, 0, value);
    }
    for (j = 0; j < 4; j++) {
        n = j;
        waza = (u16)(u32)pokemonGetStatus(srcPokemon, 0, 0x7F, n);
        pokemonSetStatus(dstPokemon, 0, 0x7F, n, (u32)waza);
        if (wazaGetStatus(0, (u32)waza, 2, 0) < 5) {
            pokemonSetStatus(dstPokemon, 0, 0x80, n, (u32)pokemonGetStatus(srcPokemon, 0, 0x80, n));
        } else {
            pokemonSetStatus(dstPokemon, 0, 0x80, n, 5);
        }
    }
    for (k = 0; k < 7; k++) {
        id2 = fightFields.ids[k];
        pokemonSetStatus(dst, 0, id2, 0, (u32)pokemonGetStatus(src, 0, id2, 0));
    }
    pokemonSetStatus(dst, 0, 0x100, 0, (u16)(u32)pokemonGetStatus(src, 0, 0x100, 0));
    for (m = 0; m < 2; m++) {
        n2 = m;
        pokemonSetStatus(dst, 0, 0xFF, n2, (u16)(u32)pokemonGetStatus(src, 0, 0xFF, n2));
    }
}
