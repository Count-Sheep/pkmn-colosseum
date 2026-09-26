#include "dolphin/types.h"

#pragma section ".sdata2"
#define SDATA2 __declspec(section ".sdata2")

/*
 * Mixed HSD JObj/LObj/MObj/Mtx .sdata2 constants and assert strings. Source
 * references tie the range to hsd_jobj_display.c, hsd_lobj.c, hsd_mobj.c,
 * and hsd_mobj_ext.c. lbl_8047DBC8 is an empty panic string.
 */
SDATA2 const u8 lbl_8047DC18[7] = "mobj.c";
SDATA2 const u8 lbl_8047DC20[5] = "tobj";
SDATA2 const u8 lbl_8047DC28[4] = "mat";
SDATA2 const f32 lbl_8047DC2C = 1.0f;
SDATA2 const u8 lbl_8047DC30[5] = "mobj";
SDATA2 const u8 lbl_8047DC38[5] = "list";
SDATA2 const f32 lbl_8047DC40 = 0.0f;
SDATA2 const f32 lbl_8047DC44 = 255.0f;
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
