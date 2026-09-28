/**
 * @file mailMain_exact_801D23C0.c
 * @brief mailMainReceiveTerminate, 0x801D23C0 - 0x801D2404.
 *
 * Function-boundary carve of the mailMain TU (see mailMain.c): no jump
 * table, no pooled constant; its only data is the .bss mail-effect state
 * (lbl_80467390), kept extern. GC/1.3 -O4,p with the TU's unit-wide
 * -opt nopeephole, no pragmas.
 */
#include "game/battle/battle_waza_types.h"

extern s32 lbl_80467390[];

/* Expire the receive timer and fade out the active sound handle. */
void mailMainReceiveTerminate(void) {
    u32 handle;
    lbl_80467390[1] = 0x258;
    handle = lbl_80467390[2];
    if (handle != 0) {
        fn_801669E4(handle, 0, 0);
    }
}
