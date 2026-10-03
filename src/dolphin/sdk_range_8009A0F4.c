/**
 * @file sdk_range_8009A0F4.c
 * @brief dolphin-sdk code, 0x8009A0F4 - 0x8009A2C8 (6 fns).
 *
 * Range unit assigned from the propagated subsystem map
 * (tools/subsystem_propagation.py, >=80% single-label dominance;
 * campaign 2026-07-01). All functions asm-only until matched; the
 * range name stays honest until internal TU structure is proven.
 */
#include "dolphin/types.h"
#include "dolphin/os/OSContext.h"

extern void OSReport(const char* format, ...);
extern const char lbl_8047897C;

extern volatile u32 __DIRegs[16] : 0xCC006000;

u16 OSExceptionVector(u32 savedR3, u32 savedR4, u32 savedR5) {
    OSContext* context = *(OSContext* volatile*)0xC0;

    context->gpr[3] = savedR3;
    context->gpr[5] = savedR5;
    return context->state |= OS_CONTEXT_STATE_EXC;
}

/* OS.c's C functions show unpeepholed codegen (mr, no folded offset). */
#pragma push
#pragma peephole off
u32 __OSGetDIConfig(void) {
    return __DIRegs[9] & 0xFF;
}

void OSRegisterVersion(const char* version) {
    OSReport(&lbl_8047897C, version);
}
#pragma pop

void OSInitAlarm(void) {
    typedef void (*OSExceptionHandler)(u8 exception, OSContext* context, u32 dsisr, u32 dar);
    typedef struct {
        void* head;
        void* tail;
    } OSAlarmQueue;
    extern OSExceptionHandler __OSGetExceptionHandler(u8 exception);
    extern OSExceptionHandler __OSSetExceptionHandler(u8 exception, OSExceptionHandler handler);
    extern void DecrementerExceptionHandler_8009A8DC(u8 exception, OSContext* context, u32 dsisr,
                                                      u32 dar);
    extern OSAlarmQueue AlarmQueue_8047A6E0;

    if (__OSGetExceptionHandler(8) != DecrementerExceptionHandler_8009A8DC) {
        AlarmQueue_8047A6E0.tail = NULL;
        AlarmQueue_8047A6E0.head = NULL;
        __OSSetExceptionHandler(8, DecrementerExceptionHandler_8009A8DC);
    }
}
