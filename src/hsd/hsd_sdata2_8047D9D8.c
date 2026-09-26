#include "dolphin/types.h"

#pragma section ".sdata2"
#define SDATA2 __declspec(section ".sdata2")

/*
 * Mixed HSD CObj and DObj .sdata2 constants (0x8047D990..0x8047DA30; fobj.c
 * owns 0x8047DA30..0x8047DA60 and hsd_sdata2_8047DA60.c holds fog.c's).
 * Source references and symbolmap strings tie the labels to hsd_cobj.c,
 * hsd_displayfunc.c, hsd_render.c, hsd_dobj.c and hsd_state.c.
 * lbl_8047D9D8 and lbl_8047DA28 are empty assertion/panic strings padded by
 * compiler layout before the following aligned strings.
 */
SDATA2 const u8 lbl_8047D9D8[8] = "";
SDATA2 const u8 lbl_8047D9E0[7] = "jobj.h";
SDATA2 const u8 lbl_8047D9E8[5] = "jobj";
SDATA2 const u8 lbl_8047D9F0[2] = "x";
SDATA2 const f32 lbl_8047D9F4 = 0.0f;
SDATA2 const f64 lbl_8047D9F8 = 0.5;
SDATA2 const f64 lbl_8047DA00 = 3.0;
SDATA2 const f64 lbl_8047DA08 = 0.0;
SDATA2 const f32 lbl_8047DA10 = -1.0f;
SDATA2 const f32 lbl_8047DA14 = 1.0f;
SDATA2 const u8 lbl_8047DA18[7] = "dobj.c";
SDATA2 const u8 lbl_8047DA20[5] = "dobj";
SDATA2 const u8 lbl_8047DA28[8] = "";
