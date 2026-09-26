#include "dolphin/types.h"

#pragma section ".sdata2"
#define SDATA2 __declspec(section ".sdata2")

/*
 * Mixed HSD .sdata2 constants and assert strings. Source references and
 * symbolmap strings tie this range to HSD object/PObj/RObj code; sized string
 * declarations and scalar f32/f64/u32 constants preserve compiler layout.
 */
SDATA2 const u8 lbl_8047DD50[7] = "robj.c";
SDATA2 const u8 lbl_8047DD58[7] = "rvalue";
SDATA2 const f32 lbl_8047DD60 = 0.0f;
SDATA2 const u32 lbl_8047DD64 = 0;
SDATA2 const u8 lbl_8047DD68[5] = "jobj";
SDATA2 const u8 lbl_8047DD70[7] = "jobj.h";
SDATA2 const f32 lbl_8047DD78 = 57.295780181884766f;
SDATA2 const f32 lbl_8047DD7C = 0.01745329238474369f;
SDATA2 const u8 lbl_8047DD80[4] = "new";
SDATA2 const f32 lbl_8047DD84 = 1.0f;
SDATA2 const f64 lbl_8047DD88 = 4503601774854144.0;
