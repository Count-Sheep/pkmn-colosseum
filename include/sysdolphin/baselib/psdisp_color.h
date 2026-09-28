#ifndef SYSDOLPHIN_BASELIB_PSDISP_COLOR_H
#define SYSDOLPHIN_BASELIB_PSDISP_COLOR_H

/*
 * The static inline helpers of HAL's psdisp.c (doldecomp/melee
 * sysdolphin/baselib/psdisp.c and fog.h) that setupTevReg, setupChanReg,
 * psRemoveFog and the display routines expand: fog.h's HSD_FogUnref and the
 * primitive/environment and material/ambient colour interpolators. Shared by
 * the psdisp.c candidate and its exact carve (ps_exact_8016E3F8.c); they
 * are only ever inlined and emit no symbol.
 */

#include "dolphin/types.h"
#include "hsd/hsd_fog.h"
#include "sysdolphin/baselib/object.h"
#include "sysdolphin/baselib/psstructs.h"

/* fog.h */
static inline void HSD_FogUnref(HSD_Fog* fog)
{
    if (fog == NULL) {
        return;
    }
    if (ref_DEC(fog)) {
        hsdDelete(fog);
    }
}

static inline void getColorPrimEnv(HSD_Particle* pp, GXColor* primCol,
                            GXColor* envCol)
{
    if (pp->primColCount) {
        int scale = 65536 * pp->primColRemain / pp->primColCount;
        primCol->r = ((pp->primColTarget.r << 16) +
                      (pp->primCol.r - pp->primColTarget.r) * scale) >>
                     16;
        primCol->g = ((pp->primColTarget.g << 16) +
                      (pp->primCol.g - pp->primColTarget.g) * scale) >>
                     16;
        primCol->b = ((pp->primColTarget.b << 16) +
                      (pp->primCol.b - pp->primColTarget.b) * scale) >>
                     16;
        primCol->a = ((pp->primColTarget.a << 16) +
                      (pp->primCol.a - pp->primColTarget.a) * scale) >>
                     16;
    } else {
        *primCol = pp->primCol;
    }
    if (pp->envColCount) {
        int scale = 65536 * pp->envColRemain / pp->envColCount;
        envCol->r = ((pp->envColTarget.r << 16) +
                     (pp->envCol.r - pp->envColTarget.r) * scale) >>
                    16;
        envCol->g = ((pp->envColTarget.g << 16) +
                     (pp->envCol.g - pp->envColTarget.g) * scale) >>
                    16;
        envCol->b = ((pp->envColTarget.b << 16) +
                     (pp->envCol.b - pp->envColTarget.b) * scale) >>
                    16;
        envCol->a = ((pp->envColTarget.a << 16) +
                     (pp->envCol.a - pp->envColTarget.a) * scale) >>
                    16;
    } else {
        *envCol = pp->envCol;
    }
}

static inline void getColorMatAmb(HSD_Particle* pp, GXColor* matCol, GXColor* ambCol)
{
    if (pp->matColCount) {
        int scale = 65536 * pp->matColRemain / pp->matColCount;
        u8 rgb = ((pp->matRGBTarget << 16) +
                  (pp->matRGB - pp->matRGBTarget) * scale) >>
                 16;
        matCol->r = matCol->g = matCol->b = rgb;
        matCol->a =
            ((pp->matATarget << 16) + (pp->matA - pp->matATarget) * scale) >>
            16;
    } else {
        matCol->r = matCol->g = matCol->b = pp->matRGB;
        matCol->a = pp->matA;
    }
    if (pp->ambColCount) {
        int scale = 65536 * pp->ambColRemain / pp->ambColCount;
        u8 rgb = ((pp->ambRGBTarget << 16) +
                  (pp->ambRGB - pp->ambRGBTarget) * scale) >>
                 16;
        ambCol->r = ambCol->g = ambCol->b = rgb;
        ambCol->a =
            ((pp->ambATarget << 16) + (pp->ambA - pp->ambATarget) * scale) >>
            16;
    } else {
        ambCol->r = ambCol->g = ambCol->b = pp->ambRGB;
        ambCol->a = pp->ambA;
    }
}

#endif /* SYSDOLPHIN_BASELIB_PSDISP_COLOR_H */
