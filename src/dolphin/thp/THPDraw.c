/**
 * @file THPDraw.c
 * @brief Dolphin SDK THP player sample: YUV -> RGB GX drawing,
 *        0x801E1E1C - 0x801E25C8.
 *
 * Built with -inline noauto,deferred (functions are emitted in reverse source
 * order). The GX helpers below are the retail GX entry points that this
 * project has not named yet; their SDK names are given alongside.
 */
#include "dolphin/types.h"
#include "dolphin/gx/GX.h"
#include "dolphin/mtx.h"

typedef struct GXColorS10 {
    s16 r;
    s16 g;
    s16 b;
    s16 a;
} GXColorS10;

typedef struct GXTexObj {
    u32 dummy[8];
} GXTexObj;

typedef union PPCWGPipe {
    u8 u8;
    s8 s8;
    u16 u16;
    s16 s16;
    u32 u32;
    s32 s32;
    f32 f32;
} PPCWGPipe;

#define GXWGFifo (*(volatile PPCWGPipe*)0xCC008000)

extern void GXSetZMode(u32 compareEnable, u32 func, u32 updateEnable);
extern void GXSetBlendMode(u32 type, u32 srcFactor, u32 dstFactor, u32 op);
extern void GXInvalidateTexAll(void);
extern void GXLoadTexObj(GXTexObj* obj, u32 id);
extern void GXLoadPosMtxImm(Mtx mtx, u32 id);
extern void PSMTXIdentity(Mtx mtx);
extern void C_MTXOrtho(f32 mtx[4][4], f32 t, f32 b, f32 l, f32 r, f32 n, f32 f);

extern void fn_800B7874(u32 attr, u32 type);                       /* GXSetVtxDesc */
extern void fn_800B7D3C(void);                                     /* GXClearVtxDesc */
extern void fn_800B7D74(u32 fmt, u32 attr, u32 cnt, u32 type, u8 frac); /* GXSetVtxAttrFmt */
extern void fn_800B857C(u32 dst, u32 func, u32 src, u32 mtx, u8 normalize,
                        u32 postMtx);                              /* GXSetTexCoordGen2 */
extern void fn_800B884C(u8 nTexGens);                              /* GXSetNumTexGens */
extern void fn_800B928C(u8 type, u8 fmt, u16 nverts);              /* GXBegin */
extern void fn_800B9E6C(u8 nChans);                                /* GXSetDispCopyGamma */
extern void fn_800BA6B0(u8 nChans);                                /* GXSetNumChans */
extern void fn_800BA9E4(GXTexObj* obj, void* image, u16 width, u16 height,
                        u32 format, u32 wrapS, u32 wrapT, u8 mipmap); /* GXInitTexObj */
extern void fn_800BACA0(GXTexObj* obj, u32 minFilt, u32 magFilt, f32 minLOD,
                        f32 maxLOD, f32 lodBias, u8 biasClamp, u8 doEdgeLOD,
                        u32 maxAniso);                             /* GXInitTexObjLOD */
extern void GXSetTevOp(u32 stage, u32 mode);
extern void fn_800BC1A0(u32 stage, u32 a, u32 b, u32 c, u32 d);    /* GXSetTevColorIn */
extern void fn_800BC1E4(u32 stage, u32 a, u32 b, u32 c, u32 d);    /* GXSetTevAlphaIn */
extern void fn_800BC228(u32 stage, u32 op, u32 bias, u32 scale, u8 clamp,
                        u32 outReg);                               /* GXSetTevColorOp */
extern void fn_800BC290(u32 stage, u32 op, u32 bias, u32 scale, u8 clamp,
                        u32 outReg);                               /* GXSetTevAlphaOp */
