/**
 * @file runtime.c
 * @brief MetroWerks PowerPC EABI runtime (Runtime.PPCEABI.H.a, runtime.c):
 *        .text 0x800C46B0 - 0x800C4D8C and its __constants, .rodata
 *        0x8026FE58 - 0x8026FE70.
 *
 * Every routine here is hand-written assembly in the MetroWerks runtime
 * library, as zeldaret/tww keeps it. Evidence for each one, routine by
 * routine: docs/asm_evidence/runtime.md.
 */

#include "dolphin/types.h"

/* Entry points inside __save_fpr/__restore_fpr/__save_gpr/__restore_gpr. */
void _savefpr_14(void);
void _savefpr_15(void);
void _savefpr_16(void);
void _savefpr_17(void);
void _savefpr_18(void);
void _savefpr_19(void);
void _savefpr_20(void);
void _savefpr_21(void);
void _savefpr_22(void);
void _savefpr_23(void);
void _savefpr_24(void);
void _savefpr_25(void);
void _savefpr_26(void);
void _savefpr_27(void);
void _savefpr_28(void);
void _savefpr_29(void);
void _savefpr_30(void);
void _savefpr_31(void);
void _restfpr_14(void);
void _restfpr_15(void);
void _restfpr_16(void);
void _restfpr_17(void);
void _restfpr_18(void);
void _restfpr_19(void);
void _restfpr_20(void);
void _restfpr_21(void);
void _restfpr_22(void);
void _restfpr_23(void);
void _restfpr_24(void);
void _restfpr_25(void);
void _restfpr_26(void);
void _restfpr_27(void);
void _restfpr_28(void);
void _restfpr_29(void);
void _restfpr_30(void);
void _restfpr_31(void);
void _savegpr_14(void);
void _savegpr_15(void);
void _savegpr_16(void);
void _savegpr_17(void);
void _savegpr_18(void);
void _savegpr_19(void);
void _savegpr_20(void);
void _savegpr_21(void);
void _savegpr_22(void);
void _savegpr_23(void);
void _savegpr_24(void);
void _savegpr_25(void);
void _savegpr_26(void);
void _savegpr_27(void);
void _savegpr_28(void);
void _savegpr_29(void);
void _savegpr_30(void);
void _savegpr_31(void);
void _restgpr_14(void);
void _restgpr_15(void);
void _restgpr_16(void);
void _restgpr_17(void);
void _restgpr_18(void);
void _restgpr_19(void);
void _restgpr_20(void);
void _restgpr_21(void);
void _restgpr_22(void);
void _restgpr_23(void);
void _restgpr_24(void);
void _restgpr_25(void);
void _restgpr_26(void);
void _restgpr_27(void);
void _restgpr_28(void);
void _restgpr_29(void);
void _restgpr_30(void);
void _restgpr_31(void);

static const u32 __constants[] = {
    0x00000000, 0x00000000, 0x41F00000, 0x00000000, 0x41E00000, 0x00000000,
};

/* __cvt_fp2unsigned: MetroWerks runtime asm; evidence: docs/asm_evidence/runtime.md */
asm u32 __cvt_fp2unsigned(register f64 d) {
    nofralloc
    stwu    r1, -0x10(r1)
    lis     r4, __constants@h
    ori     r4, r4, __constants@l
    li      r3, 0x0
    lfd     f0, 0x0(r4)
    lfd     f3, 0x8(r4)
    lfd     f4, 0x10(r4)
    fcmpu   cr0, f1, f0
    fcmpu   cr6, f1, f3
    blt     L_800C4704
    subi    r3, r3, 0x1
    bge     cr6, L_800C4704
    fcmpu   cr7, f1, f4
    fmr     f2, f1
    blt     cr7, L_800C46F0
    fsub    f2, f1, f4
L_800C46F0:
    fctiwz  f2, f2
    stfd    f2, 0x8(r1)
    lwz     r3, 0xc(r1)
    blt     cr7, L_800C4704
    addis   r3, r3, 0x8000
L_800C4704:
    addi    r1, r1, 0x10
    blr
}

