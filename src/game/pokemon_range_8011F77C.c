/**
 * @file pokemon_range_8011F77C.c
 * @brief Residual Pokemon candidate, 0x8011F77C - 0x8011F910.
 */
#define POKEMON_RANGE_SPLIT
#define POKEMON_RANGE_RESIDUAL_8011F77C
#include "src/game/pokemon_range_8011F5FC.c"

/* pokemonGetDp (0x8011FC14) expanded in place: retail reloads the NULL
 * check and uses the shared 0x8(r1) conversion slot, as an inlined call
 * does. XD keeps pokemonGetDp out of line (GXXE01 0x8013EFA0,
 * StarsMmd/Colo-XD-PBR-symbol-maps @ 6b51d3af), and its
 * pokemonGetDarkPokemonLevel (trevor403/xd-asm @ b1087f18 FUN_8013ebd0)
 * calls it where this one expands it. The literals are this TU's pool
 * (0x8047CFF0...), which pokemon_range_8011FCA4 also reads; they stay extern
 * until the pokemon TU links whole. */
static inline f32 pokemonDpGet(u8* ptr)
{
    s32 value;

    if (ptr == NULL) {
        return lbl_8047CFF0;
    }
    value = pokemonGetStatus(ptr, 0, 0xC5, 0);
    return (f32)value / lbl_8047CFF4;
}

s32 pokemonGetDarkPokemonLevel(u8* pokemon)
{
    u16 divisor;
    f32 level;

    if (pokemon == NULL) {
        return 7;
    }
    if ((u8)pokemonGetStatus(pokemon, 0, 0xC2, 0) == 1) {
        divisor = (u16)pokemonGetStatus(pokemon, 0, 0xC4, 0);
        if (divisor == 0) {
            divisor = 1;
        }
        level = pokemonDpGet(pokemon);
        if (level < lbl_8047CFF0) {
            level = lbl_8047CFF0;
        } else {
            level = (lbl_8047CFF4 * level) / (f32)divisor;
        }
        if (level >= lbl_8047CFF4) {
            return 0;
        }
        if (level >= lbl_8047CFF8) {
            return 1;
        }
        if (level >= lbl_8047CFFC) {
            return 2;
        }
        if (level >= lbl_8047D000) {
            return 3;
        }
        if (level >= lbl_8047D004) {
            return 4;
        }
        if (level > lbl_8047CFF0) {
            return 5;
        }
        return 6;
    }
    return 7;
}
