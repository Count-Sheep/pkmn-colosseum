/**
 * @file tobj_exact_801BE490.c
 * @brief sysdolphin tobj.c, _HSD_TObjGetCurrentByType and HSD_TObjLoadDesc:
 *        .text 0x801BE490-0x801BE598.
 *
 * Built with the sysdolphin library flags (GC/1.3.2 -O4,p -O1
 * -inline auto,deferred -use_lmw_stmw on); deferred inlining emits
 * functions in reverse definition order, so they are listed from the last
 * address down. Text-only unit carved from tobj.c: MakeTextureMtx before it
 * needs the TU's .sdata2 floats and TObjLoad after it is not exact yet.
 *
 * HSD_TObjLoadDesc carries an expansion of HSD_TObjAlloc (same assert,
 * line 2180 "new"): in tobj.c HSD_TObjAlloc is defined after it and
 * deferred auto-inlining expands it here. This unit cannot see that
 * definition, so it repeats the body as a static inline; the global
 * HSD_TObjAlloc itself is linked from tobj_exact_801BBAC8.c.
 */

#include "hsd/hsd_class.h"
#include "hsd/hsd_tobj.h"

extern HSD_ClassInfo* fn_80193748(const char* name); /* hsdSearchClassInfo */
extern void* fn_80193828(HSD_ClassInfo* info);       /* hsdNew */
extern void __assert(const char* file, u32 line, const char* expr);

extern u8 lbl_8036D3F0[];          /* hsdTObj */
extern const char lbl_8047DEB0[7]; /* "tobj.c" */
extern const char lbl_8047DECC[4]; /* "new" */
extern const char lbl_8047DF10[8]; /* "tobj" */

extern HSD_ClassInfo* lbl_8047B378; /* default_class */
extern HSD_TObj* lbl_8047B37C;      /* tobj_head */

#define hsdTObjClass HSD_CLASS_INFO(lbl_8036D3F0)
#define default_class lbl_8047B378
#define tobj_head lbl_8047B37C

static inline HSD_TObj* HSD_TObjAlloc(void)
{
    HSD_TObj* tobj =
        fn_80193828(default_class != NULL ? default_class : hsdTObjClass);
    if (tobj == NULL) {
        __assert(lbl_8047DEB0, 2180, lbl_8047DECC);
    }
    return tobj;
}

HSD_TObj* HSD_TObjLoadDesc(HSD_TObjDesc* td)
{
    if (td != NULL) {
        HSD_TObj* tobj;
        HSD_ClassInfo* info;

        if (td->class_name == NULL ||
            (info = fn_80193748(td->class_name)) == NULL)
        {
            tobj = HSD_TObjAlloc();
        } else {
            tobj = fn_80193828(info);
            if (tobj == NULL) {
                __assert(lbl_8047DEB0, 493, lbl_8047DF10);
            }
        }
        HSD_TOBJ_METHOD(tobj)->load(tobj, td);
        return tobj;
    } else {
        return NULL;
    }
}

HSD_TObj* _HSD_TObjGetCurrentByType(HSD_TObj* from, u32 mapping)
{
    HSD_TObj* tp;

    if (from == NULL) {
        tp = tobj_head;
    } else {
        tp = from->next;
    }

    for (; tp != NULL; tp = tp->next) {
        if (tobj_coord(tp) == mapping) {
            return tp;
        }
    }
    return NULL;
}
