/**
 * @file gs_part.c
 * @brief GSpart (model part/joint-subtree accessors)
 *
 * Split from gs_range_800E202C.c (0x800EE150-0x800EE928) — one XD source unit per
 * segment (Fable re-split, 2026-07-07). Functions asm-only until matched.
 */
#include "dolphin/types.h"
#include "hsd/hsd_dobj.h"
#include "hsd/hsd_jobj.h"

typedef struct GSpart {
    u8 inUse;
    u8 _pad;
    u16 index;
    void *model;
    void *jobj;
} GSpart;

extern GSpart *lbl_8047ABBC;
extern u32 lbl_8047ABC0;
extern void *lbl_8047ABA8;
extern u32 lbl_8047ABAC;
extern u32 lbl_8047ABB0;
extern u8 lbl_8047ABC4;
extern u32 lbl_8047ABC8;
extern s32 lbl_8047ABCC;
extern void* lbl_8047ABD0;

extern void* modelGetRenderJObj(void* model);
extern void fn_801A3918(void* root, void (*callback)(void*), u32 flags);
extern void* GSmaterialCreate(void);
extern void GSlogWrite(const char* format, ...);
extern char lbl_80270F10[];
extern char lbl_80270F44[];

void fn_800EE20C(void* jobj);
GSpart* GSpartCreate(void);
void _partFindIndexCB__FP9_HSD_JObjPPvi(void* jobj);

GSpart* GSmodelGetPart(void* model, s32 index)
{
    s32 traversalIndex = index;
    void* jobj;
    GSpart* part;

    if (*(u32*)model & 0x20000) {
        traversalIndex++;
    }

    lbl_8047ABAC = traversalIndex;
    lbl_8047ABB0 = 0;
    lbl_8047ABA8 = NULL;
    jobj = modelGetRenderJObj(model);
    if (traversalIndex != 0) {
        fn_801A3918(jobj, fn_800EE20C, 0);
        if (lbl_8047ABA8 == NULL) {
            return NULL;
        }
    } else {
        lbl_8047ABA8 = jobj;
    }

    part = GSpartCreate();
    if (part == NULL) {
        return NULL;
    }
    part->model = model;
    part->jobj = lbl_8047ABA8;
    part->index = index;
    return part;
}

void fn_800EE20C(void *jobj)
{
    if (lbl_8047ABB0++ == lbl_8047ABAC) {
        lbl_8047ABA8 = jobj;
    }
}

u32 GSpartGetJObjIndex(void* jobj, void* root)
{
    if (jobj == root) {
        return 0;
    }

    lbl_8047ABC4 = 0;
    lbl_8047ABD0 = root;
    lbl_8047ABC8 = 0;
    lbl_8047ABCC = -1;
    fn_801A3918(jobj, _partFindIndexCB__FP9_HSD_JObjPPvi, 0);
    return lbl_8047ABCC;
}

void fn_800EE288(GSpart* part)
{
    u8* model = part->model;
    s32 count = 4;

    while (count-- != 0) {
        if (*(u32*)(model + 0xE8) == part->index) {
            *(u32*)(model + 0xE4) = 0;
            *(u32*)(model + 0xE8) = -1;
            return;
        }
    }
}

typedef struct GSpartRotationSlot {
    s32 callback;
    u32 partIndex;
    void* userData;
} GSpartRotationSlot;

static inline GSpartRotationSlot* GSpartFindRotationSlot(GSpart* part)
{
    u8* model = part->model;
    GSpartRotationSlot* slot;
    u32 i;

    slot = (GSpartRotationSlot*)(model + 0xE4);
    for (i = 0; i < 4; i++, slot++) {
        if (slot->partIndex == part->index) {
            GSlogWrite(lbl_80270F10);
            return NULL;
        }
    }

    slot = (GSpartRotationSlot*)(model + 0xE4);
    for (i = 0; i < 4; i++, slot++) {
        if (slot->callback == 0) {
            return slot;
        }
    }
    GSlogWrite(lbl_80270F44);
    return NULL;
}

void GSpartRegisterRotation(GSpart* part, void* userData, void* callback)
{
    GSpartRotationSlot* slot;

    if (callback == NULL) {
        return;
    }

    slot = GSpartFindRotationSlot(part);
    if (slot != NULL) {
        slot->callback = (s32)callback;
        slot->partIndex = part->index;
        slot->userData = userData;
    }
}

typedef struct GSpartVec {
    f32 x;
    f32 y;
    f32 z;
} GSpartVec;

