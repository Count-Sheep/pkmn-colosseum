/**
 * @file mobj_exact_801A6A34.c
 * @brief sysdolphin mobj.c: MObjInfoInit, MObjAmnesia and MObjRelease,
 *        .text 0x801A6A34-0x801A6C34.
 *
 * mobj.c runs from MObjInfoInit (0x801A6A34) to 0x801A8478. These are the
 * last three functions of HAL's (Melee's) mobj.c; deferred inlining emits the
 * TU in reverse definition order, which puts them first, followed by
 * HSD_MObjDeleteShadowTexture (0x801A6C34, mobj_exact_801A6C34.c). They were
 * filed in the hsd_memory_r58 residual units only because those covered the
 * whole memory.c/mobj.c boundary range.
 *
 * Text-only unit built with the sysdolphin library flags (GC/1.3.2 -O4,p -O1
 * -inline auto,deferred -use_lmw_stmw on -str reuse,readonly), like the rest
 * of mobj.c's exact units. hsdMObj (.data), hsdClass, the class-name strings
 * (.rodata) and the .sbss globals are owned by other units and stay extern.
 *
 * Colosseum's MObjInfoInit also installs MObjUpdateFunc as the class's
 * animation update and the unset method, which Melee's version does not have.
 * The symbols keep their address names: lbl_8036CB30 is hsdMObj,
 * lbl_8036C638 hsdClass, lbl_8047B2D0 default_class, lbl_8047B2D8 tobj_toon,
 * lbl_8047B2DC tobj_shadows, lbl_80274E38 "sysdolphin_base_library" and
 * lbl_80274E50 "hsd_mobj".
 */
#include "dolphin/types.h"
#include "hsd/hsd_aobj.h"
#include "hsd/hsd_class.h"
#include "hsd/hsd_mobj.h"
#include "hsd/hsd_texp.h"
#include "hsd/hsd_tobj.h"

extern HSD_MObjInfo lbl_8036CB30; /* hsdMObj */
extern HSD_ClassInfo lbl_8036C638; /* hsdClass */
extern HSD_MObjInfo* lbl_8047B2D0; /* default_class */
extern HSD_TObj* lbl_8047B2D8; /* tobj_toon */
extern HSD_TObj* lbl_8047B2DC; /* tobj_shadows */
extern char lbl_80274E38[]; /* "sysdolphin_base_library" */
extern char lbl_80274E50[]; /* "hsd_mobj" */

extern void fn_80193AF0(void* mem, s32 size); /* hsdFreeMemPiece */
extern void MObjUpdateFunc(void* obj, u32 type, HSD_ObjData* val);

void MObjRelease(HSD_Class* o)
{
    HSD_MObj* mobj = HSD_MOBJ(o);

    HSD_AObjRemove(mobj->aobj);
    fn_80193AF0(mobj->mat, sizeof(HSD_Material));
    HSD_TObjRemoveAll(mobj->tobj);

    if (mobj->tevdesc != NULL) {
        HSD_TExpFreeTevDesc(mobj->tevdesc);
    }
    if (mobj->texp != NULL) {
        fn_801B7178(mobj->texp, HSD_TE_ALL, 1);
    }
    if (mobj->pe != NULL) {
        fn_80193AF0(mobj->pe, sizeof(HSD_PEDesc));
    }
    HSD_PARENT_INFO(&lbl_8036CB30)->release(o);
}

void MObjAmnesia(HSD_ClassInfo* info)
{
    if (info == HSD_CLASS_INFO(lbl_8047B2D0)) {
        lbl_8047B2D0 = NULL;
    }
    if (info == HSD_CLASS_INFO(&lbl_8036CB30)) {
        lbl_8047B2D8 = NULL;
        lbl_8047B2DC = NULL;
    }
    HSD_PARENT_INFO(&lbl_8036CB30)->amnesia(info);
}

void MObjInfoInit(void)
{
    hsdInitClassInfo(HSD_CLASS_INFO(&lbl_8036CB30), &lbl_8036C638,
                     lbl_80274E38, lbl_80274E50, sizeof(HSD_MObjInfo),
                     sizeof(HSD_MObj));

    HSD_CLASS_INFO(&lbl_8036CB30)->release = MObjRelease;
    HSD_CLASS_INFO(&lbl_8036CB30)->amnesia = MObjAmnesia;
    HSD_MOBJ_INFO(&lbl_8036CB30)->setup = HSD_MObjSetup;
    HSD_MOBJ_INFO(&lbl_8036CB30)->unset = HSD_MObjUnset;
    HSD_MOBJ_INFO(&lbl_8036CB30)->load = MObjLoad;
    HSD_MOBJ_INFO(&lbl_8036CB30)->make_texp = MObjMakeTExp;
    HSD_MOBJ_INFO(&lbl_8036CB30)->setup_tev = MObjSetupTev;
    HSD_MOBJ_INFO(&lbl_8036CB30)->update = MObjUpdateFunc;
}
