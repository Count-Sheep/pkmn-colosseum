#include "dolphin/types.h"

#pragma section ".sdata2"
#define SDATA2 __declspec(section ".sdata2")

/*
 * Name-entry, worldmap and shop .sdata2 constants after gs_title_80023DA8.c's
 * pool (split from sdata2_8047B8A0.c at 0x8047B920): the list-id table at
 * B920, packed route-id words at B928/B92C, and the map animation/phase
 * constants.
 */
SDATA2 const u32 lbl_8047B920[2] = { 0, 0x00002EF7u };
SDATA2 const u32 lbl_8047B928 = 0x040A040Bu;
SDATA2 const u32 lbl_8047B92C = 0x040B040Cu;
SDATA2 const f32 lbl_8047B930 = 0.0f;
SDATA2 const f32 lbl_8047B934 = 1.0f;
SDATA2 const f32 lbl_8047B938 = 255.0f;
SDATA2 const f32 lbl_8047B93C = 0.25f;
SDATA2 const f32 lbl_8047B940 = 0.5f;
