/* Name-entry keyboard cursor, 0x8002749C-0x800275F4. */
#include "dolphin/types.h"

extern u8 lbl_802EF0A8[];
extern f32 lbl_8047B93C, lbl_8047B934, lbl_8047B940, lbl_8047B938;

s32 menuNameEntryDraw50Cursor(void* window, u8* draw)
{
    u8* ctx;
    f32 dh;
    f32 width;
    f32 height;
    f32 dw;
    s32 column;
    s32 row;
    s16 left;
    f32 top;
    s16 right;
    s16 bottom;
    u8 alpha;

    ctx = *(u8**)((u8*)window + 0x60);
    column = **(s32**)(ctx + 0x28);
    width = ((s16*)(lbl_802EF0A8 + *(s16*)(draw + 6) * 0x1c))[3];
    height = ((s16*)(lbl_802EF0A8 + *(s16*)(draw + 6) * 0x1c))[4];
    row = **(s32**)(ctx + 0x2c);
    dw = lbl_8047B93C * (width * (**(f32**)(ctx + 0x30)));
    dh = lbl_8047B93C * (height * (**(f32**)(ctx + 0x30)));
    if (column < 0xf) {
        left = (f32)(**(s32**)(ctx + 0x38) + column * 0x1b) - dw * lbl_8047B940;
        top = (f32)(**(s32**)(ctx + 0x3c) + row * 0x23) - dh * lbl_8047B940;
        right = width + dw;
        bottom = height + dh;
        alpha = lbl_8047B938 * (lbl_8047B934 - (**(f32**)(ctx + 0x30)));
        *(s16*)(draw + 0x50) = left;
        *(s16*)(draw + 0x52) = top;
        *(s16*)(draw + 0x54) = right;
        *(s16*)(draw + 0x56) = bottom;
    } else {
        alpha = 0;
    }
    draw[0x67] = alpha;
    return 0;
}