/* __save_fpr: MetroWerks runtime asm; evidence: docs/asm_evidence/runtime.md */
asm void __save_fpr(void) {
    nofralloc
    entry   _savefpr_14
    stfd    f14, -0x90(r11)
    entry   _savefpr_15
    stfd    f15, -0x88(r11)
    entry   _savefpr_16
    stfd    f16, -0x80(r11)
    entry   _savefpr_17
    stfd    f17, -0x78(r11)
    entry   _savefpr_18
    stfd    f18, -0x70(r11)
    entry   _savefpr_19
    stfd    f19, -0x68(r11)
    entry   _savefpr_20
    stfd    f20, -0x60(r11)
    entry   _savefpr_21
    stfd    f21, -0x58(r11)
    entry   _savefpr_22
    stfd    f22, -0x50(r11)
    entry   _savefpr_23
    stfd    f23, -0x48(r11)
    entry   _savefpr_24
    stfd    f24, -0x40(r11)
    entry   _savefpr_25
    stfd    f25, -0x38(r11)
    entry   _savefpr_26
    stfd    f26, -0x30(r11)
    entry   _savefpr_27
    stfd    f27, -0x28(r11)
    entry   _savefpr_28
    stfd    f28, -0x20(r11)
    entry   _savefpr_29
    stfd    f29, -0x18(r11)
    entry   _savefpr_30
    stfd    f30, -0x10(r11)
    entry   _savefpr_31
    stfd    f31, -0x8(r11)
    blr
}

/* __restore_fpr: MetroWerks runtime asm; evidence: docs/asm_evidence/runtime.md */
asm void __restore_fpr(void) {
    nofralloc
    entry   _restfpr_14
    lfd     f14, -0x90(r11)
    entry   _restfpr_15
    lfd     f15, -0x88(r11)
    entry   _restfpr_16
    lfd     f16, -0x80(r11)
    entry   _restfpr_17
    lfd     f17, -0x78(r11)
    entry   _restfpr_18
    lfd     f18, -0x70(r11)
    entry   _restfpr_19
    lfd     f19, -0x68(r11)
    entry   _restfpr_20
    lfd     f20, -0x60(r11)
    entry   _restfpr_21
    lfd     f21, -0x58(r11)
    entry   _restfpr_22
    lfd     f22, -0x50(r11)
    entry   _restfpr_23
    lfd     f23, -0x48(r11)
    entry   _restfpr_24
    lfd     f24, -0x40(r11)
    entry   _restfpr_25
    lfd     f25, -0x38(r11)
    entry   _restfpr_26
    lfd     f26, -0x30(r11)
    entry   _restfpr_27
    lfd     f27, -0x28(r11)
    entry   _restfpr_28
    lfd     f28, -0x20(r11)
    entry   _restfpr_29
    lfd     f29, -0x18(r11)
    entry   _restfpr_30
    lfd     f30, -0x10(r11)
    entry   _restfpr_31
    lfd     f31, -0x8(r11)
    blr
}

/* __save_gpr: MetroWerks runtime asm; evidence: docs/asm_evidence/runtime.md */
asm void __save_gpr(void) {
    nofralloc
    entry   _savegpr_14
    stw     r14, -0x48(r11)
    entry   _savegpr_15
    stw     r15, -0x44(r11)
    entry   _savegpr_16
    stw     r16, -0x40(r11)
    entry   _savegpr_17
    stw     r17, -0x3c(r11)
    entry   _savegpr_18
    stw     r18, -0x38(r11)
    entry   _savegpr_19
    stw     r19, -0x34(r11)
    entry   _savegpr_20
    stw     r20, -0x30(r11)
    entry   _savegpr_21
    stw     r21, -0x2c(r11)
    entry   _savegpr_22
    stw     r22, -0x28(r11)
    entry   _savegpr_23
    stw     r23, -0x24(r11)
    entry   _savegpr_24
    stw     r24, -0x20(r11)
    entry   _savegpr_25
    stw     r25, -0x1c(r11)
    entry   _savegpr_26
    stw     r26, -0x18(r11)
    entry   _savegpr_27
    stw     r27, -0x14(r11)
    entry   _savegpr_28
    stw     r28, -0x10(r11)
    entry   _savegpr_29
    stw     r29, -0xc(r11)
    entry   _savegpr_30
    stw     r30, -0x8(r11)
    entry   _savegpr_31
    stw     r31, -0x4(r11)
    blr
}

