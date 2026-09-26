/**
 * @file util.c
 * @brief HAL sysdolphin util.c, 0x801BF098-0x801BF1F0.
 *
 * Adapted from the Melee decompilation (doldecomp/melee,
 * src/sysdolphin/baselib/util.c). The library is built with deferred
 * inlining, so functions are listed in HAL's order and MWCC emits them in
 * reverse (the retail address order). The TU owns its jump table (.data)
 * and its "util.c"/"0" assert strings (.sdata2).
 */
#include "dolphin/types.h"
#include "dolphin/gx/GX.h"
#include "sysdolphin/baselib/debug.h"

void HSD_MulColor(GXColor* arg0, GXColor* arg1, GXColor* dest)
{
    dest->r = (arg0->r * arg1->r) / 255U;
    dest->g = (arg0->g * arg1->g) / 255U;
    dest->b = (arg0->b * arg1->b) / 255U;
    dest->a = (arg0->a * arg1->a) / 255U;
}

u32 HSD_GetNbBits(u32 c)
{
    int ret = 0;
    int i;

    for (i = 0; i < 32; i++) {
        if (c & (1 << i)) {
            ret++;
        }
    }
    return ret;
}

s32 HSD_Index2PosNrmMtx(u32 arg0)
{
    switch (arg0) {
    case 0:
        return 0;
    case 1:
        return 3;
    case 2:
        return 6;
    case 3:
        return 9;
    case 4:
        return 12;
    case 5:
        return 15;
    case 6:
        return 18;
    case 7:
        return 21;
    case 8:
        return 24;
    case 9:
        return 27;
    default:
        HSD_ASSERT(132, 0);
        return 0;
    }
}
