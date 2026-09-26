#include "dolphin/types.h"

#pragma section ".sdata2"
#define SDATA2 __declspec(section ".sdata2")

/*
 * HSD fog.c .sdata2 constants and assert strings (0x8047DA60..0x8047DA90),
 * split from hsd_sdata2_8047D990.c where fobj.c's own pool
 * (0x8047DA30..0x8047DA60) now belongs to hsd/fobj.c.
 */
SDATA2 const u8 lbl_8047DA60[8] = "hsd_fog";
SDATA2 const f32 lbl_8047DA68 = 0.0f;
SDATA2 const f32 lbl_8047DA6C = 1.0f;
SDATA2 const f32 lbl_8047DA70 = 255.0f;
SDATA2 const u8 lbl_8047DA74[6] = "fog.c";
SDATA2 const u8 lbl_8047DA7C[4] = "adj";
SDATA2 const u8 lbl_8047DA80[4] = "fog";
SDATA2 const f32 lbl_8047DA84 = 640.0f;
SDATA2 const f32 lbl_8047DA88 = 0.5f;
SDATA2 const f32 lbl_8047DA8C = -1.0f;
