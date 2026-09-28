/**
 * @file gs_gfx_dl_exact_800DAF60.c
 * @brief GSgfxDLBegin, 0x800DAF60 - 0x800DB098.
 *
 * Function-boundary carve of the GSgfx dl TU (0x800DA578 - 0x800DB890,
 * see gs_gfx_dl.c): no jump table, no pooled constant. Its data is the
 * GSgfx state pointer and the capture table (lbl_8047AA80, lbl_8047AAD4
 * and lbl_8047AAD8 in .sbss), which stay extern.
 *
 * The free-slot search is the GS library's find-free idiom (GSpartFindFree,
 * materialFindFree, lightFindFree in the linked GS units, the same code
 * shape in GSmaterialCreate, GSpartCreate, GSlightCreate/GSlightLoad):
 * both loop exits land on the caller's NULL test with the result in the
 * loop register (found: branch with r31 = slot; exhausted: li r31,0),
 * which is the early-return search expanded at its call. Written in place
 * (break with a NULL fallback) the function reaches 93.0-95.8%.
 */
#include "game/gs_gfx_layer.h"

extern void fn_800E209C(u16 handle);
extern void* fn_800E27B0(u16 handle);
extern u16 fn_800E2C04(u32 size, u32 align);

extern GSgfxDLCapture* lbl_8047AAD4;
extern u32 lbl_8047AAD8;

static inline GSgfxDLCapture* GSgfxFindFreeDLCapture(GSgfxDLCapture* capture) {
    u32 i;

    for (i = 0; i < lbl_8047AAD8; i++) {
        if (capture->active == 0) {
            return capture;
        }
        capture++;
    }
    return NULL;
}

u32 GSgfxDLBegin(GSVtxDesc* desc, u32 size) {
    GSgfxDLCapture* capture;

    if (lbl_8047AA80->captureActive == 1) {
        return 0;
    }
    if (lbl_8047AA80->vtxPending == 1) {
        return 0;
    }
    capture = GSgfxFindFreeDLCapture(lbl_8047AAD4);
    if (capture == NULL) {
        return 0;
    }
    capture->active = 1;
    capture->overflow = 0;
    capture->totalVerts = 0;
    capture->totalPrims = 0;
    capture->handle = fn_800E2C04(size, 0x20);
    if (capture->handle == 0) {
        return 0;
    }
    capture->size = size;
    capture->data = fn_800E27B0(capture->handle);
    if (capture->data == NULL) {
        fn_800E209C(capture->handle);
        return 0;
    }
    capture->desc = desc;
    lbl_8047AA80->captureActive = 1;
    lbl_8047AA80->captureDL = capture;
    lbl_8047AA80->captureCursor = capture->data;
    return 1;
}
