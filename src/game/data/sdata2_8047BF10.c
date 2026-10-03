#include "dolphin/types.h"

#pragma section ".sdata2"
#define SDATA2 __declspec(section ".sdata2")

/*
 * .sdata2 run 0x8047BF10 - 0x8047BFA0 (menuColosseumBattle and neighbours),
 * moved unchanged out of the former sdata2_8047BEA0.c when the menuCB units
 * menuCB_80055E38.c and menuCB_80056C54.c took ownership of their pools
 * (0x8047BEA0-0x8047BEE0 and 0x8047BEE0-0x8047BF10).
 */
SDATA2 const f32 lbl_8047BF10[2] = { 0.5f, 0.0f };
SDATA2 const f32 lbl_8047BF18 = 57.29578f;
SDATA2 const u8 lbl_8047BF1C[2] = "0";
SDATA2 const u8 lbl_8047BF20[2] = "p";
SDATA2 const u8 lbl_8047BF24[3] = "pt";
SDATA2 const u8 lbl_8047BF28[7] = "handle";
SDATA2 const u32 lbl_8047BF30 = 0;
SDATA2 const u32 lbl_8047BF34 = 1;
SDATA2 const u32 lbl_8047BF38 = 0;
SDATA2 const u32 lbl_8047BF3C = 1;
SDATA2 const u32 lbl_8047BF40[2] = { 0x01020408, 0 };
SDATA2 const u32 lbl_8047BF48[2] = { 0x80, 0 };
SDATA2 const u32 lbl_8047BF50 = 0;
SDATA2 const u32 lbl_8047BF54 = 1;
SDATA2 const u32 lbl_8047BF58 = 0x00060006;
SDATA2 const u32 lbl_8047BF5C = 0x00060006;
SDATA2 const f32 lbl_8047BF60 = 0.0f;
SDATA2 const f32 lbl_8047BF64 = 4.0f;
SDATA2 const f32 lbl_8047BF68 = 0.5f;
SDATA2 const f32 lbl_8047BF6C = 1.3f;
SDATA2 const f32 lbl_8047BF70 = 2.0f;
SDATA2 const f32 lbl_8047BF74 = -2.0f;
SDATA2 const f32 lbl_8047BF78 = 0.01f;
SDATA2 const f32 lbl_8047BF7C = 6.0f;
SDATA2 const f64 lbl_8047BF80 = 4.503601774854144e+15;
SDATA2 const f64 lbl_8047BF88 = 4.503599627370496e+15;
SDATA2 const f32 lbl_8047BF90 = 1.0f;
SDATA2 const f32 lbl_8047BF94 = 10.0f;
SDATA2 const f32 lbl_8047BF98 = 20.0f;
SDATA2 const f32 lbl_8047BF9C = -20.0f;
