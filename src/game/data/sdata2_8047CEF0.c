#include "dolphin/types.h"

#pragma section ".sdata2"
#define SDATA2 __declspec(section ".sdata2")

/*
 * Mixed numeric .sdata2 run after the GScolsys2Walk.cpp pool, up to the
 * GScolsys2Human pool at 0x8047CF20 (owned by
 * GScolsys2Human_range_8010FAF4.c).
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
