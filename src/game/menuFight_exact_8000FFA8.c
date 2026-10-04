/**
 * @file menuFight_exact_8000FFA8.c
 * @brief menuFightDrawBall, 0x8000FFA8 - 0x800100C0, with .data
 *        0x802E4CD8 - 0x802E4D8C.
 *
 * Function-boundary carve of the menuFight TU (see menuFight.c, where the
 * body is unchanged). DrawBall's switch table sits at 0x802E4D2C, which is
 * only 4-aligned, so the unit starts its .data with the preceding table
 * (menuFightDrawSecretWazaSelect's, 0x802E4CD8, 8-aligned) and the compiler
 * places DrawBall's own table right after it. GC/1.3 -O4,p with the TU's
 * unit-wide -opt nopeephole, no pragmas.
 */
#include "dolphin/types.h"

extern void windowDrawSprite();
extern u8 menuFightDrawSecretWazaSelect[];

/* RULE-EXCEPTION(user-approved): data stand-in for another function's switch table, carried so DrawBall's table keeps its 4-aligned address — see docs/RULE_EXCEPTIONS.md */
void* jumptable_802E4CD8[21] = {
    (void*)((u8*)menuFightDrawSecretWazaSelect + 0x80),
    (void*)((u8*)menuFightDrawSecretWazaSelect + 0x88),
    (void*)((u8*)menuFightDrawSecretWazaSelect + 0x90),
    (void*)((u8*)menuFightDrawSecretWazaSelect + 0x78),
    (void*)((u8*)menuFightDrawSecretWazaSelect + 0x94),
    (void*)((u8*)menuFightDrawSecretWazaSelect + 0x80),
    (void*)((u8*)menuFightDrawSecretWazaSelect + 0x88),
    (void*)((u8*)menuFightDrawSecretWazaSelect + 0x90),
    (void*)((u8*)menuFightDrawSecretWazaSelect + 0x78),
    (void*)((u8*)menuFightDrawSecretWazaSelect + 0x80),
    (void*)((u8*)menuFightDrawSecretWazaSelect + 0x88),
    (void*)((u8*)menuFightDrawSecretWazaSelect + 0x90),
    (void*)((u8*)menuFightDrawSecretWazaSelect + 0x78),
    (void*)((u8*)menuFightDrawSecretWazaSelect + 0x80),
    (void*)((u8*)menuFightDrawSecretWazaSelect + 0x88),
    (void*)((u8*)menuFightDrawSecretWazaSelect + 0x90),
    (void*)((u8*)menuFightDrawSecretWazaSelect + 0x78),
    (void*)((u8*)menuFightDrawSecretWazaSelect + 0x80),
    (void*)((u8*)menuFightDrawSecretWazaSelect + 0x88),
    (void*)((u8*)menuFightDrawSecretWazaSelect + 0x90),
    (void*)((u8*)menuFightDrawSecretWazaSelect + 0x78),
};

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
