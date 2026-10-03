/**
 * @file OSContext.c
 * @brief Dolphin SDK OSContext.c, 0x8009B914 - 0x8009C2E0, linked whole with
 *        its .data (0x803107E0-0x803109B8: OSDumpContext's strings, then
 *        __OSContextInit's message).
 *
 * The SDK writes eleven of the fifteen functions in assembly; their bodies
 * are the retail instructions and are admitted under
 * docs/asm_evidence/oscontext.md (registered in
 * docs/asm_evidence/registry.json). OSGetCurrentContext, OSClearContext,
 * OSDumpContext and __OSContextInit are C.
 */
#include "dolphin/types.h"
#include "dolphin/db/DB.h"
#include "dolphin/os/OS.h"
#include "dolphin/os/OSContext.h"
#include "dolphin/os/OSThread.h"

#define OS_FPUCONTEXT (*(OSContext* volatile*)0x800000D8)

extern BOOL OSDisableInterrupts(void);

/* Hand-written Dolphin SDK asm; evidence: docs/asm_evidence/oscontext.md */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
asm void __OSLoadFPUContext(register u8 unused, register OSContext* context) {
    nofralloc
    lhz r5, 0x1a2(r4)
    clrlwi. r5, r5, 31
    beq L_8009BA34
    lfd f0, 0x190(r4)
    mtfsf 255, f0
    mfspr r5, 0x398
    extrwi. r5, r5, 1, 2
    beq L_8009B9B4
    psq_l f0, 0x1c8(r4), 0, 0
    psq_l f1, 0x1d0(r4), 0, 0
    psq_l f2, 0x1d8(r4), 0, 0
    psq_l f3, 0x1e0(r4), 0, 0
    psq_l f4, 0x1e8(r4), 0, 0
    psq_l f5, 0x1f0(r4), 0, 0
    psq_l f6, 0x1f8(r4), 0, 0
    psq_l f7, 0x200(r4), 0, 0
    psq_l f8, 0x208(r4), 0, 0
    psq_l f9, 0x210(r4), 0, 0
    psq_l f10, 0x218(r4), 0, 0
    psq_l f11, 0x220(r4), 0, 0
    psq_l f12, 0x228(r4), 0, 0
    psq_l f13, 0x230(r4), 0, 0
    psq_l f14, 0x238(r4), 0, 0
    psq_l f15, 0x240(r4), 0, 0
    psq_l f16, 0x248(r4), 0, 0
    psq_l f17, 0x250(r4), 0, 0
    psq_l f18, 0x258(r4), 0, 0
    psq_l f19, 0x260(r4), 0, 0
    psq_l f20, 0x268(r4), 0, 0
    psq_l f21, 0x270(r4), 0, 0
    psq_l f22, 0x278(r4), 0, 0
    psq_l f23, 0x280(r4), 0, 0
    psq_l f24, 0x288(r4), 0, 0
    psq_l f25, 0x290(r4), 0, 0
    psq_l f26, 0x298(r4), 0, 0
    psq_l f27, 0x2a0(r4), 0, 0
    psq_l f28, 0x2a8(r4), 0, 0
    psq_l f29, 0x2b0(r4), 0, 0
    psq_l f30, 0x2b8(r4), 0, 0
    psq_l f31, 0x2c0(r4), 0, 0
L_8009B9B4:
    lfd f0, 0x90(r4)
    lfd f1, 0x98(r4)
    lfd f2, 0xa0(r4)
    lfd f3, 0xa8(r4)
    lfd f4, 0xb0(r4)
    lfd f5, 0xb8(r4)
    lfd f6, 0xc0(r4)
    lfd f7, 0xc8(r4)
    lfd f8, 0xd0(r4)
    lfd f9, 0xd8(r4)
    lfd f10, 0xe0(r4)
    lfd f11, 0xe8(r4)
    lfd f12, 0xf0(r4)
    lfd f13, 0xf8(r4)
    lfd f14, 0x100(r4)
    lfd f15, 0x108(r4)
    lfd f16, 0x110(r4)
    lfd f17, 0x118(r4)
    lfd f18, 0x120(r4)
    lfd f19, 0x128(r4)
    lfd f20, 0x130(r4)
    lfd f21, 0x138(r4)
    lfd f22, 0x140(r4)
    lfd f23, 0x148(r4)
    lfd f24, 0x150(r4)
    lfd f25, 0x158(r4)
    lfd f26, 0x160(r4)
    lfd f27, 0x168(r4)
    lfd f28, 0x170(r4)
    lfd f29, 0x178(r4)
    lfd f30, 0x180(r4)
    lfd f31, 0x188(r4)
L_8009BA34:
    blr
}
#pragma pop

