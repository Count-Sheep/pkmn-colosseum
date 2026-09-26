/**
 * @file random.c
 * @brief HAL sysdolphin random.c: the library's linear congruential
 *        generator, 0x801ADC08-0x801ADD0C.
 *
 * Adapted from the Melee decompilation (doldecomp/melee,
 * src/sysdolphin/baselib/random.c). Colosseum's newer sysdolphin forgets the
 * seed pointer through the memory module's released-memory check instead of
 * a [low, high) range. The library is built with deferred inlining, so the
 * functions are listed in HAL's order and MWCC emits them in reverse (the
 * retail address order); HSD_Randi inlines HSD_Rand.
 *
 * Symbols keep their dtk names (fn_/lbl_) because game code already links
 * against them: HSD_Rand is fn_801ADCD8, HSD_Randf fn_801ADC7C, HSD_Randi
 * fn_801ADC3C, the seed lbl_80478C90 and HSD_RandSeedPtr lbl_80478C94 (the
 * GS math code repoints it).
 */
#include "dolphin/types.h"

/* HSD_IsMemoryReleased: nonzero when the pointer lies in freed memory. */
int fn_801A6990(void* ptr);

/* seed */
u32 lbl_80478C90 = 1;
/* HSD_RandSeedPtr */
u32* lbl_80478C94 = &lbl_80478C90;

/* HSD_Rand */
s32 fn_801ADCD8(void)
{
    *lbl_80478C94 = *lbl_80478C94 * 214013 + 2531011;
    return *lbl_80478C94 >> 0x10;
}

/* HSD_Randf */
f32 fn_801ADC7C(void)
{
    *lbl_80478C94 = *lbl_80478C94 * 214013 + 2531011;
    return (f32) (*lbl_80478C94 >> 0x10) / (1 << 16);
}

/* HSD_Randi */
s32 fn_801ADC3C(s32 max_val)
{
    return max_val * fn_801ADCD8() / (1 << 16);
}

void _HSD_RandForgetMemory(void)
{
    if (fn_801A6990(lbl_80478C94)) {
        lbl_80478C94 = &lbl_80478C90;
    }
}
