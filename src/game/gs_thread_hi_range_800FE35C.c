/**
 * @file gs_thread_hi_range_800FE35C.c
 * @brief Sprite screen environment: the whole retail TU at
 *        .text 0x800FE35C-0x800FE6DC (scissor, sprite camera/projection,
 *        screen scale and sprite origin accessors).
 *
 * Extent: GSmsg's text ends at 0x800FE35C and gs_gapp.c starts at
 * 0x800FE6DC. Its .sdata2 pool is 0x8047CD50-0x8047CD80 (the int-to-float
 * bias, then 640, 480, 0.5, the 15-degree field of view, 0.0, -1.0, 30,
 * 0.1, 30000): GSmsg's constants end at 0x8047CD50 and gapp's start at
 * 0x8047CD80, and no other code references this run. Compiled as one unit
 * this source emits exactly that pool in that order. The screen scale
 * (lbl_80478B10/14, .sdata) and sprite origin (lbl_8047AC70/72, .sbss)
 * stay extern.
 *
 * Flags: GC/1.3, -O4,p and -opt nopeephole unit-wide, no local pragmas.
 * With that single setting fn_800FE35C, spriteSetEnv and the four
 * accessors are exact; without -opt nopeephole fn_800FE35C's prologue
 * store is scheduled after the argument loads, unlike retail.
 *
 * Still a candidate: fn_800FE38C differs from retail in one register
 * choice only (retail computes x1 = x0 + w into w's register r5, this
 * source into x0's r3; 2 instructions). The pool cannot be carved away
 * from it: fn_800FE38C is the bias constant's first user.
 *
 * fn_800FE35C (gs_thread_hi_exact_800FE35C.c) and the accessors
 * fn_800FE6A0 / fn_800FE6AC / fn_800FE6D0 (gs_thread_hi_exact_800FE6A0.c)
 * use none of the pool and link as carves; this whole-TU source scores
 * 0x800FE38C - 0x800FE6A0 (fn_800FE38C, spriteSetEnv).
 *
 * fn_800FE38C wall (2026-09-28, lane U3): the register replay is exact
 * (GC/2.6 replays the unit's code identically). x1's temporary is coloured
 * after the xoris that consumes it and takes x0's dead r3; retail's r5
 * needs r3 blocked at that point. Tried without effect or worse: params
 * reused as x0/x1 (w += x0 ...), x1 = w + x0, (f32)(x0 + w) in place,
 * no sx/sy locals, x0/x1 computed back to back, declaration orders.
 */
#include "dolphin/types.h"

extern f32 lbl_80478B10; /* screen x scale */
extern f32 lbl_80478B14; /* screen y scale */
extern s16 lbl_8047AC70; /* sprite origin x */
extern s16 lbl_8047AC72; /* sprite origin y */

extern f64 tan(f64 x);
extern void set__5GSvecFfff(f32* v, f32 x, f32 y, f32 z);
extern void fn_800E0218(f32* mtx, f32* eye, f32* up, f32* target);
extern void fn_800D9BD0(f32 fovy, f32 aspect, f32 nearZ, f32 farZ);
extern void fn_800D834C(void);
extern void fn_800D7FE4(f32* mtx);
extern void fn_800DA4C4(s32 a, s32 b, s32 c);
extern void fn_800D888C(u32 mask);
extern void fn_800DA2BC(s32 a, s32 b, s32 c);
extern void fn_800DA100(s32 a, s32 b, s32 c, s32 d, s32 e, s32 f);
extern void fn_800DA1E8(s32 a, s32 b, s32 c);
extern void fn_800DA028(s32 a);
extern void fn_800D9D68(u16 left, u16 top, u16 right, u16 bottom);

void fn_800FE35C(void) {
    fn_800D9D68(0, 0, 639, 479);
}

void fn_800FE38C(s32 x, s32 y, s32 w, s32 h) {
    s32 x0, y0, x1, y1;
    f32 sx, sy;
    s32 bottom, right, top, left;

    x0 = lbl_8047AC70 + x;
    y0 = lbl_8047AC72 + y;
    x1 = x0 + w;
    y1 = y0 + h;
    sx = lbl_80478B10;
    sy = lbl_80478B14;
    left = (f32)x0 * sx;
    top = (f32)y0 * sy;
    right = (f32)x1 * sx;
    bottom = (f32)y1 * sy;
    if (left >= 640) left = 639;
    if (top >= 480) top = 479;
    if (right >= 640) right = 639;
    if (bottom >= 480) bottom = 479;
    if (left < 0) left = 0;
    if (top < 0) top = 0;
    if (right < 0) right = 0;
    if (bottom < 0) bottom = 0;
    fn_800D9D68(left, top, right, bottom);
}

void spriteSetEnv(void) {
    f32 eye[3];
    f32 target[3];
    f32 up[3];
    f32 mtx[12];
    f32 cx;
    f32 cy;
    f32 width;
    f32 height;
    f32 z;
    f32 fov;
    f32 half;

    width = 640.0f / lbl_80478B10;
    height = 480.0f / lbl_80478B14;
    fov = 0.2617994f;
    half = 0.5f;
    cx = width * half;
    cy = height * half;
    z = cy / (f32)tan(fov);

    set__5GSvecFfff(eye, cx - lbl_8047AC70, cy - lbl_8047AC72, z);
    set__5GSvecFfff(target, cx - lbl_8047AC70, cy - lbl_8047AC72, 0.0f);
    set__5GSvecFfff(up, 0.0f, -1.0f, 0.0f);
    fn_800E0218(mtx, eye, up, target);

    fn_800D9BD0(30.0f, -(width / height), 0.1f, 30000.0f);
    fn_800D834C();
    fn_800D7FE4(mtx);
    fn_800DA4C4(1, 6, 7);
    fn_800D888C(0x80000000);
    fn_800DA2BC(2, 2, 1);
    fn_800DA100(0, 7, 0, 1, 7, 0);
    fn_800DA1E8(0, 2, 0);
    fn_800DA028(0);
}

void fn_800FE6A0(f32 x, f32 y) {
    lbl_80478B10 = x;
    lbl_80478B14 = y;
}

void fn_800FE6AC(s16* x, s16* y) {
    if (x != NULL) {
        *x = lbl_8047AC70;
    }
    if (y != NULL) {
        *y = lbl_8047AC72;
    }
}

void fn_800FE6D0(s16 x, s16 y) {
    lbl_8047AC70 = x;
    lbl_8047AC72 = y;
}
