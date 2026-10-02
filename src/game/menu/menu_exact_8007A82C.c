/**
 * @file menu_exact_8007A82C.c
 * @brief Report whether the menu fade has completed.
 */
#include "dolphin/types.h"

extern s32 fadeCheck(s32);

s32 fn_8007A82C(void)
{
    return fadeCheck(1);
}
