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
extern u32 wazaGetStatus(u32 context, u16 wazaId, u16 field, u32 flags);

u8 fightOutPokemonCheckCanOutOkWazaBanme(void* fighter, u16 slot, u8 mode, u16* output);

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

/* The Pokemon record held in `pokemon`'s 0xD6 slot, or NULL. */
static inline void* fightPokemonGetSlotRecord(void* pokemon) {
    void* slot = pokemonGetStatus(pokemon, 0, 0xD6, 0);

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

/* Asks whether the joutai `id` is set in the 0xD6 slot data `slot` (moves
 * 0x7C/0xC8 keep it in the slot's 0xCC record, 0xCD on the slot itself). */
static inline u8 fightPokemonSlotIsJoutai(void* slot, u16 id) {
    extern u16 fn_80119ED0(u16 id);
    extern u8 fn_8011B67C(void* obj, u16 id);
    extern u8 fn_80121ADC(void* obj, u16 id);
    void* data;

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
}

/* Upper half of the random status the joutai `id` saved. */
static inline u32 fightPokemonGetJoutaiRnd2(void* pokemon, u16 id) {
    extern u16 fn_80119ED0(u16 id);
    extern u32 fn_8012165C(void* obj, u16 id);
    extern u32 fn_8011A6D4(void* obj, u16 id);
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
            return fn_8012165C(data, id);
        } else if (fn_80119ED0(id) != 0xCD) {
            return 0;
        } else {
            return fn_8011A6D4(slot, id);
        }
    } else if (fn_80119ED0(id) != 0xD8) {
        return 0;
    } else {
        return fn_8011A6D4(pokemon, id);
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

typedef struct {
    u16 ids[9];
} JoutaiIdTable9;

/* fightOutPokemonInitJoutaiKeep (0x80200B10), which retail inlines here:
 * clears the nine "keep" joutai and cancels their 0xEE effects. */
static inline void fightPokemonInitJoutaiKeep(void* pokemon) {
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

/* The Pokemon record of the 0xD6 slot data `slot`. */
static inline void* fightPokemonSlotGetRecord(void* slot) {
    return pokemonGetStatus(slot, 0, 0xCC, 0);
}

/* The item `fighter` holds, 0 when it has none or joutai 0x3D (embargo)
 * blocks it. */
static inline u16 fightPokemonGetSoubiItem(void* fighter) {
    extern u16 pokemonGetSoubiItemSoubiDataId(void* pokemon);
    void* slot;
    void* record;

    slot = pokemonGetStatus(fighter, 0, 0xD6, 0);
    if (slot == NULL) {
        record = NULL;
    } else {
        record = fightPokemonSlotGetRecord(slot);
    }
    if (record == NULL) {
        return 0;
    }
    if (fightPokemonSlotIsJoutai(slot, 0x3D) == 1) {
        return 0;
    }
    return pokemonGetSoubiItemSoubiDataId(record);
}

/* 0x801FFEC8: why `fighter` may not use the move in `slot` now (0 = it
 * may): 1 encore, 2 the move it just used under joutai 0x1B (clears the
 * keep joutai when `mode` is 1), 3 taunt, 4 imprison, 5 choice item
 * lock (the locked move goes to `output`), 6 invalid or no PP. */
u8 fightOutPokemonCheckCanOutOkWazaBanme(void* fighter, u16 slot, u8 mode, u16* output) {
    extern u8 fightFloorCheckHuuinWazaFightOutPokemon(u32 floor, void* fighter, u16 waza);
    u8 result;
    void* pokemon;
    u16 waza;
    u16 power;
    u8 pp;
    u16 item;
    u16 lastWaza;
    u16 lockWaza;

    result = 0;
    if (fighter == NULL) {
        return 6;
    }
    if (fighter == NULL) {
        pokemon = NULL;
    } else {
        pokemon = fightPokemonGetSlotRecord(fighter);
    }
    waza = (u16)(u32)pokemonGetStatus(pokemon, 0, 0x7F, slot);
    power = wazaGetStatus(0, waza, 7, 0);
    pp = (u8)(u32)pokemonGetStatus(pokemon, 0, 0x80, slot);

    item = fightPokemonGetSoubiItem(fighter);

    lastWaza = (u16)(u32)pokemonGetStatus(fighter, 0, 0xF0, 0);
    if (fightPokemonIsJoutai(fighter, 0x29) == 1) {
        lockWaza = fightPokemonGetJoutaiRnd2(fighter, 0x29);
        if (lockWaza != 0 && lockWaza == waza && lockWaza != 0x165) {
            result = 1;
        }
    }
    if (fightPokemonIsJoutai(fighter, 0x1B) == 1 && waza == lastWaza && waza != 0xA5) {
        if (mode == 1) {
            fightPokemonInitJoutaiKeep(fighter);
        }
        result = 2;
    }
    if (fightPokemonIsJoutai(fighter, 0x30) == 1 && power == 0) {
        result = 3;
    }
    if (fightFloorCheckHuuinWazaFightOutPokemon(0, fighter, waza) == 1) {
        result = 4;
    }
    if (item == 0x1D) {
        if (fightPokemonIsJoutai(fighter, 0x36) == 1) {
            lockWaza = fightPokemonGetJoutaiRnd2(fighter, 0x36);
        } else {
            lockWaza = 0;
        }
        if (lockWaza != 0 && lockWaza != 0x165 && lockWaza != 0xFFFF && lockWaza != waza) {
            result = 5;
        }
        if (output != NULL) {
            *output = lockWaza;
        }
    }
    if (pokemonWazaCheckValid(pokemon, slot) == 0 || pp == 0) {
        result = 6;
    }
    return result;
}
