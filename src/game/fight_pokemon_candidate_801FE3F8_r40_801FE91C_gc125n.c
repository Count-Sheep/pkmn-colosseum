/**
 * @file fight_pokemon_candidate_801FE3F8_r40_801FE91C_gc125n.c
 * @brief fightOutPokemonCopyHensinStatus carve, 0x801FE91C - 0x801FEC10.
 *
 * Transform (hensin) copy: the target's Pokemon record takes the source's
 * fourteen battle fields (lbl_80279CB8), its four moves with their PP
 * capped at 5, and the fight Pokemon takes the seven fields listed in
 * lbl_80279CD4 plus fields 0x100 and 0xFF.
 *
 * Built with the unit flag `-opt noloopinvariants` (see configure.py and
 * docs/RULE_EXCEPTIONS.md).
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

/* RULE-EXCEPTION(user-approved): both field tables are members of one local
 * object (fight table at offset 0, a u16 pad, pokemon table at offset 16)
 * instead of two locals; with one object MWCC orders the pokemon copy's tail
 * store before the fight copy's loads, as retail does (separate locals give a
 * late tail store); codegen only - see docs/RULE_EXCEPTIONS.md */
typedef struct {
    HensinFightFields fight;
    u16 pad;
    HensinPokemonFields pokemon;
} HensinFields;

/* The Pokemon record held in `pokemon`'s 0xD6 slot, or NULL. */
static inline void* fightPokemonGetSlotPokemon(void* pokemon) {
    void* slot = pokemonGetStatus(pokemon, 0, 0xD6, 0);

    if (slot == NULL) {
        return NULL;
    }
    return pokemonGetStatus(slot, 0, 0xCC, 0);
}

static inline void* fightPokemonGetPokemon(void* fightPokemon) {
    if (fightPokemon == NULL) {
        return NULL;
    }
    return fightPokemonGetSlotPokemon(fightPokemon);
}

/* RULE-EXCEPTION(user-approved): single-use inline accessor with a
 * codegen-only cast - the pokemon table is addressed as `(u8*)fields + 16`
 * (== &fields->pokemon, offset 16 on any compiler: a 14-byte fight table and a
 * u16 pad), which keeps retail's hoisted sp+0x18 loop base;
 * `fields->pokemon.ids[index]` folds the 16 into the index and adds it inside
 * the loop - see docs/RULE_EXCEPTIONS.md */
static inline u16 hensinPokemonFieldId(HensinFields* fields, u8 index) {
    return ((HensinPokemonFields*)((u8*)fields + 16))->ids[index];
}

/* RULE-EXCEPTION(user-approved): single-use inline accessor, the fight-table
 * twin of hensinPokemonFieldId; read in place (`id = fields.fight.ids[i]`)
 * the fight loop's counter and id take r26/r25 instead of retail's r27/r26 -
 * see docs/RULE_EXCEPTIONS.md */
static inline u16 hensinFightFieldId(HensinFields* fields, u8 index) {
    return fields->fight.ids[index];
}

void fightOutPokemonCopyHensinStatus(void* src, void* dst) {
    extern const HensinPokemonFields lbl_80279CB8;
    extern const HensinFightFields lbl_80279CD4;
    HensinFields fields;
    u8 i;
    u32 value;
    u16 id;
    void* srcPokemon;
    void* dstPokemon;

    fields.pokemon = lbl_80279CB8;
    fields.fight = lbl_80279CD4;
    if (src == NULL) {
        return;
    }
    if (dst == NULL) {
        return;
    }
    srcPokemon = fightPokemonGetPokemon(src);
    dstPokemon = fightPokemonGetPokemon(dst);
    for (i = 0; i < 14; i++) {
        id = hensinPokemonFieldId(&fields, i);
        value = (u32)pokemonGetStatus(srcPokemon, 0, id, 0);
        pokemonSetStatus(dstPokemon, 0, id, 0, value);
    }
    for (i = 0; i < 4; i++) {
        /* RULE-EXCEPTION(user-approved): scalar reuse - the move id is held in
         * `value`, the field loop's value local (not live across the loops);
         * as a multi-def web it is stored straight from r3 (retail's
         * `mr r25,r3`), where its own local goes through r0; codegen only -
         * see docs/RULE_EXCEPTIONS.md */
        value = (u32)pokemonGetStatus(srcPokemon, 0, 0x7F, i);
        pokemonSetStatus(dstPokemon, 0, 0x7F, i, value);
        if (wazaGetStatus(0, value, 2, 0) < 5) {
            pokemonSetStatus(dstPokemon, 0, 0x80, i, (u32)pokemonGetStatus(srcPokemon, 0, 0x80, i));
        } else {
            pokemonSetStatus(dstPokemon, 0, 0x80, i, 5);
        }
    }
    for (i = 0; i < 7; i++) {
        id = hensinFightFieldId(&fields, i);
        pokemonSetStatus(dst, 0, id, 0, (u32)pokemonGetStatus(src, 0, id, 0));
    }
    pokemonSetStatus(dst, 0, 0x100, 0, (u16)(u32)pokemonGetStatus(src, 0, 0x100, 0));
    for (i = 0; i < 2; i++) {
        pokemonSetStatus(dst, 0, 0xFF, i, (u16)(u32)pokemonGetStatus(src, 0, 0xFF, i));
    }
}
