/**
 * @file gs_thread_hi_exact_800FE6A0.c
 * @brief Sprite screen scale and origin accessors, 0x800FE6A0 - 0x800FE6DC.
 *
 * Function-boundary carve of the sprite screen-environment TU
 * (0x800FE35C - 0x800FE6DC, see gs_thread_hi_range_800FE35C.c): the
 * screen scale (lbl_80478B10/14, .sdata) and sprite origin
 * (lbl_8047AC70/72, .sbss) stay extern, as in the TU source. Built with
 * the TU's flags (GC/1.3 -O4,p, -opt nopeephole unit-wide); no local
 * pragmas.
 */
#include "dolphin/types.h"

extern f32 lbl_80478B10; /* screen x scale */
extern f32 lbl_80478B14; /* screen y scale */
extern s16 lbl_8047AC70; /* sprite origin x */
extern s16 lbl_8047AC72; /* sprite origin y */

void fn_800FE6A0(f32 x, f32 y) {
    lbl_80478B10 = x;
    lbl_80478B14 = y;
}

void fn_800FE6AC(s16* x, s16* y) {
    if (x != NULL) {
        *x = lbl_8047AC70;
    }
    if (y != NULL) {
        *y = lbl_8047AC72;
    }
}

void fn_800FE6D0(s16 x, s16 y) {
    lbl_8047AC70 = x;
    lbl_8047AC72 = y;
}
