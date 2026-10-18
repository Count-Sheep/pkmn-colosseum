/**
 * @file pda_exact_800495C8.c
 * @brief PDA mail-list backdrop, 0x800495C8 - 0x800499BC: fn_800495C8 fades
 *        the panel, draws the scanlines and the scrolling sheared page edge.
 *        Copied from pda_range_80037158.c.
 */
#include "dolphin/types.h"

typedef struct PdaSprite {
    u8 pad00[0x4];
    s8 flags;
    u8 pad05;
    s16 eventId;
    u8 pad08[0x44];
    s32 messageId;
    s16 field_50;
    s16 field_52;
    s16 x;
    s16 y;
    u8 pad58[0xc];
    u8 colorR;
    u8 colorG;
    u8 colorB;
    u8 alpha;
    u8 pad68[0x8];
    f32 value;
    u8 pad74[0x17];
    u8 alphaByte;
    u8 pad8c[9];
    s8 selectedIndex;
} PdaSprite;

/* Mail-screen state: [0] selected tab, [1] menu state, +0x08 frame step,
 * +0x28/+0x2C backdrop fade (current/target), +0x49 off-screen display. */
extern u8 lbl_803A6A60[];
extern u8 lbl_80314E08[];

extern s32 fn_800411EC(void);
extern void menuOffScreenSetDisp(s32 disp);
extern void fn_800D5648(f32 width);
extern void fn_800D5BA0(s32 index, u32 color);
extern void fn_800D61E4(s16 x, s16 y);
extern void fn_800D6728(void);
extern void fn_800D67BC(s32 count);
extern void fn_800D6A00(s32 primitive);
extern void fn_800D7820(void* data);
extern void fn_800D888C(s32 mode);
extern void fn_800D88DC(s32 mode);

static inline void pda_800495C8_Scanline(PdaSprite* sprite, s16 y, s32 alpha)
{
    u32 color;

    fn_800D88DC(1);
    fn_800D888C(6);
    fn_800D7820(lbl_80314E08);
    fn_800D6A00(1);
    fn_800D5648(1.0f);
    fn_800D67BC(2);
    fn_800D61E4(0, y);
    color = alpha | 0x00800000;
    fn_800D5BA0(0, color);
    fn_800D61E4(sprite->x, y);
    fn_800D5BA0(0, color);
    fn_800D6728();
}

/* RULE-EXCEPTION(user-approved): single-use inline helper pda_800495C8_RowLine (fn_800495C8) - see docs/RULE_EXCEPTIONS.md */
static inline void pda_800495C8_RowLine(PdaSprite* sprite, f32 y, s32 alpha)
{
    s16 row = 480.0f * y;

    pda_800495C8_Scanline(sprite, row, alpha);
}

/* RULE-EXCEPTION(user-approved): single-use inline helper pda_800495C8_Panel (fn_800495C8) - see docs/RULE_EXCEPTIONS.md */
static inline void pda_800495C8_Panel(PdaSprite* sprite, s32 alpha)
{
    u32 color;

    fn_800D61E4(0, 0);
    color = (u8)alpha | 0x03140e00;
    fn_800D5BA0(0, color);
    fn_800D61E4(sprite->x, 0);
    fn_800D5BA0(0, color);
    fn_800D61E4(0, sprite->y);
    fn_800D5BA0(0, color);
    fn_800D61E4(sprite->x, sprite->y);
    fn_800D5BA0(0, color);
    fn_800D6728();
}

/* Mail-list backdrop: fade panel, scanlines, and the sheared page edge. */
s32 fn_800495C8(void* work, PdaSprite* sprite)
{
    u8* G = lbl_803A6A60;
    u8* data;
    f32 top;
    f32 y;
    u8 fade;
    s32 j;
    s32 i;
    s32 alpha;
    f32 step;

    data = *(u8**)((u8*)work + 0x60);
    if (*(f32*)(G + 0x2c) != *(f32*)(G + 0x28)) {
        step = *(f32*)(G + 8) / 0.5f;
        if (*(f32*)(G + 0x2c) > *(f32*)(G + 0x28)) {
            *(f32*)(G + 0x28) = *(f32*)(G + 0x28) + step;
            if (*(f32*)(G + 0x28) > 1.0f) {
                *(f32*)(G + 0x28) = 1.0f;
            }
        } else {
            *(f32*)(G + 0x28) = *(f32*)(G + 0x28) - step;
            if (*(f32*)(G + 0x28) < 0.0f) {
                *(f32*)(G + 0x28) = 0.0f;
            }
        }
    }
    fn_800411EC();
    if (*(s8*)(lbl_803A6A60 + 0) == 1) {
        if (lbl_803A6A60[0x49] != 0 && lbl_803A6A60[0x4a] == 0) {
            menuOffScreenSetDisp(0);
            lbl_803A6A60[0x49] = 0;
        }
        if (*(s8*)(lbl_803A6A60 + 1) >= 1) {
            *(f32*)(G + 0x2c) = 0.0f;
            return 0;
        }
        *(f32*)(G + 0x2c) = 1.0f;
    } else {
        if (lbl_803A6A60[0x49] != 1) {
            menuOffScreenSetDisp(1);
            lbl_803A6A60[0x49] = 1;
        }
        *(f32*)(G + 0x2c) = 1.0f;
    }

    fn_800D88DC(1);
    fn_800D888C(6);
    fn_800D7820(lbl_80314E08);
    fn_800D6A00(4);
    fn_800D67BC(4);
    alpha = (s32)(255.0f * *(f32*)(G + 0x28));
    pda_800495C8_Panel(sprite, alpha);

    for (i = 0; i < 0x1e0; i += 4) {
        pda_800495C8_Scanline(sprite, i, 0x80);
    }

    top = **(f32**)(data + 8);
    for (j = 0; j < 12; j++) {
        fade = (s32)(255.0f * (1.0f - (f32)j / 12.0f));
        y = top - 4.0f * (f32)j / 480.0f;
        if (y < 0.0f) {
            y += 1.0f;
        }
        /* RULE-EXCEPTION(user-approved): redundant mask on the u8 row alpha, chosen for code generation (fn_800495C8) - see docs/RULE_EXCEPTIONS.md */
        pda_800495C8_RowLine(sprite, y, fade & 0xff);
    }
    return 0;
}
