#include "dolphin/types.h"

#pragma section ".sdata2"
#define SDATA2 __declspec(section ".sdata2")

/*
 * Mixed game .sdata2 run. Assert strings are referenced by gs_material and
 * gs_particle; the remaining constants are referenced by gs_thread and
 * gs_model. 0x8047CCC8-0x8047CD00 is the literal pool of game/input/input.c.
 */
SDATA2 const u8 lbl_8047CC98[5] = "aobj";
SDATA2 const u8 lbl_8047CCA0[7] = "jobj.h";
SDATA2 const u8 lbl_8047CCA8[5] = "jobj";
SDATA2 const f32 lbl_8047CCB0 = 0.0f;
SDATA2 const f32 lbl_8047CCB4 = 1.0f;
SDATA2 const u8 lbl_8047CCB8[2] = "\n";
SDATA2 const f32 lbl_8047CCBC = 0.0f;
SDATA2 const f64 lbl_8047CCC0 = 4.503601774854144e+15;
