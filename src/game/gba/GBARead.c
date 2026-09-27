/**
 * @file GBARead.c
 * @brief Dolphin SDK GBA library: JOY Bus read command
 *        (0x8025F524-0x8025F618, 2 fns).
 *
 * GBAReadAsync is auto-inlined into GBARead and dead-stripped from the
 * retail DOL.
 *
 * GBARead's unused "s32 tmp" and "gba" locals are the SDK text as other
 * retail-matched decomps of this library carry it (zeldaret/tww,
 * ACreTeam/forest); together they give retail's 8-byte larger frame.
 * The previous source used an equivalent unused local.
 */

#include "dolphin/gba/GBAPriv.h"

extern void* memcpy(void* dst, const void* src, u32 n);

static void ReadProc(s32 chan) {
    GBAControl* gba;

    gba = &__GBA[chan];
    if (gba->ret != GBA_READY) {
        return;
    }

    memcpy(gba->ptr, gba->input, 4);
    gba->status[0] = gba->input[4] & GBA_JSTAT_MASK;
}

s32 GBAReadAsync(s32 chan, u8* dst, u8* status, GBACallback callback) {
    GBAControl* gba;

    gba = &__GBA[chan];
    if (gba->callback != NULL) {
        return GBA_BUSY;
    }

    gba->output[0] = 0x14;
    gba->ptr = dst;
    gba->status = status;
    gba->callback = callback;
    return __GBATransfer(chan, 1, 5, ReadProc);
}

s32 GBARead(s32 chan, u8* dst, u8* status) {
    s32 tmp;
    GBAControl* gba = &__GBA[chan];
    s32 ret;

    ret = GBAReadAsync(chan, dst, status, __GBASyncCallback);
    if (ret != GBA_READY) {
        return ret;
    }
    return __GBASync(chan);
}
