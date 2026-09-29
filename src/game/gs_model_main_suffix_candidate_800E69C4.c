/**
 * GSmodelSetRenderFlags, 0x800E69C4 - 0x800E6B20 (one-function carve).
 * Builds the material list on the first reference (modelInitMaterialList,
 * XD _modelInitMaterialList) and ORs the render flags into every material.
 */
#define GSMODEL_SUFFIX_ISOLATED
#include "src/game/gs_model_main_suffix_800E4AC0.c"
#include "game/gs_model_material_internal.h"

s32 GSmaterialGetFlags(void* material);
void GSmaterialSetFlags(void* material, s32 flags);

void GSmodelSetRenderFlags(GSmodel* model, s32 flags)
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
            GSmaterialSetFlags(*materials,
                               flags | GSmaterialGetFlags(*materials));
        }
    }
}