/* Hand-written Dolphin SDK asm; evidence: docs/asm_evidence/oscontext.md */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
asm void __OSSaveFPUContext(register u8 unused1, register u8 unused2, register OSContext* context) {
    nofralloc
    lhz r3, 0x1a2(r5)
    ori r3, r3, 0x1
    sth r3, 0x1a2(r5)
    stfd f0, 0x90(r5)
    stfd f1, 0x98(r5)
    stfd f2, 0xa0(r5)
    stfd f3, 0xa8(r5)
    stfd f4, 0xb0(r5)
    stfd f5, 0xb8(r5)
    stfd f6, 0xc0(r5)
    stfd f7, 0xc8(r5)
    stfd f8, 0xd0(r5)
    stfd f9, 0xd8(r5)
    stfd f10, 0xe0(r5)
    stfd f11, 0xe8(r5)
    stfd f12, 0xf0(r5)
    stfd f13, 0xf8(r5)
    stfd f14, 0x100(r5)
    stfd f15, 0x108(r5)
    stfd f16, 0x110(r5)
    stfd f17, 0x118(r5)
    stfd f18, 0x120(r5)
    stfd f19, 0x128(r5)
    stfd f20, 0x130(r5)
    stfd f21, 0x138(r5)
    stfd f22, 0x140(r5)
    stfd f23, 0x148(r5)
    stfd f24, 0x150(r5)
    stfd f25, 0x158(r5)
    stfd f26, 0x160(r5)
    stfd f27, 0x168(r5)
    stfd f28, 0x170(r5)
    stfd f29, 0x178(r5)
    stfd f30, 0x180(r5)
    stfd f31, 0x188(r5)
    mffs f0
    stfd f0, 0x190(r5)
    lfd f0, 0x90(r5)
    mfspr r3, 0x398
    extrwi. r3, r3, 1, 2
    beq L_8009BB5C
    psq_st f0, 0x1c8(r5), 0, 0
    psq_st f1, 0x1d0(r5), 0, 0
    psq_st f2, 0x1d8(r5), 0, 0
    psq_st f3, 0x1e0(r5), 0, 0
    psq_st f4, 0x1e8(r5), 0, 0
    psq_st f5, 0x1f0(r5), 0, 0
    psq_st f6, 0x1f8(r5), 0, 0
    psq_st f7, 0x200(r5), 0, 0
    psq_st f8, 0x208(r5), 0, 0
    psq_st f9, 0x210(r5), 0, 0
    psq_st f10, 0x218(r5), 0, 0
    psq_st f11, 0x220(r5), 0, 0
    psq_st f12, 0x228(r5), 0, 0
    psq_st f13, 0x230(r5), 0, 0
    psq_st f14, 0x238(r5), 0, 0
    psq_st f15, 0x240(r5), 0, 0
    psq_st f16, 0x248(r5), 0, 0
    psq_st f17, 0x250(r5), 0, 0
    psq_st f18, 0x258(r5), 0, 0
    psq_st f19, 0x260(r5), 0, 0
    psq_st f20, 0x268(r5), 0, 0
    psq_st f21, 0x270(r5), 0, 0
    psq_st f22, 0x278(r5), 0, 0
    psq_st f23, 0x280(r5), 0, 0
    psq_st f24, 0x288(r5), 0, 0
    psq_st f25, 0x290(r5), 0, 0
    psq_st f26, 0x298(r5), 0, 0
    psq_st f27, 0x2a0(r5), 0, 0
    psq_st f28, 0x2a8(r5), 0, 0
    psq_st f29, 0x2b0(r5), 0, 0
    psq_st f30, 0x2b8(r5), 0, 0
    psq_st f31, 0x2c0(r5), 0, 0
L_8009BB5C:
    blr
}
#pragma pop

