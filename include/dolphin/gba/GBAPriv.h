#ifndef DOLPHIN_GBA_GBAPRIV_H
#define DOLPHIN_GBA_GBAPRIV_H

#include "dolphin/types.h"
#include "dolphin/gba.h"
#include "dolphin/dsp.h"
#include "dolphin/os/OSContext.h"
#include "dolphin/os/OSThread.h"

/*
 * Dolphin SDK GBA library internals (the SDK's private __gba.h).
 * One GBAControl per channel, 0x100 bytes each (the SDK's __GBA[4]).
 */

typedef void (*GBATransferCallback)(s32 chan);

typedef struct GBASecParam {
    /* 0x00 */ u8 readbuf[4];
    /* 0x04 */ s32 paletteColor;
    /* 0x08 */ s32 paletteSpeed;
    /* 0x0C */ s32 length;
    /* 0x10 */ u32* out;
    /* 0x14 */ u8 _padding0[12];
    /* 0x20 */ u32 keyA;
    /* 0x24 */ s32 keyB;
    /* 0x28 */ u8 _padding1[24];
} GBASecParam;

typedef struct GBABootInfo {
    /* 0x00 */ s32 paletteColor;
    /* 0x04 */ s32 paletteSpeed;
    /* 0x08 */ u8* programp;
    /* 0x0C */ s32 length;
    /* 0x10 */ u8* status;
    /* 0x14 */ GBACallback callback;
    /* 0x18 */ u8 readbuf[4];
    /* 0x1C */ u8 writebuf[4];
    /* 0x20 */ int i;
    /* 0x24 */ u32 start;
    /* 0x28 */ OSTime begin;
    /* 0x30 */ int firstXfer;
    /* 0x34 */ int curOffset;
    /* 0x38 */ u32 crc;
    /* 0x3C */ u32 dummyWord[7];
    /* 0x58 */ u32 keyA;
    /* 0x5C */ s32 keyB;
    /* 0x60 */ u32 initialCode;
    /* 0x64 */ int realLength;
} GBABootInfo;

typedef struct GBAControl {
    /* 0x00 */ u8 output[5];
    /* 0x05 */ u8 input[5];
    /* 0x0C */ s32 outputBytes;
    /* 0x10 */ s32 inputBytes;
    /* 0x14 */ u8* status;
    /* 0x18 */ u8* ptr;
    /* 0x1C */ GBACallback callback;
    /* 0x20 */ s32 ret;
    /* 0x24 */ OSThreadQueue threadQueue;
    /* 0x30 */ OSTime delay;
    /* 0x38 */ GBATransferCallback proc;
    /* 0x40 */ GBABootInfo bootInfo;
    /* 0xA8 */ DSPTaskInfo task;
    /* 0xF8 */ GBASecParam* param;
} GBAControl;

extern GBAControl __GBA[GBA_MAX_CHAN];
extern BOOL __GBAReset;

void __GBAHandler(s32 chan, u32 error, OSContext* context);
void __GBASyncCallback(s32 chan, s32 ret);
s32 __GBASync(s32 chan);
s32 __GBATransfer(s32 chan, s32 w1, s32 w2, GBATransferCallback callback);

#endif /* DOLPHIN_GBA_GBAPRIV_H */
