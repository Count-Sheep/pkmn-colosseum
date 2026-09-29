#ifndef GAME_GS_MODEL_MATERIAL_INTERNAL_H
#define GAME_GS_MODEL_MATERIAL_INTERNAL_H

s32 fn_800EE0E8(void* model);
s32 GSpartGetMaterialCount(GSpart* part);
void* GSpartGetMaterial(GSpart* part, s32 index);

static inline u16 GSmodelAcquireMaterials(GSmodel* model)
{
    s32 count;
    s32 partCount;
    s32 partIndex;

    if (model->modulationRefCount == 0) {
        count = 0;
        partCount = fn_800EE0E8(model);
        for (partIndex = 0; partIndex < partCount; partIndex++) {
            GSpart* part = GSmodelGetPart(model, partIndex);
            count += GSpartGetMaterialCount(part);
            GSpartFree(part);
        }

        if (count != 0) {
            u32 handle = _toolentryAlloc__FUl(count * sizeof(void*));

            if (handle != 0) {
                void** dst = fn_800E27B0(handle);

                model->materialList = (GSmodelMaterialList*)dst;
                model->materialListHandle = handle;
                model->materialCount = count;
                for (partIndex = 0; partIndex < partCount; partIndex++) {
                    GSpart* part = GSmodelGetPart(model, partIndex);
                    s32 partMaterialCount = GSpartGetMaterialCount(part);
                    s32 materialIndex;

                    for (materialIndex = 0; materialIndex < partMaterialCount;) {
                        *dst = GSpartGetMaterial(part, materialIndex);
                        materialIndex++;
                        dst++;
                    }
                    GSpartFree(part);
                }
            } else {
                count = 0;
            }
        }
    }

    count = model->materialCount;
    model->modulationRefCount++;
    return count;
}

/*
 * Builds the model's material list on first use: counts the materials of
 * every part, allocates the pointer array and fills it. Returns the count
 * (0 when the model has none); a failed allocation still returns the count.
 * XD names it _modelInitMaterialList__FP8_GSmodel (NXXJ01.map line 6150,
 * GSmodel.o, UNUSED 0xEC: inlined everywhere; StarsMmd/Colo-XD-PBR-symbol-maps).
 * Admitted as a reconstructed helper: the same sequence is expanded in
 * GSmodelSetTextureChange and its siblings (GSmodelSetPEdescr,
 * GSmodelEnableModulation, GSmodelEnableColorSwap, GSmodelEnableEnvMap,
 * GSmodelInitMaterialAlpha, GSmodelSetRenderFlags, fn_800E584C), and its
 * return value is routed through the helper's count and copied to the
 * caller's (mr r24,r26 in GSmodelSetTextureChange), which the in-place
 * forms do not emit.
 */
static inline s32 modelInitMaterialList(GSmodel* model)
{
    void** dst;
    s32 partIndex;
    GSpart* part;
    s32 partCount;
    s32 count;
    s32 materialIndex;
    u32 handle;
    s32 partMaterialCount;

    count = 0;
    partCount = fn_800EE0E8(model);
    for (partIndex = 0; partIndex < partCount; partIndex++) {
        part = GSmodelGetPart(model, partIndex);
        count += GSpartGetMaterialCount(part);
        GSpartFree(part);
    }
    if (count == 0) {
        return 0;
    }
    handle = _toolentryAlloc__FUl(count * sizeof(void*));
    if ((u16)handle != 0) {
        dst = (void**)(model->materialList = (GSmodelMaterialList*)fn_800E27B0(handle));
        model->materialListHandle = handle;
        model->materialCount = count;
        for (partIndex = 0; partIndex < partCount; partIndex++) {
            part = GSmodelGetPart(model, partIndex);
            partMaterialCount = GSpartGetMaterialCount(part);

            for (materialIndex = 0; materialIndex < partMaterialCount; materialIndex++, dst++) {
                *dst = GSpartGetMaterial(part, materialIndex);
            }
            GSpartFree(part);
        }
    }
    return count;
}

#endif
