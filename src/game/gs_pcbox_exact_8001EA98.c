/**
 * @file gs_pcbox_exact_8001EA98.c
 * @brief fn_8001EA98 (0x8001EA98 - 0x8001EC08): draws a translucent
 *        rectangle 10 units larger than the given box on each side, then
 *        its white outline.
 *
 * Function-boundary carve of the 0x8001E3E0 segment (see
 * gs_pcbox_range_8001E3E0.c), built with GC/1.3 -O4,p and "-opt
 * nopeephole" (without it the prologue's register copies are scheduled
 * between the saves). Text only; its one pooled literal, the 1.0f line
 * width (0x8047B7E0), is shared with fn_8001E644 and menuSub.c, so it is
 * read through an extern stand-in. Callers pass the box as ints
 * (gs_event_exec.c, gs_task.c).
 */
#include "dolphin/types.h"

extern u8 lbl_80314E08[];
/* RULE-EXCEPTION(title-path): extern stand-in for the TU's shared 1.0f pool literal - see docs/RULE_EXCEPTIONS.md */
extern const f32 lbl_8047B7E0;
extern void fn_800D5648(f32 width);
extern void fn_800D5BA0(s32 index, s32 color);
extern void fn_800D61E4(s16 x, s16 y);
extern void fn_800D6728(void);
extern void fn_800D67BC(s32 count);
extern void fn_800D6A00(s32 primitive);
extern void fn_800D7820(void* desc);
extern void fn_800D888C(s32 mode);
extern void fn_800D88DC(s32 mode);

/* 0x8001EA98 | 0x170 */
void fn_8001EA98(s32 x, s32 y, s32 width, s32 height) {
    s32 right;
    s32 bottom;

    fn_800D88DC(1);
    fn_800D888C(6);
    fn_800D6A00(7);
    fn_800D7820(lbl_80314E08);
    fn_800D67BC(2);
    fn_800D61E4(x - 10, y - 10);
    fn_800D5BA0(0, 0xC0);
    right = x + width + 10;
    bottom = y + height + 10;
    fn_800D61E4(right, bottom);
    fn_800D5BA0(0, 0xC0);
    fn_800D6728();

    fn_800D5648(lbl_8047B7E0);
    fn_800D6A00(2);
    fn_800D67BC(5);
    fn_800D61E4(x - 10, y - 10);
    fn_800D5BA0(0, -1);
    fn_800D61E4(right, y - 10);
    fn_800D5BA0(0, -1);
    fn_800D61E4(right, bottom);
    fn_800D5BA0(0, -1);
    fn_800D61E4(x - 10, bottom);
    fn_800D5BA0(0, -1);
    fn_800D61E4(x - 10, y - 10);
    fn_800D5BA0(0, -1);
    fn_800D6728();
}
