/**
 * @file fight_pokemon_r58_801FF1BC_o1.c
 * @brief fightOutPokemon move-selection checks, 0x801FF1BC - 0x80200A5C:
 *        fightOutPokemonCheckFightActionWazaSelect,
 *        fightOutPokemonGetOutOkWazaBanmeAry and
 *        fightOutPokemonCheckCanOutOkWazaBanme.
 */
#include "dolphin/types.h"

extern void* pokemonGetStatus(void* context, u32 slot, u16 tableId, u16 flags);
extern u8 pokemonWazaCheckValid(void* pokemon, u16 slot);

u8 fightOutPokemonCheckCanOutOkWazaBanme(void* fighter, u16 slot, u32 mode, u16* output);

/* The Pokemon record held in `pokemon`'s 0xD6 slot, or NULL. */
static inline void* fightPokemonGetSlotPokemon(void* pokemon) {
    void* slot;

    if (pokemon == NULL) {
        return NULL;
    }
    slot = pokemonGetStatus(pokemon, 0, 0xD6, 0);
    if (slot == NULL) {
        return NULL;
    }
    return pokemonGetStatus(slot, 0, 0xCC, 0);
}

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

/* Status word of the joutai `id` kept in `pokemon`'s 0xD6 slot data. */
static inline u32 fightPokemonGetSlotJoutaiStatus(void* pokemon, u16 id) {
    extern u16 fn_80119ED0(u16 id);
    extern u32 fn_80121574(void* obj, u16 id);
    extern u32 fn_8011A3E4(void* obj, u16 id);
    void* slot;
    void* data;

    slot = pokemonGetStatus(pokemon, 0, 0xD6, 0);
    if (fn_80119ED0(id) == 0x7C || fn_80119ED0(id) == 0xC8) {
        if (slot == NULL) {
            data = NULL;
        } else {
            data = pokemonGetStatus(slot, 0, 0xCC, 0);
        }
        return fn_80121574(data, id);
    } else if (fn_80119ED0(id) != 0xCD) {
        return 0;
    } else {
        return fn_8011A3E4(slot, id);
    }
}

/* Status word the joutai `id` saved, read from where fightPokemonIsJoutai looks. */
static inline u32 fightPokemonGetJoutaiStatus(void* pokemon, u16 id) {
    extern u16 fn_80119ED0(u16 id);
    extern u32 fn_8011A3E4(void* obj, u16 id);

    if (fn_80119ED0(id) == 0x7C || fn_80119ED0(id) == 0xC8 || fn_80119ED0(id) == 0xCD) {
        return fightPokemonGetSlotJoutaiStatus(pokemon, id);
    } else if (fn_80119ED0(id) != 0xD8) {
        return 0;
    } else {
        return fn_8011A3E4(pokemon, id);
    }
}

/* Number of `pokemon`'s moves it may use now. */
static inline u8 fightPokemonCountOutOkWaza(void* pokemon) {
    u8 ok[4];
    u8 count;
    u8 i;
    u16 waza;

    if (pokemon == NULL) {
        return 0;
    }
    for (i = 0; i < 4; i++) {
        ok[i] = 0;
    }
    count = 0;
    for (i = 0; i < 4; i++) {
        if (pokemonWazaCheckValid(fightPokemonGetSlotPokemon(pokemon), i) != 0) {
            waza = (u16)(u32)pokemonGetStatus(fightPokemonGetSlotPokemon(pokemon), 0, 0x7F, i);
            if (waza != 0 && waza != 0x165 && waza != 0x163 &&
                fightOutPokemonCheckCanOutOkWazaBanme(pokemon, i, 0, NULL) == 0) {
                count++;
            }
        }
    }
    return count;
}

/* 0x801FFB30: 1 when `pokemon` has no move it may use (or its encore
 * (joutai 0x2A) move cannot be used), so it must struggle. */
u8 fightOutPokemonGetOutOkWazaBanmeAry(void* pokemon) {
    if (fightPokemonCountOutOkWaza(pokemon) == 0) {
        return 1;
    }
    if (fightPokemonIsJoutai(pokemon, 0x2A) == 1) {
        if (fightOutPokemonCheckCanOutOkWazaBanme(pokemon, fightPokemonGetJoutaiStatus(pokemon, 0x2A), 0, NULL) != 0) {
            return 1;
        }
    }
    return 0;
}
