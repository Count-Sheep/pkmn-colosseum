/**
 * @file mailMain_exact_801D2B08.c
 * @brief mailMainInit, 0x801D2B08 - 0x801D2B4C.
 *
 * Function-boundary carve of the mailMain TU (see mailMain.c): no jump
 * table, no pooled constant; its only data is the .bss mail-effect state
 * (lbl_80467390), kept extern, and cbStep, called by its global symbol.
 * GC/1.3 -O4,p with the TU's unit-wide -opt nopeephole, no pragmas.
 */
#include "game/battle/battle_waza_types.h"

extern s32 lbl_80467390[];

/* Register the multi-hit step callback and clear the mail-effect state. */
void mailMainInit(void) {
    s32* state;

    heroMoveAddStepCallback(cbStep, 0);
    state = lbl_80467390;
    state[0] = 0;
    state[2] = 0;
    state[3] = 0;
}
