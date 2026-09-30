#include "dolphin/types.h"

#pragma section ".sdata2"
#define SDATA2 __declspec(section ".sdata2")

/*
 * HSD WObj/Class/CObj .sdata2 constants and the people info-bios unit's
 * pool (0x8047D8A8-0x8047D8B8). References in people_exact_8018F5B4.c,
 * hsd_wobj.c, hsd_class.c, hsd_cobj.c, and symbolmap strings tie the range
 * to those owners. (0x8047D890-0x8047D8A8, the tail of the people TU's own
 * pool, links with game/people/people.c; 0x8047D908-0x8047D950, bytecode.c's
 * pool, links with hsd/bytecode.c.)
 */
SDATA2 const f32 lbl_8047D8A8 = 0.0f;
SDATA2 const f32 lbl_8047D8AC = 0.01745329238474369f;
SDATA2 const f32 lbl_8047D8B0[2] = { 1.0f, 0.0f };
SDATA2 const f32 lbl_8047D8B8 = -100.0f;
SDATA2 const f32 lbl_8047D8BC = 740.0f;
SDATA2 const f32 lbl_8047D8C0[2] = { 580.0f, 0.0f };
SDATA2 const u8 lbl_8047D8C8[7] = "wobj.c";
SDATA2 const u8 lbl_8047D8D0[5] = "wobj";
SDATA2 const u8 lbl_8047D8D8[7] = "jobj.h";
SDATA2 const u8 lbl_8047D8E0[5] = "jobj";
SDATA2 const f64 lbl_8047D8E8 = 0.0;
SDATA2 const f32 lbl_8047D8F0 = 0.0f;
SDATA2 const f64 lbl_8047D8F8 = 1.0;
SDATA2 const f32 lbl_8047D900 = 1.0f;
SDATA2 const u8 lbl_8047D904[3] = "jp";
