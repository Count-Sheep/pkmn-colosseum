/**
 * @file pkjb_exact_80071E34.c
 * @brief Exact island 0x80071E34 - 0x80071EA4: a standalone copy of
 *        fn_80071E34 from pkjb_candidate_80071AE4.c (body unchanged there).
 */
#include "game/menu/pkjb_uploader_shared.h"

s32 fn_80071EA4(s32 chan, u32* data);

/* 0x80071E34 | 0x70: fn_80071EA4 with the key state held. */
s32 fn_80071E34(s32 chan, u32* data)
{
    s32 key;
    s32 result;

    key = chan + 1;
    gbaCommandSetKeyState(key, 2);
    result = fn_80071EA4(chan, data);
    gbaCommandSetKeyState(key, 1);
    return result;
}
