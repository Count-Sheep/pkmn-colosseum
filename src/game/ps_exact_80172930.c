/**
 * @file ps_exact_80172930.c
 * @brief psCopyGeneratorData, psApplyOffsetLocalRotation and
 *        psApplyVelocityLocalRotation (0x80172930 - 0x80172BBC).
 *
 * Three data-free functions of HAL's psinterpret.c (0x8016F430 -
 * 0x80173624), carved like its already linked _psListGetNext, getTime and
 * getFloat: they reference no pooled literal, string or table, so they
 * build to the same code outside the whole unit. The rest of the unit
 * (psInterpretParticle0 and the functions using its .sdata2/.rodata/.data
 * pools) stays in the candidate chunks until psInterpretParticle0 is exact.
 *
 * Built with the particle library flags (pslist.c, particle.c, generator.c,
 * psdisptev.c): GC/1.3.2 -O4,p -inline auto,deferred -use_lmw_stmw on
 * -sdata 8 -sdata2 8 -str reuse,readonly; defined in reverse address order.
 */
#include "dolphin/mtx.h"
#include "dolphin/types.h"
#include "sysdolphin/baselib/psstructs.h"

extern void PSMTXIdentity(Mtx m);
extern void PSMTXScale(Mtx m, f32 x, f32 y, f32 z);
extern void PSMTXRotRad(Mtx m, char axis, f32 rad);
extern void PSMTXConcat(const Mtx a, const Mtx b, Mtx ab);
extern void psSetGeneratorAngleRadiusScale(HSD_Generator* gp, f32* scale,
                                           u8 motion);

/* Rotates the particle's velocity by the generator's local rotation. */
void psApplyVelocityLocalRotation(HSD_Particle* pp)
{
    Mtx rot_x;
    Mtx rot_y;
    Mtx rot_z;
    Vec vel;

    if (pp->gen == NULL || !(pp->gen->posFlags & 4)) {
        return;
    }
    vel.x = pp->vel.x;
    vel.y = pp->vel.y;
    vel.z = pp->vel.z;
    PSMTXRotRad(rot_x, 'X', pp->gen->rot.x);
    PSMTXRotRad(rot_y, 'Y', pp->gen->rot.y);
    PSMTXRotRad(rot_z, 'Z', pp->gen->rot.z);
    PSMTXConcat(rot_y, rot_x, rot_x);
    PSMTXConcat(rot_z, rot_x, rot_x);
    PSMTXMultVec(rot_x, &vel, &vel);
    pp->vel.x = vel.x;
    pp->vel.y = vel.y;
    pp->vel.z = vel.z;
}

/* Transforms an offset by the generator's local rotation (and scale). */
void psApplyOffsetLocalRotation(HSD_Particle* pp, Vec* ofs)
{
    Mtx rot_x;
    Mtx rot_y;
    Mtx rot_z;
    Mtx scale;

    if (pp->gen == NULL || !(pp->gen->posFlags & 8)) {
        return;
    }
    if (pp->gen->posFlags & 0x10) {
        PSMTXIdentity(scale);
    } else {
        PSMTXScale(scale, pp->gen->scale.x, pp->gen->scale.y,
                   pp->gen->scale.z);
    }
    PSMTXRotRad(rot_x, 'X', pp->gen->rot.x);
    PSMTXRotRad(rot_y, 'Y', pp->gen->rot.y);
    PSMTXRotRad(rot_z, 'Z', pp->gen->rot.z);
    PSMTXConcat(rot_y, rot_x, rot_x);
    PSMTXConcat(rot_z, rot_x, rot_x);
    PSMTXConcat(scale, rot_x, rot_x);
    PSMTXMultVec(rot_x, ofs, ofs);
}

/* Copies the rotation/scale block, flags and JObj of a parent generator. */
void psCopyGeneratorData(HSD_Generator* gp, HSD_Generator* src)
{
    if (src == NULL || gp == NULL) {
        return;
    }
    if (src->kind & 0x20000000) {
        gp->kind |= 0x20000000;
    }
    gp->rot.x = src->rot.x;
    gp->rot.y = src->rot.y;
    gp->rot.z = src->rot.z;
    gp->scale.x = src->scale.x;
    gp->scale.y = src->scale.y;
    gp->scale.z = src->scale.z;
    gp->posFlags = src->posFlags;
    gp->posFlags &= ~2;
    gp->jobj = src->jobj;
    if (src->posFlags & 0x1000) {
        psSetGeneratorAngleRadiusScale(gp, &src->scale2.x, 1);
    } else {
        psSetGeneratorAngleRadiusScale(gp, &src->scale2.x, 0);
    }
}
