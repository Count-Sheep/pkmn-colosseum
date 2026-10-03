/**
 * @file OSTime_range_800A2778.c
 * @brief The head of the Dolphin SDK's OSTime.c, 0x800A2778 - 0x800A27FC:
 *        OSGetTime and OSGetTick (hand-written asm) and __OSGetSystemTime.
 *        The calendar half links as dolphin/os/OSTime.c.
 */
#include "dolphin/types.h"

typedef s64 OSTime;
typedef u32 OSTick;

extern BOOL OSDisableInterrupts(void);
extern BOOL OSRestoreInterrupts(BOOL level);

/* Hand-written Dolphin SDK asm; evidence: docs/asm_evidence/os_time.md.
 * The time base is read high, low, high until the two high reads agree; the
 * only branch is that retry back to the start of the routine. */
asm OSTime OSGetTime(void) {
    nofralloc

    mftb    r3, 269
    mftb    r4, 268
    mftb    r5, 269
    cmpw    r3, r5
    bne     OSGetTime
    blr
}

/* Hand-written Dolphin SDK asm; evidence: docs/asm_evidence/os_time.md. */
asm OSTick OSGetTick(void) {
    nofralloc

    mftb    r3, 268
    blr
}

OSTime __OSGetSystemTime(void) {
    BOOL enabled;
    OSTime* timeAdjustAddr = (OSTime*)(0x80000000 + 0x30D8);
    OSTime result;

    enabled = OSDisableInterrupts();
    result = *timeAdjustAddr + OSGetTime();
    OSRestoreInterrupts(enabled);

    return result;
}
