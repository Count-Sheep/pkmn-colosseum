/**
 * @file gs_gfx_dl_exact_800DA578.c
 * @brief GSgfxParseDisplayList, 0x800DA578 - 0x800DA6F0.
 *
 * Function-boundary carve of the first function of the GSgfx dl TU
 * (0x800DA578 - 0x800DB890, see gs_gfx_dl.c). It needs nothing the TU
 * owns: its command switches compile to compare chains (no jump table)
 * and it has no pooled constant. Its only relocation is the call to
 * _dlParseSurface, which stays in the candidate chunk.
 *
 * Walks a GX display list and hands each primitive to _dlParseSurface;
 * NOPs are skipped, 0x61 (BP register load) commands step over their
 * payload, and any other command ends the walk.
 */
#include "game/gs_gfx_layer.h"

typedef struct GSgfxVtxDescList {
    s32 attr;
    s32 attr_type;
    s32 comp_cnt;
    s32 comp_type;
    u8 frac;
    u16 stride;
    void* vertex;
} GSgfxVtxDescList;

typedef struct GSgfxParseCallbackList {
    void (*begin)(s32 prim, u16 count, u32 attrs, void* user);
    void (*beginVertex)(void* user);
    void (*vertexAttr)(u32 attr, void* data, void* user);
    void (*endVertex)(void* user);
    void (*end)(void* user);
} GSgfxParseCallbackList;

extern GSgfxDLCapture* lbl_8047AAD4;
extern u32 lbl_8047AAD8;

u8* _dlParseSurface__F13GSgfxPrimTypeP16_HSD_VtxDescListPUcUsP22GSgfxParseCallbackListPv_802B1590(s32 prim, GSgfxVtxDescList* desc, u8* ptr, u16 count, GSgfxParseCallbackList* callbacks, void* user);

void GSgfxParseDisplayList(GSgfxVtxDescList* desc, u8* ptr, u32 size, GSgfxParseCallbackList* callbacks, void* user) {
    s32 prim;
    u8* end;
    u16 count;
    s32 cmd;

    end = ptr + size;
    while (ptr < end) {
        cmd = *ptr++ & 0xF8;
        switch (cmd) {
        case 0x00:
            break;
        case 0x80:
        case 0x90:
        case 0x98:
        case 0xA0:
        case 0xA8:
        case 0xB0:
        case 0xB8:
            count = *(u16*)ptr;
            ptr = (u8*)((u16*)ptr + 1);
            switch (cmd) {
            case 0xB8:
                prim = 0;
                break;
            case 0xA8:
                prim = 1;
                break;
            case 0xB0:
                prim = 2;
                break;
            case 0x90:
                prim = 3;
                break;
            case 0x98:
                prim = 4;
                break;
            case 0xA0:
                prim = 5;
                break;
            case 0x80:
                prim = 6;
                break;
            }
            ptr = _dlParseSurface__F13GSgfxPrimTypeP16_HSD_VtxDescListPUcUsP22GSgfxParseCallbackListPv_802B1590(prim, desc, ptr, count, callbacks, user);
            break;
        case 0x61:
            ptr += 4;
            break;
        default:
            return;
        }
    }
}
