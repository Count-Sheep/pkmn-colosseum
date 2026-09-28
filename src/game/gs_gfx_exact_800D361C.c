/**
 * @file gs_gfx_exact_800D361C.c
 * @brief fn_800D361C, 0x800D361C - 0x800D36B4.
 *
 * Function-boundary carve of the GSgfx TU (see gs_gfx.c): wait for the next
 * retrace and record how many retraces passed since the previous frame; in
 * mode 1, keep waiting until at least renderEnabled retraces have passed
 * (and report exactly that many unless vsyncFlag is set). No jump table, no
 * pooled constant; its only data is the .sbss state pointer lbl_8047AA80,
 * kept extern. GC/1.3 -O4,p like the TU, no pragmas.
 */
#include "dolphin/types.h"
#include "game/gs_gfx.h"

extern void VIWaitForRetrace(void);

void fn_800D361C(u8 mode) {
    VIWaitForRetrace();
    lbl_8047AA80->frameDelta = lbl_8047AA80->xfbCount - lbl_8047AA80->xfbAddr0;

    if (mode == 1) {
        while (lbl_8047AA80->frameDelta < lbl_8047AA80->renderEnabled) {
            VIWaitForRetrace();
            lbl_8047AA80->frameDelta = lbl_8047AA80->xfbCount - lbl_8047AA80->xfbAddr0;
        }

        if (lbl_8047AA80->vsyncFlag == 0) {
            lbl_8047AA80->frameDelta = lbl_8047AA80->renderEnabled;
        }
    }

    lbl_8047AA80->xfbAddr0 = lbl_8047AA80->xfbCount;
}
