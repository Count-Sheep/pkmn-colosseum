#include "dolphin/types.h"

#define qr0 0

asm void GXLoadTexMtxImm(register f32 mtx[3][4], register u32 id,
                         register s32 type) {
    nofralloc
    cmplwi r4, 0x40
    blt _L_800BD5A4
    subi r0, r4, 0x40
    slwi r4, r0, 2
    addi r7, r4, 0x500
    b _L_800BD5A8
_L_800BD5A4:
    slwi r7, r4, 2
_L_800BD5A8:
    cmpwi r5, 0x1
    bne _L_800BD5B8
    li r4, 0x8
    b _L_800BD5BC
_L_800BD5B8:
    li r4, 0xc
_L_800BD5BC:
    subi r0, r4, 0x1
    slwi r6, r0, 16
    li r0, 0x10
    lis r4, 0xcc01
    stb r0, -0x8000(r4)
    or r0, r7, r6
    cmpwi r5, 0x0
    stw r0, -0x8000(r4)
    bne _L_800BD618
    addi r4, r4, -0x8000
    psq_l fp5, 0x0(r3), 0, qr0
    psq_l fp4, 0x8(r3), 0, qr0
    psq_l fp3, 0x10(r3), 0, qr0
    psq_l fp2, 0x18(r3), 0, qr0
    psq_l fp1, 0x20(r3), 0, qr0
    psq_l fp0, 0x28(r3), 0, qr0
    psq_st fp5, 0x0(r4), 0, qr0
    psq_st fp4, 0x0(r4), 0, qr0
    psq_st fp3, 0x0(r4), 0, qr0
    psq_st fp2, 0x0(r4), 0, qr0
    psq_st fp1, 0x0(r4), 0, qr0
    psq_st fp0, 0x0(r4), 0, qr0
    blr
_L_800BD618:
    addi r4, r4, -0x8000
    psq_l fp3, 0x0(r3), 0, qr0
    psq_l fp2, 0x8(r3), 0, qr0
    psq_l fp1, 0x10(r3), 0, qr0
    psq_l fp0, 0x18(r3), 0, qr0
    psq_st fp3, 0x0(r4), 0, qr0
    psq_st fp2, 0x0(r4), 0, qr0
    psq_st fp1, 0x0(r4), 0, qr0
    psq_st fp0, 0x0(r4), 0, qr0
    blr
}
