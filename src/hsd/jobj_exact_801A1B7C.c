/**
 * @file jobj_exact_801A1B7C.c
 * @brief HAL jobj.c: JObjAnimAll (fn_801A1B7C) and HSD_JObjAnim
 *        (fn_801A1F2C), 0x801A1B7C - 0x801A20C8.
 *
 * Bodies are jobj.c's (src/hsd/jobj.c), under the HSD library flags
 * (GC/1.3.2 -O4,p -O1 -inline auto,deferred -use_lmw_stmw on
 * -str reuse,readonly), no local pragmas. JObjAnimAll expands HSD_JObjAnim
 * and itself several levels deep; HSD_JObjAnim expands HSD_JObjCheckDepend,
 * which expands HSD_JObjMtxIsDirty. Where MWCC's auto-inline budget runs
 * out the code calls jobj.c's out-of-line copies, HSD_JObjCheckDepend
 * (fn_801A3D04, linked in hsd_jobj_exact_801A301C.c) and HSD_JObjMtxIsDirty
 * (fn_8019D980, linked in hsd_jobj_r51_8019D620_o2.c).
 *
 * To expand them the carve must define both. As non-static inlines MWCC
 * emits them as weak copies in this object; the linker keeps the strong /
 * first definitions already linked at 0x801A3D04 and 0x8019D980 and
 * discards these, so the linked range holds only the two functions
 * (main.dol SHA-1 OK). That is carve-only output, so it is tagged below.
 */
#include "dolphin/mtx.h"
#include "dolphin/types.h"
#include "hsd/hsd_aobj.h"
#include "hsd/hsd_class.h"
#include "hsd/hsd_dobj.h"
#include "hsd/hsd_robj.h"
#include "sysdolphin/baselib/debug.h"
#include "sysdolphin/baselib/jobj.h"

extern void fn_801B0040(HSD_RObj* robj);
#define HSD_RObjAnimAll fn_801B0040

/* RULE-EXCEPTION(title-path): named stand-ins for jobj.c's pooled assert
 * literals ("jobj.h", "jobj") — see docs/RULE_EXCEPTIONS.md */
extern char lbl_8047DB34;
extern char lbl_8047DB3C;

/* RULE-EXCEPTION(title-path): carve-only weak copies of jobj.c's
 * out-of-line HSD_JObjMtxIsDirty / HSD_JObjCheckDepend, discarded at link in
 * favour of the linked fn_8019D980 / fn_801A3D04 — see
 * docs/RULE_EXCEPTIONS.md */
/* HSD_JObjMtxIsDirty */
inline BOOL fn_8019D980(HSD_JObj* jobj)
{
    if (jobj == NULL) {
        __assert(&lbl_8047DB34, 605, &lbl_8047DB3C);
    }
    return !(jobj->flags & JOBJ_USER_DEF_MTX) &&
           (jobj->flags & JOBJ_MTX_DIRTY);
}

/* HSD_JObjCheckDepend */
inline void fn_801A3D04(HSD_JObj* jobj)
{
    if (jobj == NULL || fn_8019D980(jobj)) {
        return;
    }
    if (jobj->flags & JOBJ_USER_DEF_MTX) {
        if (!(jobj->flags & JOBJ_MTX_INDEP_PARENT) && jobj->parent != NULL &&
            fn_8019D980(jobj->parent))
        {
            jobj->flags |= JOBJ_MTX_DIRTY;
        }
    } else if ((jobj->parent != NULL &&
                (jobj->parent->flags & JOBJ_MTX_DIRTY)) ||
               (jobj->flags & JOBJ_EFFECTOR) == JOBJ_JOINT1 ||
               (jobj->flags & JOBJ_EFFECTOR) == JOBJ_JOINT2 ||
               (jobj->flags & JOBJ_EFFECTOR) == JOBJ_EFFECTOR ||
               jobj->robj != NULL)
    {
        jobj->flags |= JOBJ_MTX_DIRTY;
    }
}

/* HSD_JObjAnim */
void fn_801A1F2C(HSD_JObj* jobj)
{
    if (jobj != NULL) {
        fn_801A3D04(jobj);
        HSD_AObjInterpretAnim(jobj->aobj, jobj, HSD_JOBJ_METHOD(jobj)->update);
        HSD_RObjAnimAll(jobj->robj);
        if (union_type_dobj(jobj)) {
            HSD_DObjAnimAll(jobj->u.dobj);
        }
    }
}

/* JObjAnimAll */
void fn_801A1B7C(HSD_JObj* jobj)
{
    HSD_JObj* child;
    if (jobj != NULL) {
        fn_801A1F2C(jobj);
        if (!(jobj->flags & JOBJ_INSTANCE)) {
            child = jobj->child;
            while (child != NULL) {
                fn_801A1B7C(child);
                child = child->next;
            }
        }
    }
}
