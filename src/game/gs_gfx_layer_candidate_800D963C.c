/**
 * @file gs_gfx_layer_candidate_800D963C.c
 * @brief GSgfx layer: TEV stage presets (the GXSetTevOp equivalent),
 * 0x800D963C - 0x800D9AF0.
 *
 * Function-boundary carve of fn_800D963C. The mode switch compiles to a
 * compare tree (no jump table), every value is an `li` immediate, and
 * the GSgfx state (lbl_8047AA80) stays extern.
 *
 * Each preset is four TEV setter expansions (colour op, colour inputs,
 * alpha op, alpha inputs), each storing its fields and setting the TEV
 * dirty bit: the same sequences repeated across all five cases, which is
 * the repeated-expansion evidence for the shared setters in
 * game/gs_gfx_layer.h.
 *
 * The preset values are GX's GXSetTevOp tables, but under GS's own mode
 * numbering (2 = replace, 3 = blend). The case bodies are in GX's source
 * order (modulate, decal, blend, replace, passclr), which is retail's
 * block order.
 */
#include "game/gs_gfx_layer.h"

void fn_800D963C(u32 stage, s32 mode) {
    switch (mode) {
    case 0:
        GSgfxSetTevColorOp(stage, 0, 0, 0, 1, 0);
        GSgfxSetTevColorIn(stage, 15, 10, 8, 15);
        GSgfxSetTevAlphaOp(stage, 0, 0, 0, 1, 0);
        GSgfxSetTevAlphaIn(stage, 7, 4, 5, 7);
        break;
    case 1:
        GSgfxSetTevColorOp(stage, 0, 0, 0, 1, 0);
        GSgfxSetTevColorIn(stage, 10, 8, 9, 15);
        GSgfxSetTevAlphaOp(stage, 0, 0, 0, 1, 0);
        GSgfxSetTevAlphaIn(stage, 7, 7, 7, 5);
        break;
    case 3:
        GSgfxSetTevColorOp(stage, 0, 0, 0, 1, 0);
        GSgfxSetTevColorIn(stage, 10, 12, 8, 15);
        GSgfxSetTevAlphaOp(stage, 0, 0, 0, 1, 0);
        GSgfxSetTevAlphaIn(stage, 7, 5, 4, 7);
        break;
    case 2:
        GSgfxSetTevColorOp(stage, 0, 0, 0, 1, 0);
        GSgfxSetTevColorIn(stage, 15, 15, 15, 8);
        GSgfxSetTevAlphaOp(stage, 0, 0, 0, 1, 0);
        GSgfxSetTevAlphaIn(stage, 7, 7, 7, 4);
        break;
    case 4:
        GSgfxSetTevColorOp(stage, 0, 0, 0, 1, 0);
        GSgfxSetTevColorIn(stage, 15, 15, 15, 10);
        GSgfxSetTevAlphaOp(stage, 0, 0, 0, 1, 0);
        GSgfxSetTevAlphaIn(stage, 7, 7, 7, 5);
        break;
    }
}
