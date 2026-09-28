#include "dolphin/types.h"

#pragma section ".sdata2"
#define SDATA2 __declspec(section ".sdata2")

/*
 * Mixed numeric .sdata2 run after the GScolsys2Walk.cpp pool, up to
 * floor.c's pool at 0x8047CF70.
 */
SDATA2 const u32 lbl_8047CEF0 = 0x00010002;
SDATA2 const u16 lbl_8047CEF4 = 0x0004;
SDATA2 const u32 lbl_8047CEF8 = 0x00010002;
SDATA2 const u16 lbl_8047CEFC = 0x0004;
SDATA2 const f32 lbl_8047CF00 = 0.0f;
SDATA2 const f32 lbl_8047CF04 = 1.0f;
SDATA2 const f32 lbl_8047CF08[2] = { 0.0001f, 0.0f };
SDATA2 const f32 lbl_8047CF10 = 0.0f;
SDATA2 const f32 lbl_8047CF14 = 1000000.0f;
SDATA2 const f32 lbl_8047CF18[2] = { -1000000.0f, 0.0f };
SDATA2 const f32 lbl_8047CF20 = 0.0f;
SDATA2 const f64 lbl_8047CF28 = 0.5;
SDATA2 const f64 lbl_8047CF30 = 3.0;
SDATA2 const f64 lbl_8047CF38 = 0.0;
SDATA2 const f32 lbl_8047CF40 = 1.0f;
SDATA2 const f32 lbl_8047CF44 = 0.0001f;
SDATA2 const u32 lbl_8047CF48 = 0x00010002;
SDATA2 const u16 lbl_8047CF4C = 0x0004;
SDATA2 const u32 lbl_8047CF50 = 0x00010002;
SDATA2 const u16 lbl_8047CF54 = 0x0004;
SDATA2 const f32 lbl_8047CF58 = 0.0f;
SDATA2 const f32 lbl_8047CF5C = 1.0f;
SDATA2 const f32 lbl_8047CF60 = 0.0f;
SDATA2 const f32 lbl_8047CF64 = 1.0f;
SDATA2 const f32 lbl_8047CF68 = 0.0f;
SDATA2 const f32 lbl_8047CF6C = 1.0f;
/* 0x8047CF70-0x8047CFA0 is floor.c's literal pool, owned by game/floor.c. */
