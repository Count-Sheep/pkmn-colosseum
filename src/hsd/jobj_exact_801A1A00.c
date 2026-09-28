/**
 * @file jobj_exact_801A1A00.c
 * @brief HAL jobj.c: JObjSetupInstanceMtx (fn_801A1A00), 0x801A1A00 -
 *        0x801A1B40.
 *
 * The viewing matrix of an instance joint: the instance's matrix relative
 * to the instanced tree's root, in front of vmtx or else the current
 * camera's viewing matrix. In jobj.c it is a static helper that
 * HSD_JObjDispAll (fn_801A13CC) expands at the two instance levels it
 * inlines and calls out of line for the deeper ones; this carve is that
 * out-of-line copy, written with jobj.c's body under the HSD library flags
 * (GC/1.3.2 -O4,p -O1 -inline auto,deferred -use_lmw_stmw on
 * -str reuse,readonly), no local pragmas. HSD_JObjSetupMatrix (with
 * HSD_JObjMtxIsDirty) and HSD_CObjGetViewingMtxPtrDirect are fully
 * expanded here, as in retail.
 *
 * Text-only unit. HSD_JObjMtxIsDirty's assert strings "jobj.h" (__FILE__)
 * and "jobj" are jobj.c's .sdata2 entries, which retail addresses
 * individually through r2 (lbl_8047DB34 / lbl_8047DB3C, owned by
 * hsd_sdata2_8047DB20.c); they stay extern, as in jobj_exact_801A0FBC.c
 * and hsd_jobj_exact_801A1980.c.
 */
#include "dolphin/mtx.h"
#include "hsd/hsd_cobj.h"

#define iref_DEC hsd_inline_iref_DEC
#define ref_INC hsd_inline_ref_INC
#include "hsd/hsd_jobj.h"
#undef iref_DEC
#undef ref_INC

extern char lbl_8047DB34[7]; /* "jobj.h" */
extern char lbl_8047DB3C[5]; /* "jobj" */
extern void __assert(const char* file, u32 line, const char* expr);
/* HSD_JObjSetupMatrixSub */
extern void fn_8019D9DC(HSD_JObj* jobj);
extern void PSMTXInverse(const Mtx src, Mtx inv);
extern void PSMTXConcat(const Mtx a, const Mtx b, Mtx ab);

static inline s32 JObjMtxIsDirty(HSD_JObj* jobj)
{
    s32 dirty;

    if (jobj == NULL) {
        __assert(lbl_8047DB34, 0x25D, lbl_8047DB3C);
    }
    dirty = 0;
    if (!(jobj->flags & JOBJ_USER_DEF_MTX) &&
        (jobj->flags & JOBJ_MTX_DIRTY))
    {
        dirty = 1;
    }
    return dirty;
}

static inline void JObjSetupMatrix(HSD_JObj* jobj)
{
    if (jobj == NULL || !JObjMtxIsDirty(jobj)) {
        return;
    }
    fn_8019D9DC(jobj);
}

/* JObjSetupInstanceMtx */
void fn_801A1A00(MtxPtr vmtx, HSD_JObj* jobj, Mtx mtx)
{
    HSD_CObj* cobj;

    JObjSetupMatrix(jobj);
    JObjSetupMatrix(jobj->child);
    PSMTXInverse(jobj->child->mtx, mtx);
    PSMTXConcat(jobj->mtx, mtx, mtx);
    if (vmtx) {
        PSMTXConcat(vmtx, mtx, mtx);
    } else {
        cobj = HSD_CObjGetCurrent();
        if (cobj != NULL) {
            PSMTXConcat((MtxPtr) HSD_CObjGetViewingMtxPtrDirect(cobj), mtx,
                        mtx);
        }
    }
}
