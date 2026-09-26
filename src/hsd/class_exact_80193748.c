/**
 * @file class_exact_80193748.c
 * @brief HAL class.c: hsdSearchClassInfo (fn_80193748), hsdIsDescendantOf
 *        (fn_80193788) and hsdNew (fn_80193828), 0x80193748 - 0x801938FC.
 *
 * A contiguous exact run carved from class.c, built with the HSD library
 * flags (GC/1.3.2 -O4,p -O1 -inline auto,deferred -use_lmw_stmw on
 * -str reuse,readonly) and no local pragmas. The functions are written in
 * HAL's (Melee's) order, the reverse of their addresses, as deferred
 * inlining requires.
 *
 * ClassInfoInit is HAL's class.c helper (Melee: "if the class info is not
 * initialised, call its info_init"). It is auto-inlined into its callers and,
 * with nothing else referencing it, dead-stripped at link time (as
 * shadow.c's stripped helpers are), so it never reaches the binary.
 *
 * hsdIsDescendantOf copies its arguments into locals only after the NULL
 * checks, which is what keeps retail's register use: the checks test the
 * argument registers, then the class walk runs in r31/r30.
 *
 * ClassInfoInitGet is a reconstructed inline (it ensures the class info is
 * initialised and returns it). hsdNew carries its fingerprint: after the
 * allocation, retail copies the class info into a second callee-saved
 * register (mr r31,r29) and makes the info_init call, the obj_size load and
 * the init call through that copy, while the class_info store and the
 * destroy call keep using the argument (r29). That is the helper's return
 * value in its own home register; ClassInfoInit (void) cannot produce it,
 * nor can a local copy, which MWCC propagates away. Melee's retail
 * hsdChangeClass has the same shape (its decompilation spells it as
 * HSD_PushClassInfo followed by the init check), so it is a HAL construct.
 * The symbols keep their address names.
 */
#include "dolphin/types.h"
#include "hsd/hsd_class.h"
#include "hsd/hsd_hash.h"

extern void* memset(void* dst, int val, u32 size);
extern HSD_Hash* lbl_8047B228; /* current_hash */

void ClassInfoInit(HSD_ClassInfo* info)
{
    if ((info->head.flags & 1) == 0) {
        (*info->head.info_init)();
    }
}

static inline HSD_ClassInfo* ClassInfoInitGet(HSD_ClassInfo* info)
{
    HSD_ClassInfo* ret = info;

    if (!(ret->head.flags & 1)) {
        ret->head.info_init();
    }
    return ret;
}

void* fn_80193828(HSD_ClassInfo* info)
{
    HSD_ClassInfo* ci;
    HSD_Class* cls;

    ClassInfoInit(info);
    cls = info->alloc(info);
    if (cls == NULL) {
        return NULL;
    }
    ci = ClassInfoInitGet(info);
    memset(cls, 0, ci->head.obj_size);
    cls->class_info = info;
    if (ci->init(cls) < 0) {
        info->destroy(cls);
        return NULL;
    }
    return cls;
}

BOOL fn_80193788(void* info, void* p)
{
    HSD_ClassInfo* c;
    HSD_ClassInfo* parent;

    if (info == NULL || p == NULL) {
        return FALSE;
    }
    c = info;
    parent = p;
    ClassInfoInit(c);
    ClassInfoInit(parent);
    while (c != NULL) {
        if (c == parent) {
            return TRUE;
        }
        c = c->head.parent;
    }
    return FALSE;
}

HSD_ClassInfo* fn_80193748(const char* class_name)
{
    if (lbl_8047B228 != NULL) {
        return HSD_HashSearch(lbl_8047B228, (void*) class_name, 0);
    }
    return NULL;
}
