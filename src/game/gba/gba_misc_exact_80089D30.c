#include "dolphin/types.h"

/* Retail stores this six-entry retry table in small uninitialized data. */
extern u16 lbl_8047A684[6] __attribute__((section(".sdata")));

/*
 * Runs fn_80071AE4 for a 1-based port and clears the port's retry
 * counter; returns fn_80071AE4's result.
 */
s32 fn_80089D30(s32 index)
{
    extern s32 fn_80071AE4(s32);
    s32 result;

    result = fn_80071AE4(index - 1);
    lbl_8047A684[index - 1] = 0;
    return result;
}

void fn_80089D74(s32 index)
{
    extern void fn_800722A0(s32);
    fn_800722A0(index - 1);
}
