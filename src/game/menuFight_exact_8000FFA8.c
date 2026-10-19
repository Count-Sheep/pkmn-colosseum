/**
 * @file menuFight_exact_8000FFA8.c
 * @brief menuFightDrawBall, 0x8000FFA8 - 0x800100C0.
 *
 * Part of the menuFight carve menuFight_exact_8000F768.c (which includes this
 * file after DrawSecretWazaSelect and the secret-move top window functions,
 * so DrawBall's switch table lands at 0x802E4D2C right after
 * DrawSecretWazaSelect's). Body unchanged from menuFight.c.
 */
#include "dolphin/types.h"

extern void windowDrawSprite();

void menuFightDrawBall(u8* ctx, u8* npc) {
    extern u8* windowGetFreeWork(u8* a);
    s32 msg;
    s32 slot;
    u8* state;

    msg = 0;
    slot = -1;
    state = windowGetFreeWork(ctx);
    switch (*(s16*)(npc + 6)) {
    case 0x11AA:
    case 0x11B0:
    case 0x11B6:
    case 0x11BC:
        slot = 0;
        break;
    case 0x11AB:
    case 0x11B1:
    case 0x11B7:
    case 0x11BD:
        slot = 1;
        break;
    case 0x11AC:
    case 0x11B2:
    case 0x11B8:
    case 0x11BE:
        slot = 2;
        break;
    case 0x11AD:
    case 0x11B3:
    case 0x11B9:
    case 0x11BF:
        slot = 3;
        break;
    case 0x11AE:
    case 0x11B4:
    case 0x11BA:
    case 0x11C0:
        slot = 4;
        break;
    case 0x11AF:
    case 0x11B5:
    case 0x11BB:
    case 0x11C1:
        slot = 5;
        break;
    }
    if (slot >= 0) {
        switch (state[slot]) {
        case 0:
            msg = 0x1B4;
            break;
        case 1:
            msg = 0x3AC;
            break;
        case 2:
            msg = 0x3AE;
            break;
        case 3:
            msg = 0x3AD;
            break;
        }
        if ((u16)msg != 0) {
            windowDrawSprite(0, 0, ctx, msg, 0);
        }
    }
}
