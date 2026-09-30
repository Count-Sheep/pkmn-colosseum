/**
 * @file fade_exact_801C6908.c
 * @brief fn_801C6908 (0x801C6908 - 0x801C6934): the fade effect's random
 *        helper and the "fade effect requested" flag setter.
 *
 * Function-boundary carve of the fade effect TU (fade_range_801C4CB8.c),
 * GC/1.3 -O4,p, text only. fn_801C6928 is called from fadeDaemon's setup
 * (fade.c); the flag is read and cleared by the effect loop in the range.
 */
#include "dolphin/types.h"

extern u32 _fadeEffectGetRandom__FUl(u32 range);
extern u8 lbl_8047B3B0;

/* 0x801C6908 | 0x20 */
u32 fn_801C6908(u32 range)
{
    return _fadeEffectGetRandom__FUl(range);
}

/* 0x801C6928 | 0xC */
void fn_801C6928(void)
{
    lbl_8047B3B0 = 1;
}
