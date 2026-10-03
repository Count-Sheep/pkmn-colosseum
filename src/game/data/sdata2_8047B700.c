#include "dolphin/types.h"

#pragma section ".sdata2"
#define SDATA2 __declspec(section ".sdata2")

/*
 * Mixed early game .sdata2 constants after game/gs_range_8000D290.c's own
 * 0x8047B6F8 literal (the run continues from sdata2_8047B6B8.c). Ends where
 * gs_range_80011EA4.c's pool (0x8047B718-0x8047B740) begins.
 */
SDATA2 const f32 lbl_8047B700 = 0.0f;
SDATA2 const f32 lbl_8047B704 = 0.785398185f;
SDATA2 const f32 lbl_8047B708 = 2.3561945f;
SDATA2 const f64 lbl_8047B710 = 4.503601774854144e+15;
