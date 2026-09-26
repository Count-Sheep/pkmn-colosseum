#include "dolphin/types.h"

#pragma section ".sdata2"
#define SDATA2 __declspec(section ".sdata2")

/*
 * Mixed HSD JObj/LObj/MObj/Mtx .sdata2 constants and assert strings. Source
 * references tie the range to hsd_jobj_display.c, hsd_lobj.c, hsd_mobj.c,
 * and hsd_mobj_ext.c. lbl_8047DBC8 is an empty panic string.
 */
SDATA2 const f64 lbl_8047DB90 = 1.0;
SDATA2 const f64 lbl_8047DB98 = 4503601774854144.0;
SDATA2 const u8 lbl_8047DBA0[7] = "list.c";
SDATA2 const u8 lbl_8047DBA8[5] = "prev";
SDATA2 const u8 lbl_8047DBB0[5] = "list";
