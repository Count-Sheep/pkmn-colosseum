#include "dolphin/types.h"

#pragma section ".sdata2"
#define SDATA2 __declspec(section ".sdata2")

/* psappsrt.c's pool; particle.c (0x8047D5B0..0x8047D5C0) owns the pool
 * before this one and psdisp.c (0x8047D5C8..0x8047D628) the one after. */
SDATA2 const f32 lbl_8047D5C0 = 0.0f;
SDATA2 const f32 lbl_8047D5C4 = 1.0f;
