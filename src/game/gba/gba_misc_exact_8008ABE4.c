#include "dolphin/types.h"

/* Per-port result of the last poll. */
extern s32 lbl_803FB308[4];
/* Per-port GBA link state (1 = poll keys, 2 = busy, 3 = send 0x11). */
extern s32 lbl_803FB318[4];
/* Per-port GBA key word returned by the last successful poll. */
extern u16 lbl_8047A67C[4];
/* Retail stores this six-entry retry table in small uninitialized data. */
extern u16 lbl_8047A684[6] __attribute__((section(".sdata")));

/*
 * Sets a 1-based port's GBA link state and clears its poll result, retry
 * counter and key word; returns the previous state.
 */
s32 gbaCommandSetKeyState(s32 index, s32 state)
{
    s32 old = lbl_803FB318[index - 1];

    lbl_803FB318[index - 1] = state;
    lbl_803FB308[index - 1] = 0;
    lbl_8047A684[index - 1] = 0;
    lbl_8047A67C[index - 1] = 0;
    return old;
}
