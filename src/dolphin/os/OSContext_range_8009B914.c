#include "dolphin/os/OSContext.h"

#define HID2 920
#define OS_CACHED_REGION_PREFIX 0x8000
#define OS_CONTEXT_FPSCR 400
#define OS_CONTEXT_PSF0 456
#define OS_CONTEXT_PSF1 464
#define OS_CONTEXT_PSF2 472
#define OS_CONTEXT_PSF3 480
#define OS_CONTEXT_PSF4 488
#define OS_CONTEXT_PSF5 496
#define OS_CONTEXT_PSF6 504
#define OS_CONTEXT_PSF7 512
#define OS_CONTEXT_PSF8 520
#define OS_CONTEXT_PSF9 528
#define OS_CONTEXT_PSF10 536
#define OS_CONTEXT_PSF11 544
#define OS_CONTEXT_PSF12 552
#define OS_CONTEXT_PSF13 560
#define OS_CONTEXT_PSF14 568
#define OS_CONTEXT_PSF15 576
#define OS_CONTEXT_PSF16 584
#define OS_CONTEXT_PSF17 592
#define OS_CONTEXT_PSF18 600
#define OS_CONTEXT_PSF19 608
#define OS_CONTEXT_PSF20 616
#define OS_CONTEXT_PSF21 624
#define OS_CONTEXT_PSF22 632
#define OS_CONTEXT_PSF23 640
#define OS_CONTEXT_PSF24 648
#define OS_CONTEXT_PSF25 656
#define OS_CONTEXT_PSF26 664
#define OS_CONTEXT_PSF27 672
#define OS_CONTEXT_PSF28 680
#define OS_CONTEXT_PSF29 688
#define OS_CONTEXT_PSF30 696
#define OS_CONTEXT_PSF31 704

