/**
 * @file GBA.c
 * @brief Dolphin SDK GBA library: channel control blocks, init, status and
 *        reset commands (0x8025F2FC-0x8025F524, 5 fns, plus the reset
 *        function info in .data, SecParams/__GBA in .bss and __GBAReset in
 *        .sbss).
 *
 * GBAGetStatusAsync and GBAResetAsync are auto-inlined into their
 * synchronous wrappers and dead-stripped from the retail DOL.
 *
 * The synchronous wrappers open with the library's usual
 * "GBAControl* gba = &__GBA[chan];" even though they never use it; the
 * unused local keeps an 8-byte stack slot, which is why retail's frames
 * are 0x20 rather than 0x18. This is the SDK text as reconstructed from
 * the same library in other retail-matched decomps (zeldaret/tww,
 * ACreTeam/forest, FFCC), which all carry it in GBAGetStatus and GBAReset.
 */

#include "dolphin/gba/GBAPriv.h"
#include "dolphin/os/OSAlarm.h"
#include "dolphin/os/OSClock.h"
#include "dolphin/os/OSReset.h"

static GBASecParam SecParams[GBA_MAX_CHAN];
GBAControl __GBA[GBA_MAX_CHAN];
BOOL __GBAReset = FALSE;

static BOOL OnReset(BOOL final);

static OSResetFunctionInfo ResetFunctionInfo = {OnReset, 127};

static void ShortCommandProc(s32 chan) {
    GBAControl* gba;

    gba = &__GBA[chan];
    if (gba->ret != GBA_READY) {
        return;
    }

    if (gba->input[0] != 0 || gba->input[1] != 4) {
        gba->ret = GBA_NOT_READY;
        return;
    }

    gba->status[0] = gba->input[2] & GBA_JSTAT_MASK;
}

void GBAInit(void) {
    GBAControl* gba;
    s32 chan;

    for (chan = 0; chan < GBA_MAX_CHAN; ++chan) {
        gba = &__GBA[chan];
        gba->delay = OSMicrosecondsToTicks(60);
        OSInitThreadQueue(&gba->threadQueue);
        gba->param = &SecParams[chan];
    }

    OSInitAlarm();
    DSPInit();
    __GBAReset = FALSE;
    OSRegisterResetFunction(&ResetFunctionInfo);
}

s32 GBAGetStatusAsync(s32 chan, u8* status, GBACallback callback) {
    GBAControl* gba;

    gba = &__GBA[chan];
    if (gba->callback != NULL) {
        return GBA_BUSY;
    }

    gba->output[0] = 0x00;
    gba->status = status;
    gba->callback = callback;
    return __GBATransfer(chan, 1, 3, ShortCommandProc);
}

s32 GBAGetStatus(s32 chan, u8* status) {
    GBAControl* gba = &__GBA[chan];
    s32 ret;

    ret = GBAGetStatusAsync(chan, status, __GBASyncCallback);
    if (ret != GBA_READY) {
        return ret;
    }
    return __GBASync(chan);
}

s32 GBAResetAsync(s32 chan, u8* status, GBACallback callback) {
    GBAControl* gba;

    gba = &__GBA[chan];
    if (gba->callback != NULL) {
        return GBA_BUSY;
    }

    gba->output[0] = 0xFF;
    gba->status = status;
    gba->callback = callback;
    return __GBATransfer(chan, 1, 3, ShortCommandProc);
}

s32 GBAReset(s32 chan, u8* status) {
    GBAControl* gba = &__GBA[chan];
    s32 ret;

    ret = GBAResetAsync(chan, status, __GBASyncCallback);
    if (ret != GBA_READY) {
        return ret;
    }
    return __GBASync(chan);
}

static BOOL OnReset(BOOL final) {
    __GBAReset = TRUE;
    return TRUE;
}
