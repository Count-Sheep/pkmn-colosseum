/**
 * GSmodelInitMaterialAlpha, 0x800E66B8 - 0x800E6804 (one-function carve).
 * Builds the material list on the first reference (modelInitMaterialList,
 * XD _modelInitMaterialList) and stores every material's alpha.
 */
#define GSMODEL_SUFFIX_ISOLATED
#include "src/game/gs_model_main_suffix_800E4AC0.c"
#include "game/gs_model_material_internal.h"

void GSmaterialStoreAlpha(void* material);

void GSmodelInitMaterialAlpha(GSmodel* model)
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
            GSmaterialStoreAlpha(*materials);
        }
    }
}
