/**
 * GSmodelEnableModulation, 0x800E5BE0 - 0x800E5D40 (one-function carve).
 * Builds the material list on the first reference (modelInitMaterialList,
 * XD _modelInitMaterialList) and sets and enables the modulation colour on every material.
 */
#define GSMODEL_SUFFIX_ISOLATED
#include "src/game/gs_model_main_suffix_800E4AC0.c"
#include "game/gs_model_material_internal.h"

void GSmaterialEnableExtension(void* material, s32 mode);

void GSmodelEnableModulation(GSmodel* model, const GScolor* modulation)
{
    s32 i;
    s32 count;
    void** materials;

    if (model->modulationRefCount == 0) {
        count = modelInitMaterialList(model);
    } else {
        count = model->materialCount;
    }
    model->modulationRefCount++;
    materials = (void**)model->materialList;

    for (i = 0; i < count; i++, materials++) {
        if (*materials != NULL) {
            GSmaterialSetModulate(*materials, modulation);
            GSmaterialEnableExtension(*materials, 1);
        }
    }
}
