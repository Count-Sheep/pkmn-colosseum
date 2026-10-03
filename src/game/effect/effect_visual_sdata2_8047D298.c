#include "dolphin/types.h"

#pragma section ".sdata2"
#define SDATA2 __declspec(section ".sdata2")

/*
 * effect_visual .sdata2 constants. Source references for this range are in
 * src/game/effect/effect_visual.c; the target slice has no relocations.
 * Compiler alignment accounts for the padding before f64 constants and after
 * the short strings.
 */
SDATA2 const f32 lbl_8047D298 = 255.0f;
SDATA2 const f32 lbl_8047D29C = 640.0f;
SDATA2 const f32 lbl_8047D2A0[2] = { 480.0f, 0.0f };
SDATA2 const f32 lbl_8047D2A8 = 1.0f;
SDATA2 const f32 lbl_8047D2AC = 0.0f;
SDATA2 const f32 lbl_8047D2B0 = 640.0f;
SDATA2 const f32 lbl_8047D2B4 = 480.0f;
