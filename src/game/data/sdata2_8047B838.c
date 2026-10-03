#include "dolphin/types.h"

#pragma section ".sdata2"
#define SDATA2 __declspec(section ".sdata2")

/* Pool after the conversion biases that src/game/gs_pcbox_exact_8001EF78.c
 * owns (0x8047B828-0x8047B838), up to gs_title_80020618.c's own pool
 * (0x8047B868). */
SDATA2 const f32 lbl_8047B838 = 3.1415927f;
SDATA2 const f32 lbl_8047B83C = 2.0f;
SDATA2 const f32 lbl_8047B840 = 200.0f;
SDATA2 const f32 lbl_8047B844 = 128.0f;
SDATA2 const f32 lbl_8047B848 = 255.0f;
SDATA2 const f32 lbl_8047B84C = 300.0f;
SDATA2 const f32 lbl_8047B850 = -300.0f;
SDATA2 const f32 lbl_8047B854 = 400.0f;
SDATA2 const f32 lbl_8047B858 = -400.0f;
SDATA2 const f32 lbl_8047B85C = 500.0f;
SDATA2 const f32 lbl_8047B860[2] = { -500.0f, 0.0f };
