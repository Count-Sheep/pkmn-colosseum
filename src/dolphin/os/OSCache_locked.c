#include "dolphin/types.h"

#define HID2 920

asm void LCDisable(void) {
    nofralloc
    lis r3, 0xe000
    li r4, 0x200
    mtctr r4
_L_8009B51C:
    dcbi r0, r3
    addi r3, r3, 0x20
    bdnz _L_8009B51C
    mfspr r4, HID2
    rlwinm r4, r4, 0, 4, 2
    mtspr HID2, r4
    blr
}

asm void LCStoreBlocks(register void* destAddr, register void* srcAddr, register u32 nBlocks) {
    nofralloc
    extrwi r6, r5, 5, 25
    clrlwi r3, r3, 4
    or r6, r6, r3
    mtspr DMA_U, r6
    clrlslwi r6, r5, 30, 2
    or r6, r6, r4
    ori r6, r6, 0x2
    mtspr DMA_L, r6
    blr
}

asm u32 LCStoreData(register void* destAddr, register void* srcAddr, register u32 nBytes) {
    nofralloc
    mflr r0
    stw r0, 0x4(r1)
    stwu r1, -0x28(r1)
    stw r31, 0x24(r1)
    stw r30, 0x20(r1)
    stw r29, 0x1c(r1)
    stw r28, 0x18(r1)
    mr r28, r3
    mr r29, r4
    addi r0, r5, 0x1f
    srwi r3, r0, 5
    addi r0, r3, 0x7f
    mr r31, r3
    srwi r30, r0, 7
    b _L_8009B598
_L_8009B598:
    b _L_8009B59C
_L_8009B59C:
    b _L_8009B5DC
_L_8009B5A0:
    cmplwi r31, 0x80
    bge _L_8009B5C0
    mr r3, r28
    mr r4, r29
    mr r5, r31
    bl LCStoreBlocks
    li r31, 0x0
    b _L_8009B5DC
_L_8009B5C0:
    mr r3, r28
    mr r4, r29
    li r5, 0x0
    bl LCStoreBlocks
    subi r31, r31, 0x80
    addi r28, r28, 0x1000
    addi r29, r29, 0x1000
_L_8009B5DC:
    cmplwi r31, 0x0
    bne _L_8009B5A0
    mr r3, r30
    lwz r0, 0x2c(r1)
    lwz r31, 0x24(r1)
    lwz r30, 0x20(r1)
    lwz r29, 0x1c(r1)
    lwz r28, 0x18(r1)
    addi r1, r1, 0x28
    mtlr r0
    blr
}

asm u32 LCQueueLength(void) {
    nofralloc
    mfspr r4, HID2
    extrwi r3, r4, 4, 4
    blr
}

asm void LCQueueWait(register u32 len) {
    nofralloc
    mfspr r4, HID2
    extrwi r4, r4, 4, 4
    cmpw r4, r3
    bgt LCQueueWait
    blr
}
