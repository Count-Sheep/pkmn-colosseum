/**
 * @file gs_gfx_layer_candidate_800D923C.c
 * @brief GSgfx layer: flush dirty channel / texgen / TEV state to GX,
 * 0x800D923C - 0x800D963C.
 *
 * Function-boundary carve of fn_800D923C. The switch on the indirect
 * stage type compiles to a compare tree (no jump table), there is no
 * pooled constant, and the GSgfx state (lbl_8047AA80) and the draw
 * statistics (lbl_804001F0) stay extern.
 *
 * The state layout below covers only the fields this function reads;
 * every offset is taken from the target's loads (0x60 channel count,
 * 0x7A TEV stage count, the per-stage arrays from 0x7B, 0x3AC indirect
 * stage count, 0x414 dirty mask).
 */
#include "dolphin/types.h"

typedef struct GXColor {
    u8 r, g, b, a;
} GXColor;

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

typedef struct GSgfxState {
    u8 pad_000[0x60];
    u8 numChans;                 /* 0x060 */
    GSChanCtrl chanCtrl[2][2];   /* 0x061 */
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
    u32 dirty;                   /* 0x414 */
} GSgfxState;

typedef struct GSgfxStats {
    u32 pad_00[6];
    u32 chanUpdates;             /* 0x18 */
    u32 texGenUpdates;           /* 0x1C */
    u32 tevUpdates;              /* 0x20 */
} GSgfxStats;

extern GSgfxState* lbl_8047AA80;
extern GSgfxStats lbl_804001F0;

extern void fn_800BA6B0(u8 num);
extern void fn_800BA6F4(u32 chan, u8 enable, u8 ambSrc, u8 matSrc, u8 lightMask, u8 diffFn, u8 attnFn);
extern void fn_800B884C(u8 num);
extern void fn_800BC8C8(u8 num);
extern void fn_800BBC0C(u8 num);
extern void fn_800BBAF8(u32 stage, u8 coord, u8 map);
extern void fn_800BB97C(u32 stage, u8 scaleS, u8 scaleT);
extern void fn_800BB81C(u32 id, f32 (*mtx)[3], u8 scale);
extern void fn_800BC6F0(u32 stage, u8 coord, u8 map, u8 color);
extern void fn_800BC228(u32 stage, u8 op, u8 bias, u8 scale, u8 clamp, u8 out);
extern void fn_800BC290(u32 stage, u8 op, u8 bias, u8 scale, u8 clamp, u8 out);
extern void fn_800BC1A0(u32 stage, u8 a, u8 b, u8 c, u8 d);
extern void fn_800BC1E4(u32 stage, u8 a, u8 b, u8 c, u8 d);
extern void fn_800BC454(u32 stage, u32 sel);
extern void fn_800BC4C0(u32 stage, u32 sel);
extern void fn_800BB780(u32 stage, u8 indStage, u8 format, u8 biasSel, u8 mtxSel, u8 wrapS, u8 wrapT, u8 addPrev, u8 utcLod, u8 alphaSel);
extern void fn_800BBC7C(u32 stage, u8 indStage, u8 signedOffsets, u8 replaceMode, u8 mtxSel);
extern void fn_800BBCE0(u32 stage, u8 indStage, u16 sizeS, u16 sizeT, u16 spacingS, u16 spacingT, u8 format, u8 mtxSel, u8 biasSel, u8 alphaSel);
extern void fn_800BBE8C(u32 stage, u8 indStage, u8 mtxSel);
extern void fn_800BBF98(u32 stage, u8 indStage, u8 mtxSel);
extern void fn_800BBFDC(u32 stage);
extern void fn_800BBC34(u32 stage);
extern void fn_800BC3E0(u32 id, GXColor color);
extern void fn_800BC580(u32 table, u32 r, u32 g, u32 b, u32 a);

/*
 * The GSgfx state pointer is re-read at every use, as in retail (it is
 * reloaded after each GX call). The per-stage arrays are walked through
 * locals taken once after the TEV stage count is set; the K colour
 * selectors are indexed through the global.
 */
