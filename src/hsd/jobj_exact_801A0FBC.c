/**
 * @file jobj_exact_801A0FBC.c
 * @brief HAL jobj.c: HSD_JObjLoadJoint, 0x801A0FBC - 0x801A1098.
 *
 * A single exact function carved out of the jobj.c range, built with the
 * HSD library flags (GC/1.3.2 -O4,p -O1 -inline auto,deferred
 * -use_lmw_stmw on -str reuse,readonly) and no local pragmas. The body is
 * Melee's HSD_JObjLoadJoint (jobj.c) with JObjLoadJointSub expanded, and
 * within it HSD_JObjAlloc (whose out-of-line copy is fn_8019F718, owned by
 * hsd_jobj_exact_8019F718.c), as retail does. Both are written here as
 * static inline copies of jobj.c's definitions: retail expands them at
 * several sites in jobj.c (JObjLoadJointSub here and in JObjLoad,
 * HSD_JObjAlloc in both of those), and a carve cannot define the global
 * fn_8019F718 a second time.
 *
 * Text-only unit. The data it uses is owned by other units and stays
 * extern with its address names: hsdJObj (.data 0x8036C8E0),
 * default_class (.sbss 0x8047B298), and the two assert strings, which
 * retail addresses individually through r2 (not through a pool base):
 * lbl_8047DB20 ("jobj.c", __FILE__) and lbl_8047DB3C ("jobj").
 */
#include "dolphin/types.h"
#include "hsd/hsd_class.h"
#include "sysdolphin/baselib/jobj.h"

/* class.c */
extern HSD_ClassInfo* fn_80193748(const char* class_name);
extern void* fn_80193828(HSD_ClassInfo* info);
#define hsdSearchClassInfo(n) fn_80193748(n)
#define hsdNew(i) fn_80193828(i)

extern void __assert(const char* file, u32 line, const char* expr);

extern HSD_JObjInfo lbl_8036C8E0; /* hsdJObj */
extern HSD_ClassInfo* lbl_8047B298; /* default_class */
extern char lbl_8047DB20[7]; /* "jobj.c" */
extern char lbl_8047DB3C[5]; /* "jobj" */

void HSD_JObjResolveRefsAll(HSD_JObj* jobj, HSD_Joint* joint);

static inline HSD_JObj* HSD_JObjAlloc(void)
{
    HSD_JObj* jobj = hsdNew(lbl_8047B298 != NULL
                                ? lbl_8047B298
                                : &lbl_8036C8E0.parent.parent);
    if (jobj == NULL) {
        __assert(lbl_8047DB20, 2015, lbl_8047DB3C);
    }
    return jobj;
}

static inline HSD_JObj* JObjLoadJointSub(HSD_Joint* joint, HSD_JObj* parent)
{
    HSD_JObj* jobj;
    HSD_ClassInfo* info;

    if (joint == NULL) {
        return NULL;
    }
    if (joint->class_name == NULL ||
        !(info = hsdSearchClassInfo(joint->class_name)))
    {
        jobj = HSD_JObjAlloc();
    } else {
        jobj = hsdNew(info);
        if (jobj == NULL) {
            __assert(lbl_8047DB20, 981, lbl_8047DB3C);
        }
    }
    HSD_JOBJ_METHOD(jobj)->load(jobj, joint, parent);
    return jobj;
}

HSD_JObj* HSD_JObjLoadJoint(HSD_Joint* joint)
{
    HSD_JObj* jobj = JObjLoadJointSub(joint, NULL);
    HSD_JObjResolveRefsAll(jobj, joint);
    return jobj;
}
