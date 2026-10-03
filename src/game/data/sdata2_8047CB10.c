#include "dolphin/types.h"

#pragma section ".sdata2"
#define SDATA2 __declspec(section ".sdata2")

/*
 * GSmath .sdata2 constants (0x8047CB10-0x8047CB40), split from
 * sdata2_8047CAC8.c. This range stops before GSmathInitCosTable's
 * compiler-owned conversion constant at 0x8047CB40.
 */
SDATA2 const f32 lbl_8047CB10[2] = { 1.0f, 0.0f };
SDATA2 const f32 lbl_8047CB18 = 90.0f;
SDATA2 const f32 lbl_8047CB1C = 1.0f;
SDATA2 const f32 lbl_8047CB20 = 180.0f;
SDATA2 const f32 lbl_8047CB24 = -1.0f;
SDATA2 const f64 lbl_8047CB28 = 180.0;
SDATA2 const f32 lbl_8047CB30 = 0.5f;
SDATA2 const f32 lbl_8047CB34 = 2.0f;
SDATA2 const f32 lbl_8047CB38 = 0.01745329238474369f;
