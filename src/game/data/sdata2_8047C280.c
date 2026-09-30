#include "dolphin/types.h"

#pragma section ".sdata2"
#define SDATA2 __declspec(section ".sdata2")

/*
 * 0x8047C280 - 0x8047C2A0, split from sdata2_8047C1A0.c when the OSFatal
 * carve (dolphin/sdk_candidate_8009CD38.c) took its pool 0x8047C240 -
 * 0x8047C280.
 */
SDATA2 const u32 lbl_8047C280 = 0x2ABE003D;
SDATA2 const u32 lbl_8047C284 = 0x003D003D;
SDATA2 const f32 lbl_8047C288 = 1.0f;
SDATA2 const f32 lbl_8047C28C = 0.0f;
SDATA2 const f32 lbl_8047C290 = 0.5f;
SDATA2 const f32 lbl_8047C294 = 3.0f;
SDATA2 const f32 lbl_8047C298 = 2.0f;
SDATA2 const f32 lbl_8047C29C = -1.0f;
