/**
 * @file mobj_exact_801A8354.c
 * @brief sysdolphin mobj.c, .text 0x801A8354-0x801A8478 (the TU's tail).
 *
 * HSD_MObjReqAnimByFlags, HSD_MObjAddAnim, HSD_MObjClearFlags,
 * HSD_MObjSetFlags, HSD_MObjGetFlags and HSD_MObjSetCurrent. Built with the
 * sysdolphin library flags (GC/1.3.2 -O4,p -O1 -inline auto,deferred
 * -use_lmw_stmw on); deferred inlining emits functions in reverse
 * definition order, so they are listed from the last address down.
 *
 * Text-only unit: current_mobj (.sbss 0x8047B2D4) stays extern. See
 * mobj_exact_801A6CA4.c for the rest of the TU.
 */

#include "hsd/hsd_mobj.h"
#include "hsd/hsd_aobj.h"
#include "hsd/hsd_tobj.h"

/* current_mobj */
extern HSD_MObj* lbl_8047B2D4;

/* 0x801A8470 | 0x8 */
void HSD_MObjSetCurrent(HSD_MObj* mobj)
{
    lbl_8047B2D4 = mobj;
}

/* 0x801A8458 | 0x18 */
u32 HSD_MObjGetFlags(HSD_MObj* mobj)
{
    if (mobj != NULL) {
        return mobj->rendermode;
    }
    return 0;
}

/* 0x801A8440 | 0x18 */
void HSD_MObjSetFlags(HSD_MObj* mobj, u32 flags)
{
    if (mobj == NULL) {
        return;
    }
    mobj->rendermode |= flags;
}

/* 0x801A8428 | 0x18 */
void HSD_MObjClearFlags(HSD_MObj* mobj, u32 flags)
{
    if (mobj == NULL) {
        return;
    }
    mobj->rendermode &= ~flags;
}

/* 0x801A83BC | 0x6C */
void HSD_MObjAddAnim(HSD_MObj* mobj, HSD_MatAnim* matanim)
{
    if (mobj == NULL) {
        return;
    }
    if (matanim == NULL) {
        return;
    }
    if (mobj->aobj != NULL) {
        HSD_AObjRemove(mobj->aobj);
    }
    mobj->aobj = HSD_AObjLoadDesc(matanim->aobjdesc);
    HSD_TObjAddAnimAll(mobj->tobj, matanim->texanim);
}

/* 0x801A8354 | 0x68 */
void HSD_MObjReqAnimByFlags(HSD_MObj* mobj, f32 startframe, u32 flags)
{
    if (mobj == NULL) {
        return;
    }
    if (flags & MOBJ_ANIM) {
        HSD_AObjReqAnim(mobj->aobj, startframe);
    }
    HSD_TObjReqAnimAllByFlags(mobj->tobj, startframe, flags);
}

