#include "dolphin/os/OSContext.h"

#define HID2 920
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

asm void OSFillFPUContext(register OSContext* context) {
    nofralloc
    mfmsr   r5
    ori     r5, r5, 0x2000
    mtmsr   r5
    isync

    stfd    fp0,  context->fpr[0]
    stfd    fp1,  context->fpr[1]
    stfd    fp2,  context->fpr[2]
    stfd    fp3,  context->fpr[3]
    stfd    fp4,  context->fpr[4]
    stfd    fp5,  context->fpr[5]
    stfd    fp6,  context->fpr[6]
    stfd    fp7,  context->fpr[7]
    stfd    fp8,  context->fpr[8]
    stfd    fp9,  context->fpr[9]
    stfd    fp10, context->fpr[10]
    stfd    fp11, context->fpr[11]
    stfd    fp12, context->fpr[12]
    stfd    fp13, context->fpr[13]
    stfd    fp14, context->fpr[14]
    stfd    fp15, context->fpr[15]
    stfd    fp16, context->fpr[16]
    stfd    fp17, context->fpr[17]
    stfd    fp18, context->fpr[18]
    stfd    fp19, context->fpr[19]
    stfd    fp20, context->fpr[20]
    stfd    fp21, context->fpr[21]
    stfd    fp22, context->fpr[22]
    stfd    fp23, context->fpr[23]
    stfd    fp24, context->fpr[24]
    stfd    fp25, context->fpr[25]
    stfd    fp26, context->fpr[26]
    stfd    fp27, context->fpr[27]
    stfd    fp28, context->fpr[28]
    stfd    fp29, context->fpr[29]
    stfd    fp30, context->fpr[30]
    stfd    fp31, context->fpr[31]

    mffs    fp0
    stfd    fp0,  OS_CONTEXT_FPSCR(context)

    lfd     fp0,  context->fpr[0]

    mfspr   r5, HID2
    rlwinm. r5, r5, 3, 31, 31
    bc      12, 2, _return

    psq_st  fp0, OS_CONTEXT_PSF0(context), 0, 0
    psq_st  fp1, OS_CONTEXT_PSF1(context), 0, 0
    psq_st  fp2, OS_CONTEXT_PSF2(context), 0, 0
    psq_st  fp3, OS_CONTEXT_PSF3(context), 0, 0
    psq_st  fp4, OS_CONTEXT_PSF4(context), 0, 0
    psq_st  fp5, OS_CONTEXT_PSF5(context), 0, 0
    psq_st  fp6, OS_CONTEXT_PSF6(context), 0, 0
    psq_st  fp7, OS_CONTEXT_PSF7(context), 0, 0
    psq_st  fp8, OS_CONTEXT_PSF8(context), 0, 0
    psq_st  fp9, OS_CONTEXT_PSF9(context), 0, 0
    psq_st  fp10, OS_CONTEXT_PSF10(context), 0, 0
    psq_st  fp11, OS_CONTEXT_PSF11(context), 0, 0
    psq_st  fp12, OS_CONTEXT_PSF12(context), 0, 0
    psq_st  fp13, OS_CONTEXT_PSF13(context), 0, 0
    psq_st  fp14, OS_CONTEXT_PSF14(context), 0, 0
    psq_st  fp15, OS_CONTEXT_PSF15(context), 0, 0
    psq_st  fp16, OS_CONTEXT_PSF16(context), 0, 0
    psq_st  fp17, OS_CONTEXT_PSF17(context), 0, 0
    psq_st  fp18, OS_CONTEXT_PSF18(context), 0, 0
    psq_st  fp19, OS_CONTEXT_PSF19(context), 0, 0
    psq_st  fp20, OS_CONTEXT_PSF20(context), 0, 0
    psq_st  fp21, OS_CONTEXT_PSF21(context), 0, 0
    psq_st  fp22, OS_CONTEXT_PSF22(context), 0, 0
    psq_st  fp23, OS_CONTEXT_PSF23(context), 0, 0
    psq_st  fp24, OS_CONTEXT_PSF24(context), 0, 0
    psq_st  fp25, OS_CONTEXT_PSF25(context), 0, 0
    psq_st  fp26, OS_CONTEXT_PSF26(context), 0, 0
    psq_st  fp27, OS_CONTEXT_PSF27(context), 0, 0
    psq_st  fp28, OS_CONTEXT_PSF28(context), 0, 0
    psq_st  fp29, OS_CONTEXT_PSF29(context), 0, 0
    psq_st  fp30, OS_CONTEXT_PSF30(context), 0, 0
    psq_st  fp31, OS_CONTEXT_PSF31(context), 0, 0

_return:
    blr
}
