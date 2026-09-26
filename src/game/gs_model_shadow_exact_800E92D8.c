/**
 * @file gs_model_shadow_exact_800E92D8.c
 * @brief modelShadowInit__Fv (0x800E92D8 - 0x800E9358).
 *
 * The source is gs_model_shadow.c. The shadow distance constant it loads,
 * lbl_8047CBC8 (0.0f in .sdata2), is declared const: MWCC then schedules
 * the load ahead of the prologue stores as retail does.
 */
#define PR410_GS_MODEL_SHADOW_SPLIT
#define PR410_GS_MODEL_SHADOW_INIT
#include "src/game/gs_model_shadow.c"
