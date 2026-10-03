/**
 * @file PPCArch.c
 * @brief The Dolphin SDK's PPCArch.c, 0x80097FFC - 0x80098108: the SPR/MSR
 *        primitives (hardware asm), the FPSCR accessors and
 *        PPCDisableSpeculation.
 */
#include "dolphin/os/PPCArch.h"

asm u32 PPCMfmsr(void) {
    nofralloc
    mfmsr r3
    blr
}

asm void PPCMtmsr(register u32 val) {
    nofralloc
    mtmsr r3
    blr
}

asm u32 PPCMfhid0(void) {
    nofralloc
    mfspr r3, HID0
    blr
}

asm void PPCMthid0(register u32 val) {
    nofralloc
    mtspr HID0, r3
    blr
}

asm u32 PPCMfl2cr(void) {
    nofralloc
    mfspr r3, L2CR
    blr
}

asm void PPCMtl2cr(register u32 val) {
    nofralloc
    mtspr L2CR, r3
    blr
}

asm void PPCMtdec(register u32 val) {
    nofralloc
    mtdec r3
    blr
}

asm void PPCSync(void) {
    nofralloc
    sc
    blr
}

asm void PPCHalt(void) {
    nofralloc
    sync
_ppc_halt_loop:
    nop
    li r3, 0
    nop
    b _ppc_halt_loop
}

asm void PPCMtmmcr0(register u32 val) {
    nofralloc
    mtspr MMCR0, r3
    blr
}

asm void PPCMtmmcr1(register u32 val) {
    nofralloc
    mtspr MMCR1, r3
    blr
}

asm void PPCMtpmc1(register u32 val) {
    nofralloc
    mtspr PMC1, r3
    blr
}

asm void PPCMtpmc2(register u32 val) {
    nofralloc
    mtspr PMC2, r3
    blr
}

asm void PPCMtpmc3(register u32 val) {
    nofralloc
    mtspr PMC3, r3
    blr
}

asm void PPCMtpmc4(register u32 val) {
    nofralloc
    mtspr PMC4, r3
    blr
}

/* The SDK's FPSCR accessors are C with inline asm that pins fp31; evidence:
   docs/asm_evidence/ppcarch.md */
u32 PPCMffpscr(void) {
    union {
        f64 f;
        struct {
            u32 fpscr_pad;
            u32 fpscr;
        } u;
    } m;

    asm {
        mffs  fp31
        stfd  fp31, m.f
    }

    return m.u.fpscr;
}

/* Evidence: docs/asm_evidence/ppcarch.md */
void PPCMtfpscr(register u32 newFPSCR) {
    union {
        f64 f;
        struct {
            u32 fpscr_pad;
            u32 fpscr;
        } u;
    } m;

    asm {
        li    r4, 0
        stw   r4, m.u.fpscr_pad
        stw   newFPSCR, m.u.fpscr
        lfd   fp31, m.f
        mtfsf 0xff, fp31
    }
}

asm u32 PPCMfhid2(void) {
    nofralloc
    mfspr r3, 920
    blr
}

asm void PPCMthid2(register u32 val) {
    nofralloc
    mtspr 920, r3
    blr
}

asm void PPCMtwpar(register u32 val) {
    nofralloc
    mtspr WPAR, r3
    blr
}


extern u32 PPCMfhid0(void);
extern void PPCMthid0(u32 hid0);

void PPCDisableSpeculation(void) {
    u32 hid0 = PPCMfhid0();
    hid0 |= 0x200;
    PPCMthid0(hid0);
}
