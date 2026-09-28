/**
 * @file ps_exact_8016E3F8.c
 * @brief HAL psdisp.c: setupTevReg, setupChanReg, psRemoveFog (fn_8016EA88),
 *        0x8016E3F8 - 0x8016EB30.
 *
 * Function-boundary carve of psdisp.c (see src/game/psdisp.c for the TU
 * extent). No jump table, no pooled constant; the data is psdisp.c's .sbss
 * fog and colour-cache state (lbl_8047B128, lbl_8047B130-lbl_8047B140),
 * kept extern. Built with the particle library flags (GC/1.3.2 -O4,p
 * -inline auto,deferred -use_lmw_stmw on -sdata 8 -sdata2 8 -str
 * reuse,readonly), no local pragmas; defined in reverse address order as
 * deferred generation emits them. The bodies are psdisp.c's; the colour
 * helpers and HSD_FogUnref they expand (Melee's static inlines) come from
 * the shared psdisp_color.h.
 */
#include "dolphin/types.h"
#include "hsd/hsd_lobj.h"
#include "sysdolphin/baselib/object.h"
#include "sysdolphin/baselib/psstructs.h"
#include "sysdolphin/baselib/psdisp_color.h"

#define GXSetChanAmbColor fn_800BA4C8
#define GXSetChanMatColor fn_800BA5BC
#define GXSetTevColor fn_800BC2F8
extern void fn_800BA4C8(s32 chan, GXColor color); /* GXSetChanAmbColor */
extern void fn_800BA5BC(s32 chan, GXColor color); /* GXSetChanMatColor */
extern void fn_800BC2F8(s32 reg, GXColor color);  /* GXSetTevColor */
extern void HSD_MulColor(GXColor* a, GXColor* b, GXColor* dst);

#define GX_COLOR0 0
#define GX_TEVREG0 1
#define GX_TEVREG1 2
#define GX_TEVREG2 3
#define GX_MAX_LIGHT 0x100
#define PrimEnv (1 << 7)
#define Trail (1 << 20)
#define DispLighting (1u << 31)

extern HSD_Fog* lbl_8047B128;
extern GXColor lbl_8047B130, lbl_8047B134, lbl_8047B138, lbl_8047B13C, lbl_8047B140;
#define psFog lbl_8047B128
#define prevColorMat lbl_8047B130
#define prevColorEnv lbl_8047B134
#define prevColorPrim lbl_8047B138
#define prevChanAmb lbl_8047B13C
#define prevChanMat lbl_8047B140

/* psRemoveFog */
void fn_8016EA88(void)
{
    if (psFog != NULL) {
        HSD_FogUnref(psFog);
        psFog = NULL;
    }
}

void setupChanReg(HSD_Particle* pp)
{
    GXColor prim_color;
    GXColor amb_color;
    GXColor mat_color;
    HSD_LObj* lobj;

    if (pp->kind & DispLighting) {
        getColorMatAmb(pp, &mat_color, &amb_color);
        if (pp->kind & PrimEnv) {
            prim_color.r = prim_color.g = prim_color.b = 0xFF;
        } else {
            getColorPrimEnv(pp, &prim_color, &mat_color);
            amb_color.r = (u8) ((amb_color.r * prim_color.r) >> 8);
            amb_color.g = (u8) ((amb_color.g * prim_color.g) >> 8);
            amb_color.b = (u8) ((amb_color.b * prim_color.b) >> 8);
        }
        if (prim_color.r != prevChanMat.r || prim_color.g != prevChanMat.g ||
            prim_color.b != prevChanMat.b)
        {
            prevChanMat = prim_color;
            GXSetChanMatColor(GX_COLOR0, prevChanMat);
        }
        lobj = HSD_LObjGetActiveByID(GX_MAX_LIGHT);
        if (lobj != NULL) {
            HSD_MulColor(&amb_color, &lobj->color, &amb_color);
        } else {
            amb_color.r = amb_color.g = amb_color.b = 0;
        }
        if (amb_color.r != prevChanAmb.r || amb_color.g != prevChanAmb.g ||
            amb_color.b != prevChanAmb.b)
        {
            prevChanAmb = amb_color;
            GXSetChanAmbColor(GX_COLOR0, prevChanAmb);
        }
    }
}

void setupTevReg(HSD_Particle* pp)
{
    GXColor prim_color;
    GXColor env_color;
    GXColor mat_color;
    GXColor amb_color;

    getColorPrimEnv(pp, &prim_color, &env_color);
    if ((pp->kind & PrimEnv) ||
        (!(pp->kind & DispLighting) && !(pp->kind & Trail)))
    {
        if (prevColorPrim.r != prim_color.r ||
            prevColorPrim.g != prim_color.g ||
            prevColorPrim.b != prim_color.b || prevColorPrim.a != prim_color.a)
        {
            prevColorPrim = prim_color;
            GXSetTevColor(GX_TEVREG0, prevColorPrim);
        }
        if (pp->kind & PrimEnv) {
            if (prevColorEnv.r != env_color.r ||
                prevColorEnv.g != env_color.g ||
                prevColorEnv.b != env_color.b || prevColorEnv.a != env_color.a)
            {
                prevColorEnv = env_color;
                GXSetTevColor(GX_TEVREG1, prevColorEnv);
            }
        } else if (prevColorEnv.r != 0 || prevColorEnv.g != 0 ||
                   prevColorEnv.b != 0 || prevColorEnv.a != 0)
        {
            prevColorEnv.r = prevColorEnv.g = prevColorEnv.b = prevColorEnv.a =
                0;
            GXSetTevColor(GX_TEVREG1, prevColorEnv);
        }
    }
    if (pp->kind & DispLighting) {
        getColorMatAmb(pp, &mat_color, &amb_color);
        if (pp->kind & PrimEnv) {
            if (prevColorMat.r != mat_color.r ||
                prevColorMat.g != mat_color.g ||
                prevColorMat.b != mat_color.b || prevColorMat.a != mat_color.a)
            {
                prevColorMat = mat_color;
                GXSetTevColor(GX_TEVREG2, prevColorMat);
            }
        } else {
            mat_color.a = (u8) ((mat_color.a * prim_color.a) >> 8);
            if (prevColorMat.r != mat_color.r ||
                prevColorMat.g != mat_color.g ||
                prevColorMat.b != mat_color.b || prevColorMat.a != mat_color.a)
            {
                prevColorMat = mat_color;
                GXSetTevColor(GX_TEVREG2, prevColorMat);
            }
        }
    }
}
