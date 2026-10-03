#include "dolphin/types.h"

#pragma section ".sdata2"
#define SDATA2 __declspec(section ".sdata2")

/*
 * Mixed Dolphin SDK and CRT .sdata2 constants. 0x8047C388 - 0x8047C3E0 is
 * dolphin/sdk_candidate_800BC8F8.c's own pool. Target relocations tie the
 * rest to stdio, wcstombs, float2str, and fdlibm-style acos constants.
 */
SDATA2 const f32 lbl_8047C3E0 = 0.0f;
SDATA2 const f32 lbl_8047C3E4 = 1.0f;
SDATA2 const f32 lbl_8047C3E8 = 0.5f;
SDATA2 const f64 lbl_8047C3F0 = 4503599627370496.0;
SDATA2 const f32 lbl_8047C3F8 = 342.0f;
SDATA2 const f32 lbl_8047C3FC = 16777215.0f;

/* 0x8047C400 (0.0) is dolphin/sdk_range_800C5458.c's own literal;
 * 0x8047C408 is sdata2_8047C408.c. */
