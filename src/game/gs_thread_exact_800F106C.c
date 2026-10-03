/**
 * @file gs_thread_exact_800F106C.c
 * @brief GS VM native-call trampoline, 0x800F106C - 0x800F10E8.
 *
 * fn_800F106C calls the native function in lbl_8047AC38 with eight integer
 * arguments from lbl_8047AC40 and eight float arguments from lbl_8047AC3C.
 * It is first-party assembly: the evidence, including a compiler probe, is
 * docs/asm_evidence/gs_thread.md (registered in
 * docs/asm_evidence/registry.json).
 *
 * gs_thread.c includes this file so the candidate units built from it keep
 * seeing the same definition.
 */

#include "dolphin/types.h"

extern u32 lbl_8047AC38;
extern u32 lbl_8047AC3C;
extern u32 lbl_8047AC40;

/* First-party assembly; evidence: docs/asm_evidence/gs_thread.md */
asm void fn_800F106C(void) {
    nofralloc
    stwu r1, -0x10(r1)
    stw r0, 0x8(r1)
    mflr r0
    stw r0, 0x14(r1)
    lwz r4, lbl_8047AC38
    mtctr r4
    lwz r3, lbl_8047AC3C
    lfs f8, 0x1c(r3)
    lfs f7, 0x18(r3)
    lfs f6, 0x14(r3)
    lfs f5, 0x10(r3)
    lfs f4, 0xc(r3)
    lfs f3, 0x8(r3)
    lfs f2, 0x4(r3)
    lfs f1, 0x0(r3)
    lwz r3, lbl_8047AC40
    lwz r10, 0x1c(r3)
    lwz r9, 0x18(r3)
    lwz r8, 0x14(r3)
    lwz r7, 0x10(r3)
    lwz r6, 0xc(r3)
    lwz r5, 0x8(r3)
    lwz r4, 0x4(r3)
    lwz r3, 0x0(r3)
    crclr 6
    bctrl
    lwz r0, 0x14(r1)
    mtlr r0
    lwz r0, 0x8(r1)
    addi r1, r1, 0x10
    blr
}
