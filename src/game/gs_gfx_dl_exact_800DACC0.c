/**
 * @file gs_gfx_dl_exact_800DACC0.c
 * @brief GSgfx display-list capture: GSgfxDLFree, GSgfxDLDraw and
 *        GSgfxDLEnd, 0x800DACC0 - 0x800DAF60.
 *
 * Function-boundary carve of the GSgfx dl TU (0x800DA578 - 0x800DB890,
 * see gs_gfx_dl.c): these functions reference no jump table and no pooled
 * constant. Their only data is the GSgfx state pointer lbl_8047AA80
 * (.sbss), which stays extern. GSgfxDLBegin follows as its own carve.
 */
#include "game/gs_gfx_layer.h"

extern void DCFlushRange(void* addr, u32 size);
extern void GXCallDisplayList(void* list, u32 size);
extern void fn_800D4F98(u32, u32, ...);
extern void fn_800D6A5C(u32 verts, u32 prims);
extern void fn_800D7A70(GSVtxDesc* desc);
extern void fn_800D892C(GSVtxDesc* desc);
extern void fn_800E24B0(u16 handle);
extern void fn_800E209C(u16 handle);
extern void fn_800E2AF8(u16 handle, u32 size);

void GSgfxDLFree(GSgfxDLCapture* capture) {
    if (lbl_8047AA80->captureDL != capture) {
        fn_800E24B0(capture->handle);
        fn_800E209C(capture->handle);
        capture->active = 0;
    }
}

void GSgfxDLDraw(GSgfxDLCapture* capture) {
    if (lbl_8047AA80->recordMode == 1) {
        fn_800D4F98(0x2a, 1, capture);
    } else if (lbl_8047AA80->layerDraw == lbl_8047AA80->layerCur &&
               (lbl_8047AA80->drawMask & lbl_8047AA80->drawEnable) &&
               capture->size != 0) {
        fn_800D7A70(capture->desc);
        fn_800D892C(capture->desc);
        GXCallDisplayList(capture->data, capture->size);
        fn_800D6A5C(capture->totalVerts, capture->totalPrims);
    }
}

GSgfxDLCapture* GSgfxDLEnd(void) {
    GSgfxDLCapture* capture;
    u8* end;
    u32 pad;

    if (!lbl_8047AA80->captureActive) {
        return NULL;
    }
    capture = lbl_8047AA80->captureDL;
    end = (u8*)(((u32)lbl_8047AA80->captureCursor + 0x1F) & ~0x1F);
    if ((u32)end > (u32)capture->data + capture->size || capture->overflow != 0) {
        fn_800E24B0(capture->handle);
        fn_800E209C(capture->handle);
        return NULL;
    }
    for (pad = end - lbl_8047AA80->captureCursor; pad != 0; pad--) {
        *lbl_8047AA80->captureCursor++ = 0;
    }
    capture->size = end - (u8*)capture->data;
    fn_800E2AF8(capture->handle, capture->size);
    DCFlushRange(capture->data, capture->size);
    lbl_8047AA80->captureActive = 0;
    lbl_8047AA80->captureDL = NULL;
    lbl_8047AA80->captureCursor = NULL;
    return capture;
}
