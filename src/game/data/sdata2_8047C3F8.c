#include "dolphin/types.h"

#pragma section ".sdata2"
#define SDATA2 __declspec(section ".sdata2")

/*
 * GX transform constants used by the code at 0x800BD58C (split from
 * sdata2_8047C3E8.c when dolphin/sdk_candidate_800BD454.c took its own
 * literal at 0x8047C3F0).
 */
SDATA2 const f32 lbl_8047C3F8 = 342.0f;
SDATA2 const f32 lbl_8047C3FC = 16777215.0f;

/* 0x8047C400 (0.0) is dolphin/sdk_range_800C5458.c's own literal;
 * 0x8047C408 is sdata2_8047C408.c. */
