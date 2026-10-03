/**
 * @file fight_pokemon_r58_80200B10_o1.c
 * @brief fightOutPokemonInitJoutaiKeep carve, 0x80200B10 - 0x80200E00.
 *
 * Ends the nine "keep" joutai (status-condition) ids listed in
 * lbl_80279C90, cancelling the 0xEE effect's trajectories like
 * fightOutPokemonInitJoutaiKie does for its three ids.
 */
#include "dolphin/types.h"

extern void* pokemonGetStatus(void* context, u32 slot, u16 tableId, u16 flags);

typedef struct {
    u16 ids[9];
} JoutaiIdTable9;

/* Copies of fight_out_pokemon.c's accessor inlines. */
static inline void* fightPokemonGetPokemonPtrInline(void* fp)
{
    void* p;

    if (fp == NULL) {
        p = NULL;
    } else {
        void* tmp = pokemonGetStatus(fp, 0, 0xCC, 0);
        p = tmp;
    }
    return p;
}

static inline u8 fightPokemonCheckJoutaiInline(void* fp, u16 id)
{
    extern u16 fn_80119ED0(u16 id);
    extern u8 fn_80121ADC(void* obj, u16 id);
    extern u8 fn_8011B67C(void* obj, u16 id);

    if (fn_80119ED0(id) == 0x7C || fn_80119ED0(id) == 0xC8) {
        return fn_80121ADC(fightPokemonGetPokemonPtrInline(fp), id);
    }
    if (fn_80119ED0(id) != 0xCD) {
        return 0;
    }
    return fn_8011B67C(fp, id);
}

static inline u8 fightOutPokemonCheckJoutaiInline(void* fo, u16 id)
{
    extern u16 fn_80119ED0(u16 id);
    extern u8 fn_8011B67C(void* obj, u16 id);

    if (fn_80119ED0(id) == 0x7C || fn_80119ED0(id) == 0xC8 || fn_80119ED0(id) == 0xCD) {
        return fightPokemonCheckJoutaiInline(pokemonGetStatus(fo, 0, 0xD6, 0), id);
    }
    if (fn_80119ED0(id) != 0xD8) {
        return 0;
    }
    return fn_8011B67C(fo, id);
}

static inline void fightPokemonClearJoutaiInline(void* fp, u16 id)
{
    extern u16 fn_80119ED0(u16 id);
    extern void fn_80121B4C(void* obj, u16 id);
    extern void fn_8011B788(void* obj, u16 id);

    if (fn_80119ED0(id) == 0x7C || fn_80119ED0(id) == 0xC8) {
        fn_80121B4C(fightPokemonGetPokemonPtrInline(fp), id);
    } else if (fn_80119ED0(id) == 0xCD) {
        fn_8011B788(fp, id);
    }
}

static inline void fightOutPokemonClearJoutaiInline(void* fo, u16 id)
{
    extern u16 fn_80119ED0(u16 id);
    extern void fn_8011B788(void* obj, u16 id);

    if (fn_80119ED0(id) == 0x7C || fn_80119ED0(id) == 0xC8 || fn_80119ED0(id) == 0xCD) {
        fightPokemonClearJoutaiInline(pokemonGetStatus(fo, 0, 0xD6, 0), id);
    } else if (fn_80119ED0(id) == 0xD8) {
        fn_8011B788(fo, id);
    }
}

static inline void fightOutPokemonCureJoutaiInline(void* fo, u16 id)
{
    extern void fn_801DA36C(void* effect, s32 trajType);
    void* effect = pokemonGetStatus(fo, 0, 0xEE, 0);

    if (id == 0) {
        if (effect != NULL) {
            fn_801DA36C(effect, 1);
            fn_801DA36C(effect, 2);
        }
    } else if (effect != NULL) {
        if (id == 8) {
            fn_801DA36C(effect, 1);
        }
        if (id == 7) {
            fn_801DA36C(effect, 2);
        }
    }
    fightOutPokemonClearJoutaiInline(fo, id);
}

void fightOutPokemonInitJoutaiKeep(void* pokemon) {
    extern JoutaiIdTable9 lbl_80279C90;
    JoutaiIdTable9 table = lbl_80279C90;
    u8 i;
    u16* ids;
    u16 id;

    if (pokemon == NULL) {
        return;
    }
    ids = table.ids;
    for (i = 0; i < 9; i++) {
        id = ids[i];
        if (fightOutPokemonCheckJoutaiInline(pokemon, id) == 1) {
            fightOutPokemonCureJoutaiInline(pokemon, id);
        }
    }
}
