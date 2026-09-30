#include "dolphin/types.h"

#pragma section ".sdata2"
#define SDATA2 __declspec(section ".sdata2")

/*
 * 0x8047C408 - 0x8047C410, split from sdata2_8047C3A0.c when the MSL carve
 * (dolphin/sdk_range_800C5458.c) took its 0.0 literal at 0x8047C400.
 */
SDATA2 const u32 lbl_8047C408 = 0x0000C0E0;
