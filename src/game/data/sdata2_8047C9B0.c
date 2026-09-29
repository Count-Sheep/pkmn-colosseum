#include "dolphin/types.h"

#pragma section ".sdata2"
#define SDATA2 __declspec(section ".sdata2")

/*
 * Mixed gs_render_util, gs_gfx, and gs_render constants (continued from
 * sdata2_8047C9A0.c; 0x8047C9A8, the unsigned int-to-float bias, is the
 * compiler literal of gs_range_800D1070.c). Assert strings are referenced
 * from cobj/aobj checks; numeric labels are render camera/light constants
 * and int-to-float conversion biases.
 */
SDATA2 const f32 lbl_8047C9B0 = 1.20000005f;
SDATA2 const f64 lbl_8047C9B8 = 0.0;
SDATA2 const f32 lbl_8047C9C0 = 2000.0f;
SDATA2 const u8 lbl_8047C9C4[7] = "cobj.h";
SDATA2 const u8 lbl_8047C9CC[5] = "cobj";
SDATA2 const f32 lbl_8047C9D4 = 60.0f;
SDATA2 const f32 lbl_8047C9D8 = 1.33333302f;
SDATA2 const u8 lbl_8047C9DC[7] = "aobj.h";
SDATA2 const u8 lbl_8047C9E4[12] = "aobj";
SDATA2 const f32 lbl_8047C9F0 = 255.0f;
SDATA2 const f64 lbl_8047C9F8 = 4.503599627370496e+15;
SDATA2 const f32 lbl_8047CA00 = 0.0f;
SDATA2 const f32 lbl_8047CA04 = 640.0f;
SDATA2 const f32 lbl_8047CA08 = 480.0f;
SDATA2 const f32 lbl_8047CA0C = 1.0f;
SDATA2 const f32 lbl_8047CA10 = 30.0f;
SDATA2 const f32 lbl_8047CA14 = 1.17900002f;
SDATA2 const f32 lbl_8047CA18 = 0.100000001f;
SDATA2 const f32 lbl_8047CA1C = 30000.0f;
/* 0x8047CA20-0x8047CA30 and 0x8047CA30-0x8047CA40 are the literal pools of
 * gs_gfx_candidate_800D3FA4.c and gs_gfx_candidate_800D55D0.c; the rest
 * continues in sdata2_8047CA40.c. */
