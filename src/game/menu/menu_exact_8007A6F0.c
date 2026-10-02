/**
 * @file menu_exact_8007A6F0.c
 * @brief Coupon-rank sprite display callback.
 */
#include "dolphin/types.h"

typedef struct MenuRankSprite {
    u8 pad_00[6];
    s16 id;
} MenuRankSprite;

extern u32 lbl_804788F0;
extern u32 lbl_802E61D8[];
extern u32 lbl_8047A628;
extern u32 lbl_8047A638;
extern void* heroGetStatus(void*, u32, u32);
extern void winSpriteSetDisp(void*, s32);

void fn_8007A6F0(s32 unused, MenuRankSprite* sprite)
{
    u32 value;
    s32 rank;

    if ((s32)lbl_8047A638 == 4) {
        value = lbl_8047A628;
    } else {
        value = (u32)heroGetStatus(0, 0xE, 0);
    }

    for (rank = lbl_804788F0 - 1; rank >= 0; rank--) {
        if (lbl_802E61D8[rank] <= value) {
            break;
        }
    }
    if (rank < 0) {
        rank = 0;
    }

    winSpriteSetDisp(sprite, 0);
    switch (rank) {
    case 1:
        if (sprite->id == 0x10C3) {
            winSpriteSetDisp(sprite, 1);
        }
        break;
    case 2:
        if (sprite->id == 0x10C4) {
            winSpriteSetDisp(sprite, 1);
        }
        break;
    case 3:
        if (sprite->id == 0x10C5) {
            winSpriteSetDisp(sprite, 1);
        }
        break;
    default:
        if (sprite->id == 0x10C2) {
            winSpriteSetDisp(sprite, 1);
        }
        break;
    }
}
