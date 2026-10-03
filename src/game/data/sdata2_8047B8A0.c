#include "dolphin/types.h"

#pragma section ".sdata2"
#define SDATA2 __declspec(section ".sdata2")

/*
 * The title code's 0.0f and 0.5f before gs_title_80023DA8.c's own pool
 * (0x8047B8A8-0x8047B920); the run continues in sdata2_8047B920.c.
 */
SDATA2 const f32 lbl_8047B8A0 = 0.0f;
SDATA2 const f32 lbl_8047B8A4 = 0.5f;
