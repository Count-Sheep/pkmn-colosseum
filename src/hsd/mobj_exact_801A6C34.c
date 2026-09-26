/**
 * @file mobj_exact_801A6C34.c
 * @brief sysdolphin mobj.c: HSD_MObjDeleteShadowTexture,
 *        .text 0x801A6C34-0x801A6CA4.
 *
 * mobj.c runs from MObjInfoInit (0x801A6A34) to 0x801A8478. In Melee's
 * mobj.c HSD_MObjDeleteShadowTexture follows HSD_MObjAddShadowTexture
 * (0x801A6CA4, mobj_exact_801A6CA4.c) and precedes MObjRelease
 * (0x801A6B8C); deferred inlining emits the TU in reverse, which puts it
 * between the two. It was filed in the hsd_memory_r58 residual units only
 * because they covered the whole memory.c/mobj.c boundary range.
 *
 * Text-only unit built with the sysdolphin library flags; tobj_shadows
 * (.sbss) is owned elsewhere and stays extern.
 */
#include "hsd/hsd_tobj.h"

/* HAL: tobj_shadows, the shadow textures every material samples */
extern HSD_TObj* lbl_8047B2DC;

void HSD_MObjDeleteShadowTexture(HSD_TObj* tobj)
{
    if (tobj != NULL) {
        HSD_TObj** cur = &lbl_8047B2DC;

        while (*cur != NULL) {
            if (*cur == tobj) {
                *cur = tobj->next;
                tobj->next = NULL;
                return;
            }
            cur = &(*cur)->next;
        }
    } else {
        HSD_TObj* next;

        while (lbl_8047B2DC != NULL) {
            next = lbl_8047B2DC->next;
            lbl_8047B2DC->next = NULL;
            lbl_8047B2DC = next;
        }
    }
}
