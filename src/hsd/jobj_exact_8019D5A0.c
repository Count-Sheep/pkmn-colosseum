/**
 * @file jobj_exact_8019D5A0.c
 * @brief HAL jobj.c: JObjInit and the two particle-callback setters,
 *        0x8019D5A0 - 0x8019D620.
 *
 * A contiguous exact run carved out of the jobj.c range while the rest of
 * the module (JObjReleaseChild, HSD_JObjSetMtxDirtySub, ...) is still being
 * matched. It is built with the HSD library flags (GC/1.3.2 -O4,p -O1
 * -inline auto,deferred -use_lmw_stmw on -str reuse,readonly) and no local
 * pragmas. Deferred inlining emits a unit in reverse definition order, so
 * the functions are written in HAL's order: the setters first.
 *
 * fn_8019D618 is Melee's HSD_JObjSetDPtclCallback: it sits between
 * HSD_JObjSetMtxDirtySub and JObjInit in HAL's order, and the callback it
 * stores (lbl_8047B2A0) is the one the JObj animation interpreter calls as
 * (0, lo, hi, ...). fn_8019D610 is a Colosseum-era addition that stores the
 * second callback, lbl_8047B2A8, which the same interpreter calls with
 * (jobj, value). The symbols keep their address names.
 */
#include "hsd/hsd_class.h"
#include "hsd/hsd_jobj.h"

extern u8 lbl_8036C8E0[];     /* hsdJObj (HSD_JObjInfo) */
extern const f32 lbl_8047DB30; /* 1.0f */
extern void* lbl_8047B2A0;    /* dptcl callback */
extern void* lbl_8047B2A8;    /* jobj/value callback */

void fn_8019D618(void* value)
{
    lbl_8047B2A0 = value;
}

void fn_8019D610(void* value)
{
    lbl_8047B2A8 = value;
}

s32 JObjInit(HSD_Class* o)
{
    s32 status =
        ((HSD_ClassInfo*) lbl_8036C8E0)->head.parent->init((HSD_Class*) o);

    if (status >= 0) {
        HSD_JObj* jobj = (HSD_JObj*) o;

        status = 0;
        jobj->flags = JOBJ_MTX_DIRTY;
        jobj->scale_x = lbl_8047DB30;
        jobj->scale_y = lbl_8047DB30;
        jobj->scale_z = lbl_8047DB30;
    }
    return status;
}
