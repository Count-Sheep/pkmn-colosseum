/**
 * @file hsd_jobj_exact_801A1980.c
 * @brief HAL jobj.c: the out-of-line header-inline copies at
 *        0x801A1980 - 0x801A1A00.
 *
 * jobj.c's HSD_JObjDispAll (fn_801A13CC) stops expanding two header inlines
 * and calls out-of-line copies of them, which MWCC emits after it:
 *   fn_801A1980  HSD_CObjGetViewingMtxPtrDirect (cobj.h)
 *   fn_801A1988  HSD_JObjSetupMatrix (jobj.h), HSD_JObjMtxIsDirty expanded
 * (see the symbol list of the whole-TU candidate src/hsd/jobj.c). This carve
 * writes the two bodies as plain functions under the HSD library flags
 * (GC/1.3.2 -O4,p -O1 -inline auto,deferred -use_lmw_stmw on
 * -str reuse,readonly), no local pragmas; deferred inlining emits them in
 * reverse source order, as in jobj.c.
 *
 * Text-only unit. HSD_JObjMtxIsDirty's assert strings "jobj.h" (__FILE__)
 * and "jobj" are jobj.c's .sdata2 entries, which retail addresses
 * individually through r2 (lbl_8047DB34 / lbl_8047DB3C, owned by
 * hsd_sdata2_8047DB20.c); they stay extern, as in jobj_exact_801A0FBC.c.
 */

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

/* HSD_JObjSetupMatrix */
void fn_801A1988(HSD_JObj* jobj)
{
    if (jobj == NULL || !JObjMtxIsDirty(jobj)) {
        return;
    }
    fn_8019D9DC(jobj);
}

/* HSD_CObjGetViewingMtxPtrDirect */
f32* fn_801A1980(HSD_CObj* cobj)
{
    return (f32*) cobj->view_mtx;
}
