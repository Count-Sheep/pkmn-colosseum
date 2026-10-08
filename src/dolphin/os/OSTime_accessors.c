#include "dolphin/types.h"

asm OSTime OSGetTime(void) {
retry:
    nofralloc
    mftbu r3
    mftb r4
    mftbu r5
    cmpw r3, r5
    bne retry
    blr
}

asm u32 OSGetTick(void) {
    nofralloc
    mftb r3
    blr
}