void fn_800D923C(void) {
    s32 i;

    if (lbl_8047AA80->dirty & 1) {
        fn_800BA6B0(lbl_8047AA80->numChans);
        for (i = 0; i < lbl_8047AA80->numChans; i++) {
            GSChanCtrl* chan = lbl_8047AA80->chanCtrl[i];
            fn_800BA6F4(i, chan[0].enable, chan[0].ambSrc, chan[0].matSrc,
                        chan[0].lightMask, chan[0].diffFn, chan[0].attnFn);
            fn_800BA6F4(i + 2, chan[1].enable, chan[1].ambSrc, chan[1].matSrc,
                        chan[1].lightMask, chan[1].diffFn, chan[1].attnFn);
        }
        lbl_804001F0.chanUpdates++;
    }

    if (lbl_8047AA80->dirty & 2) {
        fn_800B884C(lbl_8047AA80->numTexGens);
        lbl_804001F0.texGenUpdates++;
    }

    if (lbl_8047AA80->dirty & 4) {
        GSTevOrder* order;
        GSTevOp* colorOp;
        GSTevOp* alphaOp;
        GSTevIn* colorIn;
        GSTevIn* alphaIn;
        u8* indEnable;
        u8 numInd;
        GSTevInd* ind;

        fn_800BC8C8(lbl_8047AA80->numTevStages);
        numInd = lbl_8047AA80->numIndStages;
        order = lbl_8047AA80->tevOrder;
        colorOp = lbl_8047AA80->colorOp;
        alphaOp = lbl_8047AA80->alphaOp;
        colorIn = lbl_8047AA80->colorIn;
        alphaIn = lbl_8047AA80->alphaIn;
        indEnable = lbl_8047AA80->indEnable;
        ind = lbl_8047AA80->ind;
        fn_800BBC0C(numInd);
        if (numInd != 0) {
            GSIndOrder* indOrder = lbl_8047AA80->indOrder;
            GSIndMtx* indMtx = lbl_8047AA80->indMtx;

            for (i = 0; i < numInd; indOrder++, i++) {
                fn_800BBAF8(i, indOrder->coord, indOrder->map);
                fn_800BB97C(i, indOrder->scaleS, indOrder->scaleT);
            }
            for (i = 0; i < 3; i++, indMtx++) {
                fn_800BB81C(i + 1, indMtx->mtx, indMtx->scale);
            }
        }

        for (i = 0; i < lbl_8047AA80->numTevStages; order++, colorOp++, alphaOp++, colorIn++, alphaIn++, indEnable++, ind++, i++) {
            fn_800BC6F0(i, order->coord, order->map, order->color);
            fn_800BC228(i, colorOp->op, colorOp->bias, colorOp->scale, colorOp->clamp, colorOp->out);
            fn_800BC290(i, alphaOp->op, alphaOp->bias, alphaOp->scale, alphaOp->clamp, alphaOp->out);
            fn_800BC1A0(i, colorIn->a, colorIn->b, colorIn->c, colorIn->d);
            fn_800BC1E4(i, alphaIn->a, alphaIn->b, alphaIn->c, alphaIn->d);
            fn_800BC454(i, lbl_8047AA80->kcolorSel[i]);
            fn_800BC4C0(i, lbl_8047AA80->kalphaSel[i]);
            if (*indEnable != 0 && numInd != 0) {
                switch (ind->type) {
                case 0:
                    fn_800BB780(i, ind->indStage, ind->format, ind->biasSel, ind->mtxSel,
                                ind->wrapS, ind->wrapT, ind->addPrev, ind->utcLod, ind->alphaSel);
                    break;
                case 1:
                    fn_800BBC7C(i, ind->indStage, ind->warpSigned, ind->warpReplace, ind->mtxSel);
                    break;
                case 2:
                    fn_800BBCE0(i, ind->indStage, ind->tileSizeS, ind->tileSizeT, ind->tileSpacingS,
                                ind->tileSpacingT, ind->format, ind->mtxSel, ind->biasSel, ind->alphaSel);
                    break;
                case 3:
                    fn_800BBE8C(i, ind->indStage, ind->mtxSel);
                    break;
                case 4:
                    fn_800BBF98(i, ind->indStage, ind->mtxSel);
                    break;
                case 5:
                    fn_800BBFDC(i);
                    break;
                }
            } else {
                fn_800BBC34(i);
            }
        }

        for (i = 0; i < 4; i++) {
            fn_800BC3E0(i, lbl_8047AA80->kcolor[i]);
        }
        fn_800BC580(0, 0, 1, 2, 3);
        lbl_804001F0.tevUpdates++;
    }

    lbl_8047AA80->dirty = 0;
}