extern void fn_800BC36C(u32 id, GXColorS10 color);                 /* GXSetTevColorS10 */
extern void fn_800BC3E0(u32 id, GXColor color);                    /* GXSetTevKColor */
extern void fn_800BC454(u32 stage, u32 sel);                       /* GXSetTevKColorSel */
extern void fn_800BC4C0(u32 stage, u32 sel);                       /* GXSetTevKAlphaSel */
extern void fn_800BC52C(u32 stage, u32 rasSel, u32 texSel);        /* GXSetTevSwapMode */
extern void fn_800BC580(u32 table, u32 red, u32 green, u32 blue, u32 alpha); /* GXSetTevSwapModeTable */
extern void fn_800BC6F0(u32 stage, u32 coord, u32 map, u32 color); /* GXSetTevOrder */
extern void fn_800BC8C8(u8 nStages);                               /* GXSetNumTevStages */
extern void fn_800BCE30(u8 enable);                                /* GXSetColorUpdate */
extern void fn_800BCE5C(u8 enable);                                /* GXSetAlphaUpdate */
extern void fn_800BCEF4(u32 pixFmt, u32 zFmt);                     /* GXSetPixelFmt */
extern void fn_800BD2E0(f32 mtx[4][4], u32 type);                  /* GXSetProjection */
extern void fn_800BD554(u32 id);                                   /* GXSetCurrentMtx */
extern void fn_800BD744(f32 left, f32 top, f32 wd, f32 ht, f32 nearz, f32 farz); /* GXSetViewport */
extern void fn_800BD7A0(u32 left, u32 top, u32 wd, u32 ht);        /* GXSetScissor */

void fn_801E24B0(void)
{
    GXSetZMode(1, 7, 0);
    GXSetBlendMode(0, 1, 0, 0xF);
    fn_800B884C(1);
    fn_800BA6B0(0);
    fn_800BC8C8(1);
    fn_800BC6F0(0, 0, 0, 0xFF);
    GXSetTevOp(0, 3);
    fn_800BC52C(0, 0, 0);
    fn_800BC52C(1, 0, 0);
    fn_800BC52C(2, 0, 0);
    fn_800BC52C(3, 0, 0);
    fn_800BC580(0, 0, 1, 2, 3);
    fn_800BC580(1, 0, 0, 0, 3);
    fn_800BC580(2, 1, 1, 1, 3);
    fn_800BC580(3, 2, 2, 2, 3);
}

