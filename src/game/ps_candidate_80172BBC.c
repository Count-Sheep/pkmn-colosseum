/**
 * @file ps_candidate_80172BBC.c
 * @brief HAL psinterpret.c: applyForceJObj, 0x80172BBC - 0x80172D00.
 *
 * Function-boundary carve of psinterpret.c (see src/game/psinterpret.c for
 * the TU extent and the body this copies). The function has no jump table
 * and no data of its own, but it reads two entries of psinterpret.c's
 * .sdata2 pool: 0.0f (0x8047D630) and, through the inlined
 * HSD_JObjSetupMatrix -> HSD_JObjMtxIsDirty assert, jobj.h's __FILE__
 * "jobj.h" (0x8047D670) and expression "jobj" (0x8047D678). That pool is
 * shared with psInterpretParticle0, which is not exact yet, and is linked
 * as game/data/sdata2_8047D630.c, so this carve refers to the entries by
 * their pool names instead of emitting its own literals.
 *
 * Built with the particle library flags (GC/1.3.2 -O4,p -inline
 * auto,deferred -use_lmw_stmw on -sdata 8 -sdata2 8 -str reuse,readonly),
 * no local pragmas.
 */
#include "dolphin/types.h"
#include "sysdolphin/baselib/debug.h"
#include "sysdolphin/baselib/psstructs.h"

/* RULE-EXCEPTION(title-path): named stand-ins for psinterpret.c's pooled
 * literals (0.0f and jobj.h's assert strings) - see docs/RULE_EXCEPTIONS.md */
extern const f32 lbl_8047D630;     /* 0.0f */
extern const char lbl_8047D670[7]; /* "jobj.h" */
extern const char lbl_8047D678[5]; /* "jobj" */

#undef HSD_ASSERT
#define HSD_ASSERT(line, cond) \
    ((cond) ? ((void) 0) : __assert(lbl_8047D670, line, lbl_8047D678))

#include "sysdolphin/baselib/jobj.h"

/* Pulls the particle towards a JObj; returns whether it arrived. */
s32 applyForceJObj(HSD_Particle* pp, HSD_JObj* jobj, f32 force, f32 radius)
{
    f32 dx;
    f32 dy;
    f32 dz;
    f32 len;

    if (jobj == NULL || radius < lbl_8047D630) {
        return 0;
    }
    HSD_JObjSetupMatrix(jobj);
    dx = jobj->mtx[0][3] - pp->pos.x;
    dy = jobj->mtx[1][3] - pp->pos.y;
    dz = jobj->mtx[2][3] - pp->pos.z;
    len = dx * dx + dy * dy + dz * dz;
    if (len <= radius * radius) {
        return 1;
    }
    if (lbl_8047D630 == len) {
        return 0;
    }
    len = force / len;
    pp->vel.x += len * dx;
    pp->vel.y += len * dy;
    pp->vel.z += len * dz;
    return 0;
}
