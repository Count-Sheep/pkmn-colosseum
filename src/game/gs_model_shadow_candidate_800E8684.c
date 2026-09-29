/**
 * modelShadowRender__FP10GSgfxLayer (0x800E8684 - 0x800E8EFC), XD shadow.o.
 */
#define PR410_GS_MODEL_SHADOW_SPLIT
#define PR410_GS_MODEL_SHADOW_PREFIX
#include "src/game/gs_model_shadow.c"

typedef struct GSshadowRect {
    GSshadowVec origin;
    GSshadowVec up_v;
    GSshadowVec right_v;
    GSshadowVec eye_v;
    GSshadowVec eye_vn;
    f32 distance;
    f32 top;
    f32 bottom;
    f32 left;
    f32 right;
    int perspective;
} GSshadowRect;

typedef struct GSshadowObj {
    void* objects; /* HSD_SList* */
    void* camera;  /* HSD_CObj* */
    u8 pad_08[0x18];
    u8 intensity; /* 0x20 */
} GSshadowObj;

typedef struct GSshadowList {
    struct GSshadowList* next;
    void* data;
} GSshadowList;

typedef struct GSshadowModel {
    u32 flags;
    u8 pad_04[0x2C];
    GSshadowVec scale; /* 0x30 */
    u8 pad_3C[0x10];
    GSshadowBound bound; /* 0x4C */
} GSshadowModel;

extern u8* lbl_8047AB74;
extern u32 lbl_8047AB78;
extern u32 lbl_8047AB80;
extern u32 lbl_8047AB84;
extern u8 lbl_8047AB94;
extern u32 lbl_8047AB90;
extern u32 lbl_8047AB8C;
extern const f32 lbl_8047CBC4; /* 0.01f */
extern const f32 lbl_8047CBC8; /* 0.0f */
extern const f32 lbl_8047CBCC; /* 30.0f */
extern const f32 lbl_8047CBD0; /* 0.1f */
extern const f32 lbl_8047CBD4; /* 3000.0f */
extern const char lbl_8047CBD8[7]; /* "shadow" */
extern const char lbl_80270E98[]; /* "shadow.h" */
extern const f32 lbl_80478AC0[]; /* NaN */
extern void* modelGetRenderJObj(GSmodel* model);
extern GSmodel* GSmodelSearchModelList(void* jobj);
extern void fn_800E3D14(GSmodel* model, GSshadowVec* out);
extern void GSvecAdd(GSshadowVec* dst, GSshadowVec* a, GSshadowVec* b);
extern void set__5GSvecFfff(GSshadowVec* dst, f32 x, f32 y, f32 z);
extern void fn_800E00AC(GSshadowVec* dst, GSshadowVec* src, f32 scalar);
extern int HSD_ViewingRectCheck(GSshadowRect* rect);
extern void fn_801B019C(GSshadowRect* rect, GSshadowVec* position, f32 top,
                        f32 bottom, f32 left, f32 right);
extern void fn_801B0408(GSshadowRect* rect, GSshadowVec* eye,
                        GSshadowVec* interest, GSshadowVec* up, int perspective);
extern void fn_801B04E0(GSshadowObj* shadow, f32 top, f32 bottom, f32 left,
                        f32 right);
extern void fn_801B06DC(GSshadowObj* shadow);
extern void fn_801B073C(GSshadowObj* shadow, void* object);
extern void fn_801B07D4(GSshadowObj* shadow, void* object);
extern void fn_801B0A98(GSshadowObj* shadow, void* lightData, f32 value);
extern void fn_801B0BD8(GSshadowObj* shadow);
extern void fn_801B0EB8(GSshadowObj* shadow);
extern void fn_801B1524(GSshadowObj* shadow, u16 width, u16 height);
extern void HSD_ShadowInit(GSshadowObj* shadow);
extern void HSD_CObjSetProjectionType(void* cobj, s32 type);
extern void HSD_CObjSetInterest(void* cobj, GSshadowVec* interest);
extern void HSD_CObjSetNear(void* cobj, f32 value);
extern void HSD_CObjSetFar(void* cobj, f32 value);
extern f32 HSD_CObjGetEyeDistance(void* cobj);
extern void HSD_CObjGetEyePosition(void* cobj, GSshadowVec* out);
extern void HSD_CObjGetUpVector(void* cobj, GSshadowVec* out);
extern void fn_8019C6FC(void);
extern void fn_8019C708(u32 arg);
extern void __assert(const char*, s32, const char*);
extern GSshadowSlot*
_modelShadowFindValidReceiveModel__FP8_GSmodelP8_GSmodelP7GSlightP7GSbound(
    GSmodel* model, GSmodel* receiveModel, GSlight* light,
    GSshadowBound* bound);
