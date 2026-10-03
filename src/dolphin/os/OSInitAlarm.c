/**
 * @file OSInitAlarm.c
 * @brief OSInitAlarm, 0x8009A27C - 0x8009A2C8: the first function of the
 *        Dolphin SDK's OSAlarm.c (OSCreateAlarm follows in OSAlarmCreate.c).
 */
#include "dolphin/types.h"
#include "dolphin/os/OSContext.h"

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
