/**
 * GSmodelEnableColorSwap, 0x800E5E34 - 0x800E5FAC (one-function carve).
 * Builds the material list on the first reference (modelInitMaterialList,
 * XD _modelInitMaterialList) and sets and enables the colour-swap channels on every material.
 */
#define GSMODEL_SUFFIX_ISOLATED
#include "src/game/gs_model_main_suffix_800E4AC0.c"
#include "game/gs_model_material_internal.h"

void GSmaterialSetColorChannels(void* material, void* color0, void* color1,
                                void* color2, void* color3);
void GSmaterialEnableExtension(void* material, s32 mode);

void GSmodelEnableColorSwap(GSmodel* model, void* color0,
                            void* color1, void* color2, void* color3)
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
            GSmaterialSetColorChannels(*materials, color0, color1, color2,
                                       color3);
            GSmaterialEnableExtension(*materials, 2);
        }
    }
}