extern void
_modelShadowAddAsNewReceiver__FP8_GSmodelP8_GSmodelP7GSlightP7GSbound(
    GSmodel* model, GSmodel* receiveModel, GSlight* light,
    GSshadowBound* bound);

/* shadow.h (assert line 107). */
static inline void HSD_ShadowSetIntensity(GSshadowObj* shadow, u8 intensity)
{
    if (shadow == NULL) {
        __assert(lbl_80270E98, 107, lbl_8047CBD8);
    }
    shadow->intensity = intensity;
}

void modelShadowRender__FP10GSgfxLayer(void* layer)
{
    GSshadowSlot* slot;
    GSmodel* model;
    GSmodel* castModel;
    GSlight* light;
    GSshadowList* list;
    GSshadowRect rect;
    GSshadowVec avg;
    GSshadowVec pos;
    GSshadowVec up;
    s32 index;
    u32 i;
    u32 j;
    u32 count;
    u8 valid;
    f32 size;

    (void)layer;

    for (i = 0; i < 6; i++) {
        lbl_80401490[i].flag = 0;
        lbl_80401490[i].model = NULL;
        for (j = 0; j < 16; j++) {
            lbl_80401490[i].receivers[j] = NULL;
        }
    }

    for (i = 0; i < lbl_8047AB78; i++) {
        model = (GSmodel*)(lbl_8047AB74 + i * 0x170);
        valid = FALSE;
        if (!(*(u32*)model & 1)) {
            continue;
        }
        if ((*(u32*)model & 2) && (*(u32*)model & 0x00400000) &&
            (*(u32*)model & 0x10000000))
        {
            valid = TRUE;
        }
        if (*(u32*)model & 0x20000000) {
            valid = TRUE;
        }
        if ((*(u32*)model & 0x400) && (*(u32*)model & 2)) {
            valid = TRUE;
        }
        if (!valid) {
            continue;
        }
        for (j = 0; j < model->shadowVtxCount; j++) {
            light = model->shadowLight;
            castModel = ((GSmodel**)model->shadowVtxBuffer)[j];
            if (light == NULL) {
                continue;
            }
            if (modelShadowGetAvgScl(&((GSshadowModel*)model)->scale) <
                lbl_8047CBC4)
            {
                continue;
            }
            slot = _modelShadowFindValidReceiveModel__FP8_GSmodelP8_GSmodelP7GSlightP7GSbound(
                castModel, model, light, &((GSshadowModel*)model)->bound);
            if (slot != NULL) {
                if (modelShadowFindCastModel(slot, model) == -1) {
                    index = modelShadowFindCastModel(slot, NULL);
                    if (index == -1) {
                        _modelShadowAddAsNewReceiver__FP8_GSmodelP8_GSmodelP7GSlightP7GSbound(
                            castModel, model, light,
                            &((GSshadowModel*)model)->bound);
                    } else {
                        u32 bsize;

                        slot->receivers[index] = model;
                        bsize = modelShadowBoundToSize(
                            &((GSshadowModel*)model)->bound);
                        if (bsize < slot->minSize) {
                            slot->minSize = bsize;
                        }
                        if (bsize > slot->maxSize) {
                            slot->maxSize = bsize;
                        }
                    }
                }
            } else {
                _modelShadowAddAsNewReceiver__FP8_GSmodelP8_GSmodelP7GSlightP7GSbound(
                    castModel, model, light, &((GSshadowModel*)model)->bound);
            }
        }
    }

    for (i = 0; i < 6; i++) {
        slot = &lbl_80401490[i];
        if (slot->model == NULL) {
            fn_801B06DC(slot->obj);
            slot->flag = 0;
            continue;
        }
        if (!(*(u32*)slot->model & 1)) {
            continue;
        }
        set__5GSvecFfff(&avg, lbl_8047CBC8, lbl_8047CBC8, lbl_8047CBC8);
        count = 0;
        for (j = 0; j < 16; j++) {
            castModel = slot->receivers[j];
            if (castModel != NULL && (*(u32*)castModel & 1)) {
                fn_801B07D4(slot->obj, modelGetRenderJObj(castModel));
                fn_800E3D14(castModel, &pos);
                GSvecAdd(&avg, &avg, &pos);
                count++;
            }
        }
        if (count == 0) {
            slot->flag = 0;
            continue;
        }
        switch (slot->light->classObj->flags & 3) {
        case 1:
            HSD_CObjSetProjectionType(((GSshadowObj*)slot->obj)->camera, 3);
            break;
        case 2:
        case 3:
            HSD_CObjSetProjectionType(((GSshadowObj*)slot->obj)->camera, 2);
            break;
        }
        fn_800E00AC(&avg, &avg, count);
        HSD_CObjSetInterest(((GSshadowObj*)slot->obj)->camera, &avg);
        fn_801B0A98(slot->obj, slot->light->classObj, lbl_8047CBCC);
        HSD_CObjSetNear(((GSshadowObj*)slot->obj)->camera, lbl_8047CBD0);
        HSD_CObjSetFar(((GSshadowObj*)slot->obj)->camera, lbl_8047CBD4);
        size = HSD_CObjGetEyeDistance(((GSshadowObj*)slot->obj)->camera);
        if (size == lbl_80478AC0[0] || size < lbl_8047CBD0) {
            fn_801B073C(slot->obj, NULL);
            return;
        }
        HSD_CObjGetEyePosition(((GSshadowObj*)slot->obj)->camera, &pos);
        HSD_CObjGetUpVector(((GSshadowObj*)slot->obj)->camera, &up);
        fn_801B0408(&rect, &pos, &avg, &up,
                    (slot->light->classObj->flags & 3) != 1);
        for (list = ((GSshadowObj*)slot->obj)->objects; list != NULL;
             list = list->next)
        {
            model = GSmodelSearchModelList(list->data);
            if (model == NULL) {
                continue;
            }
            fn_800E3D14(model, &pos);
            if (count == 1) {
                size = modelShadowBoundToSize(&((GSshadowModel*)model)->bound) +
                       lbl_8047AB80;
            } else {
                size = modelShadowBoundToSize(&((GSshadowModel*)model)->bound) +
                       lbl_8047AB84;
            }
            if (size < lbl_8047CBD0) {
                size = lbl_8047CBD0;
            }
            fn_801B019C(&rect, &pos, size, -size, -size, size);
        }
        if (HSD_ViewingRectCheck(&rect) != 0) {
            fn_801B04E0(slot->obj, rect.top, rect.bottom, rect.left,
                        rect.right);
        } else {
            size = slot->maxSize + lbl_8047AB84;
            if (size < lbl_8047CBD0) {
                size = lbl_8047CBD0;
            }
            fn_801B04E0(slot->obj, size, -size, -size, size);
        }
        HSD_ShadowSetIntensity(slot->obj, 0xFF - lbl_8047AB94);
        fn_801B1524(slot->obj, lbl_8047AB90, lbl_8047AB8C);
        slot->flag = 1;
        fn_8019C708(3);
        HSD_ShadowInit(slot->obj);
        fn_801B0EB8(slot->obj);
        fn_801B0BD8(slot->obj);
        fn_8019C6FC();
        fn_801B073C(slot->obj, NULL);
    }
}