/* Hand-written Dolphin SDK asm; evidence: docs/asm_evidence/oscontext.md */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
asm void OSSaveFPUContext(register OSContext* context) {
    nofralloc
    addi r5, r3, 0x0
    b __OSSaveFPUContext
}
#pragma pop

/* Hand-written Dolphin SDK asm; evidence: docs/asm_evidence/oscontext.md */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
asm void OSSetCurrentContext(register OSContext* context) {
    nofralloc
    lis r4, 0x8000
    stw r3, 0xd4(r4)
    clrlwi r5, r3, 2
    stw r5, 0xc0(r4)
    lwz r5, 0xd8(r4)
    cmpw r5, r3
    bne L_8009BBA0
    lwz r6, 0x19c(r3)
    ori r6, r6, 0x2000
    stw r6, 0x19c(r3)
    mfmsr r6
    ori r6, r6, 0x2
    mtmsr r6
    blr
L_8009BBA0:
    lwz r6, 0x19c(r3)
    rlwinm r6, r6, 0, 19, 17
    stw r6, 0x19c(r3)
    mfmsr r6
    rlwinm r6, r6, 0, 19, 17
    ori r6, r6, 0x2
    mtmsr r6
    isync
    blr
}
#pragma pop

OSContext* OSGetCurrentContext(void) {
    return *(OSContext* volatile*)0x800000D4;
}

/* Hand-written Dolphin SDK asm; evidence: docs/asm_evidence/oscontext.md */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
asm u32 OSSaveContext(register OSContext* context) {
    nofralloc
    stmw r13, 0x34(r3)
    mfspr r0, 0x391
    stw r0, 0x1a8(r3)
    mfspr r0, 0x392
    stw r0, 0x1ac(r3)
    mfspr r0, 0x393
    stw r0, 0x1b0(r3)
    mfspr r0, 0x394
    stw r0, 0x1b4(r3)
    mfspr r0, 0x395
    stw r0, 0x1b8(r3)
    mfspr r0, 0x396
    stw r0, 0x1bc(r3)
    mfspr r0, 0x397
    stw r0, 0x1c0(r3)
    mfcr r0
    stw r0, 0x80(r3)
    mflr r0
    stw r0, 0x84(r3)
    stw r0, 0x198(r3)
    mfmsr r0
    stw r0, 0x19c(r3)
    mfctr r0
    stw r0, 0x88(r3)
    mfxer r0
    stw r0, 0x8c(r3)
    stw r1, 0x4(r3)
    stw r2, 0x8(r3)
    li r0, 0x1
    stw r0, 0xc(r3)
    li r3, 0x0
    blr
}
#pragma pop

/* Hand-written Dolphin SDK asm; evidence: docs/asm_evidence/oscontext.md */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
asm void OSLoadContext(register OSContext* context) {
    nofralloc
    lis r4, OSDisableInterrupts@ha
    lwz r6, 0x198(r3)
    addi r5, r4, OSDisableInterrupts@l
    cmplw r6, r5
    ble L_8009BC78
    lis r4, (OSDisableInterrupts+0xC)@ha
    addi r0, r4, (OSDisableInterrupts+0xC)@l
    cmplw r6, r0
    bge L_8009BC78
    stw r5, 0x198(r3)
L_8009BC78:
    lwz r0, 0x0(r3)
    lwz r1, 0x4(r3)
    lwz r2, 0x8(r3)
    lhz r4, 0x1a2(r3)
    rlwinm. r5, r4, 0, 30, 30
    beq L_8009BCA0
    rlwinm r4, r4, 0, 31, 29
    sth r4, 0x1a2(r3)
    lmw r5, 0x14(r3)
    b L_8009BCA4
L_8009BCA0:
    lmw r13, 0x34(r3)
L_8009BCA4:
    lwz r4, 0x1a8(r3)
    mtspr 0x391, r4
    lwz r4, 0x1ac(r3)
    mtspr 0x392, r4
    lwz r4, 0x1b0(r3)
    mtspr 0x393, r4
    lwz r4, 0x1b4(r3)
    mtspr 0x394, r4
    lwz r4, 0x1b8(r3)
    mtspr 0x395, r4
    lwz r4, 0x1bc(r3)
    mtspr 0x396, r4
    lwz r4, 0x1c0(r3)
    mtspr 0x397, r4
    lwz r4, 0x80(r3)
    mtcrf 255, r4
    lwz r4, 0x84(r3)
    mtlr r4
    lwz r4, 0x88(r3)
    mtctr r4
    lwz r4, 0x8c(r3)
    mtxer r4
    mfmsr r4
    rlwinm r4, r4, 0, 17, 15
    rlwinm r4, r4, 0, 31, 29
    mtmsr r4
    lwz r4, 0x198(r3)
    mtsrr0 r4
    lwz r4, 0x19c(r3)
    mtsrr1 r4
    lwz r4, 0x10(r3)
    lwz r3, 0xc(r3)
    rfi
}
#pragma pop

