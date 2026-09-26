#include "dolphin/types.h"

#pragma section ".sdata2"
#define SDATA2 __declspec(section ".sdata2")

/*
 * mtx.c's .sdata2 (the "mtx.c" assert file name, the short assert strings and
 * the float pool) up to 0x8047DC90; mobj.c's pool before it is now owned by
 * hsd/mobj.c.
 */
SDATA2 const u8 lbl_8047DC48[6] = "mtx.c";
SDATA2 const u8 lbl_8047DC50[4] = "mtx";
SDATA2 const u8 lbl_8047DC54[4] = "vec";
SDATA2 const f32 lbl_8047DC58 = 1.0f;
SDATA2 const f32 lbl_8047DC5C = 0.0f;
SDATA2 const f64 lbl_8047DC60 = 0.0;
SDATA2 const f64 lbl_8047DC68 = -1.0;
SDATA2 const f32 lbl_8047DC70 = 1.000000013351432e-10f;
SDATA2 const f64 lbl_8047DC78 = 0.5;
SDATA2 const f64 lbl_8047DC80 = 3.0;
SDATA2 const f32 lbl_8047DC88 = 1.5707963705062866f;
SDATA2 const f32 lbl_8047DC8C = -1.0f;
