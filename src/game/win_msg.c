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
 * source loads them straight into r29/r28 as the 0x50 branch does. The
 * 0x50 branch loads into the homes in retail too, so only the second
 * branch materialises its loads in temporaries first.
 *
 * Tried (2026-09-30), none of them reproduces the pair: intermediate s16
 * locals in the else branch alone and in both branches, static inline
 * width/height accessors, a switch with a default in place of the if/else
 * (worse, 6 lines), a ternary per field (much worse), s16 and int widths,
 * indexing item as ((s16*)item)[0x2A], and every declaration order of
 * work/width/height/offset. MWCC folds all of them: the allocator replay
 * (mwdbg, tools/local_campaign.py explain) shows the copy propagation pass
 * leaves no copy in either branch, both blocks loading straight into the
 * merge registers, whereas retail's second block must hold distinct
 * virtual registers copied into the homes. Copies into a user variable do
 * survive that pass here (the parameter and windowGetFreeWork homes keep
 * theirs), so the shape that keeps these two has not been found yet rather
 * than being impossible. The flags are not the cause: -O3 and below are far
 * worse (35+ lines), and -O4,s and plain -O4 give the same 4. A two-member
 * size struct assigned whole in the else branch is not it either: MWCC
 * keeps the struct on the stack (33 lines).
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
