/**
 * @file gs_gfx_dl_exact_800DA6F0.c
 * @brief GSgfx display lists: _dlParseSurface, 0x800DA6F0 - 0x800DA880,
 *        with its switch table (.data 0x803152B8 - 0x80315320).
 *
 * Function-boundary carve of the GSgfx dl TU (0x800DA578 - 0x800DB890,
 * see gs_gfx_dl.c). _dlParseSurface builds the vertex attribute mask with
 * a switch whose jump table, jumptable_803152B8, is the first of the TU's
 * four .data switch tables: it starts the TU's 8-aligned .data, only
 * _dlParseSurface references it, and it ends at 0x80315320, where the
 * _dlParseVertex tables start (8-aligned). No pooled constant; it calls
 * _dlParseVertex out of line. GC/1.3 -O4,p like the TU, no pragmas; the
 * body is the TU's.
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

u8* _dlParseVertex__FP16_HSD_VtxDescListPUcP22GSgfxParseCallbackListPv_802B1720(GSgfxVtxDescList* desc, u8* ptr, GSgfxParseCallbackList* callbacks, void* user);

u8* _dlParseSurface__F13GSgfxPrimTypeP16_HSD_VtxDescListPUcUsP22GSgfxParseCallbackListPv_802B1590(s32 prim, GSgfxVtxDescList* desc, u8* ptr, u16 count, GSgfxParseCallbackList* callbacks, void* user) {
    GSgfxVtxDescList* it;
    u32 mask;
    s32 attr;

    mask = 0;
    it = desc;
    while ((attr = it->attr) != 0xff) {
        switch (attr) {
        case 0:
            mask |= 1;
            break;
        case 1:
            mask |= 0x40;
            break;
        case 9:
            mask |= 2;
            break;
        case 10:
            mask |= 4;
            break;
        case 25:
            mask |= 8;
            break;
        case 11:
            mask |= 0x10;
            break;
        case 12:
            mask |= 0x20;
            break;
        case 13:
            mask |= 0x80;
            break;
        case 14:
            mask |= 0x100;
            break;
        case 15:
            mask |= 0x200;
            break;
        case 16:
            mask |= 0x400;
            break;
        case 17:
            mask |= 0x800;
            break;
        case 18:
            mask |= 0x1000;
            break;
        case 19:
            mask |= 0x2000;
            break;
        case 20:
            mask |= 0x4000;
            break;
        }
        it++;
    }

    if (callbacks->begin != 0) {
        callbacks->begin(prim, count, mask, user);
    }

    while (count-- != 0) {
        if (callbacks->beginVertex != 0) {
            callbacks->beginVertex(user);
        }
        it = desc;
        while (it->attr != 0xff) {
            ptr = _dlParseVertex__FP16_HSD_VtxDescListPUcP22GSgfxParseCallbackListPv_802B1720(it, ptr, callbacks, user);
            it++;
        }
        if (callbacks->endVertex != 0) {
            callbacks->endVertex(user);
        }
    }

    if (callbacks->end != 0) {
        callbacks->end(user);
    }
    return ptr;
}
