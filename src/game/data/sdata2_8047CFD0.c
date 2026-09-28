#include "dolphin/types.h"

#pragma section ".sdata2"
#define SDATA2 __declspec(section ".sdata2")

/*
 * gs_field_world .sdata2 constants from 0x8047CFD0 (field_camera's pool
 * first; the floorCharacterBios pool, 0x8047CFC0-0x8047CFD0, precedes
 * them). Carved from sdata2_8047CFA0.c. Most labels are numeric
 * field/camera constants.
 */
SDATA2 const f32 lbl_8047CFD0 = 0.0f;
SDATA2 const f32 lbl_8047CFD4 = 0.25f;
SDATA2 const f32 lbl_8047CFD8 = 0.75f;
SDATA2 const f32 lbl_8047CFDC = 1.0f;
SDATA2 const f32 lbl_8047CFE0[2] = { 0.0010000000474974513f, 0.0f };
SDATA2 const f32 lbl_8047CFE8 = 0.0f;
SDATA2 const f32 lbl_8047CFEC = 1.0f;
SDATA2 const f32 lbl_8047CFF0 = 0.0f;
SDATA2 const f32 lbl_8047CFF4 = 100.0f;
SDATA2 const f32 lbl_8047CFF8 = 80.0f;
SDATA2 const f32 lbl_8047CFFC = 60.0f;
SDATA2 const f32 lbl_8047D000 = 40.0f;
SDATA2 const f32 lbl_8047D004 = 20.0f;
SDATA2 const f64 lbl_8047D008 = 4.503601774854144e+15;
SDATA2 const f64 lbl_8047D010 = 4.503599627370496e+15;
SDATA2 const f32 lbl_8047D018[2] = { -1.0f, 0.0f };
