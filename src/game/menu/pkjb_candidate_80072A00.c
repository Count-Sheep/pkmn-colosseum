/**
 * @file pkjb_candidate_80072A00.c
 * @brief pkjb_uploader.c carve, 0x80072A00 - 0x80072C74 (candidate).
 *
 * fn_80072A00 is 99.7%: every instruction matches except that the key
 * channel and the result swap callee-saved registers (retail: key r29,
 * result r28; here key r28, result r29). Writing the body directly in the
 * function instead of through pkjbWait60 gets that pair right but moves the
 * loop timeout from r20 to r27 and shifts the others.
 * Unit flags and shared helpers: see game/menu/pkjb_uploader_shared.h.
 */
#include "game/menu/pkjb_uploader_shared.h"

/* Command 0x60, then ping with 0xAA until the GBA answers (3 s). */
static inline s32 pkjbWait60(s32 chan)
{
    u32 command;
    u8 length;
    s32 result;
    u32 timeout;
    u32 start;
    s32 ping;

    result = fn_80073C38(chan);
    if (result != 0) {
        return result;
    }
    command = 0x60;
    result = GBAWrite(chan, (u8*)&command, &length);
    if (result != 0) {
        return 11;
    }
    timeout = OS_TIMER_CLOCK * 3;
    start = OSGetTick();
    do {
        if (OSGetTick() - start > timeout) {
            return 16;
        }
        ping = pkjbPingAA(chan);
        _threadSwitch();
    } while (ping != 0);
    return result;
}

/* 0x80072A00 | 0x274 */
s32 fn_80072A00(s32 chan)
{
    s32 key;
    s32 result;

    key = chan + 1;
    gbaCommandSetKeyState(key, 2);
    result = pkjbWait60(chan);
    gbaCommandSetKeyState(key, 1);
    return result;
}
