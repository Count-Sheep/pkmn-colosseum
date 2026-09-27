/**
 * @file GBAXfer.c
 * @brief Dolphin SDK GBA library: JOY Bus transfer core
 *        (0x8025F70C-0x8025FA20, 5 fns).
 *
 * __GBATransfer queries the channel type; TypeAndStatusCallback starts the
 * SI transfer when a GBA is attached and __GBAHandler completes it, running
 * the command's result procedure and then the caller's callback on a clean
 * exception context.
 */

#include "dolphin/gba/GBAPriv.h"
#include "dolphin/os/OSInterrupt.h"
#include "dolphin/si/SI.h"

u32 SIGetTypeAsync(s32 chan, SITypeAndStatusCallback callback);

void __GBAHandler(s32 chan, u32 error, OSContext* context) {
    GBAControl* gba;
    GBATransferCallback proc;
    GBACallback callback;
    OSContext exceptionContext;

    gba = &__GBA[chan];
    if (__GBAReset) {
        return;
    }

    if (error & 0xf) {
        gba->ret = GBA_NOT_READY;
    } else {
        gba->ret = GBA_READY;
    }

    if (gba->proc) {
        proc = gba->proc;
        gba->proc = NULL;
        proc(chan);
    }

    if (gba->callback == NULL) {
        return;
    }

    OSClearContext(&exceptionContext);
    OSSetCurrentContext(&exceptionContext);
    callback = gba->callback;
    gba->callback = NULL;
    callback(chan, gba->ret);
    OSClearContext(&exceptionContext);
    OSSetCurrentContext(context);
}

void __GBASyncCallback(s32 chan, s32 ret) {
    OSWakeupThread(&__GBA[chan].threadQueue);
}

s32 __GBASync(s32 chan) {
    GBAControl* gba;
    s32 enabled;
    s32 ret;

    gba = &__GBA[chan];
    enabled = OSDisableInterrupts();
    while (gba->callback) {
        OSSleepThread(&gba->threadQueue);
    }
    ret = gba->ret;
    OSRestoreInterrupts(enabled);
    return ret;
}

static void TypeAndStatusCallback(s32 chan, u32 type) {
    GBAControl* gba;
    GBATransferCallback proc;
    GBACallback callback;
    OSContext* context;
    OSContext exceptionContext;

    gba = &__GBA[chan];
    if (__GBAReset) {
        return;
    }

    if ((type & 0xff) != 0 || (type & 0xffff0000) != 0x40000) {
        gba->ret = GBA_NOT_READY;
    } else {
        if (SITransfer(chan, gba->output, gba->outputBytes, gba->input, gba->inputBytes,
                       __GBAHandler, gba->delay)) {
            return;
        }
        gba->ret = GBA_BUSY;
    }

    if (gba->proc) {
        proc = gba->proc;
        gba->proc = NULL;
        proc(chan);
    }

    if (gba->callback) {
        context = OSGetCurrentContext();
        OSClearContext(&exceptionContext);
        OSSetCurrentContext(&exceptionContext);
        callback = gba->callback;
        gba->callback = NULL;
        callback(chan, gba->ret);
        OSClearContext(&exceptionContext);
        OSSetCurrentContext(context);
        __OSReschedule();
    }
}

s32 __GBATransfer(s32 chan, s32 w1, s32 w2, GBATransferCallback proc) {
    s32 enabled;
    GBAControl* gba;

    gba = &__GBA[chan];
    enabled = OSDisableInterrupts();
    gba->proc = proc;
    gba->outputBytes = w1;
    gba->inputBytes = w2;
    SIGetTypeAsync(chan, TypeAndStatusCallback);
    OSRestoreInterrupts(enabled);
    return GBA_READY;
}
