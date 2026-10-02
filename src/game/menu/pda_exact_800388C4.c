/**
 * @file pda_exact_800388C4.c
 * @brief Byte-exact PDA teardown helper, 0x800388C4 - 0x80038990.
 */
#include "dolphin/types.h"

#pragma scheduling off
void fn_800388C4(void)
{
    extern void menuCloseCustom(s32 slot, s32 arg1, s32 arg2);

    menuCloseCustom(0x19, 0, 1);
    menuCloseCustom(0x1a, 0, 1);
    menuCloseCustom(0x1b, 0, 1);
    menuCloseCustom(0x18, 0, 1);
    menuCloseCustom(0x1e, 0, 1);
    menuCloseCustom(0x1f, 0, 1);
    menuCloseCustom(0x20, 0, 1);
    menuCloseCustom(0x21, 0, 1);
    menuCloseCustom(0x22, 0, 1);
    menuCloseCustom(0x23, 0, 1);
    menuCloseCustom(0x1d, 0, 1);
}
#pragma scheduling reset
