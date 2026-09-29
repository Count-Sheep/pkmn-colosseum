/**
 * @file ps_exact_80172630.c
 * @brief HAL psinterpret.c: the out-of-line jobj.h inlines HSD_JObjAddTz,
 *        HSD_JObjAddTy, HSD_JObjAddTx and HSD_JObjSetupMatrix,
 *        0x80172630 - 0x801728B0.
 *
 * psInterpretParticle0 is too large for the auto-inliner's budget, so the
 * jobj.h inlines it calls are emitted out of line right after it (see
 * src/game/psinterpret.c). This carve holds those four copies; U8ClampAdd,
 * the next header inline (0x801728B0), stays with the candidate chunks
 * because its u8-to-float conversion reads the pool's 2^52 constant, which
 * only the compiler can name.
 *
 * The bodies are jobj.h's. Their only data is jobj.h's assert strings,
 * __FILE__ "jobj.h" (0x8047D670) and the expression "jobj" (0x8047D678), in
 * psinterpret.c's .sdata2 pool. That pool is shared with
 * psInterpretParticle0, which is not exact yet, and is linked as
 * game/data/sdata2_8047D630.c, so the asserts name those entries instead
 * of emitting their own strings. The copies are defined as ordinary
 * functions (the header's versions are set aside by renaming them) because
 * psInterpretParticle0 in the neighbouring chunk calls them across the
 * split boundary. Deferred generation emits them in reverse definition
 * order.
 *
 * Built with the particle library flags (GC/1.3.2 -O4,p -inline
 * auto,deferred -use_lmw_stmw on -sdata 8 -sdata2 8 -str reuse,readonly),
 * no local pragmas.
 */
#include "dolphin/types.h"
#include "sysdolphin/baselib/debug.h"

/* RULE-EXCEPTION(title-path): named stand-ins for psinterpret.c's pooled
 * jobj.h assert strings - see docs/RULE_EXCEPTIONS.md */
extern const char lbl_8047D670[7]; /* "jobj.h" */
extern const char lbl_8047D678[5]; /* "jobj" */

#undef HSD_ASSERT
#define HSD_ASSERT(line, cond) \
    ((cond) ? ((void) 0) : __assert(lbl_8047D670, line, lbl_8047D678))

/* RULE-EXCEPTION(title-path): header static inlines re-declared as global
 * out-of-line functions so the unlinked psInterpretParticle0 chunk can call
 * them across the split - see docs/RULE_EXCEPTIONS.md */
#define HSD_JObjAddTx jobj_h_HSD_JObjAddTx
#define HSD_JObjAddTy jobj_h_HSD_JObjAddTy
#define HSD_JObjAddTz jobj_h_HSD_JObjAddTz
#define HSD_JObjSetupMatrix jobj_h_HSD_JObjSetupMatrix
#include "sysdolphin/baselib/jobj.h"
#undef HSD_JObjAddTx
#undef HSD_JObjAddTy
#undef HSD_JObjAddTz
#undef HSD_JObjSetupMatrix

void HSD_JObjSetupMatrix(HSD_JObj* jobj)
{
    if (!jobj || !HSD_JObjMtxIsDirty(jobj)) {
        return;
    }
    fn_8019D9DC(jobj);
}

void HSD_JObjAddTx(HSD_JObj* jobj, f32 x)
{
    HSD_ASSERT(1109, jobj);
    jobj->translate.x += x;
    if (!(jobj->flags & JOBJ_MTX_INDEP_SRT)) {
        HSD_JObjSetMtxDirty(jobj);
    }
}

void HSD_JObjAddTy(HSD_JObj* jobj, f32 y)
{
    HSD_ASSERT(1120, jobj);
    jobj->translate.y += y;
    if (!(jobj->flags & JOBJ_MTX_INDEP_SRT)) {
        HSD_JObjSetMtxDirty(jobj);
    }
}

void HSD_JObjAddTz(HSD_JObj* jobj, f32 z)
{
    HSD_ASSERT(1131, jobj);
    jobj->translate.z += z;
    if (!(jobj->flags & JOBJ_MTX_INDEP_SRT)) {
        HSD_JObjSetMtxDirty(jobj);
    }
}
