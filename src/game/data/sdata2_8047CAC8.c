#include "dolphin/types.h"

#pragma section ".sdata2"
#define SDATA2 __declspec(section ".sdata2")

/*
 * gs_material .sdata2 constants (0x8047CAC8-0x8047CAF8). The GSlog
 * formatters' pool before it (0x8047CAA0-0x8047CAC8: "(float)", "(null)" and
 * the %f constants) is compiled by game/gs_log.cpp. gs_math_range_800E09E8's
 * pool (0x8047CAF8-0x8047CB10) follows; sdata2_8047CB10.c owns the rest.
 */
SDATA2 const f32 lbl_8047CAC8 = 0.0f;
SDATA2 const f32 lbl_8047CACC = 255.0f;
SDATA2 const f64 lbl_8047CAD0 = 4.503599627370496e+15;
SDATA2 const f32 lbl_8047CAD8 = 1.0f;
SDATA2 const f32 lbl_8047CADC = 0.0f;
SDATA2 const f32 lbl_8047CAE0 = 1.0f;
SDATA2 const f32 lbl_8047CAE4 = 0.0f;
SDATA2 const f32 lbl_8047CAE8[2] = { 3.0f, 0.0f };
SDATA2 const f32 lbl_8047CAF0 = 0.0f;
SDATA2 const f32 lbl_8047CAF4 = 1.0f;
