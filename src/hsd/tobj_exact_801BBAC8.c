/**
 * @file tobj_exact_801BBAC8.c
 * @brief sysdolphin tobj.c, class/alloc head: .text 0x801BBAC8-0x801BBF28.
 *
 * TObjInfoInit, TObjAmnesia, TObjRelease, TObjInit (fn_801BBCE0),
 * HSD_ImageDescFree/Remove/Alloc, HSD_TObjAlloc, HSD_TObjInsert
 * (fn_801BBE3C), HSD_TObjRemoveAll and HSD_TObjRemove. The tobj.c TU runs to
 * 0x801BF098; the rest still needs its own .data jump tables and .sdata2
 * floats, or is not yet matched, and stays in the candidate units.
 *
 * Built with the sysdolphin library flags (GC/1.3.2 -O4,p -O1
 * -inline auto,deferred -use_lmw_stmw on). Deferred inlining emits
 * functions in reverse definition order, so they are listed here from the
 * last address down.
 *
 * Text-only unit: hsdTObj (.data 0x8036D3F0), the strings (.rodata /
 * .sdata2) and default_class / tobj_head (.sbss 0x8047B378/0x8047B37C) are
 * owned by other units and stay extern. TObjInfoInit stays global because
 * hsdTObj, which points at it, lives in a carved data unit; TObjLoad,
 * TObjMakeTExp, MakeTextureMtx and TObjUpdateFunc (fn_801BE85C) are defined
 * in the rest of the TU.
 */

#include "dolphin/types.h"
#include "hsd/hsd_aobj.h"
#include "hsd/hsd_class.h"
#include "hsd/hsd_debug.h"
#include "hsd/hsd_object.h"
#include "hsd/hsd_tobj.h"

extern void* fn_80193828(HSD_ClassInfo* info); /* hsdNew           */
extern void* fn_80193B10(s32 size);            /* hsdAllocMemPiece */
extern void fn_80193AF0(void* mem, s32 size);  /* hsdFreeMemPiece  */
extern void fn_801A6960(void* mem);            /* HSD_Free         */
extern void* memset(void* dst, int c, u32 n);

extern u8 lbl_8036D3F0[];   /* hsdTObj */
extern char lbl_80275638[]; /* "sysdolphin_base_library" */
extern char lbl_80275650[]; /* "hsd_tobj" */

extern const char lbl_8047DEB0[7]; /* "tobj.c" */
extern const char lbl_8047DEB8[8]; /* "idesc" */
extern const char lbl_8047DECC[4]; /* "new" */

extern HSD_ClassInfo* lbl_8047B378; /* default_class */
extern HSD_TObj* lbl_8047B37C;      /* tobj_head */

#define hsdTObjInfo HSD_TOBJ_INFO(lbl_8036D3F0)
#define hsdTObjClass HSD_CLASS_INFO(lbl_8036D3F0)
#define default_class lbl_8047B378
#define tobj_head lbl_8047B37C

void TObjInfoInit(void);
int TObjLoad(HSD_TObj* tobj, HSD_TObjDesc* td);
void MakeTextureMtx(HSD_TObj* tobj);
void TObjMakeTExp(HSD_TObj* tobj, u32 lightmap, u32 lightmap_done,
                  HSD_TExp** c, HSD_TExp** a, HSD_TExp** list);
/* TObjUpdateFunc */
void fn_801BE85C(void* obj, u32 type, HSD_ObjData* val);

static inline void HSD_TlutFree(HSD_Tlut* tlut)
{
    fn_80193AF0(tlut, sizeof(HSD_Tlut));
}

static inline void HSD_TlutRemove(HSD_Tlut* tlut)
{
    if (tlut != NULL) {
        HSD_TlutFree(tlut);
    }
}

static inline void HSD_TObjTevFree(HSD_TObjTev* tev)
{
    fn_80193AF0(tev, sizeof(HSD_TObjTev));
}

static inline void HSD_TObjTevRemove(HSD_TObjTev* tev)
{
    if (tev != NULL) {
        HSD_TObjTevFree(tev);
    }
}

void HSD_TObjRemove(HSD_TObj* tobj)
{
    hsdDelete(tobj);
}

void HSD_TObjRemoveAll(HSD_TObj* tobj)
{
    while (tobj != NULL) {
        HSD_TObj* next = tobj->next;
        hsdDelete(tobj);
        tobj = next;
    }
}

/* HSD_TObjInsert: splice `next` in behind `tobj`. */
void fn_801BBE3C(HSD_TObj* tobj, HSD_TObj* next)
{
    if (tobj == NULL || next == NULL) {
        return;
    }
    next->next = tobj->next;
    tobj->next = next;
}

HSD_TObj* HSD_TObjAlloc(void)
{
    HSD_TObj* tobj =
        fn_80193828(default_class != NULL ? default_class : hsdTObjClass);
    if (tobj == NULL) {
        __assert(lbl_8047DEB0, 2180, lbl_8047DECC);
    }
    return tobj;
}

HSD_ImageDesc* HSD_ImageDescAlloc(void)
{
    HSD_ImageDesc* idesc = fn_80193B10(sizeof(HSD_ImageDesc));
    if (idesc == NULL) {
        __assert(lbl_8047DEB0, 2295, lbl_8047DEB8);
    }
    memset(idesc, 0, sizeof(HSD_ImageDesc));
    return idesc;
}

void HSD_ImageDescRemove(HSD_ImageDesc* idesc)
{
    fn_80193AF0(idesc, sizeof(HSD_ImageDesc));
}

void HSD_ImageDescFree(HSD_ImageDesc* idesc)
{
    fn_80193AF0(idesc, sizeof(HSD_ImageDesc));
}

/* TObjInit: the class `init` method. */
int fn_801BBCE0(HSD_TObj* tobj)
{
    int result = hsdTObjClass->head.parent->init((HSD_Class*) tobj);
    if (result >= 0) {
        tobj->anim_id = TOBJ_ID_NULL;
        result = 0;
    }
    return result;
}

static void TObjRelease(HSD_Class* o)
{
    HSD_TObj* tobj = HSD_TOBJ(o);

    HSD_AObjRemove(tobj->aobj);
    HSD_TlutRemove(tobj->tlut);
    HSD_TObjTevRemove(tobj->tev);

    if (tobj->tluttbl != NULL) {
        int i;
        for (i = 0; tobj->tluttbl[i] != NULL; i++) {
            HSD_TlutRemove(tobj->tluttbl[i]);
        }
        fn_801A6960(tobj->tluttbl);
    }

    hsdTObjClass->head.parent->release(o);
}

static void TObjAmnesia(HSD_ClassInfo* info)
{
    if (info == HSD_CLASS_INFO(default_class)) {
        default_class = NULL;
    }
    if (info == hsdTObjClass) {
        tobj_head = NULL;
    }
    hsdTObjClass->head.parent->amnesia(info);
}

void TObjInfoInit(void)
{
    hsdInitClassInfo(hsdTObjClass, HSD_CLASS_INFO(&hsdObj), lbl_80275638,
                     lbl_80275650, sizeof(HSD_TObjInfo), sizeof(HSD_TObj));

    hsdTObjClass->init = (int (*)(HSD_Class*)) fn_801BBCE0;
    hsdTObjClass->release = TObjRelease;
    hsdTObjClass->amnesia = TObjAmnesia;
    hsdTObjInfo->load = TObjLoad;
    hsdTObjInfo->make_texp = TObjMakeTExp;
    hsdTObjInfo->make_mtx = MakeTextureMtx;
    hsdTObjInfo->update = fn_801BE85C;
}