/* __restore_gpr: MetroWerks runtime asm; evidence: docs/asm_evidence/runtime.md */
asm void __restore_gpr(void) {
    nofralloc
    entry   _restgpr_14
    lwz     r14, -0x48(r11)
    entry   _restgpr_15
    lwz     r15, -0x44(r11)
    entry   _restgpr_16
    lwz     r16, -0x40(r11)
    entry   _restgpr_17
    lwz     r17, -0x3c(r11)
    entry   _restgpr_18
    lwz     r18, -0x38(r11)
    entry   _restgpr_19
    lwz     r19, -0x34(r11)
    entry   _restgpr_20
    lwz     r20, -0x30(r11)
    entry   _restgpr_21
    lwz     r21, -0x2c(r11)
    entry   _restgpr_22
    lwz     r22, -0x28(r11)
    entry   _restgpr_23
    lwz     r23, -0x24(r11)
    entry   _restgpr_24
    lwz     r24, -0x20(r11)
    entry   _restgpr_25
    lwz     r25, -0x1c(r11)
    entry   _restgpr_26
    lwz     r26, -0x18(r11)
    entry   _restgpr_27
    lwz     r27, -0x14(r11)
    entry   _restgpr_28
    lwz     r28, -0x10(r11)
    entry   _restgpr_29
    lwz     r29, -0xc(r11)
    entry   _restgpr_30
    lwz     r30, -0x8(r11)
    entry   _restgpr_31
    lwz     r31, -0x4(r11)
    blr
}

/* __div2u: MetroWerks runtime asm; evidence: docs/asm_evidence/runtime.md */
asm void __div2u(void) {
    nofralloc
    cmpwi   r3, 0x0
    cntlzw  r0, r3
    cntlzw  r9, r4
    bne     L_800C4850
    addi    r0, r9, 0x20
L_800C4850:
    cmpwi   r5, 0x0
    cntlzw  r9, r5
    cntlzw  r10, r6
    bne     L_800C4864
    addi    r9, r10, 0x20
L_800C4864:
    cmpw    r0, r9
    subfic  r10, r0, 0x40
    bgt     L_800C491C
    addi    r9, r9, 0x1
    subfic  r9, r9, 0x40
    add     r0, r0, r9
    subf    r9, r9, r10
    mtctr   r9
    cmpwi   r9, 0x20
    subi    r7, r9, 0x20
    blt     L_800C489C
    srw     r8, r3, r7
    li      r7, 0x0
    b       L_800C48B0
L_800C489C:
    srw     r8, r4, r9
    subfic  r7, r9, 0x20
    slw     r7, r3, r7
    or      r8, r8, r7
    srw     r7, r3, r9
L_800C48B0:
    cmpwi   r0, 0x20
    subic   r9, r0, 0x20
    blt     L_800C48C8
    slw     r3, r4, r9
    li      r4, 0x0
    b       L_800C48DC
L_800C48C8:
    slw     r3, r3, r0
    subfic  r9, r0, 0x20
    srw     r9, r4, r9
    or      r3, r3, r9
    slw     r4, r4, r0
L_800C48DC:
    li      r10, -0x1
    addic   r7, r7, 0x0
L_800C48E4:
    adde    r4, r4, r4
    adde    r3, r3, r3
    adde    r8, r8, r8
    adde    r7, r7, r7
    subfc   r0, r6, r8
    subfe.  r9, r5, r7
    blt     L_800C490C
    mr      r8, r0
    mr      r7, r9
    addic   r0, r10, 0x1
L_800C490C:
    bdnz    L_800C48E4
    adde    r4, r4, r4
    adde    r3, r3, r3
    blr
L_800C491C:
    li      r4, 0x0
    li      r3, 0x0
    blr
}