/* Hand-written Dolphin SDK asm; evidence: docs/asm_evidence/oscontext.md */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
asm u32 OSGetStackPointer(void) {
    nofralloc
    mr r3, r1
    blr
}
#pragma pop

/* Hand-written Dolphin SDK asm; evidence: docs/asm_evidence/oscontext.md */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
asm int OSSwitchFiber(register u32 pc, register u32 newsp) {
    nofralloc
    mflr r0
    mr r5, r1
    stwu r5, -0x8(r4)
    mr r1, r4
    stw r0, 0x4(r5)
    mtlr r3
    blrl
    lwz r5, 0x0(r1)
    lwz r0, 0x4(r5)
    mtlr r0
    mr r1, r5
    blr
}
#pragma pop

void OSClearContext(OSContext* context) {
    context->mode = 0;
    context->state = 0;

    if (context == OS_FPUCONTEXT) {
        OS_FPUCONTEXT = NULL;
    }
}

/* Hand-written Dolphin SDK asm; evidence: docs/asm_evidence/oscontext.md */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
asm void OSInitContext(register OSContext* context, register u32 pc, register u32 newsp) {
    nofralloc
    stw r4, 0x198(r3)
    stw r5, 0x4(r3)
    li r11, 0x0
    ori r11, r11, 0x9032
    stw r11, 0x19c(r3)
    li r0, 0x0
    stw r0, 0x80(r3)
    stw r0, 0x8c(r3)
    stw r2, 0x8(r3)
    stw r13, 0x34(r3)
    stw r0, 0xc(r3)
    stw r0, 0x10(r3)
    stw r0, 0x14(r3)
    stw r0, 0x18(r3)
    stw r0, 0x1c(r3)
    stw r0, 0x20(r3)
    stw r0, 0x24(r3)
    stw r0, 0x28(r3)
    stw r0, 0x2c(r3)
    stw r0, 0x30(r3)
    stw r0, 0x38(r3)
    stw r0, 0x3c(r3)
    stw r0, 0x40(r3)
    stw r0, 0x44(r3)
    stw r0, 0x48(r3)
    stw r0, 0x4c(r3)
    stw r0, 0x50(r3)
    stw r0, 0x54(r3)
    stw r0, 0x58(r3)
    stw r0, 0x5c(r3)
    stw r0, 0x60(r3)
    stw r0, 0x64(r3)
    stw r0, 0x68(r3)
    stw r0, 0x6c(r3)
    stw r0, 0x70(r3)
    stw r0, 0x74(r3)
    stw r0, 0x78(r3)
    stw r0, 0x7c(r3)
    stw r0, 0x1a4(r3)
    stw r0, 0x1a8(r3)
    stw r0, 0x1ac(r3)
    stw r0, 0x1b0(r3)
    stw r0, 0x1b4(r3)
    stw r0, 0x1b8(r3)
    stw r0, 0x1bc(r3)
    stw r0, 0x1c0(r3)
    b OSClearContext
}
#pragma pop

