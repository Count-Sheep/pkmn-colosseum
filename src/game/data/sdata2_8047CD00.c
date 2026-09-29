#include "dolphin/types.h"

#pragma section ".sdata2"
#define SDATA2 __declspec(section ".sdata2")

/*
 * Game .sdata2 run following the pad manager's literal pool (input.c owns
 * 0x8047CCC8-0x8047CD00). The first constants are referenced by GSmsg.
 */
SDATA2 const u32 lbl_8047CD00 = 0xFFFFFFFF;
SDATA2 const u32 lbl_8047CD04 = 0xFFFFFFFF;
SDATA2 const f32 lbl_8047CD08 = 1.0f;
SDATA2 const f64 lbl_8047CD10 = 4.503601774854144e+15;
SDATA2 const f64 lbl_8047CD18 = 1.0;
SDATA2 const f64 lbl_8047CD20 = 0.5;
SDATA2 const f64 lbl_8047CD28 = 4.503599627370496e+15;
SDATA2 const f32 lbl_8047CD30 = 2.0f;
SDATA2 const f32 lbl_8047CD34 = 0.5f;
SDATA2 const f32 lbl_8047CD38 = 0.4f;
SDATA2 const f32 lbl_8047CD3C = 8.0f;
SDATA2 const f32 lbl_8047CD40 = 3.1415927f;
SDATA2 const f32 lbl_8047CD44 = 25.0f;
SDATA2 const f32 lbl_8047CD48 = 4.0f;
SDATA2 const f32 lbl_8047CD4C = 0.001953125f;
/* 0x8047CD50-0x8047CD80 is the literal pool of gs_thread_hi_range_800FE35C.c. */
/* 0x8047CD80-0x8047CDC0 is the literal pool of gapp_exact_80101B90.c. */
