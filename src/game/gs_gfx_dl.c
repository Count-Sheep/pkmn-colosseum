/**
 * @file gs_gfx_dl.c
 * @brief GSgfx display lists: parse a GX display list back into vertex
 * callbacks, and record (GSgfxDLBegin / GSgfxDLEnd) or replay
 * (GSgfxDLDraw) captured lists. 0x800DA578 - 0x800DB098.
 *
 * XD source unit: game/pxdvs/GSAPI/GSgfxM/dl.cpp (C++; the parse
 * helpers keep their mangled names). The TU runs on to 0x800DB890:
 * fn_800DB098 (linked as its own carve) and fn_800DB758 follow, and the
 * TU's .data is the four switch tables at 0x803152B8 - 0x80315384
 * (_dlParseSurface, both _dlParseVertex switches, fn_800DB758), laid out
 * contiguously. It can only be linked as one object once every function
 * in it is exact; _dlParseVertex is the last one short (see there).
 */
#include "game/gs_gfx_layer.h"

extern void DCFlushRange(void* addr, u32 size);
extern void GXCallDisplayList(void* list, u32 size);
extern void fn_800D4F98(u32, u32, ...);
extern void fn_800D6A5C(u32 verts, u32 prims);
extern void fn_800D6A80(u16 vertCount, s32 type, u32* totalVerts, u32* totalPrims);
extern void fn_800D7A70(GSVtxDesc* desc);
extern void fn_800D892C(GSVtxDesc* desc);
extern void fn_800E24B0(u16 handle);
extern void fn_800E209C(u16 handle);
extern void* fn_800E27B0(u16 handle);
extern void fn_800E2AF8(u16 handle, u32 size);
extern u16 fn_800E2C04(u32 size, u32 align);

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
u8* _dlParseVertex__FP16_HSD_VtxDescListPUcP22GSgfxParseCallbackListPv_802B1720(GSgfxVtxDescList* desc, u8* ptr, GSgfxParseCallbackList* callbacks, void* user);

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
        case 11:
            mask |= 8;
            break;
        case 12:
            mask |= 0x10;
            break;
        case 13:
            mask |= 0x20;
            break;
        case 14:
            mask |= 0x80;
            break;
        case 15:
            mask |= 0x100;
            break;
        case 16:
            mask |= 0x200;
            break;
        case 17:
            mask |= 0x400;
            break;
        case 18:
            mask |= 0x800;
            break;
        case 19:
            mask |= 0x1000;
            break;
        case 20:
            mask |= 0x2000;
            break;
        case 25:
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

/*
 * Decodes one attribute of one vertex and hands it to the vertexAttr
 * callback as attr | component | type flags.
 *
 * The flags, the component count and the direct-data size are left unset
 * on the out-of-range paths (an attribute outside 0-20/25, an unknown
 * component type or format): retail has no initialising instruction on
 * those paths either and passes whatever the registers hold.
 *
 * 98.9%: two register copies are still missing. Retail computes the
 * colour attribute flag (the 11/12 ternary) and the else-path component
 * flag in r4 and then moves them to their home registers (mr r7,r4 /
 * mr r8,r4). This source gives the same code with those values built in
 * place. Every other instruction is identical.
 */
