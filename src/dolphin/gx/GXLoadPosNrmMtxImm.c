#include "dolphin/types.h"

#define qr0 0

asm void GXLoadPosMtxImm(register f32 mtx[3][4], register u32 id) {
    nofralloc
    lis r5, 0xcc01
    li r0, 0x10
    slwi r4, r4, 2
    stb r0, -0x8000(r5)
    oris r0, r4, 0xb
    stw r0, -0x8000(r5)
    addi r4, r5, -0x8000
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
}

asm void GXLoadNrmMtxImm(register f32 mtx[3][4], register u32 id) {
    nofralloc
    mulli r5, r4, 0x3
    lis r4, 0xcc01
    li r0, 0x10
    addi r5, r5, 0x400
    stb r0, -0x8000(r4)
    oris r0, r5, 0x8
    stwu r0, -0x8000(r4)
    psq_l fp5, 0x0(r3), 0, qr0
    lfs fp4, 0x8(r3)
    psq_l fp3, 0x10(r3), 0, qr0
    lfs fp2, 0x18(r3)
    psq_l fp1, 0x20(r3), 0, qr0
    lfs fp0, 0x28(r3)
    psq_st fp5, 0x0(r4), 0, qr0
    stfs fp4, 0x0(r4)
    psq_st fp3, 0x0(r4), 0, qr0
    stfs fp2, 0x0(r4)
    psq_st fp1, 0x0(r4), 0, qr0
    stfs fp0, 0x0(r4)
    blr
}
