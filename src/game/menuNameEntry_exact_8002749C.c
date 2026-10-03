/* Name-entry keyboard cursor, 0x8002749C-0x800275F4. */
#include "dolphin/types.h"

extern u8 lbl_802EF0A8[];
/* RULE-EXCEPTION(user-approved): extern named stand-ins for the menuNameEntry TU's shared .sdata2 float pool (also read by 80026740, 80026FEC, 800280FC, 80028728, 80028830, 80028FBC) — see docs/RULE_EXCEPTIONS.md */
extern const f32 lbl_8047B93C, lbl_8047B934, lbl_8047B940, lbl_8047B938;

s32 menuNameEntryDraw50Cursor(void* window, u8* draw)
{
    u8* ctx;
    f32 width;
    f32 height;
    f32 scale;
    f32 dw;
    f32 dh;
    s32 column;
    s32 row;
    s16 left;
    s16 top;
    s16 right;
    s16 bottom;
    u8 alpha;
    s32 y;
    s32 x;

    ctx = *(u8**)((u8*)window + 0x60);
    column = **(s32**)(ctx + 0x28);
    width = ((s16*)(lbl_802EF0A8 + *(s16*)(draw + 6) * 0x1c))[3];
    height = ((s16*)(lbl_802EF0A8 + *(s16*)(draw + 6) * 0x1c))[4];
    row = **(s32**)(ctx + 0x2c);
    scale = **(f32**)(ctx + 0x30);
    dw = lbl_8047B93C * (width * scale);
    dh = lbl_8047B93C * (height * scale);
    if (column < 0xf) {
        alpha = lbl_8047B938 * (lbl_8047B934 - scale);
        x = **(s32**)(ctx + 0x38) + column * 0x1b;
        y = **(s32**)(ctx + 0x3c) + row * 0x23;
        left = (f32)x - dw * lbl_8047B940;
        *(s16*)(draw + 0x50) = left;
        top = (f32)y - dh * lbl_8047B940;
        right = width + dw;
        bottom = height + dh;
        *(s16*)(draw + 0x52) = top;
        *(s16*)(draw + 0x54) = right;
        *(s16*)(draw + 0x56) = bottom;
    } else {
        alpha = 0;
    }
    draw[0x67] = alpha;
    return 0;
}
