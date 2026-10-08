#include "dolphin/os/OSContext.h"

#define OS_CACHED_REGION_PREFIX 0x8000
#define OS_CONTEXT_R3 12
#define OS_CONTEXT_R4 16
#define OS_CONTEXT_R5 20
#define OS_CONTEXT_CR 128
#define OS_CONTEXT_LR 132
#define OS_CONTEXT_CTR 136
#define OS_CONTEXT_XER 140
#define OS_CONTEXT_SRR0 408
#define OS_CONTEXT_SRR1 412

asm void OSSwitchFPUContext(register u8 exception, register OSContext* context) {
    nofralloc
    mfmsr   r5
    ori     r5, r5, 0x2000
    mtmsr   r5
    isync
    lwz     r5, OS_CONTEXT_SRR1(context)
    ori     r5, r5, 0x2000
    mtsrr1  r5
    addis   r3, r0, OS_CACHED_REGION_PREFIX
    lwz     r5, 0x00D8(r3)
    stw     context, 0x00D8(r3)
    cmpw    r5, r4
    beq     _restoreAndExit
    cmpwi   r5, 0x0
    beq     _loadNewFPUContext
    bl      __OSSaveFPUContext
_loadNewFPUContext:
    bl      __OSLoadFPUContext
_restoreAndExit:
    lwz     r3, OS_CONTEXT_CR(context)
    mtcr    r3
    lwz     r3, OS_CONTEXT_LR(context)
    mtlr    r3
    lwz     r3, OS_CONTEXT_SRR0(context)
    mtsrr0  r3
    lwz     r3, OS_CONTEXT_CTR(context)
    mtctr   r3
    lwz     r3, OS_CONTEXT_XER(context)
    mtxer   r3
    lhz     r3, context->state
    rlwinm  r3, r3, 0, 31, 29
    sth     r3, context->state
    lwz     r5, OS_CONTEXT_R5(context)
    lwz     r3, OS_CONTEXT_R3(context)
    lwz     r4, OS_CONTEXT_R4(context)
    rfi
}
