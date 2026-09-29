/**
 * @file jobj_exact_801A13CC.c
 * @brief HAL jobj.c: HSD_JObjDispAll (fn_801A13CC) and the functions it
 *        needs out of line, 0x801A13CC - 0x801A1B40.
 *
 * HSD_JObjDispAll is recursive; with "-inline auto" MWCC expands it into
 * itself a few levels deep, expanding JObjSetupInstanceMtx and the header
 * inlines HSD_JObjSetupMatrix / HSD_CObjGetViewingMtxPtrDirect at the
 * shallow levels and calling out-of-line copies at the deeper ones. MWCC
 * emits those copies after DispAll, then the static JObjSetupInstanceMtx:
 *   fn_801A1980  HSD_CObjGetViewingMtxPtrDirect (cobj.h)
 *   fn_801A1988  HSD_JObjSetupMatrix (jobj.h), HSD_JObjMtxIsDirty expanded
 *   fn_801A1A00  JObjSetupInstanceMtx (static in jobj.c)
 * A carve of DispAll alone cannot link: it would emit its own copies of
 * these. So this unit spans all four and replaces the former carves
 * hsd_jobj_exact_801A1980.c and jobj_exact_801A1A00.c, whose bodies it
 * keeps. Bodies are jobj.c's (src/hsd/jobj.c); the header inlines carry
 * their retail address names so objdiff pairs the out-of-line copies.
 * HSD library flags (GC/1.3.2 -O4,p -O1 -inline auto,deferred
 * -use_lmw_stmw on -str reuse,readonly), no local pragmas. Text-only.
 */
#include "dolphin/mtx.h"
#include "hsd/hsd_cobj.h"

#define iref_DEC hsd_inline_iref_DEC
#define ref_INC hsd_inline_ref_INC
#include "hsd/hsd_jobj.h"
#undef iref_DEC
#undef ref_INC

/* RULE-EXCEPTION(title-path): named stand-ins for jobj.c's pooled assert
 * literals (HSD_JObjMtxIsDirty's __FILE__ "jobj.h" and "jobj", jobj.c's
 * .sdata2, owned by hsd_sdata2_8047DB20.c) — see docs/RULE_EXCEPTIONS.md */
extern char lbl_8047DB34[7]; /* "jobj.h" */
extern char lbl_8047DB3C[5]; /* "jobj" */
extern void __assert(const char* file, u32 line, const char* expr);
/* HSD_JObjSetupMatrixSub */
extern void fn_8019D9DC(HSD_JObj* jobj);
/* HSD_JObjDisp */
extern void fn_80197344(HSD_JObj* jobj, MtxPtr vmtx, u32 flags,
                        u32 rendermode);
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

/* HSD_JObjSetupMatrix */
inline void fn_801A1988(HSD_JObj* jobj)
{
    if (jobj == NULL || !JObjMtxIsDirty(jobj)) {
        return;
    }
    fn_8019D9DC(jobj);
}

/* HSD_CObjGetViewingMtxPtrDirect */
inline f32* fn_801A1980(HSD_CObj* cobj)
{
    return (f32*) cobj->view_mtx;
}

/* JObjSetupInstanceMtx */
static void fn_801A1A00(MtxPtr vmtx, HSD_JObj* jobj, Mtx mtx)
{
    HSD_CObj* cobj;

    fn_801A1988(jobj);
    fn_801A1988(jobj->child);
    PSMTXInverse(jobj->child->mtx, mtx);
    PSMTXConcat(jobj->mtx, mtx, mtx);
    if (vmtx) {
        PSMTXConcat(vmtx, mtx, mtx);
    } else {
        cobj = HSD_CObjGetCurrent();
        if (cobj != NULL) {
            PSMTXConcat((MtxPtr) fn_801A1980(cobj), mtx, mtx);
        }
    }
}

/* HSD_JObjDispAll */
void fn_801A13CC(HSD_JObj* jobj, MtxPtr vmtx, u32 flags, u32 rendermode)
{
    if (jobj != NULL) {
        if (jobj->flags & JOBJ_INSTANCE) {
            if (!(jobj->flags & JOBJ_HIDDEN)) {
                Mtx mtx;

                fn_801A1A00(vmtx, jobj, mtx);
                fn_801A13CC(jobj->child, mtx, flags, rendermode);
            }
        } else {
            if (jobj->flags & (flags << 18)) {
                fn_80197344(jobj, vmtx, flags, rendermode);
            }
            if (jobj->flags & (flags << 28)) {
                jobj = jobj->child;
                while (jobj != NULL) {
                    fn_801A13CC(jobj, vmtx, flags, rendermode);
                    jobj = jobj->next;
                }
            }
        }
    }
}