/* Retail keeps OSDumpContext's branch-to-next chains and mr copies: the unit
 * builds with -opt nopeephole (the C functions; the asm bodies are verbatim). */
void OSDumpContext(OSContext* context) {
    extern void OSReport(const char* format, ...);
    extern void OSSetCurrentContext(OSContext* context);
    extern BOOL OSDisableInterrupts(void);
    extern BOOL OSRestoreInterrupts(BOOL level);
    u32 i;
    u32* p;

    OSReport("------------------------- Context 0x%08x -------------------------\n", context);

    for (i = 0; i < 16; ++i) {
        OSReport("r%-2d  = 0x%08x (%14d)  r%-2d  = 0x%08x (%14d)\n", i, context->gpr[i],
                 context->gpr[i], i + 16, context->gpr[i + 16],
                 context->gpr[i + 16]);
    }

    OSReport("LR   = 0x%08x                   CR   = 0x%08x\n", context->lr, context->cr);
    OSReport("SRR0 = 0x%08x                   SRR1 = 0x%08x\n", context->srr0, context->srr1);

    OSReport("\nGQRs----------\n");
    for (i = 0; i < 4; ++i) {
        OSReport("gqr%d = 0x%08x \t gqr%d = 0x%08x\n", i, context->gqr[i], i + 4,
                 context->gqr[i + 4]);
    }

    /* The SDK flag is unsigned (0x01u): the test compares unsigned. */
    if ((context->state & OS_CONTEXT_STATE_FPSAVED) != 0u) {
        OSContext* currentContext;
        OSContext fpuContext;
        BOOL enabled;

        enabled = OSDisableInterrupts();
        currentContext = OSGetCurrentContext();
        OSClearContext(&fpuContext);
        OSSetCurrentContext(&fpuContext);

        OSReport("\n\nFPRs----------\n");
        for (i = 0; i < 32; i += 2) {
            OSReport("fr%d \t= %d \t fr%d \t= %d\n", i, (u32)context->fpr[i],
                     i + 1, (u32)context->fpr[i + 1]);
        }
        OSReport("\n\nPSFs----------\n");
        for (i = 0; i < 32; i += 2) {
            OSReport("ps%d \t= 0x%x \t ps%d \t= 0x%x\n", i, (u32)context->psf[i],
                     i + 1, (u32)context->psf[i + 1]);
        }

        OSClearContext(&fpuContext);
        OSSetCurrentContext(currentContext);
        OSRestoreInterrupts(enabled);
    }

    OSReport("\nAddress:      Back Chain    LR Save\n");
    for (i = 0, p = (u32*)context->gpr[1];
         p && (u32)p != 0xFFFFFFFF && i++ < 16; p = (u32*)*p) {
        OSReport("0x%08x:   0x%08x    0x%08x\n", p, p[0], p[1]);
    }
}

/* Hand-written Dolphin SDK asm; evidence: docs/asm_evidence/oscontext.md */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
asm void OSSwitchFPUContext(register u8 exception, register OSContext* context) {
    nofralloc
    mfmsr r5
    ori r5, r5, 0x2000
    mtmsr r5
    isync
    lwz r5, 0x19c(r4)
    ori r5, r5, 0x2000
    mtsrr1 r5
    lis r3, 0x8000
    lwz r5, 0xd8(r3)
    stw r4, 0xd8(r3)
    cmpw r5, r4
    beq L_8009C128
    cmpwi r5, 0x0
    beq L_8009C124
    bl __OSSaveFPUContext
L_8009C124:
    bl __OSLoadFPUContext
L_8009C128:
    lwz r3, 0x80(r4)
    mtcrf 255, r3
    lwz r3, 0x84(r4)
    mtlr r3
    lwz r3, 0x198(r4)
    mtsrr0 r3
    lwz r3, 0x88(r4)
    mtctr r3
    lwz r3, 0x8c(r4)
    mtxer r3
    lhz r3, 0x1a2(r4)
    rlwinm r3, r3, 0, 31, 29
    sth r3, 0x1a2(r4)
    lwz r5, 0x14(r4)
    lwz r3, 0xc(r4)
    lwz r4, 0x10(r4)
    rfi
}
#pragma pop

