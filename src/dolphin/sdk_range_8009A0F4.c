/**
 * @file sdk_range_8009A0F4.c
 * @brief dolphin-sdk code, 0x8009A0F4 - 0x8009A27C: the tail of OS.c
 *        (OSExceptionVector .. OSRegisterVersion). OSInitAlarm, the first
 *        function of OSAlarm.c, is in dolphin/os/OSInitAlarm.c.
 *
 * Range unit assigned from the propagated subsystem map
 * (tools/subsystem_propagation.py, >=80% single-label dominance;
 * campaign 2026-07-01). All functions asm-only until matched; the
 * range name stays honest until internal TU structure is proven.
 */
#include "dolphin/types.h"
#include "dolphin/os/OSContext.h"

extern void OSReport(const char* format, ...);
extern const char lbl_8047897C;

extern volatile u32 __DIRegs[16] : 0xCC006000;

void OSDefaultExceptionHandler(u8 exception, OSContext* context);
extern void __OSUnhandledException(u8 exception, OSContext* context, u32 dsisr, u32 dar);
void __OSEVStart(void);
extern u32 PPCMfhid2(void);
extern void PPCMthid2(u32 value);
extern void ICFlashInvalidate(void);
void __DBVECTOR(void);
void __OSEVSetNumber(void);
void __OSEVEnd(void);

/* Hand-written Dolphin SDK asm; evidence: docs/asm_evidence/os_vector.md */
asm void OSExceptionVector(void) {
    nofralloc

entry __OSEVStart
    mtsprg  0, r4
    lwz     r4, 0xC0(r0)
    stw     r3, 0xC(r4)
    mfsprg  r3, 0
    stw     r3, 0x10(r4)
    stw     r5, 0x14(r4)
    lhz     r3, 0x1A2(r4)
    ori     r3, r3, 0x2
    sth     r3, 0x1A2(r4)
    mfcr    r3
    stw     r3, 0x80(r4)
    mflr    r3
    stw     r3, 0x84(r4)
    mfctr   r3
    stw     r3, 0x88(r4)
    mfxer   r3
    stw     r3, 0x8C(r4)
    mfsrr0  r3
    stw     r3, 0x198(r4)
    mfsrr1  r3
    stw     r3, 0x19C(r4)
    mr      r5, r3

entry __DBVECTOR
    nop
    mfmsr   r3
    ori     r3, r3, 0x30
    mtsrr1  r3

entry __OSEVSetNumber
    li      r3, 0
    lwz     r4, 0xD4(r0)
    rlwinm. r5, r5, 0, 30, 30
    bne     recoverable
    lis     r5, OSDefaultExceptionHandler@ha
    addi    r5, r5, OSDefaultExceptionHandler@l
    mtsrr0  r5
    rfi

recoverable:
    rlwinm  r5, r3, 2, 22, 29
    lwz     r5, 0x3000(r5)
    mtsrr0  r5
    rfi

entry __OSEVEnd
    nop
}

/* Hand-written Dolphin SDK asm; evidence: docs/asm_evidence/os_vector.md.
 * It saves the remaining GPRs and the GQRs into the context and tail-branches
 * to __OSUnhandledException (a declared branch target). */
asm void OSDefaultExceptionHandler(register u8 exception, register OSContext* context) {
    nofralloc

    stw     r0, 0x0(context)
    stw     r1, 0x4(context)
    stw     r2, 0x8(context)
    stmw    r6, 0x18(context)
    mfspr   r0, GQR1
    stw     r0, 0x1A8(context)
    mfspr   r0, GQR2
    stw     r0, 0x1AC(context)
    mfspr   r0, GQR3
    stw     r0, 0x1B0(context)
    mfspr   r0, GQR4
    stw     r0, 0x1B4(context)
    mfspr   r0, GQR5
    stw     r0, 0x1B8(context)
    mfspr   r0, GQR6
    stw     r0, 0x1BC(context)
    mfspr   r0, GQR7
    stw     r0, 0x1C0(context)
    mfdsisr r5
    mfdar   r6
    stwu    r1, -0x8(r1)
    b       __OSUnhandledException
}

/* Hand-written Dolphin SDK asm (the GQR writes); evidence:
   docs/asm_evidence/os_vector.md */
void __OSPSInit(void) {
    PPCMthid2(PPCMfhid2() | 0x80000000 | 0x20000000);
    ICFlashInvalidate();
    __sync();
    asm {
        li      r3, 0
        mtspr   GQR0, r3
        mtspr   GQR1, r3
        mtspr   GQR2, r3
        mtspr   GQR3, r3
        mtspr   GQR4, r3
        mtspr   GQR5, r3
        mtspr   GQR6, r3
        mtspr   GQR7, r3
    }
}

/* OS.c's C functions show unpeepholed codegen (mr, no folded offset). */
#pragma push
/* RULE-EXCEPTION(user-approved): local peephole-control pragma — see docs/RULE_EXCEPTIONS.md */
#pragma peephole off
u32 __OSGetDIConfig(void) {
    return __DIRegs[9] & 0xFF;
}

void OSRegisterVersion(const char* version) {
    OSReport(&lbl_8047897C, version);
}
#pragma pop
