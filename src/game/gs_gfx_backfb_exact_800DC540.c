/**
 * @file gs_gfx_backfb_exact_800DC540.c
 * @brief GSgfxBackFBInit, the last function of Genius Sonority's
 *        backfb.cpp (GSAPI/GSgfxM), 0x800DC540-0x800DC560.
 *
 * Carved out of gs_gfx_backfb.c so the exact initializer can link while
 * GSgfxBeginBackFBCapture is still a candidate. Built with the
 * gs_gfx_backfb.c unit's flags.
 */
#include "dolphin/types.h"

#define GS_BACKFB_CAPTURE_MAX 4

/* One back-framebuffer capture slot (0x14 bytes); see gs_gfx_backfb.c. */
typedef struct GSbackFBCapture {
    u8 active;
    u8 _pad[3];
    void* texture;
    void* callback;
    void* userData;
    u32 param;
} GSbackFBCapture;

extern GSbackFBCapture lbl_80400EE0[GS_BACKFB_CAPTURE_MAX]; /* capture slots */
extern u8 lbl_8047AAE0;                                     /* any capture active */

/* GSgfxBackFBInit(void): frees every capture slot. */
void GSgfxBackFBInit__Fv(void)
{
    int i;

    for (i = 0; i < GS_BACKFB_CAPTURE_MAX; i++) {
        lbl_80400EE0[i].active = 0;
    }
    lbl_8047AAE0 = 0;
}
