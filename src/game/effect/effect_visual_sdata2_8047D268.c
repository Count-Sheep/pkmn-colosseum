#include "dolphin/types.h"

#pragma section ".sdata2"
#define SDATA2 __declspec(section ".sdata2")

/*
 * effect_visual .sdata2 constants after fn_8013D984's pool (0x8047D260),
 * which links with that function. Source references for this range are in
 * src/game/effect/effect_visual.c, and the target slice has no relocations.
 */
SDATA2 const f32 lbl_8047D268 = 0.5f;
SDATA2 const f32 lbl_8047D26C = 60.0f;
SDATA2 const f32 lbl_8047D270 = 30.0f;
SDATA2 const f32 lbl_8047D274 = 20.0f;
SDATA2 const f32 lbl_8047D278 = 15.0f;
SDATA2 const f32 lbl_8047D27C = 0.0f;
SDATA2 const f32 lbl_8047D280 = 1.0f;
SDATA2 const f64 lbl_8047D288 = 4.503599627370496e+15;
SDATA2 const f64 lbl_8047D290 = 4.503601774854144e+15;
