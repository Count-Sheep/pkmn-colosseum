#include "dolphin/types.h"

typedef f32 Mtx[3][4];

typedef struct Vec {
    f32 x;
    f32 y;
    f32 z;
} Vec;

void PSVECNormalize(const Vec* source, Vec* destination);
void PSVECCrossProduct(const Vec* first, const Vec* second, Vec* destination);

/* The readable C draft remains in sdk_range_800A2D38.c. */
asm void C_MTXLookAt(register Mtx m, register const Vec* cameraPosition,
                     register const Vec* cameraUp, register const Vec* target) {
    nofralloc
    mflr r0
    stw r0, 0x4(r1)
    stwu r1, -0x50(r1)
    stw r31, 0x4c(r1)
    stw r30, 0x48(r1)
    stw r29, 0x44(r1)
    mr r29, r3
    mr r30, r4
    mr r31, r5
    lfs fp1, 0x0(r30)
    addi r3, r1, 0x30
    lfs fp0, 0x0(r6)
    mr r4, r3
    fsubs fp0, fp1, fp0
    stfs fp0, 0x30(r1)
    lfs fp1, 0x4(r30)
    lfs fp0, 0x4(r6)
    fsubs fp0, fp1, fp0
    stfs fp0, 0x34(r1)
    lfs fp1, 0x8(r30)
    lfs fp0, 0x8(r6)
    fsubs fp0, fp1, fp0
    stfs fp0, 0x38(r1)
    bl PSVECNormalize
    mr r3, r31
    addi r4, r1, 0x30
    addi r5, r1, 0x24
    bl PSVECCrossProduct
    addi r3, r1, 0x24
    mr r4, r3
    bl PSVECNormalize
    addi r3, r1, 0x30
    addi r4, r1, 0x24
    addi r5, r1, 0x18
    bl PSVECCrossProduct
    lfs fp0, 0x24(r1)
    stfs fp0, 0x0(r29)
    lfs fp0, 0x28(r1)
    stfs fp0, 0x4(r29)
    lfs fp0, 0x2c(r1)
    stfs fp0, 0x8(r29)
    lfs fp3, 0x0(r30)
    lfs fp2, 0x24(r1)
    lfs fp1, 0x4(r30)
    lfs fp0, 0x28(r1)
    fmuls fp2, fp3, fp2
    lfs fp3, 0x8(r30)
    fmuls fp0, fp1, fp0
    lfs fp1, 0x2c(r1)
    fmuls fp1, fp3, fp1
    fadds fp0, fp2, fp0
    fadds fp0, fp1, fp0
    fneg fp0, fp0
    stfs fp0, 0xc(r29)
    lfs fp0, 0x18(r1)
    stfs fp0, 0x10(r29)
    lfs fp0, 0x1c(r1)
    stfs fp0, 0x14(r29)
    lfs fp0, 0x20(r1)
    stfs fp0, 0x18(r29)
    lfs fp3, 0x0(r30)
    lfs fp2, 0x18(r1)
    lfs fp1, 0x4(r30)
    lfs fp0, 0x1c(r1)
    fmuls fp2, fp3, fp2
    lfs fp3, 0x8(r30)
    fmuls fp0, fp1, fp0
    lfs fp1, 0x20(r1)
    fmuls fp1, fp3, fp1
    fadds fp0, fp2, fp0
    fadds fp0, fp1, fp0
    fneg fp0, fp0
    stfs fp0, 0x1c(r29)
    lfs fp0, 0x30(r1)
    stfs fp0, 0x20(r29)
    lfs fp0, 0x34(r1)
    stfs fp0, 0x24(r29)
    lfs fp0, 0x38(r1)
    stfs fp0, 0x28(r29)
    lfs fp3, 0x0(r30)
    lfs fp2, 0x30(r1)
    lfs fp1, 0x4(r30)
    lfs fp0, 0x34(r1)
    fmuls fp2, fp3, fp2
    lfs fp3, 0x8(r30)
    fmuls fp0, fp1, fp0
    lfs fp1, 0x38(r1)
    fmuls fp1, fp3, fp1
    fadds fp0, fp2, fp0
    fadds fp0, fp1, fp0
    fneg fp0, fp0
    stfs fp0, 0x2c(r29)
    lwz r0, 0x54(r1)
    lwz r31, 0x4c(r1)
    lwz r30, 0x48(r1)
    lwz r29, 0x44(r1)
    addi r1, r1, 0x50
    mtlr r0
    blr
}
