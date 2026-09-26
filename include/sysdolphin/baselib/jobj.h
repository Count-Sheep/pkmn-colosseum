/**
 * @file jobj.h
 * @brief HAL sysdolphin jobj.h: the HSD_JObj layout and the jobj.h inlines
 *        other baselib files expand.
 *
 * The file keeps HAL's name because its inlines assert through __FILE__:
 * retail callers pool "jobj.h" and "jobj" in their own .sdata2 (robj.c at
 * 0x8047DD68/0x8047DD70). Colosseum's jobj.h is newer than Melee's; its
 * assert lines are read from the retail code (HSD_JObjMtxIsDirty 605,
 * HSD_JObjGetMtxPtr 1148).
 *
 * Layout and inline bodies follow the Melee decompilation (doldecomp/melee,
 * src/sysdolphin/baselib/jobj.h); the 0x88-byte size is Colosseum's.
 */
#ifndef SYSDOLPHIN_BASELIB_JOBJ_H
#define SYSDOLPHIN_BASELIB_JOBJ_H

#include "dolphin/mtx.h"
#include "dolphin/types.h"
#include "hsd/hsd_forward.h"
#include "sysdolphin/baselib/debug.h"
#include "sysdolphin/baselib/object.h"

/* The SDK quaternion type (Melee keeps it in dolphin/mtx.h). */
typedef struct Quaternion {
    f32 x;
    f32 y;
    f32 z;
    f32 w;
} Quaternion;

#define JOBJ_MTX_DIRTY    (1 << 6)
#define JOBJ_USER_DEF_MTX (1 << 23)

struct HSD_JObj {
    /* 0x00 */ HSD_Obj object;
    /* 0x08 */ HSD_JObj* next;
    /* 0x0C */ HSD_JObj* parent;
    /* 0x10 */ HSD_JObj* child;
    /* 0x14 */ u32 flags;
    /* 0x18 */ union {
        HSD_SList* ptcl;
        HSD_DObj* dobj;
        HSD_Spline* spline;
    } u;
    /* 0x1C */ Quaternion rotate;
    /* 0x2C */ Vec3 scale;
    /* 0x38 */ Vec3 translate;
    /* 0x44 */ Mtx mtx;
    /* 0x74 */ Vec3* scl;
    /* 0x78 */ MtxPtr envelopemtx;
    /* 0x7C */ HSD_AObj* aobj;
    /* 0x80 */ HSD_RObj* robj;
    /* 0x84 */ u32 id;
};

/* HSD_JObjSetupMatrixSub */
void fn_8019D9DC(HSD_JObj* jobj);
void HSD_JObjUnrefThis(HSD_JObj* jobj);
/* HSD_JObjMakeMatrix */
void fn_801A3600(HSD_JObj* jobj);

static inline BOOL HSD_JObjMtxIsDirty(HSD_JObj* jobj)
{
    BOOL result;
    HSD_ASSERT(605, jobj);
    result = FALSE;
    if (!(jobj->flags & JOBJ_USER_DEF_MTX) && jobj->flags & JOBJ_MTX_DIRTY) {
        result = TRUE;
    }
    return result;
}

static inline void HSD_JObjSetupMatrix(HSD_JObj* jobj)
{
    if (!jobj || !HSD_JObjMtxIsDirty(jobj)) {
        return;
    }
    fn_8019D9DC(jobj);
}

static inline MtxPtr HSD_JObjGetMtxPtr(HSD_JObj* jobj)
{
    HSD_ASSERT(1148, jobj);
    HSD_JObjSetupMatrix(jobj);
    return jobj->mtx;
}

static inline void HSD_JObjRefThis(HSD_JObj* jobj)
{
    if (jobj == NULL) {
        return;
    }
    iref_INC(jobj);
}

#endif /* SYSDOLPHIN_BASELIB_JOBJ_H */
