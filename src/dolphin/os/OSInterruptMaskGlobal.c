#include "dolphin/types.h"

u32 OSDisableInterrupts(void);
u32 OSRestoreInterrupts(u32 level);
u32 SetInterruptMask(u32 mask, u32 current);

asm u32 __OSMaskInterrupts(register u32 global) {
    nofralloc
    mflr r0
    stw r0, 0x4(r1)
    stwu r1, -0x20(r1)
    stw r31, 0x1c(r1)
    stw r30, 0x18(r1)
    stw r29, 0x14(r1)
    mr r31, r3
    bl OSDisableInterrupts
    lis r4, 0x8000
    lwz r29, 0xc4(r4)
    mr r30, r3
    lwz r5, 0xc8(r4)
    or r0, r29, r5
    andc r3, r31, r0
    or r31, r31, r29
    stw r31, 0xc4(r4)
    or r31, r31, r5
    b _L_8009E34C
_L_8009E34C:
    b _L_8009E350
_L_8009E350:
    b _L_8009E35C
_L_8009E354:
    mr r4, r31
    bl SetInterruptMask
_L_8009E35C:
    cmplwi r3, 0x0
    bne _L_8009E354
    mr r3, r30
    bl OSRestoreInterrupts
    mr r3, r29
    lwz r0, 0x24(r1)
    lwz r31, 0x1c(r1)
    lwz r30, 0x18(r1)
    lwz r29, 0x14(r1)
    addi r1, r1, 0x20
    mtlr r0
    blr
}
asm u32 __OSUnmaskInterrupts(register u32 global) {
    nofralloc
    mflr r0
    stw r0, 0x4(r1)
    stwu r1, -0x20(r1)
    stw r31, 0x1c(r1)
    stw r30, 0x18(r1)
    stw r29, 0x14(r1)
    mr r31, r3
    bl OSDisableInterrupts
    lis r4, 0x8000
    lwz r29, 0xc4(r4)
    mr r30, r3
    lwz r5, 0xc8(r4)
    or r0, r29, r5
    and r3, r31, r0
    andc r31, r29, r31
    stw r31, 0xc4(r4)
    or r31, r31, r5
    b _L_8009E3D4
_L_8009E3D4:
    b _L_8009E3D8
_L_8009E3D8:
    b _L_8009E3E4
_L_8009E3DC:
    mr r4, r31
    bl SetInterruptMask
_L_8009E3E4:
    cmplwi r3, 0x0
    bne _L_8009E3DC
    mr r3, r30
    bl OSRestoreInterrupts
    mr r3, r29
    lwz r0, 0x24(r1)
    lwz r31, 0x1c(r1)
    lwz r30, 0x18(r1)
    lwz r29, 0x14(r1)
    addi r1, r1, 0x20
    mtlr r0
    blr
}
