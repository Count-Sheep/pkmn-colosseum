#include "dolphin/types.h"

#define HID2 920

asm void DCEnable(void) {
    nofralloc
    sync
    mfspr r3, HID0
    ori r3, r3, 0x4000
    mtspr HID0, r3
    blr
}
asm void DCInvalidateRange(register void* addr, register u32 nBytes) {
    nofralloc
    cmplwi r4, 0x0
    blelr
    clrlwi r5, r3, 27
    add r4, r4, r5
    addi r4, r4, 0x1f
    srwi r4, r4, 5
    mtctr r4
_L_8009B2C0:
    dcbi r0, r3
    addi r3, r3, 0x20
    bdnz _L_8009B2C0
    blr
}

asm void DCFlushRange(register void* addr, register u32 nBytes) {
    nofralloc
    cmplwi r4, 0x0
    blelr
    clrlwi r5, r3, 27
    add r4, r4, r5
    addi r4, r4, 0x1f
    srwi r4, r4, 5
    mtctr r4
_L_8009B2EC:
    dcbf r0, r3
    addi r3, r3, 0x20
    bdnz _L_8009B2EC
    sc
    blr
}

asm void DCStoreRange(register void* addr, register u32 nBytes) {
    nofralloc
    cmplwi r4, 0x0
    blelr
    clrlwi r5, r3, 27
    add r4, r4, r5
    addi r4, r4, 0x1f
    srwi r4, r4, 5
    mtctr r4
_L_8009B31C:
    dcbst r0, r3
    addi r3, r3, 0x20
    bdnz _L_8009B31C
    sc
    blr
}

asm void DCFlushRangeNoSync(register void* addr, register u32 nBytes) {
    nofralloc
    cmplwi r4, 0x0
    blelr
    clrlwi r5, r3, 27
    add r4, r4, r5
    addi r4, r4, 0x1f
    srwi r4, r4, 5
    mtctr r4
_L_8009B34C:
    dcbf r0, r3
    addi r3, r3, 0x20
    bdnz _L_8009B34C
    blr
}

asm void DCStoreRangeNoSync(register void* addr, register u32 nBytes) {
    nofralloc
    cmplwi r4, 0x0
    blelr
    clrlwi r5, r3, 27
    add r4, r4, r5
    addi r4, r4, 0x1f
    srwi r4, r4, 5
    mtctr r4
_L_8009B378:
    dcbst r0, r3
    addi r3, r3, 0x20
    bdnz _L_8009B378
    blr
}

asm void DCZeroRange(register void* addr, register u32 nBytes) {
    nofralloc
    cmplwi r4, 0x0
    blelr
    clrlwi r5, r3, 27
    add r4, r4, r5
    addi r4, r4, 0x1f
    srwi r4, r4, 5
    mtctr r4
_L_8009B3A4:
    dcbz r0, r3
    addi r3, r3, 0x20
    bdnz _L_8009B3A4
    blr
}

asm void ICInvalidateRange(register void* addr, register u32 nBytes) {
    nofralloc
    cmplwi r4, 0x0
    blelr
    clrlwi r5, r3, 27
    add r4, r4, r5
    addi r4, r4, 0x1f
    srwi r4, r4, 5
    mtctr r4
_L_8009B3D0:
    icbi r0, r3
    addi r3, r3, 0x20
    bdnz _L_8009B3D0
    sync
    isync
    blr
}

asm void ICFlashInvalidate(void) {
    nofralloc
    mfspr r3, HID0
    ori r3, r3, 0x800
    mtspr HID0, r3
    blr
}

asm void ICEnable(void) {
    nofralloc
    isync
    mfspr r3, HID0
    ori r3, r3, 0x8000
    mtspr HID0, r3
    blr
}

asm void __LCEnable(void) {
    nofralloc
    mfmsr r5
    ori r5, r5, 0x1000
    mtmsr r5
    lis r3, 0x8000
    li r4, 0x400
    mtctr r4
_L_8009B424:
    dcbt r0, r3
    dcbst r0, r3
    addi r3, r3, 0x20
    bdnz _L_8009B424
    mfspr r4, HID2
    oris r4, r4, 0x100f
    mtspr HID2, r4
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    lis r3, 0xe000
    ori r3, r3, 0x2
    mtdbatl 3, r3
    ori r3, r3, 0x1fe
    mtdbatu 3, r3
    isync
    lis r3, 0xe000
    li r6, 0x200
    mtctr r6
    li r6, 0x0
_L_8009B498:
    dcbz_l r6, r3
    addi r3, r3, 0x20
    bdnz _L_8009B498
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    blr
}