void fn_801E1FF8(GXRenderModeObj* rmode)
{
    s32 w;
    s32 h;
    f32 m[4][4];
    Mtx e_m;

    w = rmode->fbWidth;
    h = rmode->efbHeight;

    fn_800BCEF4(0, 0);

    C_MTXOrtho(m, 0.0f, h, 0.0f, w, 0.0f, -1.0f);
    fn_800BD2E0(m, 1);
    fn_800BD744(0.0f, 0.0f, w, h, 0.0f, 1.0f);
    fn_800BD7A0(0, 0, w, h);

    PSMTXIdentity(e_m);
    GXLoadPosMtxImm(e_m, 0);
    fn_800BD554(0);

    GXSetZMode(1, 7, 0);
    GXSetBlendMode(0, 1, 0, 0);
    fn_800BCE30(1);
    fn_800BCE5C(0);
    fn_800B9E6C(0);
    fn_800BA6B0(0);

    fn_800B884C(2);
    fn_800B857C(0, 1, 4, 60, 0, 125);
    fn_800B857C(1, 1, 4, 60, 0, 125);
    GXInvalidateTexAll();

    fn_800B7D3C();
    fn_800B7874(9, 1);
    fn_800B7874(13, 1);
    fn_800B7D74(7, 9, 1, 3, 0);
    fn_800B7D74(7, 13, 1, 2, 0);

    fn_800BC8C8(4);
    fn_800BC6F0(0, 1, 1, 0xFF);
    fn_800BC1A0(0, 15, 8, 14, 2);
    fn_800BC228(0, 0, 0, 0, 0, 0);
    fn_800BC1E4(0, 7, 4, 6, 1);
    fn_800BC290(0, 1, 0, 0, 0, 0);
    fn_800BC454(0, 12);
    fn_800BC4C0(0, 28);
    fn_800BC52C(0, 0, 0);

    fn_800BC6F0(1, 1, 2, 0xFF);
    fn_800BC1A0(1, 15, 8, 14, 0);
    fn_800BC228(1, 0, 0, 1, 0, 0);
    fn_800BC1E4(1, 7, 4, 6, 0);
    fn_800BC290(1, 1, 0, 0, 0, 0);
    fn_800BC454(1, 13);
    fn_800BC4C0(1, 29);
    fn_800BC52C(1, 0, 0);

    fn_800BC6F0(2, 0, 0, 0xFF);
    fn_800BC1A0(2, 15, 8, 12, 0);
    fn_800BC228(2, 0, 0, 0, 1, 0);
    fn_800BC1E4(2, 4, 7, 7, 0);
    fn_800BC290(2, 0, 0, 0, 1, 0);
    fn_800BC52C(2, 0, 0);

    fn_800BC6F0(3, 0xFF, 0xFF, 0xFF);
    fn_800BC1A0(3, 1, 0, 14, 15);
    fn_800BC228(3, 0, 0, 0, 1, 0);
    fn_800BC1E4(3, 7, 7, 7, 7);
    fn_800BC290(3, 0, 0, 0, 1, 0);
    fn_800BC52C(3, 0, 0);
    fn_800BC454(3, 14);

    fn_800BC36C(1, (GXColorS10){ -90, 0, -114, 135 });
    fn_800BC3E0(0, (GXColor){ 0, 0, 226, 88 });
    fn_800BC3E0(1, (GXColor){ 179, 0, 0, 182 });
    fn_800BC3E0(2, (GXColor){ 255, 0, 255, 128 });
    fn_800BC580(0, 0, 1, 2, 3);
}

void fn_801E1E1C(u8* y_data, u8* u_data, u8* v_data, s16 x, s16 y,
                 s16 textureWidth, s16 textureHeight, s16 polygonWidth,
                 s16 polygonHeight)
{
    GXTexObj tobj0;
    GXTexObj tobj1;
    GXTexObj tobj2;

    fn_800BA9E4(&tobj0, y_data, textureWidth, textureHeight, 1, 0, 0, 0);
    fn_800BACA0(&tobj0, 0, 0, 0, 0, 0, 0, 0, 0);
    GXLoadTexObj(&tobj0, 0);

    fn_800BA9E4(&tobj1, u_data, textureWidth >> 1, textureHeight >> 1, 1, 0, 0, 0);
    fn_800BACA0(&tobj1, 0, 0, 0, 0, 0, 0, 0, 0);
    GXLoadTexObj(&tobj1, 1);

    fn_800BA9E4(&tobj2, v_data, textureWidth >> 1, textureHeight >> 1, 1, 0, 0, 0);
    fn_800BACA0(&tobj2, 0, 0, 0, 0, 0, 0, 0, 0);
    GXLoadTexObj(&tobj2, 2);

    fn_800B928C(0x80, 7, 4);
    GXWGFifo.s16 = x;
    GXWGFifo.s16 = y;
    GXWGFifo.s16 = 0;
    GXWGFifo.u16 = 0;
    GXWGFifo.u16 = 0;
    GXWGFifo.s16 = x + polygonWidth;
    GXWGFifo.s16 = y;
    GXWGFifo.s16 = 0;
    GXWGFifo.u16 = 1;
    GXWGFifo.u16 = 0;
    GXWGFifo.s16 = x + polygonWidth;
    GXWGFifo.s16 = y + polygonHeight;
    GXWGFifo.s16 = 0;
    GXWGFifo.u16 = 1;
    GXWGFifo.u16 = 1;
    GXWGFifo.s16 = x;
    GXWGFifo.s16 = y + polygonHeight;
    GXWGFifo.s16 = 0;
    GXWGFifo.u16 = 0;
    GXWGFifo.u16 = 1;
}
