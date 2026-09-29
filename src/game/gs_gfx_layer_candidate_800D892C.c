/**
 * @file gs_gfx_layer_candidate_800D892C.c
 * @brief GSgfx: set up channels, texgens and TEV stages for a vertex
 * descriptor, then flush them (fn_800D923C). 0x800D892C - 0x800D923C.
 *
 * Text-only candidate (94.501724% in report.json; 93.97069% raw objdiff).
 * Function-boundary carve material: no jump
 * table, no pooled constant. The tables (lbl_80314404, lbl_803144F0,
 * lbl_80478AE0) and the saved TEV block lbl_80400B28 stay extern.
 *
 * The channel, texgen and TEV setters from game/gs_gfx_layer.h are
 * expanded throughout (the channel setter four times, the TEV-order
 * setter in both stage loops, the TEV colour setters again after
 * fn_800D963C's presets).
 *
 * `mode` is not set when flag 0x40000000 is present. Retail then reads it
 * uninitialised: r0 still holds the saved link register at the
 * `cmpwi r0, 0x1`. The source leaves it uninitialised to match.
 *
 * Retail's texgen count comes from a loop in the TEV-load path and from
 * explicit per-slot tests in the default path. The first keeps a
 * redundant branch for slot 0 and the second drops it, and each form
 * reproduces only its own path.
 *
 * Open question: retail stores the saved indirect-stage count back twice
 * when it is non-zero, first as n + 1 and then as (u8)(n + 1) - 1. The
 * ++/-- pair below reproduces those stores, but it is a placeholder, not
 * the recovered source construct. Before this unit can be accepted, the
 * real construct has to be identified (or the pair shown to be retail's
 * own code).
 *
 * Remaining wall (register allocation): in the two masked channel-setter
 * expansions retail gives the "both" flag a callee-saved register and
 * keeps the channel index in r5. This source gets the reverse, and the
 * mismatch carries into the later blocks. Declaration order, local vs
 * parameter index, loop form, parameter types, C vs C++ and GC/1.3.2/2.0
 * all leave it unchanged.
 * A current 590-row diff has 236 differing rows (16 one-sided); the first
 * substantive divergence begins in the masked channel-setter expansion.
 * Moving numTev below indEnable in the saved-TEV path recovers six rows;
 * register allocation in the masked setters remains the larger wall.
 * This remains an unlinked CodeCandidate, not accepted Matching progress.
 */
#include "game/gs_gfx_layer.h"

extern void* memcpy(void* dst, const void* src, u32 n);


extern GSgfxTevState lbl_80400B28;
extern u32 lbl_80314404[8];
extern u32 lbl_803144F0[8];
extern u32 lbl_80478AE0[2];

extern void fn_800D963C(u32 stage, s32 mode);
extern void fn_800D923C(void);

