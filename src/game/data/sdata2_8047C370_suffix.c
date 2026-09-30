#include "dolphin/types.h"

#pragma section ".sdata2"
#define SDATA2 __declspec(section ".sdata2")

/* Mixed Dolphin SDK .sdata2 constants after the GXTexture literal pool.
 * 0x8047C370 - 0x8047C388 is dolphin/sdk_range_800BB81C.c's own pool. */
SDATA2 const f32 lbl_8047C388 = 0.0f;
SDATA2 const f32 lbl_8047C38C = 1.0f;
SDATA2 const f32 lbl_8047C390 = 0.5f;
SDATA2 const f64 lbl_8047C398 = 1.0;
