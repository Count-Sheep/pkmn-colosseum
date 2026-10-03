/**
 * @file sdk_candidate_8009DF3C.c
 * @brief OSInterrupt.c head, 0x8009DF3C - 0x8009DF88: the MSR[EE]
 *        primitives OSDisableInterrupts, OSEnableInterrupts and
 *        OSRestoreInterrupts.
 */
#include "dolphin/types.h"

/* Hand-written Dolphin SDK asm; evidence: docs/asm_evidence/os_primitives.md */
asm BOOL OSDisableInterrupts(void) {
    nofralloc
    mfmsr   r3
    rlwinm  r4, r3, 0, 17, 15
    mtmsr   r4
    rlwinm  r3, r3, 17, 31, 31
    blr
}

/* Hand-written Dolphin SDK asm; evidence: docs/asm_evidence/os_primitives.md */
asm BOOL OSEnableInterrupts(void) {
    nofralloc
    mfmsr   r3
    ori     r4, r3, 0x8000
    mtmsr   r4
    rlwinm  r3, r3, 17, 31, 31
    blr
}

/* Hand-written Dolphin SDK asm; evidence: docs/asm_evidence/os_primitives.md */
asm BOOL OSRestoreInterrupts(register BOOL level) {
    nofralloc
    cmpwi   level, 0
    mfmsr   r4
    beq     _disable
    ori     r5, r4, 0x8000
    b       _restore
_disable:
    rlwinm  r5, r4, 0, 17, 15
_restore:
    mtmsr   r5
    rlwinm  r3, r4, 17, 31, 31
    blr
}
