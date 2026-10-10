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
    /* RULE-EXCEPTION(user-approved): both tables in one local struct (fight
     * table at sp+8, pokemon table at sp+0x18 behind an explicit pad) so
     * MWCC's alias analysis treats the pokemon copy's tail store and the
     * fight copy's stores as one object; that keeps the tail store ahead of
     * the fight copy's loads (retail's r7 destination and r0 tail word);
     * codegen only - see docs/RULE_EXCEPTIONS.md */
    struct {
        HensinFightFields fight;
        u16 pad;
        HensinPokemonFields pokemon;
    } fields;
    void* srcPokemon;
    void* dstPokemon;
    u8 k;
    u8 m;
    u16 id;
    u32 n2;
    u16 id2;
    u8 i;
    u8 j;
    u32 value;
    u32 n;

    fields.pokemon = lbl_80279CB8;
    fields.fight = lbl_80279CD4;
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
        /* RULE-EXCEPTION(user-approved): codegen-only cast - the pokemon table
         * is addressed as `(u8*)&fields + 16`, which hoists retail's
         * sp+0x18 loop base (a plain `fields.pokemon.ids[i]` adds the 16 per
         * iteration) - see docs/RULE_EXCEPTIONS.md */
        id = ((HensinPokemonFields*)((u8*)&fields + 16))->ids[i];
        value = (u32)pokemonGetStatus(srcPokemon, 0, id, 0);
        pokemonSetStatus(dstPokemon, 0, id, 0, value);
    }
    for (j = 0; j < 4; j++) {
        n = j;
        /* RULE-EXCEPTION(user-approved): scalar reuse - the move id is held in `value` (the field-copy loop's value), which makes it a multi-def web stored straight from r3 as in retail; codegen only - see docs/RULE_EXCEPTIONS.md */
        value = (u32)pokemonGetStatus(srcPokemon, 0, 0x7F, n);
        pokemonSetStatus(dstPokemon, 0, 0x7F, n, value);
        if (wazaGetStatus(0, value, 2, 0) < 5) {
            pokemonSetStatus(dstPokemon, 0, 0x80, n, (u32)pokemonGetStatus(srcPokemon, 0, 0x80, n));
        } else {
            pokemonSetStatus(dstPokemon, 0, 0x80, n, 5);
        }
    }
    for (k = 0; k < 7; k++) {
        id2 = fields.fight.ids[k];
        pokemonSetStatus(dst, 0, id2, 0, (u32)pokemonGetStatus(src, 0, id2, 0));
    }
    pokemonSetStatus(dst, 0, 0x100, 0, (u16)(u32)pokemonGetStatus(src, 0, 0x100, 0));
    for (m = 0; m < 2; m++) {
        n2 = m;
        pokemonSetStatus(dst, 0, 0xFF, n2, (u16)(u32)pokemonGetStatus(src, 0, 0xFF, n2));
    }
}
