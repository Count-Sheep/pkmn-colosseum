/**
 * @file fight_pokemon_r58_801FEF74_middle.c
 * @brief fightOutPokemonGetJoutaiMigawariHp carve, 0x801FEF74 - 0x801FF1BC,
 *        linked.
 *
 * Standalone copy of the shared candidate body, built at -O4,s with
 * -schedule off (configure.py) in place of the shared file's
 * push/scheduling-off/pop pragmas.
 */
#include "dolphin/types.h"

extern void* pokemonGetStatus(void* context, u32 slot, u16 tableId, u32 flags);
extern u8 fn_80121574(void* obj, s32 arg);
extern u8 fn_8011A3E4(void* obj, s32 arg);

u8 fightOutPokemonGetJoutaiMigawariHp(void* trainer) {
    extern u16 fn_80119ED0(s32 id);
    extern u8 fn_8011B67C(void* obj, s32 arg);
    extern u8 fn_80121ADC(void* obj, u32 field);
    void* obj;
    void* target;
    void* obj2;
    void* target2;
    u8 status;

    if (trainer == NULL) {
        return 0;
    }
    if (fn_80119ED0(0x14) == 0x7C || fn_80119ED0(0x14) == 0xC8 || fn_80119ED0(0x14) == 0xCD) {
        obj = pokemonGetStatus(trainer, 0, 0xD6, 0);
        if (fn_80119ED0(0x14) == 0x7C || fn_80119ED0(0x14) == 0xC8) {
            if (obj == NULL) {
                target = NULL;
            } else {
                target = pokemonGetStatus(obj, 0, 0xCC, 0);
            }
            status = fn_80121ADC(target, 0x14);
        } else if (fn_80119ED0(0x14) != 0xCD) {
            status = 0;
        } else {
            status = fn_8011B67C(obj, 0x14);
        }
    } else if (fn_80119ED0(0x14) != 0xD8) {
        status = 0;
    } else {
        status = fn_8011B67C(trainer, 0x14);
    }
    if (status == 1) {
        if (fn_80119ED0(0x14) == 0x7C || fn_80119ED0(0x14) == 0xC8 || fn_80119ED0(0x14) == 0xCD) {
            obj2 = pokemonGetStatus(trainer, 0, 0xD6, 0);
            if (fn_80119ED0(0x14) == 0x7C || fn_80119ED0(0x14) == 0xC8) {
                if (obj2 == NULL) {
                    target2 = NULL;
                } else {
                    target2 = pokemonGetStatus(obj2, 0, 0xCC, 0);
                }
                return fn_80121574(target2, 0x14);
            } else if (fn_80119ED0(0x14) != 0xCD) {
                return 0;
            } else {
                return fn_8011A3E4(obj2, 0x14);
            }
        } else if (fn_80119ED0(0x14) != 0xD8) {
            return 0;
        } else {
            return fn_8011A3E4(trainer, 0x14);
        }
    }
    return 0;
}
