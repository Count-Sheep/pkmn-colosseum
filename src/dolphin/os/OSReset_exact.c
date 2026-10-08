#include "dolphin/types.h"

asm void Reset_8009FF50(register u32 resetCode) {
    nofralloc
    b skip1
cache:
    mfspr r8, HID0
    ori r8, r8, 0x8
    mtspr HID0, r8
    isync
    sync
    nop
    b wait
skip1:
    b skip2
wait:
    mftb r5, 268
waitloop:
    mftb r6, 268
    subf r7, r5, r6
    cmplwi r7, 0x1124
    blt waitloop
    nop
    b reset
skip2:
    b skip3
reset:
    lis r8, 0xcc00
    ori r8, r8, 0x3000
    li r4, 0x3
    stw r4, 0x24(r8)
    stw resetCode, 0x24(r8)
    nop
    b hang
skip3:
    b hang2
hang:
    nop
    b hang
hang2:
    b cache
}
