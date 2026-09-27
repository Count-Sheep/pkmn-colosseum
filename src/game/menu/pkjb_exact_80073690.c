/**
 * @file pkjb_exact_80073690.c
 * @brief pkjb_uploader.c carve, 0x80073690 - 0x80073700.
 *
 * Unit flags and shared declarations: see game/menu/pkjb_uploader_shared.h.
 */
#include "game/menu/pkjb_uploader_shared.h"

s32 fn_80073700(s32 chan, u32* data);

/* 0x80073690 | 0x70: fn_80073700 with the key state held. */
s32 fn_80073690(s32 chan, u32* data)
{
    s32 key;
    s32 result;

    key = chan + 1;
    gbaCommandSetKeyState(key, 2);
    result = fn_80073700(chan, data);
    gbaCommandSetKeyState(key, 1);
    return result;
}
