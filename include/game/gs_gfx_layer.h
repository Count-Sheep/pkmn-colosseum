/**
 * @file gs_gfx_layer.h
 * @brief GSgfx layer state: the channel / texgen / TEV block that the
 * GSgfx layer setters fill and fn_800D923C flushes to GX.
 *
 * Partial view of the state behind lbl_8047AA80. Every offset is taken
 * from the target's loads and stores (fn_800D923C, fn_800D963C,
 * fn_800D892C); fields no carve touches are left as padding. It is a
 * separate view from gs_gfx.h's older GSgfxState, whose layout in this
 * range is still unverified.
 */
#ifndef GS_GFX_LAYER_H
#define GS_GFX_LAYER_H

#include "dolphin/types.h"
#include "dolphin/gx/GX.h"

typedef struct GSChanCtrl {
    u8 enable;
    u8 ambSrc;
    u8 matSrc;
    u8 lightMask;
    u8 diffFn;
    u8 attnFn;
} GSChanCtrl;

typedef struct GSTevOrder {
    u8 coord;
    u8 map;
    u8 color;
} GSTevOrder;

typedef struct GSTevOp {
    u8 op;
    u8 bias;
    u8 scale;
    u8 clamp;
    u8 out;
} GSTevOp;

typedef struct GSTevIn {
    u8 a;
    u8 b;
    u8 c;
    u8 d;
} GSTevIn;

typedef struct GSTevInd {
    u8 type;
    u8 indStage;
    u8 mtxSel;
    u8 format;
    u8 biasSel;
    u8 alphaSel;
    u8 wrapS;
    u8 wrapT;
    u8 addPrev;
    u8 utcLod;
    u8 warpSigned;
    u8 warpReplace;
    u16 tileSizeS;
    u16 tileSizeT;
    u16 tileSpacingS;
    u16 tileSpacingT;
} GSTevInd;

typedef struct GSIndOrder {
    u8 coord;
    u8 map;
    u8 scaleS;
    u8 scaleT;
} GSIndOrder;

typedef struct GSIndMtx {
    f32 mtx[2][3];
    u8 scale;
} GSIndMtx;

typedef struct GSgfxLayerState {
    u8 pad_000[0x60];
    u8 numChans;                 /* 0x060 */
    GSChanCtrl chanCtrl[2][2];   /* 0x061: [channel][colour, alpha] */
    u8 numTexGens;               /* 0x079 */
    u8 numTevStages;             /* 0x07A */
    GSTevOrder tevOrder[16];     /* 0x07B */
    GSTevOp colorOp[16];         /* 0x0AB */
    GSTevOp alphaOp[16];         /* 0x0FB */
    GSTevIn colorIn[16];         /* 0x14B */
    GSTevIn alphaIn[16];         /* 0x18B */
    u32 kcolorSel[16];           /* 0x1CC */
    u32 kalphaSel[16];           /* 0x20C */
    GXColor kcolor[4];           /* 0x24C */
    u8 indEnable[16];            /* 0x25C */
    GSTevInd ind[16];            /* 0x26C */
    u8 numIndStages;             /* 0x3AC */
    GSIndOrder indOrder[4];      /* 0x3AD */
    GSIndMtx indMtx[3];          /* 0x3C0 */
    u32 dirty;                   /* 0x414: 1 channels, 2 texgens, 4 TEV */
} GSgfxLayerState;

/* Per-frame GX state upload counters. */
typedef struct GSgfxStats {
    u32 pad_00[6];
    u32 chanUpdates;             /* 0x18 */
    u32 texGenUpdates;           /* 0x1C */
    u32 tevUpdates;              /* 0x20 */
} GSgfxStats;

extern GSgfxLayerState* lbl_8047AA80;
extern GSgfxStats lbl_804001F0;

/*
 * TEV stage setters. None exists out of line; each is expanded at every
 * use (four per case in fn_800D963C's five TEV presets), always ending in
 * the TEV dirty bit.
 */
static inline void GSgfxSetTevColorOp(u32 stage, u8 op, u8 bias, u8 scale, u8 clamp, u8 out) {
    GSTevOp* tevOp = &lbl_8047AA80->colorOp[stage];

    tevOp->op = op;
    tevOp->bias = bias;
    tevOp->scale = scale;
    tevOp->clamp = clamp;
    tevOp->out = out;
    lbl_8047AA80->dirty |= 4;
}

static inline void GSgfxSetTevAlphaOp(u32 stage, u8 op, u8 bias, u8 scale, u8 clamp, u8 out) {
    GSTevOp* tevOp = &lbl_8047AA80->alphaOp[stage];

    tevOp->op = op;
    tevOp->bias = bias;
    tevOp->scale = scale;
    tevOp->clamp = clamp;
    tevOp->out = out;
    lbl_8047AA80->dirty |= 4;
}

static inline void GSgfxSetTevColorIn(u32 stage, u8 a, u8 b, u8 c, u8 d) {
    GSTevIn* tevIn = &lbl_8047AA80->colorIn[stage];

    tevIn->a = a;
    tevIn->b = b;
    tevIn->c = c;
    tevIn->d = d;
    lbl_8047AA80->dirty |= 4;
}

static inline void GSgfxSetTevAlphaIn(u32 stage, u8 a, u8 b, u8 c, u8 d) {
    GSTevIn* tevIn = &lbl_8047AA80->alphaIn[stage];

    tevIn->a = a;
    tevIn->b = b;
    tevIn->c = c;
    tevIn->d = d;
    lbl_8047AA80->dirty |= 4;
}

#endif /* GS_GFX_LAYER_H */
