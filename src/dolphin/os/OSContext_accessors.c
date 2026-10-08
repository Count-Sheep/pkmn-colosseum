#include "dolphin/types.h"

asm u32 OSGetStackPointer(void) {
    nofralloc
    mr r3, r1
    blr
}

asm int OSSwitchFiber(register u32 pc, register u32 newsp) {
    nofralloc
    mflr r0
    mr r5, r1
    stwu r5, -8(newsp)
    mr r1, newsp
    stw r0, 4(r5)
    mtlr pc
    blrl
    lwz r5, 0(r1)
    lwz r0, 4(r5)
    mtlr r0
    mr r1, r5
    blr
}
