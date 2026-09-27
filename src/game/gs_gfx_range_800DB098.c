/**
 * @file gs_gfx_range_800DB098.c
 * @brief GSgfx display-list capture: record the current vertex,
 * 0x800DB098 - 0x800DB758.
 *
 * Function-boundary carve of fn_800DB098, the display-list-capture path
 * of the vertex emitter (fn_800D6B00 calls it while a GSgfxDLBegin
 * capture is open). It needs nothing its TU (the GSgfx dl code,
 * 0x800DA578 - 0x800DB890) owns: no jump table, no pooled constant. The
 * only relocations are to the GSgfx state pointer and to the emitter
 * functions it compares against.
 *
 * For every enabled attribute it writes the cached value in the format
 * the installed emitter would have sent (float bits, u16, u8 or packed
 * colour) and advances the capture cursor.
 */
#include "game/gs_gfx_layer.h"

typedef struct GSgfxDLCapture {
    u8 active;
    u8 overflow;
    u16 handle;
    void* data;
    u32 size;
    GSVtxDesc* desc;
    u32 totalVerts;
    u32 totalPrims;
} GSgfxDLCapture;

extern void fn_800D724C(u32);
extern void fn_800D7268(u32);
extern void fn_800D7284(u32);
extern void fn_800D72A4(u32);
extern void fn_800D72C4(u32);
extern void fn_800D72E4(u32);
extern void fn_800D7304(u32);
extern void fn_800D7328(u32);
extern void fn_800D7344(u32);
extern void fn_800D7360(u32);
extern void fn_800D737C(u32);
extern void fn_800D7398(u32);
extern void fn_800D73C4(u32);
extern void fn_800D73F8(u32);
extern void fn_800D740C(u32);
extern void fn_800D7420(u32);
extern void fn_800D7444(u32);
extern void fn_800D7468(u32);
extern void fn_800D748C(u32);
extern void fn_800D74A0(u32);
extern void fn_800D74B4(u32);
extern void fn_800D74D0(u32);
extern void fn_800D74EC(u32);
extern void fn_800D7508(u32);
extern void fn_800D7524(u32);
extern void fn_800D7540(u32);
extern void fn_800D7564(u32);
extern void fn_800D7588(u32);
extern void fn_800D75AC(u32);
extern void fn_800D75D0(u32);

