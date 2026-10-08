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
_L_80098040:
    nop
    li r3, 0x0
    nop
    b _L_80098040
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
