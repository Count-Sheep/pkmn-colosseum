#include "dolphin/types.h"

typedef void (*OSExceptionHandler)(u8 exception, void* context, u32 dsisr, u32 dar);

extern OSExceptionHandler* OSExceptionTable_8047A6C4;

OSExceptionHandler __OSGetExceptionHandler(u8 exception) {
    u8 carrier;
    int carrier2;
    carrier2 = exception;
    carrier = carrier2;
    return OSExceptionTable_8047A6C4[carrier];
}