extern HSD_JObj* lbl_804018B0[16]; /* parent chain scratch */
extern char lbl_80270F78[];
extern void GSmodelForceAnimTransformUpdate(void*);
extern u8 GSmodelIsBlending(void*);
extern void set__5GSvecFfff(GSpartVec*, f32, f32, f32);
extern void GSvecAdd(GSpartVec*, const GSpartVec*, const GSpartVec*);
extern void fn_800E0108(GSpartVec*, const GSpartVec*, const GSpartVec*);
extern void GSvecCopy(GSpartVec*, const GSpartVec*);
extern void __assert(const char* file, u32 line, const char* condition);
extern const char lbl_8047CCA0[] __attribute__((section(".sdata2"))); /* HSD jobj.h assert file */
extern const char lbl_8047CCA8[] __attribute__((section(".sdata2"))); /* "jobj" */
extern const f32 lbl_8047CCB0; /* 0.0f */
extern const f32 lbl_8047CCB4; /* 1.0f */

/*
 * HSD jobj.h accessors as retail expands them (their assert lines are
 * 605, 748/762/776 and 899/911/923). They are spelled out here because the
 * header versions build their assert strings from __FILE__.
 */
static inline BOOL partJObjMtxIsDirty(HSD_JObj* jobj)
{
    BOOL result;

    if (jobj == NULL) {
        __assert(lbl_8047CCA0, 605, lbl_8047CCA8);
    }
    result = FALSE;
    if (!(jobj->flags & JOBJ_USER_DEF_MTX) && (jobj->flags & JOBJ_MTX_DIRTY)) {
        result = TRUE;
    }
    return result;
}

static inline void partJObjSetupMatrix(HSD_JObj* jobj)
{
    if (jobj == NULL || !partJObjMtxIsDirty(jobj)) {
        return;
    }
    fn_8019D9DC(jobj);
}

static inline f32 partJObjGetRotationX(HSD_JObj* jobj)
{
    if (jobj == NULL) {
        __assert(lbl_8047CCA0, 748, lbl_8047CCA8);
    }
    return jobj->rotate_x;
}

static inline f32 partJObjGetRotationY(HSD_JObj* jobj)
{
    if (jobj == NULL) {
        __assert(lbl_8047CCA0, 762, lbl_8047CCA8);
    }
    return jobj->rotate_y;
}

static inline f32 partJObjGetRotationZ(HSD_JObj* jobj)
{
    if (jobj == NULL) {
        __assert(lbl_8047CCA0, 776, lbl_8047CCA8);
    }
    return jobj->rotate_z;
}

static inline f32 partJObjGetScaleX(HSD_JObj* jobj)
{
    if (jobj == NULL) {
        __assert(lbl_8047CCA0, 899, lbl_8047CCA8);
    }
    return jobj->scale_x;
}

static inline f32 partJObjGetScaleY(HSD_JObj* jobj)
{
    if (jobj == NULL) {
        __assert(lbl_8047CCA0, 911, lbl_8047CCA8);
    }
    return jobj->scale_y;
}

static inline f32 partJObjGetScaleZ(HSD_JObj* jobj)
{
    if (jobj == NULL) {
        __assert(lbl_8047CCA0, 923, lbl_8047CCA8);
    }
    return jobj->scale_z;
}

void GSpartGetTransform(GSpart* part, GSpartVec* positionOut,
                        GSpartVec* rotationOut, GSpartVec* scaleOut)
{
    HSD_JObj* jobj = part->jobj;
    GSpartVec position;
    GSpartVec rotation;
    GSpartVec rotate;
    GSpartVec scale;
    GSpartVec scaling;
    u32 count = 0;

    GSmodelForceAnimTransformUpdate(part->model);
    if (!GSmodelIsBlending(part->model)) {
        partJObjSetupMatrix(jobj);
        set__5GSvecFfff(&position, jobj->mtx[0][3], jobj->mtx[1][3], jobj->mtx[2][3]);
    } else if (jobj->flags & JOBJ_JOINT) {
        GSvecCopy(&position, (GSpartVec*)&jobj->translate_x);
    } else {
        set__5GSvecFfff(&position, jobj->mtx[0][3], jobj->mtx[1][3], jobj->mtx[2][3]);
    }

    if (rotationOut != NULL || scaleOut != NULL) {
        while (jobj != NULL) {
            lbl_804018B0[count++] = jobj;
            jobj = HSD_JObjGetParent(jobj);
            if (count >= 16) {
                GSlogWrite(lbl_80270F78);
                jobj = NULL;
            }
        }

        if (rotationOut != NULL) {
            u32 i = count;

            set__5GSvecFfff(&rotation, lbl_8047CCB0, lbl_8047CCB0, lbl_8047CCB0);
            for (; i != 0; i--) {
                HSD_JObj* node = lbl_804018B0[i - 1];

                rotate.x = partJObjGetRotationX(node);
                rotate.y = partJObjGetRotationY(node);
                rotate.z = partJObjGetRotationZ(node);
                GSvecAdd(&rotation, &rotation, &rotate);
            }
        }

        if (scaleOut != NULL) {
            u32 i = count;

            set__5GSvecFfff(&scale, lbl_8047CCB4, lbl_8047CCB4, lbl_8047CCB4);
            for (; i != 0; i--) {
                HSD_JObj* node = lbl_804018B0[i - 1];

                scaling.x = partJObjGetScaleX(node);
                scaling.y = partJObjGetScaleY(node);
                scaling.z = partJObjGetScaleZ(node);
                fn_800E0108(&scale, &scale, &scaling);
            }
        }
    }

    if (positionOut != NULL) {
        GSvecCopy(positionOut, &position);
    }
    if (rotationOut != NULL) {
        GSvecCopy(rotationOut, &rotation);
    }
    if (scaleOut != NULL) {
        GSvecCopy(scaleOut, &scale);
    }
}