/* __div2i: MetroWerks runtime asm; evidence: docs/asm_evidence/runtime.md */
asm void __div2i(void) {
    nofralloc
    stwu    r1, -0x10(r1)
    clrrwi. r9, r3, 31
    beq     L_800C493C
    subfic  r4, r4, 0x0
    subfze  r3, r3
L_800C493C:
    stw     r9, 0x8(r1)
    clrrwi. r10, r5, 31
    beq     L_800C4950
    subfic  r6, r6, 0x0
    subfze  r5, r5
L_800C4950:
    stw     r10, 0xc(r1)
    cmpwi   r3, 0x0
    cntlzw  r0, r3
    cntlzw  r9, r4
    bne     L_800C4968
    addi    r0, r9, 0x20
L_800C4968:
    cmpwi   r5, 0x0
    cntlzw  r9, r5
    cntlzw  r10, r6
    bne     L_800C497C
    addi    r9, r10, 0x20
L_800C497C:
    cmpw    r0, r9
    subfic  r10, r0, 0x40
    bgt     L_800C4A50
    addi    r9, r9, 0x1
    subfic  r9, r9, 0x40
    add     r0, r0, r9
    subf    r9, r9, r10
    mtctr   r9
    cmpwi   r9, 0x20
    subi    r7, r9, 0x20
    blt     L_800C49B4
    srw     r8, r3, r7
    li      r7, 0x0
    b       L_800C49C8
L_800C49B4:
    srw     r8, r4, r9
    subfic  r7, r9, 0x20
    slw     r7, r3, r7
    or      r8, r8, r7
    srw     r7, r3, r9
L_800C49C8:
    cmpwi   r0, 0x20
    subic   r9, r0, 0x20
    blt     L_800C49E0
    slw     r3, r4, r9
    li      r4, 0x0
    b       L_800C49F4
L_800C49E0:
    slw     r3, r3, r0
    subfic  r9, r0, 0x20
    srw     r9, r4, r9
    or      r3, r3, r9
    slw     r4, r4, r0
L_800C49F4:
    li      r10, -0x1
    addic   r7, r7, 0x0
L_800C49FC:
    adde    r4, r4, r4
    adde    r3, r3, r3
    adde    r8, r8, r8
    adde    r7, r7, r7
    subfc   r0, r6, r8
    subfe.  r9, r5, r7
    blt     L_800C4A24
    mr      r8, r0
    mr      r7, r9
    addic   r0, r10, 0x1
L_800C4A24:
    bdnz    L_800C49FC
    adde    r4, r4, r4
    adde    r3, r3, r3
    lwz     r9, 0x8(r1)
    lwz     r10, 0xc(r1)
    xor.    r7, r9, r10
    beq     L_800C4A58
    cmpwi   r9, 0x0
    subfic  r4, r4, 0x0
    subfze  r3, r3
    b       L_800C4A58
L_800C4A50:
    li      r4, 0x0
    li      r3, 0x0
L_800C4A58:
    addi    r1, r1, 0x10
    blr
}