void __OSContextInit(void) {
    __OSSetExceptionHandler(OS_EXCEPTION_FLOATING_POINT,
                            (__OSExceptionHandler)OSSwitchFPUContext);
    OS_FPUCONTEXT = NULL;
    DBPrintf("FPU-unavailable handler installed\n");
}

/* Hand-written Dolphin SDK asm; evidence: docs/asm_evidence/oscontext.md */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
asm void OSFillFPUContext(register OSContext* context) {
    nofralloc
    mfmsr r5
    ori r5, r5, 0x2000
    mtmsr r5
    isync
    stfd f0, 0x90(r3)
    stfd f1, 0x98(r3)
    stfd f2, 0xa0(r3)
    stfd f3, 0xa8(r3)
    stfd f4, 0xb0(r3)
    stfd f5, 0xb8(r3)
    stfd f6, 0xc0(r3)
    stfd f7, 0xc8(r3)
    stfd f8, 0xd0(r3)
    stfd f9, 0xd8(r3)
    stfd f10, 0xe0(r3)
    stfd f11, 0xe8(r3)
    stfd f12, 0xf0(r3)
    stfd f13, 0xf8(r3)
    stfd f14, 0x100(r3)
    stfd f15, 0x108(r3)
    stfd f16, 0x110(r3)
    stfd f17, 0x118(r3)
    stfd f18, 0x120(r3)
    stfd f19, 0x128(r3)
    stfd f20, 0x130(r3)
    stfd f21, 0x138(r3)
    stfd f22, 0x140(r3)
    stfd f23, 0x148(r3)
    stfd f24, 0x150(r3)
    stfd f25, 0x158(r3)
    stfd f26, 0x160(r3)
    stfd f27, 0x168(r3)
    stfd f28, 0x170(r3)
    stfd f29, 0x178(r3)
    stfd f30, 0x180(r3)
    stfd f31, 0x188(r3)
    mffs f0
    stfd f0, 0x190(r3)
    lfd f0, 0x90(r3)
    mfspr r5, 0x398
    extrwi. r5, r5, 1, 2
    beq L_8009C2DC
    psq_st f0, 0x1c8(r3), 0, 0
    psq_st f1, 0x1d0(r3), 0, 0
    psq_st f2, 0x1d8(r3), 0, 0
    psq_st f3, 0x1e0(r3), 0, 0
    psq_st f4, 0x1e8(r3), 0, 0
    psq_st f5, 0x1f0(r3), 0, 0
    psq_st f6, 0x1f8(r3), 0, 0
    psq_st f7, 0x200(r3), 0, 0
    psq_st f8, 0x208(r3), 0, 0
    psq_st f9, 0x210(r3), 0, 0
    psq_st f10, 0x218(r3), 0, 0
    psq_st f11, 0x220(r3), 0, 0
    psq_st f12, 0x228(r3), 0, 0
    psq_st f13, 0x230(r3), 0, 0
    psq_st f14, 0x238(r3), 0, 0
    psq_st f15, 0x240(r3), 0, 0
    psq_st f16, 0x248(r3), 0, 0
    psq_st f17, 0x250(r3), 0, 0
    psq_st f18, 0x258(r3), 0, 0
    psq_st f19, 0x260(r3), 0, 0
    psq_st f20, 0x268(r3), 0, 0
    psq_st f21, 0x270(r3), 0, 0
    psq_st f22, 0x278(r3), 0, 0
    psq_st f23, 0x280(r3), 0, 0
    psq_st f24, 0x288(r3), 0, 0
    psq_st f25, 0x290(r3), 0, 0
    psq_st f26, 0x298(r3), 0, 0
    psq_st f27, 0x2a0(r3), 0, 0
    psq_st f28, 0x2a8(r3), 0, 0
    psq_st f29, 0x2b0(r3), 0, 0
    psq_st f30, 0x2b8(r3), 0, 0
    psq_st f31, 0x2c0(r3), 0, 0
L_8009C2DC:
    blr
}
#pragma pop

