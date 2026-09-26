/**
 * @file hsd_range_801A69C0.c
 * @brief hsd code, 0x801A69C0 - 0x801A8428 (23 fns).
 *
 * HAL's memory.c (0x801A6928-0x801A6A34, including _HSD_MemSetCallbacks)
 * is now src/hsd/memory.c; what remains here is mobj.c code.
 *
 * Range unit assigned from the propagated subsystem map
 * (tools/subsystem_propagation.py, >=80% single-label dominance;
 * campaign 2026-07-01). All functions asm-only until matched; the
 * range name stays honest until internal TU structure is proven.
 */
#include "dolphin/types.h"
#include "hsd/hsd_aobj.h"
#include "hsd/hsd_class.h"
#include "hsd/hsd_mobj.h"
#include "hsd/hsd_tobj.h"

extern HSD_MObjInfo lbl_8036CB30; /* hsdMObj class info */
extern HSD_ClassInfo lbl_8036C638;
extern void* lbl_8047B2D0;
extern HSD_TObj* lbl_8047B2D8;
extern HSD_TObj* lbl_8047B2DC;
extern char lbl_80274E38[];
extern char lbl_80274E50[];

extern void HSD_TExpFreeTevDesc(HSD_TExpTevDesc* tevdesc);
extern void fn_801B7178(HSD_TExp* texp, u32 type, int flag);
extern void fn_80193AF0(void* mem, u32 size);
extern void HSD_MObjSetup(HSD_MObj* mobj, u32 rendermode);
extern void HSD_MObjUnset(HSD_MObj* mobj, u32 rendermode);
extern int MObjLoad(HSD_MObj* mobj, HSD_MObjDesc* desc);
extern void MObjUpdateFunc(void* obj, u32 type, HSD_ObjData* val);
void MObjAmnesia(HSD_ClassInfo* info);
void MObjRelease(HSD_Class* obj);

void MObjInfoInit(void)
{
    hsdInitClassInfo(HSD_CLASS_INFO(&lbl_8036CB30),
                     HSD_CLASS_INFO(&lbl_8036C638), lbl_80274E38,
                     lbl_80274E50, sizeof(HSD_MObjInfo), sizeof(HSD_MObj));

    HSD_CLASS_INFO(&lbl_8036CB30)->release = MObjRelease;
    HSD_CLASS_INFO(&lbl_8036CB30)->amnesia = MObjAmnesia;
    HSD_MOBJ_INFO(&lbl_8036CB30)->setup = HSD_MObjSetup;
    HSD_MOBJ_INFO(&lbl_8036CB30)->unset = HSD_MObjUnset;
    HSD_MOBJ_INFO(&lbl_8036CB30)->load = MObjLoad;
    HSD_MOBJ_INFO(&lbl_8036CB30)->make_texp = MObjMakeTExp;
    HSD_MOBJ_INFO(&lbl_8036CB30)->setup_tev = MObjSetupTev;
    HSD_MOBJ_INFO(&lbl_8036CB30)->update = MObjUpdateFunc;
}

void MObjRelease(HSD_Class* obj)
{
    HSD_MObj* mobj = HSD_MOBJ(obj);

    HSD_AObjRemove(mobj->aobj);
    fn_80193AF0(mobj->mat, sizeof(HSD_Material));
    HSD_TObjRemoveAll(mobj->tobj);
    if (mobj->tevdesc != NULL) {
        HSD_TExpFreeTevDesc(mobj->tevdesc);
    }
    if (mobj->texp != NULL) {
        fn_801B7178(mobj->texp, 7, 1);
    }
    if (mobj->pe != NULL) {
        fn_80193AF0(mobj->pe, sizeof(HSD_PEDesc));
    }
    lbl_8036CB30.parent.head.parent->release(obj);
}

/* HSD_MObjDeleteShadowTexture (0x801A6C34) is in mobj_exact_801A6C34.c. */

void MObjAmnesia(HSD_ClassInfo* info)
{
    if (info == lbl_8047B2D0) {
        lbl_8047B2D0 = 0;
    }
    if (info == (void*) &lbl_8036CB30) {
        lbl_8047B2D8 = 0;
        lbl_8047B2DC = 0;
    }
    lbl_8036CB30.parent.head.parent->amnesia(info);
}
