#include "dolphin/types.h"

asm u32 SetInterruptMask(register u32 mask, register u32 current) {
    nofralloc
    cntlzw r0, r3
    cmpwi r0, 0xc
    bge _L_8009E058
    cmpwi r0, 0x8
    beq _L_8009E138
    bge _L_8009E168
    cmpwi r0, 0x5
    bge _L_8009E0E4
    cmpwi r0, 0x0
    bge _L_8009E078
    b _L_8009E300
_L_8009E058:
    cmpwi r0, 0x11
    bge _L_8009E06C
    cmpwi r0, 0xf
    bge _L_8009E20C
    b _L_8009E1B8
_L_8009E06C:
    cmpwi r0, 0x1b
    bge _L_8009E300
    b _L_8009E24C
_L_8009E078:
    clrrwi r0, r4, 31
    cmplwi r0, 0x0
    li r5, 0x0
    bne _L_8009E08C
    ori r5, r5, 0x1
_L_8009E08C:
    rlwinm r0, r4, 0, 1, 1
    cmplwi r0, 0x0
    bne _L_8009E09C
    ori r5, r5, 0x2
_L_8009E09C:
    rlwinm r0, r4, 0, 2, 2
    cmplwi r0, 0x0
    bne _L_8009E0AC
    ori r5, r5, 0x4
_L_8009E0AC:
    rlwinm r0, r4, 0, 3, 3
    cmplwi r0, 0x0
    bne _L_8009E0BC
    ori r5, r5, 0x8
_L_8009E0BC:
    rlwinm r0, r4, 0, 4, 4
    cmplwi r0, 0x0
    bne _L_8009E0CC
    ori r5, r5, 0x10
_L_8009E0CC:
    lis r4, 0xcc00
    clrlwi r0, r5, 16
    addi r4, r4, 0x4000
    sth r0, 0x1c(r4)
    clrlwi r3, r3, 5
    b _L_8009E300
_L_8009E0E4:
    lis r5, 0xcc00
    addi r5, r5, 0x5000
    addi r5, r5, 0xa
    rlwinm r0, r4, 0, 5, 5
    lhz r6, 0x0(r5)
    cmplwi r0, 0x0
    rlwinm r6, r6, 0, 29, 22
    bne _L_8009E108
    ori r6, r6, 0x10
_L_8009E108:
    rlwinm r0, r4, 0, 6, 6
    cmplwi r0, 0x0
    bne _L_8009E118
    ori r6, r6, 0x40
_L_8009E118:
    rlwinm r0, r4, 0, 7, 7
    cmplwi r0, 0x0
    bne _L_8009E128
    ori r6, r6, 0x100
_L_8009E128:
    clrlwi r0, r6, 16
    sth r0, 0x0(r5)
    rlwinm r3, r3, 0, 8, 4
    b _L_8009E300
_L_8009E138:
    rlwinm r0, r4, 0, 8, 8
    lis r4, 0xcc00
    cmplwi r0, 0x0
    lwz r5, 0x6c00(r4)
    li r0, -0x2d
    and r5, r5, r0
    bne _L_8009E158
    ori r5, r5, 0x4
_L_8009E158:
    lis r4, 0xcc00
    stw r5, 0x6c00(r4)
    rlwinm r3, r3, 0, 9, 7
    b _L_8009E300
_L_8009E168:
    rlwinm r0, r4, 0, 9, 9
    lis r5, 0xcc00
    cmplwi r0, 0x0
    lwz r5, 0x6800(r5)
    li r0, -0x2c10
    and r5, r5, r0
    bne _L_8009E188
    ori r5, r5, 0x1
_L_8009E188:
    rlwinm r0, r4, 0, 10, 10
    cmplwi r0, 0x0
    bne _L_8009E198
    ori r5, r5, 0x4
_L_8009E198:
    rlwinm r0, r4, 0, 11, 11
    cmplwi r0, 0x0
    bne _L_8009E1A8
    ori r5, r5, 0x400
_L_8009E1A8:
    lis r4, 0xcc00
    stw r5, 0x6800(r4)
    rlwinm r3, r3, 0, 12, 8
    b _L_8009E300
_L_8009E1B8:
    lis r5, 0xcc00
    addi r6, r5, 0x6800
    addi r6, r6, 0x14
    rlwinm r0, r4, 0, 12, 12
    lwz r7, 0x0(r6)
    li r5, -0xc10
    cmplwi r0, 0x0
    and r7, r7, r5
    bne _L_8009E1E0
    ori r7, r7, 0x1
_L_8009E1E0:
    rlwinm r0, r4, 0, 13, 13
    cmplwi r0, 0x0
    bne _L_8009E1F0
    ori r7, r7, 0x4
_L_8009E1F0:
    rlwinm r0, r4, 0, 14, 14
    cmplwi r0, 0x0
    bne _L_8009E200
    ori r7, r7, 0x400
_L_8009E200:
    stw r7, 0x0(r6)
    rlwinm r3, r3, 0, 15, 11
    b _L_8009E300
_L_8009E20C:
    lis r5, 0xcc00
    addi r5, r5, 0x6800
    addi r5, r5, 0x28
    rlwinm r0, r4, 0, 15, 15
    lwz r6, 0x0(r5)
    cmplwi r0, 0x0
    clrrwi r6, r6, 4
    bne _L_8009E230
    ori r6, r6, 0x1
_L_8009E230:
    rlwinm r0, r4, 0, 16, 16
    cmplwi r0, 0x0
    bne _L_8009E240
    ori r6, r6, 0x4
_L_8009E240:
    stw r6, 0x0(r5)
    rlwinm r3, r3, 0, 17, 14
    b _L_8009E300
_L_8009E24C:
    rlwinm r0, r4, 0, 17, 17
    cmplwi r0, 0x0
    li r5, 0xf0
    bne _L_8009E260
    ori r5, r5, 0x800
_L_8009E260:
    rlwinm r0, r4, 0, 20, 20
    cmplwi r0, 0x0
    bne _L_8009E270
    ori r5, r5, 0x8
_L_8009E270:
    rlwinm r0, r4, 0, 21, 21
    cmplwi r0, 0x0
    bne _L_8009E280
    ori r5, r5, 0x4
_L_8009E280:
    rlwinm r0, r4, 0, 22, 22
    cmplwi r0, 0x0
    bne _L_8009E290
    ori r5, r5, 0x2
_L_8009E290:
    rlwinm r0, r4, 0, 23, 23
    cmplwi r0, 0x0
    bne _L_8009E2A0
    ori r5, r5, 0x1
_L_8009E2A0:
    rlwinm r0, r4, 0, 24, 24
    cmplwi r0, 0x0
    bne _L_8009E2B0
    ori r5, r5, 0x100
_L_8009E2B0:
    rlwinm r0, r4, 0, 25, 25
    cmplwi r0, 0x0
    bne _L_8009E2C0
    ori r5, r5, 0x1000
_L_8009E2C0:
    rlwinm r0, r4, 0, 18, 18
    cmplwi r0, 0x0
    bne _L_8009E2D0
    ori r5, r5, 0x200
_L_8009E2D0:
    rlwinm r0, r4, 0, 19, 19
    cmplwi r0, 0x0
    bne _L_8009E2E0
    ori r5, r5, 0x400
_L_8009E2E0:
    rlwinm r0, r4, 0, 26, 26
    cmplwi r0, 0x0
    bne _L_8009E2F0
    ori r5, r5, 0x2000
_L_8009E2F0:
    lis r4, 0xcc00
    addi r4, r4, 0x3000
    stw r5, 0x4(r4)
    rlwinm r3, r3, 0, 27, 16
_L_8009E300:
    blr
}
