/**
 * @file wazaSequenceCamera_exact_801D2F94.c
 * @brief fn_801D2F94 / fn_801D301C / fn_801D3034, 0x801D2F94 - 0x801D30BC.
 *
 * Function-boundary carve of the waza camera TU (see wazaSequenceCamera.c):
 * no jump table, no pooled constant; its data is the .sbss camera state
 * (lbl_8047B3EC/F0/F4), kept extern. GC/1.3 -O4,p like the TU, no pragmas.
 */
#include "game/battle/waza_camera_stop.h"

extern u8 lbl_8047B3F4; /* waza camera enabled */

/* End the active waza camera, if any. */
void fn_801D2F94(void)
{
    if (lbl_8047B3EC != NULL) {
        wazaCameraStop();
    }
}

/* Enable the waza camera with no sequence or animation active. */
void fn_801D301C(void)
{
    lbl_8047B3F4 = 1;
    lbl_8047B3EC = NULL;
    lbl_8047B3F0 = NULL;
}

/* End the waza camera if state is the active sequence. */
void fn_801D3034(void* state)
{
    if (state == lbl_8047B3EC) {
        wazaCameraStop();
    }
}
