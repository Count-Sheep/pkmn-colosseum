/**
 * @file state_801B2878.c
 * @brief HSD state.c, 0x801B2878 - 0x801B294C: material colour/shininess and
 *        the cull-mode setter.
 *
 * Part of HAL's state.c (see state.c), built on its own because the whole
 * TU cannot be linked yet. The functions are in HAL source order (reverse
 * of their addresses). This part owns state.c's .sdata2 floats
 * (0x8047DE50); the material state stays in its data unit.
 */
#include "hsd/hsd_state.h"

extern void fn_800B94F0(int mode); /* GXSetCullMode */

/* HSD_SetMaterialColor */
void fn_801B28C8(GXColor ambient, GXColor diffuse, GXColor specular, f32 alpha)
{
    lbl_80465710.ambient = ambient;
    lbl_80465710.diffuse = diffuse;
    lbl_80465710.specular = specular;
    if (alpha <= 0.0F) {
        alpha = 0.0F;
    } else if (alpha >= 1.0F) {
        alpha = 1.0F;
    }
    lbl_80465710.alpha = 255.0F * alpha;
}

/* HSD_SetMaterialShininess */
void fn_801B28B8(f32 shininess)
{
    lbl_80465710.shininess = shininess;
}

/* HSD_StateSetCullMode */
void fn_801B2878(int mode)
{
    if (lbl_8047B34C != mode) {
        fn_800B94F0(mode);
        lbl_8047B34C = mode;
    }
}