void fn_800D892C(GSVtxDesc* desc) {
    s32 mode;
    u32 mask;
    u32 bit;
    s32 last;
    s32 i;

    if (!(lbl_8047AA80->flags & 0x40000000)) {
        mode = 1;
        if (desc->attr[5].enabled != 0) {
            mode = 2;
        } else if (!(lbl_8047AA80->flags & 4) && !(lbl_8047AA80->flags & 1)) {
            mode = 0;
        }
        GSgfxSetNumChans(mode);
        if (mode > 0) {
            if (lbl_8047AA80->flags & 4) {
                mask = 0;
                for (bit = 0x10; bit <= 0x800; bit++) {
                    if (lbl_8047AA80->flags & bit) {
                        mask |= 1 << (bit - 0x10);
                    }
                }
                if (lbl_8047AA80->flags & 1) {
                    GSgfxSetChanCtrl(4, 1, 0, 1, mask, 2, 2);
                } else {
                    GSgfxSetChanCtrl(4, 1, 0, 0, mask, 2, 2);
                }
            } else {
                GSgfxSetChanCtrl(4, 0, 1, 1, 0, 0, 2);
            }
            if (mode == 2) {
                GSgfxSetChanCtrl(5, 0, 1, 1, 0, 0, 2);
            }
        }
    }

    if (!(lbl_8047AA80->flags & 2)) {
        GSgfxSetNumTexGens(0);
        if (mode == 1) {
            GSgfxSetNumTevStages(1);
            GSgfxSetTevOrder(0, 0xFF, 0xFF, 4);
            fn_800D963C(0, 4);
        } else {
            GSgfxSetNumTevStages(2);
            GSgfxSetTevOrder(0, 0xFF, 0xFF, 4);
            fn_800D963C(0, 4);
            GSgfxSetTevOrder(1, 0xFF, 0xFF, 5);
            GSgfxSetTevColorOp(1, 0, 0, 0, 1, 0);
            GSgfxSetTevColorIn(1, 15, 10, 12, 0);
        }
        GSgfxSetNumIndStages(0);
    } else if (lbl_8047AA80->flags & 0x80000000) {
        s32 numInd;
        u8 indEnable;
        s32 numTev;

        if (lbl_80400B28.numIndStages != 0) {
            lbl_80400B28.numIndStages++;
            lbl_80400B28.numIndStages--;
        }

        last = 0;
        for (i = 0; i < 8; i++) {
            if (desc->attr[6 + i].enabled == 1) {
                last = i;
            }
        }

        numTev = lbl_80400B28.numTevStages;
        numInd = lbl_80400B28.numIndStages;
        GSgfxSetNumTexGens(last + 1);
        GSgfxSetNumTevStages(numTev);
        GSgfxSetNumIndStages(numInd);
        for (i = 0; i < numTev; i++) {
            GSTevStageSrc* src;

            src = &lbl_8047AA80->stageSrc[i];
            indEnable = lbl_8047AA80->tev.indEnable[i] = (lbl_80400B28.indEnable[i] != 0 && numInd > 0);
            GSgfxSetTevOrder(i, lbl_80314404[src->coord], lbl_803144F0[src->map], lbl_80478AE0[src->color]);
            memcpy(&lbl_8047AA80->tev.colorOp[i], &lbl_80400B28.colorOp[i], sizeof(GSTevOp));
            memcpy(&lbl_8047AA80->tev.alphaOp[i], &lbl_80400B28.alphaOp[i], sizeof(GSTevOp));
            memcpy(&lbl_8047AA80->tev.colorIn[i], &lbl_80400B28.colorIn[i], sizeof(GSTevIn));
            memcpy(&lbl_8047AA80->tev.alphaIn[i], &lbl_80400B28.alphaIn[i], sizeof(GSTevIn));
            lbl_8047AA80->tev.kcolorSel[i] = lbl_80400B28.kcolorSel[i];
            lbl_8047AA80->tev.kalphaSel[i] = lbl_80400B28.kalphaSel[i];
            if (indEnable) {
                memcpy(&lbl_8047AA80->tev.ind[i], &lbl_80400B28.ind[i], sizeof(GSTevInd));
            }
        }
        memcpy(lbl_8047AA80->tev.kcolor, lbl_80400B28.kcolor, sizeof(lbl_80400B28.kcolor));
        if (numInd != 0) {
            memcpy(lbl_8047AA80->tev.indOrder, lbl_80400B28.indOrder, numInd * sizeof(GSIndOrder));
            memcpy(lbl_8047AA80->tev.indMtx, lbl_80400B28.indMtx, sizeof(lbl_80400B28.indMtx));
        }
    } else {
        last = 0;
        if (desc->attr[6].enabled == 1) last = 0;
        if (desc->attr[7].enabled == 1) last = 1;
        if (desc->attr[8].enabled == 1) last = 2;
        if (desc->attr[9].enabled == 1) last = 3;
        if (desc->attr[10].enabled == 1) last = 4;
        if (desc->attr[11].enabled == 1) last = 5;
        if (desc->attr[12].enabled == 1) last = 6;
        if (desc->attr[13].enabled == 1) last = 7;

        GSgfxSetNumTexGens(last + 1);
        GSgfxSetNumTevStages(last + 1);
        GSgfxSetNumIndStages(0);
        lbl_8047AA80->tev.numIndStages = lbl_80400B28.numIndStages;
        for (i = 0; i <= last; i++) {
            GSTevStageSrc* src = &lbl_8047AA80->stageSrc[i];

            GSgfxSetTevOrder(i, lbl_80314404[src->coord], lbl_803144F0[src->map], lbl_80478AE0[src->color]);
            fn_800D963C(i, src->mode);
        }
    }

    fn_800D923C();
}
