#include "dolphin/types.h"

#pragma section ".sdata2"
#define SDATA2 __declspec(section ".sdata2")

/*
 * Early game .sdata2 constants following game/gs_range_800096B4.c's own
 * literal at 0x8047B6D8 (see sdata2_8047B6B8.c). 0x8047B6F8 is
 * game/gs_range_8000D290.c's own literal; the constants from 0x8047B700 on
 * are in sdata2_8047B700.c.
 */
SDATA2 const f32 lbl_8047B6E0[2] = { 0.5f, 0.0f };
SDATA2 const f32 lbl_8047B6E8[2] = { 0.5f, 0.0f };
SDATA2 const f32 lbl_8047B6F0 = 20.0f;
