/**
 * @file OSError.c
 * @brief Dolphin SDK OSError.c, 0x8009C2E0 - 0x8009C860: OSReport, OSVReport
 *        and OSPanic (weak; the latter two unreferenced, so the linker strips
 *        them), OSSetErrorHandler and __OSUnhandledException.
 *
 * The whole file links with its data: .data 0x803109B8-0x80310CD8 (OSPanic's
 * strings first, then __OSUnhandledException's strings and switch table),
 * .bss __OSErrorTable, and .sdata __OSFpscrEnableBits plus the "\n" string.
 * OSVReport and OSPanic follow the SDK as decompiled in zeldaret/tww
 * (src/dolphin/os/OSError.c); retail keeps OSPanic's strings at the start of
 * .data although its code is stripped.
 */
#include "dolphin/types.h"
#include "dolphin/os/OS.h"
#include "dolphin/os/OSContext.h"
#include "dolphin/os/OSThread.h"

typedef struct {
    u8 gpr;
    u8 fpr;
    u16 reserved;
    u32* overflow_arg_area;
    u32* reg_save_area;
} OSErrorVaList[1];

extern s32 vprintf(const char* format, OSErrorVaList args);
extern BOOL OSDisableInterrupts(void);
extern BOOL OSRestoreInterrupts(BOOL level);
extern void OSDumpContext(OSContext* context);
extern volatile s16 __OSLastInterrupt;
extern volatile u32 __OSLastInterruptSrr0;
extern volatile s64 __OSLastInterruptTime;
extern s64 OSGetTime(void);
extern void __OSReschedule(void);
extern void OSLoadContext(OSContext* context);
extern void OSSaveFPUContext(OSContext* context);
extern u32 PPCMfmsr(void);
extern void PPCMtmsr(u32 msr);
extern u32 PPCMffpscr(void);
extern void PPCMtfpscr(u32 fpscr);
extern void PPCHalt(void);

OSErrorHandler __OSErrorTable[17];

/* FPSCR bits left standing when an FP exception is cleared. */
#define FPSCR_KEEP 0x6005F8FF
/* FPSCR VE|OE|UE|ZE|XE, and MSR FE0|FE1. */
#define FPSCR_ENABLE 0xF8
#define MSR_FE 0x900

u32 lbl_80478990 = FPSCR_ENABLE; /* __OSFpscrEnableBits */

void OSReport(const char* format, ...) {
    OSErrorVaList args;

    __builtin_va_info(&args);
    vprintf(format, args);
}

void OSVReport(const char* format, OSErrorVaList args) {
    vprintf(format, args);
}

void OSPanic(const char* file, s32 line, const char* format, ...) {
    OSErrorVaList args;
    u32 i;
    u32* p;

    OSDisableInterrupts();
    __builtin_va_info(&args);
    vprintf(format, args);
    OSReport(" in \"%s\" on line %d.\n", file, line);

    OSReport("\nAddress:      Back Chain    LR Save\n");
    for (i = 0, p = (u32*)OSGetStackPointer(); p && (u32)p != 0xFFFFFFFF && i++ < 16;
         p = (u32*)*p) {
        OSReport("0x%08x:   0x%08x    0x%08x\n", p, p[0], p[1]);
    }

    PPCHalt();
}

OSErrorHandler OSSetErrorHandler(u16 error, OSErrorHandler handler) {
    OSErrorHandler oldHandler;
    OSThread* thread;
    BOOL enabled;
    u32 msr;
    u32 fpscr;
    u32 i;

    enabled = OSDisableInterrupts();
    oldHandler = __OSErrorTable[error];
    __OSErrorTable[error] = handler;

    if (error == 16) {
        msr = PPCMfmsr();
        PPCMtmsr(msr | 0x2000);
        fpscr = PPCMffpscr();

        if (handler != 0) {
            for (thread = *(OSThread**)0x800000DC; thread != NULL;
                 thread = thread->linkActive.next) {
                thread->context.srr1 |= MSR_FE;
                if (!(thread->context.state & OS_CONTEXT_STATE_FPSAVED)) {
                    thread->context.state |= OS_CONTEXT_STATE_FPSAVED;
                    for (i = 0; i < 32; i++) {
                        *(u64*)&thread->context.fpr[i] = -1;
                        *(u64*)&thread->context.psf[i] = -1;
                    }
                    thread->context.fpscr = 4;
                }
                thread->context.fpscr |= (lbl_80478990 & FPSCR_ENABLE);
                thread->context.fpscr &= FPSCR_KEEP;
            }
            msr |= MSR_FE;
            fpscr |= (lbl_80478990 & FPSCR_ENABLE);
        } else {
            for (thread = *(OSThread**)0x800000DC; thread != NULL;
                 thread = thread->linkActive.next) {
                thread->context.srr1 &= ~MSR_FE;
                thread->context.fpscr &= ~FPSCR_ENABLE;
                thread->context.fpscr &= FPSCR_KEEP;
            }
            fpscr &= ~FPSCR_ENABLE;
            msr &= ~MSR_FE;
        }

        fpscr &= FPSCR_KEEP;
        PPCMtfpscr(fpscr);
        PPCMtmsr(msr);
    }

    OSRestoreInterrupts(enabled);
    return oldHandler;
}

