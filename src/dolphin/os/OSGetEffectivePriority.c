#include "dolphin/types.h"

asm s32 __OSGetEffectivePriority(register void* thread) {
    nofralloc
    lwz r4, 0x2D4(r3)
    lwz r5, 0x2F4(r3)
    b _check
_next:
    lwz r3, 0(r5)
    cmplwi r3, 0
    beq _advance
    lwz r0, 0x2D0(r3)
    cmpw r0, r4
    bge _advance
    mr r4, r0
_advance:
    lwz r5, 0x10(r5)
_check:
    cmplwi r5, 0
    bne _next
    mr r3, r4
    blr
}