u8* _dlParseVertex__FP16_HSD_VtxDescListPUcP22GSgfxParseCallbackListPv_802B1720(GSgfxVtxDescList* desc, u8* ptr, GSgfxParseCallbackList* callbacks, void* user) {
    u16 index;
    u8* data;
    s32 count;
    u32 attrFlag;
    u32 compFlag;
    u32 typeFlag;
    s32 attr;

    data = ptr;
    attr = desc->attr;
    switch (attr) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
        compFlag = 0x10000;
        typeFlag = 0x10000000;
        switch (attr) {
        case 0:
            attrFlag = 1;
            break;
        case 1:
            attrFlag = 0x40;
            break;
        case 2:
            attrFlag = 0x40;
            break;
        case 3:
            attrFlag = 0x40;
            break;
        case 4:
            attrFlag = 0x40;
            break;
        case 5:
            attrFlag = 0x40;
            break;
        case 6:
            attrFlag = 0x40;
            break;
        case 7:
            attrFlag = 0x40;
            break;
        case 8:
            attrFlag = 0x40;
            break;
        }
        ptr += 1;
        break;
    case 9:
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
    case 15:
    case 16:
    case 17:
    case 18:
    case 19:
    case 20:
    case 25:
        if (attr == 11 || attr == 12) {
            attrFlag = (attr == 11) ? 0x10 : 0x20;
            typeFlag = 0x10000000;
            switch (desc->comp_type) {
            case 0:
                compFlag = 0x200000;
                break;
            case 1:
                compFlag = 0x400000;
                break;
            case 2:
                compFlag = 0x800000;
                break;
            case 3:
                compFlag = 0x1000000;
                break;
            case 4:
                compFlag = 0x2000000;
                break;
            case 5:
                compFlag = 0x4000000;
                break;
            }
        } else {
            switch (desc->comp_type) {
            case 0:
                compFlag = 0x10000;
                break;
            case 1:
                compFlag = 0x20000;
                break;
            case 2:
                compFlag = 0x40000;
                break;
            case 3:
                compFlag = 0x80000;
                break;
            case 4:
                compFlag = 0x100000;
                break;
            }
            switch (attr) {
            case 9:
                attrFlag = 2;
                if (desc->comp_cnt == 0) {
                    typeFlag = 0x20000000;
                } else {
                    typeFlag = 0x40000000;
                }
                break;
            case 10:
                attrFlag = 4;
                if (desc->comp_cnt == 0) {
                    typeFlag = 0x40000000;
                }
                break;
            case 25:
                attrFlag = 8;
                if (desc->comp_cnt == 1) {
                    typeFlag = 0x40000000;
                }
                break;
            case 13:
            case 14:
            case 15:
            case 16:
            case 17:
            case 18:
            case 19:
            case 20:
                switch (attr) {
                case 13:
                    attrFlag = 0x80;
                    break;
                case 14:
                    attrFlag = 0x100;
                    break;
                case 15:
                    attrFlag = 0x200;
                    break;
                case 16:
                    attrFlag = 0x400;
                    break;
                case 17:
                    attrFlag = 0x800;
                    break;
                case 18:
                    attrFlag = 0x1000;
                    break;
                case 19:
                    attrFlag = 0x2000;
                    break;
                case 20:
                    attrFlag = 0x4000;
                    break;
                }
                if (desc->comp_cnt == 0) {
                    typeFlag = 0x10000000;
                } else {
                    typeFlag = 0x20000000;
                }
                break;
            }
        }
        if (desc->attr_type == 1) {
            data = ptr;
            switch (compFlag) {
            case 0x10000:
            case 0x20000:
                count = 1;
                break;
            case 0x40000:
            case 0x80000:
            case 0x200000:
            case 0x1000000:
                count = 2;
                break;
            case 0x400000:
            case 0x2000000:
                count = 3;
                break;
            case 0x100000:
            case 0x800000:
            case 0x4000000:
                count = 4;
                break;
            }
            switch (typeFlag) {
            case 0x10000000:
                break;
            case 0x20000000:
                count <<= 1;
                break;
            case 0x40000000:
                count *= 3;
                break;
            case 0x80000000:
                count <<= 2;
                break;
            }
            ptr += count;
        } else {
            if (desc->attr_type == 2) {
                index = *ptr++;
            } else {
                index = *(u16*)ptr;
                ptr += 2;
            }
            data = (u8*)desc->vertex + index * desc->stride;
        }
        break;
    }

    if (callbacks->vertexAttr != 0) {
        callbacks->vertexAttr(attrFlag | compFlag | typeFlag, data, user);
    }
    return ptr;
}

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
