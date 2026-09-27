/**
 * @file win_msg.c
 * @brief winMsgDraw (0x801058CC - 0x80105A3C), the head of the winMsg TU
 *        (0x801058CC - 0x80106F98; XD's winMsg.cpp). The rest of the TU,
 *        winMsgCtrl through winMsgOpen, is win_msg_exact_80105A3C.c, which
 *        also documents the TU flags (-O4,p, "-opt nopeephole", GC/2.5).
 *
 * Candidate: 97.7% under the TU flags. The one remaining difference is in
 * the non-0x50 branch of the size selection: retail loads the item's
 * width/height into r3/r0 and copies them into the homes of x/y
 * ("lha r3,0x54(r27); lha r0,0x56(r27); mr r29,r3; mr r28,r0"), while this
 * source loads them straight into r29/r28 as the 0x50 branch does.
 */
#include "dolphin/types.h"

extern void fn_800FE6D0(s16 x, s16 y);
extern void spriteSetEnv(void);
extern void fn_8001EC08(s32, s32, s16, s16, u8, s32);
extern void fn_800FBE7C(void* message, u32 state, u8 flag);
extern void* windowGetFreeWork(void* window);

/* 0x801058CC | 0x170 */
s32 winMsgDraw(u8* window, u8* item)
{
    u8* work = windowGetFreeWork(window);
    s32 width;
    s32 height;
    s16 offset;

    if (*(s32*)(window + 0x04) == 0x50) {
        width = *(s16*)(work + 0x0C);
        height = *(s16*)(work + 0x0E);
    } else {
        width = *(s16*)(item + 0x54);
        height = *(s16*)(item + 0x56);
    }

    offset = *(s16*)(item + 0x56) - 4 - height;
    fn_800FE6D0(*(s16*)(window + 0x84) + *(s16*)(item + 0x50),
                *(s16*)(window + 0x86) + *(s16*)(item + 0x52) + offset);
    spriteSetEnv();

    switch (*(s32*)(window + 0x04)) {
    case 0x40:
    case 0x50:
    case 0x51:
        fn_8001EC08(0, 2, width, height, window[0x8B], 1);
        break;
    case 0x10C:
        fn_8001EC08(0, 2, width, height, window[0x8B], 0);
        break;
    }

    switch ((s8)window[1]) {
    case 2:
        if (*(s32*)(window + 0x04) != 0x51) {
            fn_800FBE7C(*(void**)work, *(u32*)(work + 0x08),
                        ((s8)window[0] & 4) == 0);
            *(u32*)(work + 0x08) = 0;
        }
        break;
    case 0:
    case 1:
    case 3:
    case 5:
        break;
    }
    return 0;
}
