#include "dolphin/os/OSContext.h"

#define OS_CONTEXT_R1 4
#define OS_CONTEXT_R2 8
#define OS_CONTEXT_R3 12
#define OS_CONTEXT_R4 16
#define OS_CONTEXT_R5 20
#define OS_CONTEXT_R6 24
#define OS_CONTEXT_R7 28
#define OS_CONTEXT_R8 32
#define OS_CONTEXT_R9 36
#define OS_CONTEXT_R10 40
#define OS_CONTEXT_R11 44
#define OS_CONTEXT_R12 48
#define OS_CONTEXT_R13 52
#define OS_CONTEXT_R14 56
#define OS_CONTEXT_R15 60
#define OS_CONTEXT_R16 64
#define OS_CONTEXT_R17 68
#define OS_CONTEXT_R18 72
#define OS_CONTEXT_R19 76
#define OS_CONTEXT_R20 80
#define OS_CONTEXT_R21 84
#define OS_CONTEXT_R22 88
#define OS_CONTEXT_R23 92
#define OS_CONTEXT_R24 96
#define OS_CONTEXT_R25 100
#define OS_CONTEXT_R26 104
#define OS_CONTEXT_R27 108
#define OS_CONTEXT_R28 112
#define OS_CONTEXT_R29 116
#define OS_CONTEXT_R30 120
#define OS_CONTEXT_R31 124
#define OS_CONTEXT_CR 128
#define OS_CONTEXT_LR 132
#define OS_CONTEXT_CTR 136
#define OS_CONTEXT_XER 140
#define OS_CONTEXT_FPSCR 400
#define OS_CONTEXT_SRR0 408
#define OS_CONTEXT_SRR1 412
#define OS_CONTEXT_GQR0 420
#define OS_CONTEXT_GQR1 424
#define OS_CONTEXT_GQR2 428
#define OS_CONTEXT_GQR3 432
#define OS_CONTEXT_GQR4 436
#define OS_CONTEXT_GQR5 440
#define OS_CONTEXT_GQR6 444
#define OS_CONTEXT_GQR7 448

asm void OSInitContext(register OSContext* context, register u32 pc, register u32 newsp) {
    nofralloc

    stw     pc,  OS_CONTEXT_SRR0(context)
    stw     newsp,  OS_CONTEXT_R1(context)
    li      r11, 0
    ori     r11, r11, 0x00008000 | 0x00000020 | 0x00000010 | 0x00000002 | 0x00001000
    stw     r11, OS_CONTEXT_SRR1(context)
    li      r0,  0x0
    stw     r0,  OS_CONTEXT_CR(context)
    stw     r0,  OS_CONTEXT_XER(context)


    stw     r2,  OS_CONTEXT_R2(context)
    stw     r13, OS_CONTEXT_R13(context)

    stw     r0,  OS_CONTEXT_R3(context)
    stw     r0,  OS_CONTEXT_R4(context)
    stw     r0,  OS_CONTEXT_R5(context)
    stw     r0,  OS_CONTEXT_R6(context)
    stw     r0,  OS_CONTEXT_R7(context)
    stw     r0,  OS_CONTEXT_R8(context)
    stw     r0,  OS_CONTEXT_R9(context)
    stw     r0,  OS_CONTEXT_R10(context)
    stw     r0,  OS_CONTEXT_R11(context)
    stw     r0,  OS_CONTEXT_R12(context)

    stw     r0,  OS_CONTEXT_R14(context)
    stw     r0,  OS_CONTEXT_R15(context)
    stw     r0,  OS_CONTEXT_R16(context)
    stw     r0,  OS_CONTEXT_R17(context)
    stw     r0,  OS_CONTEXT_R18(context)
    stw     r0,  OS_CONTEXT_R19(context)
    stw     r0,  OS_CONTEXT_R20(context)
    stw     r0,  OS_CONTEXT_R21(context)
    stw     r0,  OS_CONTEXT_R22(context)
    stw     r0,  OS_CONTEXT_R23(context)
    stw     r0,  OS_CONTEXT_R24(context)
    stw     r0,  OS_CONTEXT_R25(context)
    stw     r0,  OS_CONTEXT_R26(context)
    stw     r0,  OS_CONTEXT_R27(context)
    stw     r0,  OS_CONTEXT_R28(context)
    stw     r0,  OS_CONTEXT_R29(context)
    stw     r0,  OS_CONTEXT_R30(context)
    stw     r0,  OS_CONTEXT_R31(context)

    stw     r0,  OS_CONTEXT_GQR0(context)
    stw     r0,  OS_CONTEXT_GQR1(context)
    stw     r0,  OS_CONTEXT_GQR2(context)
    stw     r0,  OS_CONTEXT_GQR3(context)
    stw     r0,  OS_CONTEXT_GQR4(context)
    stw     r0,  OS_CONTEXT_GQR5(context)
    stw     r0,  OS_CONTEXT_GQR6(context)
    stw     r0,  OS_CONTEXT_GQR7(context)

    b       OSClearContext
}
