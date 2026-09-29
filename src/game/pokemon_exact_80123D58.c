/**
 * @file pokemon_exact_80123D58.c
 * @brief pokemonWazaCreate (0x80123D58 - 0x80123E70): clears a move slot,
 *        sets its move id and fills its PP with the move's max PP.
 *
 * One-function carve of the Pokemon TU (pokemon_range_8011F5FC.c), GC/1.3
 * -O4,p, text only. Retail expands pokemonWazaInit (0x80125314) and
 * pokemonWazaGetMaxPP (0x80123E70) here; the carve carries single-use
 * inline copies of them, as pokemonSetWazaStatus's candidate does.
 */
#include "dolphin/types.h"

extern u32 pokemonGetStatus(u8* ptr, u32 a, u32 b, u16 c);
extern void pokemonSetStatus(u8* obj, u32 param, u16 selector, u32 index, u16 value);
extern u8 wazaGetMaxPP(u16 type_id, u8 val);

/* RULE-EXCEPTION(title-path): single-use inline copy of pokemonWazaInit (0x80125314) - see docs/RULE_EXCEPTIONS.md */
static inline void pokemonWazaInitInline(u8* ptr, u32 slot)
{
    if (ptr == NULL) {
        return;
    }
    pokemonSetStatus(ptr, 0, 0x7F, slot, 0);
    pokemonSetStatus(ptr, 0, 0x80, slot, 0);
    pokemonSetStatus(ptr, 0, 0x81, slot, 0);
}

/* RULE-EXCEPTION(title-path): single-use inline copy of pokemonWazaGetMaxPP (0x80123E70) - see docs/RULE_EXCEPTIONS.md */
static inline u8 pokemonWazaGetMaxPPInline(u8* ptr, u16 waza)
{
    u32 slot;
    u16 id;

    if (ptr == NULL) {
        return 0;
    }
    slot = waza + 4;
    id = pokemonGetStatus(ptr, 0, 0x7F, slot);
    return wazaGetMaxPP(id, pokemonGetStatus(ptr, 0, 0x81, slot));
}

/* 0x80123D58 | 0x118 */
void pokemonWazaCreate(u8* ptr, u32 slot, u32 waza)
{
    if (ptr == NULL) {
        return;
    }
    pokemonWazaInitInline(ptr, slot);
    pokemonSetStatus(ptr, 0, 0x7F, slot, waza);
    pokemonSetStatus(ptr, 0, 0x80, slot, pokemonWazaGetMaxPPInline(ptr, slot));
}
