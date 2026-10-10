#include "dolphin/types.h"

#pragma section ".sdata2"
#define SDATA2 __declspec(section ".sdata2")

/*
 * The .sdata2 run 0x8047E100 - 0x8047E150 after fade_fluid.o's pool.
 * 0x8047E0A8 - 0x8047E100 is fade_fluid.o's own pool, emitted by
 * game/effect/fade_range_801C6934.c; 0x8047DFD8 - 0x8047E0A8 is the fade
 * effect TU's pool, emitted by game/effect/fade_effect.c.
 */

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