/* __mod2u: MetroWerks runtime asm; evidence: docs/asm_evidence/runtime.md */
asm void __mod2u(void) {
    nofralloc
    cmpwi   r3, 0x0
    cntlzw  r0, r3
    cntlzw  r9, r4
    bne     L_800C4A74
    addi    r0, r9, 0x20
L_800C4A74:
    cmpwi   r5, 0x0
    cntlzw  r9, r5
    cntlzw  r10, r6
    bne     L_800C4A88
    addi    r9, r10, 0x20
L_800C4A88:
    cmpw    r0, r9
    subfic  r10, r0, 0x40
    bgtlr
    addi    r9, r9, 0x1
    subfic  r9, r9, 0x40
    add     r0, r0, r9
    subf    r9, r9, r10
    mtctr   r9
    cmpwi   r9, 0x20
    subi    r7, r9, 0x20
    blt     L_800C4AC0
    srw     r8, r3, r7
    li      r7, 0x0
    b       L_800C4AD4
L_800C4AC0:
    srw     r8, r4, r9
    subfic  r7, r9, 0x20
    slw     r7, r3, r7
    or      r8, r8, r7
    srw     r7, r3, r9
L_800C4AD4:
    cmpwi   r0, 0x20
    subic   r9, r0, 0x20
    blt     L_800C4AEC
    slw     r3, r4, r9
    li      r4, 0x0
    b       L_800C4B00
L_800C4AEC:
    slw     r3, r3, r0
    subfic  r9, r0, 0x20
    srw     r9, r4, r9
    or      r3, r3, r9
    slw     r4, r4, r0
L_800C4B00:
    li      r10, -0x1
    addic   r7, r7, 0x0
L_800C4B08:
    adde    r4, r4, r4
    adde    r3, r3, r3
    adde    r8, r8, r8
    adde    r7, r7, r7
    subfc   r0, r6, r8
    subfe.  r9, r5, r7
    blt     L_800C4B30
    mr      r8, r0
    mr      r7, r9
    addic   r0, r10, 0x1
L_800C4B30:
    bdnz    L_800C4B08
    mr      r4, r8
    mr      r3, r7
    blr
    blr
}

/* __mod2i: MetroWerks runtime asm; evidence: docs/asm_evidence/runtime.md */
asm void __mod2i(void) {
    nofralloc
    cmpwi   cr7, r3, 0x0
    bge     cr7, L_800C4B54
    subfic  r4, r4, 0x0
    subfze  r3, r3
L_800C4B54:
    cmpwi   r5, 0x0
    bge     L_800C4B64
    subfic  r6, r6, 0x0
    subfze  r5, r5
L_800C4B64:
    cmpwi   r3, 0x0
    cntlzw  r0, r3
    cntlzw  r9, r4
    bne     L_800C4B78
    addi    r0, r9, 0x20
L_800C4B78:
    cmpwi   r5, 0x0
    cntlzw  r9, r5
    cntlzw  r10, r6
    bne     L_800C4B8C
    addi    r9, r10, 0x20
L_800C4B8C:
    cmpw    r0, r9
    subfic  r10, r0, 0x40
    bgt     L_800C4C40
    addi    r9, r9, 0x1
    subfic  r9, r9, 0x40
    add     r0, r0, r9
    subf    r9, r9, r10
    mtctr   r9
    cmpwi   r9, 0x20
    subi    r7, r9, 0x20
    blt     L_800C4BC4
    srw     r8, r3, r7
    li      r7, 0x0
    b       L_800C4BD8
L_800C4BC4:
    srw     r8, r4, r9
    subfic  r7, r9, 0x20
    slw     r7, r3, r7
    or      r8, r8, r7
    srw     r7, r3, r9
L_800C4BD8:
    cmpwi   r0, 0x20
    subic   r9, r0, 0x20
    blt     L_800C4BF0
    slw     r3, r4, r9
    li      r4, 0x0
    b       L_800C4C04
L_800C4BF0:
    slw     r3, r3, r0
    subfic  r9, r0, 0x20
    srw     r9, r4, r9
    or      r3, r3, r9
    slw     r4, r4, r0
L_800C4C04:
    li      r10, -0x1
    addic   r7, r7, 0x0
L_800C4C0C:
    adde    r4, r4, r4
    adde    r3, r3, r3
    adde    r8, r8, r8
    adde    r7, r7, r7
    subfc   r0, r6, r8
    subfe.  r9, r5, r7
    blt     L_800C4C34
    mr      r8, r0
    mr      r7, r9
    addic   r0, r10, 0x1
L_800C4C34:
    bdnz    L_800C4C0C
    mr      r4, r8
    mr      r3, r7
L_800C4C40:
    bgelr   cr7
    subfic  r4, r4, 0x0
    subfze  r3, r3
    blr
}