static inline HSD_DObj* partGetDObj(GSpart* part)
{
    HSD_JObj* jobj = part->jobj;

    if (union_type_dobj(jobj)) {
        return jobj->u.dobj;
    }
    return NULL;
}

/*
 * Walks the DObj list: returns the index-th material and reports the list
 * length through count. The target expands it in GSpartGetMaterial and in
 * GSpartGetMaterialCount (index -1, with the NULL test on &count intact).
 */
static inline HSD_MObj* partFindMaterial(HSD_DObj* dobj, u32 index, u32* count)
{
    u32 i = 0;

    while (dobj != NULL) {
        if (i++ == index) {
            if (count != NULL) {
                *count = index;
            }
            return dobj->mobj;
        }
        dobj = dobj->next;
    }
    if (count != NULL) {
        *count = i;
    }
    return NULL;
}

void* GSpartGetMaterial(GSpart* part, u32 index)
{
    HSD_DObj* dobj = partGetDObj(part);
    HSD_MObj* mobj;
    void* material;

    if (dobj == NULL) {
        return NULL;
    }
    mobj = partFindMaterial(dobj, index, NULL);
    if (mobj == NULL) {
        return NULL;
    }

    material = GSmaterialCreate();
    if (material != NULL) {
        *(HSD_MObj**)((u8*)material + 8) = mobj;
    }
    return material;
}

u32 GSpartGetMaterialCount(GSpart* part)
{
    HSD_DObj* dobj = partGetDObj(part);
    u32 count;

    if (dobj == NULL) {
        return 0;
    }
    partFindMaterial(dobj, -1, &count);
    return count;
}

u8 fn_800EE7E0(GSpart* part)
{
    u8* jobj = part->jobj;
    u8* material;
    u32 value;

    if (!union_type_dobj((HSD_JObj*)jobj)) {
        value = 0;
    } else {
        material = *(u8**)(jobj + 0x18);
        if (material == NULL) {
            value = 0;
        } else {
            value = *(u32*)(material + 0xC);
        }
    }
    return -value == 0;
}

void GSpartFree(GSpart *part)
{
    part->inUse = 0;
}

static inline GSpart *GSpartFindFree(void)
{
    GSpart *part;
    u32 i;

    part = lbl_8047ABBC;
    for (i = 0; i < lbl_8047ABC0; i++, part++) {
        if (part->inUse == 0) {
            return part;
        }
    }
    return NULL;
}

GSpart *GSpartCreate(void)
{
    GSpart *part;

    part = GSpartFindFree();
    if (part == NULL) {
        return NULL;
    }
    part->inUse = 1;
    return part;
}

extern u16 lbl_8047ABB8;
extern u32 _toolentryAlloc__FUl(u32 size);
extern void* fn_800E27B0(u16 handle);

void GSpartInit(u32 count)
{
    u32 handle;
    u32 i;

    lbl_8047ABC0 = count;
    handle = _toolentryAlloc__FUl(count * sizeof(GSpart));
    lbl_8047ABB8 = handle;
    if ((u16)handle != 0) {
        lbl_8047ABBC = fn_800E27B0((u16)handle);
        for (i = 0; i < lbl_8047ABC0; i++) {
            ((u8*)lbl_8047ABBC)[i * sizeof(GSpart)] = 0;
        }
    }
}

void _partFindIndexCB__FP9_HSD_JObjPPvi(void* jobj)
{
    if (lbl_8047ABC4 == 1) {
        return;
    }
    if (lbl_8047ABD0 == jobj) {
        lbl_8047ABCC = lbl_8047ABC8;
        lbl_8047ABC4 = 1;
    }
    lbl_8047ABC8++;
}