void fn_800DB098(void) {
    GSVtxEmitFn emit;
    s32 i;
    s32 n;

    if (lbl_8047AA80->captureCursor + 0x68 > (u8*)lbl_8047AA80->captureDL->data + lbl_8047AA80->captureDL->size) {
        lbl_8047AA80->captureDL->overflow = 1;
        return;
    }

    emit = lbl_8047AA80->emitPos;
    if (emit == fn_800D75D0) {
        u32* p = (u32*)lbl_8047AA80->captureCursor;
        *p++ = *(u32*)&lbl_8047AA80->pos.f[0];
        *p++ = *(u32*)&lbl_8047AA80->pos.f[1];
        *p++ = *(u32*)&lbl_8047AA80->pos.f[2];
        lbl_8047AA80->captureCursor = (u8*)p;
    } else if (emit == fn_800D75AC || emit == fn_800D7588) {
        u16* p = (u16*)lbl_8047AA80->captureCursor;
        *p++ = lbl_8047AA80->pos.s[0];
        *p++ = lbl_8047AA80->pos.s[1];
        *p++ = lbl_8047AA80->pos.s[2];
        lbl_8047AA80->captureCursor = (u8*)p;
    } else if (emit == fn_800D7564 || emit == fn_800D7540) {
        u8* p = (u8*)lbl_8047AA80->captureCursor;
        *p++ = lbl_8047AA80->pos.b[0];
        *p++ = lbl_8047AA80->pos.b[1];
        *p++ = lbl_8047AA80->pos.b[2];
        lbl_8047AA80->captureCursor = (u8*)p;
    } else if (emit == fn_800D7524) {
        u32* p = (u32*)lbl_8047AA80->captureCursor;
        *p++ = *(u32*)&lbl_8047AA80->pos.f[0];
        *p++ = *(u32*)&lbl_8047AA80->pos.f[1];
        lbl_8047AA80->captureCursor = (u8*)p;
    } else if (emit == fn_800D7508 || emit == fn_800D74EC) {
        u16* p = (u16*)lbl_8047AA80->captureCursor;
        *p++ = lbl_8047AA80->pos.s[0];
        *p++ = lbl_8047AA80->pos.s[1];
        lbl_8047AA80->captureCursor = (u8*)p;
    } else if (emit == fn_800D74D0 || emit == fn_800D74B4) {
        u8* p = (u8*)lbl_8047AA80->captureCursor;
        *p++ = lbl_8047AA80->pos.b[0];
        *p++ = lbl_8047AA80->pos.b[1];
        lbl_8047AA80->captureCursor = (u8*)p;
    } else if (emit == fn_800D74A0) {
        u16* p = (u16*)lbl_8047AA80->captureCursor;
        *p++ = lbl_8047AA80->pos.s[0];
        lbl_8047AA80->captureCursor = (u8*)p;
    } else if (emit == fn_800D748C) {
        u8* p = (u8*)lbl_8047AA80->captureCursor;
        *p++ = lbl_8047AA80->pos.b[0];
        lbl_8047AA80->captureCursor = (u8*)p;
    }

    emit = lbl_8047AA80->emitNrm;
    if (lbl_8047AA80->captureDL->desc->attr[2].enabled == 1) {
        if (emit == fn_800D7468) {
            u32* p = (u32*)lbl_8047AA80->captureCursor;
            *p++ = *(u32*)&lbl_8047AA80->nrm.f[0];
            *p++ = *(u32*)&lbl_8047AA80->nrm.f[1];
            *p++ = *(u32*)&lbl_8047AA80->nrm.f[2];
            lbl_8047AA80->captureCursor = (u8*)p;
        } else if (emit == fn_800D7444) {
            u16* p = (u16*)lbl_8047AA80->captureCursor;
            *p++ = lbl_8047AA80->nrm.s[0];
            *p++ = lbl_8047AA80->nrm.s[1];
            *p++ = lbl_8047AA80->nrm.s[2];
            lbl_8047AA80->captureCursor = (u8*)p;
        } else if (emit == fn_800D7420) {
            u8* p = (u8*)lbl_8047AA80->captureCursor;
            *p++ = lbl_8047AA80->nrm.b[0];
            *p++ = lbl_8047AA80->nrm.b[1];
            *p++ = lbl_8047AA80->nrm.b[2];
            lbl_8047AA80->captureCursor = (u8*)p;
        } else if (emit == fn_800D740C) {
            u16* p = (u16*)lbl_8047AA80->captureCursor;
            *p++ = lbl_8047AA80->nrm.s[0];
            lbl_8047AA80->captureCursor = (u8*)p;
        } else if (emit == fn_800D73F8) {
            u8* p = (u8*)lbl_8047AA80->captureCursor;
            *p++ = lbl_8047AA80->nrm.b[0];
            lbl_8047AA80->captureCursor = (u8*)p;
        }
    }

    for (i = 4; i < 6; i++) {
        n = i - 4;
        emit = lbl_8047AA80->emitClr[n];
        if (lbl_8047AA80->captureDL->desc->attr[i].enabled == 1) {
            if (emit == fn_800D73C4) {
                u8* p = (u8*)lbl_8047AA80->captureCursor;
                *p++ = lbl_8047AA80->clr[n].b[0];
                *p++ = lbl_8047AA80->clr[n].b[1];
                *p++ = lbl_8047AA80->clr[n].b[2];
                *p++ = lbl_8047AA80->clr[n].b[3];
                lbl_8047AA80->captureCursor = (u8*)p;
            } else if (emit == fn_800D7398) {
                u8* p = (u8*)lbl_8047AA80->captureCursor;
                *p++ = lbl_8047AA80->clr[n].b[0];
                *p++ = lbl_8047AA80->clr[n].b[1];
                *p++ = lbl_8047AA80->clr[n].b[2];
                lbl_8047AA80->captureCursor = (u8*)p;
            } else if (emit == fn_800D737C) {
                u32* p = (u32*)lbl_8047AA80->captureCursor;
                *p++ = lbl_8047AA80->clr[n].w;
                lbl_8047AA80->captureCursor = (u8*)p;
            } else if (emit == fn_800D7360) {
                u16* p = (u16*)lbl_8047AA80->captureCursor;
                *p++ = lbl_8047AA80->clr[n].s;
                lbl_8047AA80->captureCursor = (u8*)p;
            } else if (emit == fn_800D7344) {
                u16* p = (u16*)lbl_8047AA80->captureCursor;
                *p++ = lbl_8047AA80->clr[n].s;
                lbl_8047AA80->captureCursor = (u8*)p;
            } else if (emit == fn_800D7328) {
                u8* p = (u8*)lbl_8047AA80->captureCursor;
                *p++ = lbl_8047AA80->clr[n].b[0];
                lbl_8047AA80->captureCursor = (u8*)p;
            }
        }
    }

    for (i = 6; i < 14; i++) {
        n = i - 6;
        emit = lbl_8047AA80->emitTex[n];
        if (lbl_8047AA80->captureDL->desc->attr[i].enabled == 1) {
            if (emit == fn_800D7304) {
                u32* p = (u32*)lbl_8047AA80->captureCursor;
                *p++ = *(u32*)&lbl_8047AA80->tex[n].f[0];
                *p++ = *(u32*)&lbl_8047AA80->tex[n].f[1];
                lbl_8047AA80->captureCursor = (u8*)p;
            } else if (emit == fn_800D72E4 || emit == fn_800D72C4) {
                u16* p = (u16*)lbl_8047AA80->captureCursor;
                *p++ = lbl_8047AA80->tex[n].s[0];
                *p++ = lbl_8047AA80->tex[n].s[1];
                lbl_8047AA80->captureCursor = (u8*)p;
            } else if (emit == fn_800D72A4 || emit == fn_800D7284) {
                u8* p = (u8*)lbl_8047AA80->captureCursor;
                *p++ = lbl_8047AA80->tex[n].b[0];
                *p++ = lbl_8047AA80->tex[n].b[1];
                lbl_8047AA80->captureCursor = (u8*)p;
            } else if (emit == fn_800D7268) {
                u16* p = (u16*)lbl_8047AA80->captureCursor;
                *p++ = lbl_8047AA80->tex[n].s[0];
                lbl_8047AA80->captureCursor = (u8*)p;
            } else if (emit == fn_800D724C) {
                u8* p = (u8*)lbl_8047AA80->captureCursor;
                *p++ = lbl_8047AA80->tex[n].b[0];
                lbl_8047AA80->captureCursor = (u8*)p;
            }
        }
    }
}
