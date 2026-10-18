/**
 * @file pkjb_candidate_80071AE4.c
 * @brief pkjb_uploader.c carve, 0x80071AE4 - 0x800722A0 (candidate).
 *
 * fn_80071E34 and fn_80071EA4 are exact; fn_80071AE4 is not yet. Its
 * buffer is the u8 block gbaCommunication.c passes (p + 0x4344, through
 * fn_80089D30). Retail reads and swaps the header once, after the hoisted
 * idle-callback addresses and in a block of its own (its temporaries reuse
 * those addresses' registers); reading it before the loop interleaves the
 * swap with the addresses, so this source reads it at the top of each
 * attempt, which gives retail's instruction order inside the loop but keeps
 * the swap in the loop and one register fewer (key is not spilled).
 *
 * fn_80071EA4 receives into a byte buffer (gbaCommunication.c passes
 * u8 readBuffer[0xD8]) one word at a time at a byte offset; the body size
 * is an unsigned word count times 4. pkjb_candidate_80071EA4.c links it
 * alone (PKJB_80071EA4_ONLY).
 * Unit flags and shared helpers: see game/menu/pkjb_uploader_shared.h.
 */
#include "game/menu/pkjb_uploader_shared.h"

s32 fn_80071EA4(s32 chan, u8* data);

#ifndef PKJB_80071EA4_ONLY
static inline s32 pkjbSendBlockIdle(s32 chan, const u8* data, s32 size)
{
    u32 word;
    u8 status;
    u32 timeout;
    u32 start;
    s32 offset;

    for (offset = 0; offset < size; offset += 4) {
        word = *(const u32*)(data + offset);
        if (GBAWrite(chan, (u8*)&word, &status) != 0) {
            return 16;
        }
        timeout = OSMillisecondsToTicks(100);
        start = OSGetTick();
        for (;;) {
            if (OSGetTick() - start > timeout) {
                return 17;
            }
            if (GBAGetStatus(chan, &status) != 0) {
                return 18;
            }
            if ((status & GBA_JSTAT_RECV) == 0) {
                break;
            }
            if (lbl_803B6E18[chan].func != NULL) {
                lbl_803B6E18[chan].func(chan, lbl_803B6E18[chan].arg);
            }
            if (lbl_803B6E08[chan] != 0) {
                return 1000;
            }
        }
        if (lbl_803B6E08[chan] != 0) {
            return 1000;
        }
    }
    return 0;
}

static inline s32 pkjbSend22(s32 chan, const u8* data, u16 count, u16 extra)
{
    u32 response;
    s32 result;
    s32 size;

    result = fn_80073C38(chan);
    if (result != 0) {
        return result;
    }
    result = pkjbSendCommand(chan, 0x22, &response);
    if (result != 0) {
        return result;
    }
    if ((response >> 24) != 0x22) {
        return 15;
    }
    size = count;
    size = size * 4 + 16;
    if (extra != 0) {
        size += 0x70;
    }
    result = pkjbSendBlockIdle(chan, data, size);
    if (result != 0) {
        return result;
    }
    return 0;
}

/* 0x80071AE4 | 0x350: command 0x22, upload a block sized by its own header. */
s32 fn_80071AE4(s32 chan, const u8* data)
{
    s32 key;
    s32 result;
    u32 timeout;
    u32 start;
    s32 expired;
    u32 packed;
    u16 extra;
    u16 count;

    key = chan + 1;
    gbaCommandSetKeyState(key, 2);
    timeout = OSMillisecondsToTicks(100);
    start = OSGetTick();
    do {
        packed = pkjbSwap32(*(u32*)(data + 12));
        extra = packed >> 16;
        count = packed;
        expired = OSGetTick() - start > timeout;
        result = pkjbSend22(chan, data, count, extra);
    } while (result == 1 && !expired);
    gbaCommandSetKeyState(key, result != 0 ? 1 : 3);
    return result;
}

/* 0x80071E34 | 0x70: fn_80071EA4 with the key state held. */
s32 fn_80071E34(s32 chan, u8* data)
{
    s32 key;
    s32 result;

    key = chan + 1;
    gbaCommandSetKeyState(key, 2);
    result = fn_80071EA4(chan, data);
    gbaCommandSetKeyState(key, 1);
    return result;
}
#endif /* !PKJB_80071EA4_ONLY */

static inline s32 pkjbRecvBlock(s32 chan, u8* data, s32 size)
{
    u32 word;
    u8 length;
    s32 offset;
    s32 result;

    for (offset = 0; offset < size; offset += 4) {
        result = pkjbRecvWait(chan, &word, &length, 100);
        if (result != 0) {
            return result;
        }
        *(u32*)(data + offset) = word;
        if (lbl_803B6E08[chan] != 0) {
            return 1000;
        }
    }
    return 0;
}

/* 0x80071EA4 | 0x3FC: command 0x33, receive a 16-byte header and its body. */
s32 fn_80071EA4(s32 chan, u8* data)
{
    u32 response;
    s32 result;
    u32 size;

    result = fn_80073C38(chan);
    if (result != 0) {
        return result;
    }
    result = pkjbSendCommand(chan, 0x33, &response);
    if (result != 0) {
        return result;
    }
    if ((response >> 24) != 0x33) {
        return 15;
    }
    result = pkjbRecvBlock(chan, data, 16);
    if (result != 0) {
        return result + 15;
    }
    size = (pkjbSwap32(*(u32*)(data + 12)) & 0xFFFF) * 4;
    result = pkjbRecvBlock(chan, data + 16, size);
    if (result != 0) {
        return result + 18;
    }
    return 0;
}