#define __OSFPUContext (*(OSContext**)0x800000D8)

volatile u16 __DSPRegs[32] : 0xCC005000;
volatile u32 __DIRegs[16] : 0xCC006000;

void __OSUnhandledException(u8 exception, OSContext* context, u32 dsisr,
                            u32 dar) {
    s64 now = OSGetTime();
    u32 msr;

    if (!(context->srr1 & 0x2)) {
        OSReport("Non-recoverable Exception %d", exception);
    } else {
        if (exception == 6 && (context->srr1 & 0x00100000) &&
            __OSErrorTable[16]) {
            exception = 16;
            msr = PPCMfmsr();
            PPCMtmsr(msr | 0x2000);
            if (__OSFPUContext != NULL) {
                OSSaveFPUContext(__OSFPUContext);
            }
            PPCMtfpscr(PPCMffpscr() & FPSCR_KEEP);
            PPCMtmsr(msr);
            if (__OSFPUContext == context) {
                OSDisableScheduler();
                __OSErrorTable[exception](exception, context, dsisr, dar);
                context->srr1 &= ~0x2000;
                __OSFPUContext = NULL;
                context->fpscr &= FPSCR_KEEP;
                OSEnableScheduler();
                __OSReschedule();
            } else {
                context->srr1 &= ~0x2000;
                __OSFPUContext = NULL;
            }
            OSLoadContext(context);
        }

        if (__OSErrorTable[exception]) {
            OSDisableScheduler();
            __OSErrorTable[exception](exception, context, dsisr, dar);
            OSEnableScheduler();
            __OSReschedule();
            OSLoadContext(context);
        }

        if (exception == 8) {
            OSLoadContext(context);
        }

        OSReport("Unhandled Exception %d", exception);
    }

    OSReport("\n");
    OSDumpContext(context);
    OSReport("\nDSISR = 0x%08x                   DAR  = 0x%08x\n", dsisr, dar);
    OSReport("TB = 0x%016llx\n", now);

    switch (exception) {
    case 2:
        OSReport("\nInstruction at 0x%x (read from SRR0) attempted to access invalid address 0x%x (read from DAR)\n", context->srr0, dar);
        break;
    case 3:
        OSReport("\nAttempted to fetch instruction from invalid address 0x%x (read from SRR0)\n", context->srr0);
        break;
    case 5:
        OSReport("\nInstruction at 0x%x (read from SRR0) attempted to access unaligned address 0x%x (read from DAR)\n", context->srr0, dar);
        break;
    case 6:
        OSReport("\nProgram exception : Possible illegal instruction/operation at or around 0x%x (read from SRR0)\n", context->srr0, dar);
        break;
    case 15:
        OSReport("\n");
        OSReport("AI DMA Address =   0x%04x%04x\n", __DSPRegs[0x18],
                 __DSPRegs[0x19]);
        OSReport("ARAM DMA Address = 0x%04x%04x\n", __DSPRegs[0x10],
                 __DSPRegs[0x11]);
        OSReport("DI DMA Address =   0x%08x\n", __DIRegs[5]);
        break;
    }

    OSReport("\nLast interrupt (%d): SRR0 = 0x%08x  TB = 0x%016llx\n", __OSLastInterrupt,
             __OSLastInterruptSrr0, __OSLastInterruptTime);
    PPCHalt();
}