asm void __OSLoadFPUContext(register u32 dummy, register OSContext* fpucontext) {
    nofralloc
    lhz r5, fpucontext->state;
    clrlwi. r5, r5, 31
    beq _return

    lfd fp0, OS_CONTEXT_FPSCR(fpucontext)
    mtfsf 0xFF, fp0
    mfspr r5, HID2
    rlwinm. r5, r5, 3, 31, 31
    beq _regular_FPRs

    psq_l fp0, OS_CONTEXT_PSF0(fpucontext), 0, 0
    psq_l fp1, OS_CONTEXT_PSF1(fpucontext), 0, 0
    psq_l fp2, OS_CONTEXT_PSF2(fpucontext), 0, 0
    psq_l fp3, OS_CONTEXT_PSF3(fpucontext), 0, 0
    psq_l fp4, OS_CONTEXT_PSF4(fpucontext), 0, 0
    psq_l fp5, OS_CONTEXT_PSF5(fpucontext), 0, 0
    psq_l fp6, OS_CONTEXT_PSF6(fpucontext), 0, 0
    psq_l fp7, OS_CONTEXT_PSF7(fpucontext), 0, 0
    psq_l fp8, OS_CONTEXT_PSF8(fpucontext), 0, 0
    psq_l fp9, OS_CONTEXT_PSF9(fpucontext), 0, 0
    psq_l fp10, OS_CONTEXT_PSF10(fpucontext), 0, 0
    psq_l fp11, OS_CONTEXT_PSF11(fpucontext), 0, 0
    psq_l fp12, OS_CONTEXT_PSF12(fpucontext), 0, 0
    psq_l fp13, OS_CONTEXT_PSF13(fpucontext), 0, 0
    psq_l fp14, OS_CONTEXT_PSF14(fpucontext), 0, 0
    psq_l fp15, OS_CONTEXT_PSF15(fpucontext), 0, 0
    psq_l fp16, OS_CONTEXT_PSF16(fpucontext), 0, 0
    psq_l fp17, OS_CONTEXT_PSF17(fpucontext), 0, 0
    psq_l fp18, OS_CONTEXT_PSF18(fpucontext), 0, 0
    psq_l fp19, OS_CONTEXT_PSF19(fpucontext), 0, 0
    psq_l fp20, OS_CONTEXT_PSF20(fpucontext), 0, 0
    psq_l fp21, OS_CONTEXT_PSF21(fpucontext), 0, 0
    psq_l fp22, OS_CONTEXT_PSF22(fpucontext), 0, 0
    psq_l fp23, OS_CONTEXT_PSF23(fpucontext), 0, 0
    psq_l fp24, OS_CONTEXT_PSF24(fpucontext), 0, 0
    psq_l fp25, OS_CONTEXT_PSF25(fpucontext), 0, 0
    psq_l fp26, OS_CONTEXT_PSF26(fpucontext), 0, 0
    psq_l fp27, OS_CONTEXT_PSF27(fpucontext), 0, 0
    psq_l fp28, OS_CONTEXT_PSF28(fpucontext), 0, 0
    psq_l fp29, OS_CONTEXT_PSF29(fpucontext), 0, 0
    psq_l fp30, OS_CONTEXT_PSF30(fpucontext), 0, 0
    psq_l fp31, OS_CONTEXT_PSF31(fpucontext), 0, 0

_regular_FPRs:
    lfd fp0,  fpucontext->fpr[0]
    lfd fp1,  fpucontext->fpr[1]
    lfd fp2,  fpucontext->fpr[2]
    lfd fp3,  fpucontext->fpr[3]
    lfd fp4,  fpucontext->fpr[4]
    lfd fp5,  fpucontext->fpr[5]
    lfd fp6,  fpucontext->fpr[6]
    lfd fp7,  fpucontext->fpr[7]
    lfd fp8,  fpucontext->fpr[8]
    lfd fp9,  fpucontext->fpr[9]
    lfd fp10, fpucontext->fpr[10]
    lfd fp11, fpucontext->fpr[11]
    lfd fp12, fpucontext->fpr[12]
    lfd fp13, fpucontext->fpr[13]
    lfd fp14, fpucontext->fpr[14]
    lfd fp15, fpucontext->fpr[15]
    lfd fp16, fpucontext->fpr[16]
    lfd fp17, fpucontext->fpr[17]
    lfd fp18, fpucontext->fpr[18]
    lfd fp19, fpucontext->fpr[19]
    lfd fp20, fpucontext->fpr[20]
    lfd fp21, fpucontext->fpr[21]
    lfd fp22, fpucontext->fpr[22]
    lfd fp23, fpucontext->fpr[23]
    lfd fp24, fpucontext->fpr[24]
    lfd fp25, fpucontext->fpr[25]
    lfd fp26, fpucontext->fpr[26]
    lfd fp27, fpucontext->fpr[27]
    lfd fp28, fpucontext->fpr[28]
    lfd fp29, fpucontext->fpr[29]
    lfd fp30, fpucontext->fpr[30]
    lfd fp31, fpucontext->fpr[31]
_return:
    blr
}
asm void __OSSaveFPUContext(register u32 dummy1, register u32 dummy2, register OSContext* fpucontext) {
    nofralloc

    lhz     r3,   fpucontext->state
    ori     r3,   r3, 1
    sth     r3,   fpucontext->state

    stfd    fp0,  fpucontext->fpr[0]
    stfd    fp1,  fpucontext->fpr[1]
    stfd    fp2,  fpucontext->fpr[2]
    stfd    fp3,  fpucontext->fpr[3]
    stfd    fp4,  fpucontext->fpr[4]
    stfd    fp5,  fpucontext->fpr[5]
    stfd    fp6,  fpucontext->fpr[6]
    stfd    fp7,  fpucontext->fpr[7]
    stfd    fp8,  fpucontext->fpr[8]
    stfd    fp9,  fpucontext->fpr[9]
    stfd    fp10, fpucontext->fpr[10]
    stfd    fp11, fpucontext->fpr[11]
    stfd    fp12, fpucontext->fpr[12]
    stfd    fp13, fpucontext->fpr[13]
    stfd    fp14, fpucontext->fpr[14]
    stfd    fp15, fpucontext->fpr[15]
    stfd    fp16, fpucontext->fpr[16]
    stfd    fp17, fpucontext->fpr[17]
    stfd    fp18, fpucontext->fpr[18]
    stfd    fp19, fpucontext->fpr[19]
    stfd    fp20, fpucontext->fpr[20]
    stfd    fp21, fpucontext->fpr[21]
    stfd    fp22, fpucontext->fpr[22]
    stfd    fp23, fpucontext->fpr[23]
    stfd    fp24, fpucontext->fpr[24]
    stfd    fp25, fpucontext->fpr[25]
    stfd    fp26, fpucontext->fpr[26]
    stfd    fp27, fpucontext->fpr[27]
    stfd    fp28, fpucontext->fpr[28]
    stfd    fp29, fpucontext->fpr[29]
    stfd    fp30, fpucontext->fpr[30]
    stfd    fp31, fpucontext->fpr[31]

    mffs    fp0
    stfd    fp0,  OS_CONTEXT_FPSCR(fpucontext)

    lfd     fp0,  fpucontext->fpr[0]

    mfspr   r3, HID2
    rlwinm. r3, r3, 3, 31, 31
    bc      12, 2, _return

    psq_st  fp0, OS_CONTEXT_PSF0(fpucontext), 0, 0
    psq_st  fp1, OS_CONTEXT_PSF1(fpucontext), 0, 0
    psq_st  fp2, OS_CONTEXT_PSF2(fpucontext), 0, 0
    psq_st  fp3, OS_CONTEXT_PSF3(fpucontext), 0, 0
    psq_st  fp4, OS_CONTEXT_PSF4(fpucontext), 0, 0
    psq_st  fp5, OS_CONTEXT_PSF5(fpucontext), 0, 0
    psq_st  fp6, OS_CONTEXT_PSF6(fpucontext), 0, 0
    psq_st  fp7, OS_CONTEXT_PSF7(fpucontext), 0, 0
    psq_st  fp8, OS_CONTEXT_PSF8(fpucontext), 0, 0
    psq_st  fp9, OS_CONTEXT_PSF9(fpucontext), 0, 0
    psq_st  fp10, OS_CONTEXT_PSF10(fpucontext), 0, 0
    psq_st  fp11, OS_CONTEXT_PSF11(fpucontext), 0, 0
    psq_st  fp12, OS_CONTEXT_PSF12(fpucontext), 0, 0
    psq_st  fp13, OS_CONTEXT_PSF13(fpucontext), 0, 0
    psq_st  fp14, OS_CONTEXT_PSF14(fpucontext), 0, 0
    psq_st  fp15, OS_CONTEXT_PSF15(fpucontext), 0, 0
    psq_st  fp16, OS_CONTEXT_PSF16(fpucontext), 0, 0
    psq_st  fp17, OS_CONTEXT_PSF17(fpucontext), 0, 0
    psq_st  fp18, OS_CONTEXT_PSF18(fpucontext), 0, 0
    psq_st  fp19, OS_CONTEXT_PSF19(fpucontext), 0, 0
    psq_st  fp20, OS_CONTEXT_PSF20(fpucontext), 0, 0
    psq_st  fp21, OS_CONTEXT_PSF21(fpucontext), 0, 0
    psq_st  fp22, OS_CONTEXT_PSF22(fpucontext), 0, 0
    psq_st  fp23, OS_CONTEXT_PSF23(fpucontext), 0, 0
    psq_st  fp24, OS_CONTEXT_PSF24(fpucontext), 0, 0
    psq_st  fp25, OS_CONTEXT_PSF25(fpucontext), 0, 0
    psq_st  fp26, OS_CONTEXT_PSF26(fpucontext), 0, 0
    psq_st  fp27, OS_CONTEXT_PSF27(fpucontext), 0, 0
    psq_st  fp28, OS_CONTEXT_PSF28(fpucontext), 0, 0
    psq_st  fp29, OS_CONTEXT_PSF29(fpucontext), 0, 0
    psq_st  fp30, OS_CONTEXT_PSF30(fpucontext), 0, 0
    psq_st  fp31, OS_CONTEXT_PSF31(fpucontext), 0, 0

_return:
    blr
}
asm void OSSaveFPUContext(register OSContext* fpucontext) {
    nofralloc
    addi    r5, fpucontext, 0
    b       __OSSaveFPUContext
}

asm void OSSetCurrentContext(register OSContext* context){
    nofralloc

    addis   r4, r0, OS_CACHED_REGION_PREFIX

    stw     context, 0x00D4(r4)

    clrlwi  r5, context, 2
    stw     r5, 0x00C0(r4)

    lwz     r5, 0x00D8(r4)
    cmpw    r5, context
    bne     _disableFPU

    lwz     r6, context->srr1
    ori     r6, r6, 0x2000
    stw     r6, context->srr1
    mfmsr   r6
    ori     r6, r6, 2
    mtmsr   r6
    blr

_disableFPU:
    lwz     r6, context->srr1
    rlwinm  r6, r6, 0, 19, 17
    stw     r6, context->srr1
    mfmsr   r6
    rlwinm  r6, r6, 0, 19, 17
    ori     r6, r6, 2
    mtmsr   r6
    isync
    blr
}
