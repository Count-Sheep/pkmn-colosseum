/**
 * @file menu_exact_8007A5E8.c
 * @brief Coupon-value and mode-sprite display callbacks.
 */
#include "dolphin/types.h"

extern u32 lbl_8047A62C;
extern u32 lbl_8047A638;
extern void* heroGetStatus(void*, u32, u32);
extern void msgctrlSetValue(s32, u32);
extern u32 GSmsgGetRect(s32);
extern void fn_800FB680(s32, s32, s32, s32);
extern void winSpriteSetDisp(void*, s32);

void fn_8007A5E8(s32 unused, u8* window)
{
    u32 value;
    u32 rect;

    if ((s32)lbl_8047A638 == 4) {
        value = lbl_8047A62C;
    } else {
        value = (u32)heroGetStatus(0, 0xD, 0);
    }
    msgctrlSetValue(0x50, value);
    rect = GSmsgGetRect(0x153);
    fn_800FB680(*(s16*)(window + 0x54) - (rect >> 16), 0, -1, 0x153);
}

void fn_8007A664(s32 unused, u8* sprite)
{
    switch (lbl_8047A638) {
    case 4:
        if (*(s16*)(sprite + 6) == 0x10BF) {
            winSpriteSetDisp(sprite, 1);
        } else {
            winSpriteSetDisp(sprite, 0);
        }
        break;
    case 3:
        if (*(s16*)(sprite + 6) == 0x10C0) {
            winSpriteSetDisp(sprite, 1);
        } else {
            winSpriteSetDisp(sprite, 0);
        }
        break;
    }
}
