#include "dolphin/types.h"

#pragma section ".sdata2"
#define SDATA2 __declspec(section ".sdata2")

/*
 * Mixed Dolphin SDK and CRT .sdata2 constants. 0x8047C388 - 0x8047C3E0 is
 * dolphin/sdk_candidate_800BC8F8.c's own pool and 0x8047C3E0 - 0x8047C3E8 is
 * dolphin/sdk_candidate_800BD16C.c's. 0x8047C3E8 (0.5) is shared by GXProject
 * and the GX transform code at 0x800BD58C. 0x8047C3F0 (the u32-to-float
 * bias) is dolphin/sdk_candidate_800BD454.c's own literal, and 0x8047C3F8 -
 * 0x8047C400 is sdata2_8047C3F8.c.
 */
SDATA2 const f32 lbl_8047C3E8 = 0.5f;
