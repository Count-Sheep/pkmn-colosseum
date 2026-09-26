#include "dolphin/types.h"

#pragma section ".sdata2"
#define SDATA2 __declspec(section ".sdata2")

/*
 * Mixed HSD .sdata2 constants and assert strings used by pobj, shadow and spline code.
 * The state.c, tev.c and texp.c blocks from 0x8047DE50 on belong to those
 * units.
 */
SDATA2 const f32 lbl_8047DE00 = 0.0f;
SDATA2 const f32 lbl_8047DE04 = 1.0f;
SDATA2 const f32 lbl_8047DE08 = 0.5f;
SDATA2 const f32 lbl_8047DE0C = 9.999999747378752e-06f;
SDATA2 const f64 lbl_8047DE10 = 4.503601774854144e+15;
SDATA2 const f32 lbl_8047DE18 = 0.125f;
SDATA2 const f32 lbl_8047DE1C = -0.0010000000474974513f;
SDATA2 const f64 lbl_8047DE20 = 0.5;
SDATA2 const f64 lbl_8047DE28 = 3.0;
SDATA2 const f64 lbl_8047DE30 = 0.0;
SDATA2 const f32 lbl_8047DE38 = 4.0f;
SDATA2 const f32 lbl_8047DE3C = 2.0f;
SDATA2 const f32 lbl_8047DE40 = 3.0f;
SDATA2 const f32 lbl_8047DE44 = 0.1666666716337204f;
SDATA2 const f32 lbl_8047DE48[2] = { 6.0f, 0.0f };