/* __shl2i: MetroWerks runtime asm; evidence: docs/asm_evidence/runtime.md */
asm void __shl2i(void) {
    nofralloc
    subfic  r8, r5, 0x20
    subic   r9, r5, 0x20
    slw     r3, r3, r5
    srw     r10, r4, r8
    or      r3, r3, r10
    slw     r10, r4, r9
    or      r3, r3, r10
    slw     r4, r4, r5
    blr
}

/* __shr2u: MetroWerks runtime asm; evidence: docs/asm_evidence/runtime.md */
asm void __shr2u(void) {
    nofralloc
    subfic  r8, r5, 0x20
    subic   r9, r5, 0x20
    srw     r4, r4, r5
    slw     r10, r3, r8
    or      r4, r4, r10
    srw     r10, r3, r9
    or      r4, r4, r10
    srw     r3, r3, r5
    blr
}

/* __shr2i: MetroWerks runtime asm; evidence: docs/asm_evidence/runtime.md */
asm void __shr2i(void) {
    nofralloc
    subfic  r8, r5, 0x20
    subic.  r9, r5, 0x20
    srw     r4, r4, r5
    slw     r10, r3, r8
    or      r4, r4, r10
    sraw    r10, r3, r9
    ble     L_800C4CB8
    or      r4, r4, r10
L_800C4CB8:
    sraw    r3, r3, r5
    blr
}

/* __cvt_dbl_usll: MetroWerks runtime asm; evidence: docs/asm_evidence/runtime.md */
asm void __cvt_dbl_usll(void) {
    nofralloc
    stwu    r1, -0x10(r1)
    stfd    f1, 0x8(r1)
    lwz     r3, 0x8(r1)
    lwz     r4, 0xc(r1)
    extrwi  r5, r3, 11, 1
    cmplwi  r5, 0x3ff
    bge     L_800C4CE8
    li      r3, 0x0
    li      r4, 0x0
    b       L_800C4D84
L_800C4CE8:
    mr      r6, r3
    clrlwi  r3, r3, 12
    oris    r3, r3, 0x10
    subi    r5, r5, 0x433
    cmpwi   r5, 0x0
    bge     L_800C4D28
    neg     r5, r5
    subfic  r8, r5, 0x20
    subic   r9, r5, 0x20
    srw     r4, r4, r5
    slw     r10, r3, r8
    or      r4, r4, r10
    srw     r10, r3, r9
    or      r4, r4, r10
    srw     r3, r3, r5
    b       L_800C4D74
L_800C4D28:
    cmpwi   r5, 0xa
    ble+    L_800C4D54
    clrrwi. r6, r6, 31
    beq     L_800C4D44
    lis     r3, 0x8000
    li      r4, 0x0
    b       L_800C4D84
L_800C4D44:
    lis     r3, 0x7fff
    ori     r3, r3, 0xffff
    li      r4, -0x1
    b       L_800C4D84
L_800C4D54:
    subfic  r8, r5, 0x20
    subic   r9, r5, 0x20
    slw     r3, r3, r5
    srw     r10, r4, r8
    or      r3, r3, r10
    slw     r10, r4, r9
    or      r3, r3, r10
    slw     r4, r4, r5
L_800C4D74:
    clrrwi. r6, r6, 31
    beq     L_800C4D84
    subfic  r4, r4, 0x0
    subfze  r3, r3
L_800C4D84:
    addi    r1, r1, 0x10
    blr
}
