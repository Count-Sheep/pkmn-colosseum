/**
 * @file state_801B25C4.c
 * @brief HSD state.c, 0x801B25C4 - 0x801B27DC: the colour/alpha-update
 *        setters and the state invalidation functions.
 *
 * Part of HAL's state.c (see state.c), built on its own because the whole
 * TU cannot be linked yet. The functions are in HAL source order (reverse
 * of their addresses); the state words stay in their data units.
 */
#include "hsd/hsd_state.h"

extern void HSD_ClearVtxDesc(void);

extern void fn_800BCE30(u8 enable); /* GXSetColorUpdate */
extern void fn_800BCE5C(u8 enable); /* GXSetAlphaUpdate */

/* HSD_StateSetColorUpdate */
void fn_801B278C(int enable)
{
    GXBool update = enable != 0;

    if (lbl_8047B31D != update) {
        fn_800BCE30(update);
        lbl_8047B31D = update;
    }
}

/* HSD_StateSetAlphaUpdate */
void fn_801B273C(int enable)
{
    GXBool update = enable != 0;

    if (lbl_8047B31C != update) {
        fn_800BCE5C(update);
        lbl_8047B31C = update;
    }
}

/*
 * _HSD_StateInvalidatePrimitive. Retail stores the line width twice (0, then
 * 0xFF); nothing else reads these two bytes.
 */
void fn_801B2718(void)
{
    lbl_8047B351 = 0;
    lbl_8047B350 = 0;
    lbl_8047B34C = -1;
    lbl_8047B351 = -1;
}

/* _HSD_StateInvalidateVtxAttr */
void fn_801B26F8(void)
{
    HSD_ClearVtxDesc();
}

/* _HSD_StateInvalidateRenderMode */
void fn_801B2654(void)
{
    lbl_8047B348 = -1;
    lbl_8047B344 = -1;
    lbl_8047B340 = -1;
    lbl_8047B33C = -1;
    lbl_8047B338 = -1;
    lbl_8047B334 = -1;
    lbl_8047B330 = -1;
    lbl_8047B32C = -1;
    lbl_8047B328 = 0;
    lbl_8047B324 = -1;
    lbl_8047B320 = -1;
    lbl_8047B31E = 0;
    lbl_8047B31D = -1;
    lbl_8047B31C = -1;
    lbl_8047B31B = -1;
    lbl_8047B31A = 0;
    lbl_8047B319 = -1;
    lbl_8047B318 = -1;
}

/* HSD_StateInvalidate */
void fn_801B25C4(int mask)
{
    int i;

    for (i = 0; lbl_8036CFA8[i].mask != HSD_STATE_NONE; i++) {
        if (mask & lbl_8036CFA8[i].mask) {
            lbl_8036CFA8[i].func();
        }
    }
}
