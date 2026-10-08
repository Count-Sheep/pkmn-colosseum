#include "dolphin/os/OSContext.h"

void __OSDispatchInterrupt(u8 exception, OSContext* context);

asm void ExternalInterruptHandler_8009E758(register u8 exception,
                                            register OSContext* context) {
    nofralloc
    stw r0, 0x0(r4)
    stw r1, 0x4(r4)
    stw r2, 0x8(r4)
    stmw r6, 0x18(r4)
    mfspr r0, GQR1
    stw r0, 0x1a8(r4)
    mfspr r0, GQR2
    stw r0, 0x1ac(r4)
    mfspr r0, GQR3
    stw r0, 0x1b0(r4)
    mfspr r0, GQR4
    stw r0, 0x1b4(r4)
    mfspr r0, GQR5
    stw r0, 0x1b8(r4)
    mfspr r0, GQR6
    stw r0, 0x1bc(r4)
    mfspr r0, GQR7
    stw r0, 0x1c0(r4)
    stwu r1, -0x8(r1)
    b __OSDispatchInterrupt
}
