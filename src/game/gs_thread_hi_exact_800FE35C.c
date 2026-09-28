/**
 * @file gs_thread_hi_exact_800FE35C.c
 * @brief fn_800FE35C, 0x800FE35C - 0x800FE38C: reset the sprite scissor to
 *        the full 640x480 screen.
 *
 * Function-boundary carve of the sprite screen-environment TU
 * (0x800FE35C - 0x800FE6DC, see gs_thread_hi_range_800FE35C.c): no data,
 * one call. Built with the TU's flags (GC/1.3 -O4,p, -opt nopeephole
 * unit-wide: without it the prologue's LR store is scheduled after the
 * argument loads, unlike retail); no local pragmas.
 */
#include "dolphin/types.h"

extern void fn_800D9D68(u16 left, u16 top, u16 right, u16 bottom);

void fn_800FE35C(void) {
    fn_800D9D68(0, 0, 639, 479);
}
