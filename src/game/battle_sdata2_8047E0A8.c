#include "dolphin/types.h"

#pragma section ".sdata2"
#define SDATA2 __declspec(section ".sdata2")

/*
 * fade_fluid.o's .sdata2 pool and the following run up to 0x8047E160.
 * 0x8047DFD8 - 0x8047E0A8 is the fade effect TU's own pool, emitted by
 * game/effect/fade_effect.c.
 */

SDATA2 const f32 lbl_8047E0A8 = 1.0f;
SDATA2 const f32 lbl_8047E0AC = 0.0f;
SDATA2 const f32 lbl_8047E0B0 = 255.0f;
SDATA2 const f64 lbl_8047E0B8 = 4.503601774854144e+15;
SDATA2 const f32 lbl_8047E0C0 = 0.5f;
SDATA2 const f32 lbl_8047E0C4 = 2.0f;
SDATA2 const f32 lbl_8047E0C8 = 4.0f;
SDATA2 const f32 lbl_8047E0CC = 8.0f;
SDATA2 const f32 lbl_8047E0D0 = 640.0f;
SDATA2 const f32 lbl_8047E0D4 = 480.0f;
SDATA2 const f64 lbl_8047E0D8 = 0.5;
SDATA2 const f64 lbl_8047E0E0 = 3.0;
SDATA2 const f64 lbl_8047E0E8 = 0.0;
SDATA2 const f32 lbl_8047E0F0 = 1.0e-6f;
SDATA2 const f64 lbl_8047E0F8 = 4.503599627370496e+15;
SDATA2 const f32 lbl_8047E100 = 1.0f;
SDATA2 const f32 lbl_8047E104 = 20.0f;
SDATA2 const f32 lbl_8047E108 = 0.5f;
SDATA2 const f32 lbl_8047E10C = 0.7853982f;
SDATA2 const f32 lbl_8047E110 = 8.0f;
SDATA2 const f32 lbl_8047E114 = 0.0f;
SDATA2 const f32 lbl_8047E118 = 4.0f;
SDATA2 const f32 lbl_8047E11C = 3.0f;
SDATA2 const f64 lbl_8047E120 = 4.503601774854144e+15;
SDATA2 const f64 lbl_8047E128 = 4.503599627370496e+15;
SDATA2 const f32 lbl_8047E130 = 5.0f;
SDATA2 const f32 lbl_8047E134 = -5.0f;
SDATA2 const f32 lbl_8047E138 = 10.0f;
SDATA2 const f32 lbl_8047E13C = 0.2f;
SDATA2 const f32 lbl_8047E140 = 25.0f;
SDATA2 const f32 lbl_8047E144 = 100.0f;
SDATA2 const f32 lbl_8047E148[2] = { 2.0f, 0.0f };
/* 0x8047E150-0x8047E160 (0.5f and the s32-to-float bias) is owned by
 * field_candidate_801CB834.c. */
