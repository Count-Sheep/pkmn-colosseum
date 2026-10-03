/**
 * @file fight_pokemon_r58_80200B10_o1.c
 * @brief fightOutPokemonInitJoutaiKeep carve, 0x80200B10 - 0x80200E00.
 *
 * Clears the nine "keep" joutai (status-condition) ids listed in
 * lbl_80279C90, cancelling the 0xEE effect's trajectories like
 * fightOutPokemonInitJoutaiKie does for its three ids.
 */
#include "dolphin/types.h"

extern void* pokemonGetStatus(void* context, u32 slot, u16 tableId, u32 flags);

typedef struct {
    u16 ids[9];
} JoutaiIdTable9;

/* Asks whether the joutai `id` is set. Moves 0x7C/0xC8/0xCD keep it in the
 * 0xD6 slot's data (0x7C/0xC8 through its 0xCC record), 0xD8 on the
 * Pokemon itself; any other kind is never set. */
static inline u8 fightPokemonIsJoutai(void* pokemon, u16 id) {
    extern u16 fn_80119ED0(u16 id);
    extern u8 fn_8011B67C(void* obj, u16 id);
    extern u8 fn_80121ADC(void* obj, u16 id);
    void* slot;
    void* data;

    if (fn_80119ED0(id) == 0x7C || fn_80119ED0(id) == 0xC8 || fn_80119ED0(id) == 0xCD) {
        slot = pokemonGetStatus(pokemon, 0, 0xD6, 0);
        if (fn_80119ED0(id) == 0x7C || fn_80119ED0(id) == 0xC8) {
            if (slot == NULL) {
                data = NULL;
            } else {
                data = pokemonGetStatus(slot, 0, 0xCC, 0);
            }
            return fn_80121ADC(data, id);
        } else if (fn_80119ED0(id) != 0xCD) {
            return 0;
        } else {
            return fn_8011B67C(slot, id);
        }
    } else if (fn_80119ED0(id) != 0xD8) {
        return 0;
    } else {
        return fn_8011B67C(pokemon, id);
    }
}

/* Clears the joutai `id`, from the same place fightPokemonIsJoutai reads. */
static inline void fightPokemonClearJoutai(void* pokemon, u16 id) {
    extern u16 fn_80119ED0(u16 id);
    extern void fn_8011B788(void* obj, u16 id);
    extern void fn_80121B4C(void* obj, u16 id);
    void* slot;
    void* data;

    if (fn_80119ED0(id) == 0x7C || fn_80119ED0(id) == 0xC8 || fn_80119ED0(id) == 0xCD) {
        slot = pokemonGetStatus(pokemon, 0, 0xD6, 0);
        if (fn_80119ED0(id) == 0x7C || fn_80119ED0(id) == 0xC8) {
            if (slot == NULL) {
                data = NULL;
            } else {
                data = pokemonGetStatus(slot, 0, 0xCC, 0);
            }
            fn_80121B4C(data, id);
        } else if (fn_80119ED0(id) == 0xCD) {
            fn_8011B788(slot, id);
        }
    } else if (fn_80119ED0(id) == 0xD8) {
        fn_8011B788(pokemon, id);
    }
}

void fightOutPokemonInitJoutaiKeep(void* pokemon) {
    extern JoutaiIdTable9 lbl_80279C90;
    extern void fn_801DA36C(void* effect, s32 trajType);
    JoutaiIdTable9 table = lbl_80279C90;
    void* effect;
    u8 i;
    u16 id;
    u16* ids;

    if (pokemon == NULL) {
        return;
    }
    ids = table.ids;
    for (i = 0; i < 9; i++) {
        id = ids[i];
        if (fightPokemonIsJoutai(pokemon, id) == 1) {
            effect = pokemonGetStatus(pokemon, 0, 0xEE, 0);
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
            fightPokemonClearJoutai(pokemon, id);
        }
    }
}
