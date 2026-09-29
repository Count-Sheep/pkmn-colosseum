#include "dolphin/types.h"

#pragma section ".sdata2"
#define SDATA2 __declspec(section ".sdata2")

/*
 * Mixed numeric .sdata2 run after the GScolsys2Human pool (0x8047CF20-
 * 0x8047CF48), up to GScolsys2Sun.c's pool at 0x8047CF68.
 */
SDATA2 const u32 lbl_8047CF48 = 0x00010002;
SDATA2 const u16 lbl_8047CF4C = 0x0004;
SDATA2 const u32 lbl_8047CF50 = 0x00010002;
SDATA2 const u16 lbl_8047CF54 = 0x0004;
SDATA2 const f32 lbl_8047CF58 = 0.0f;
SDATA2 const f32 lbl_8047CF5C = 1.0f;
SDATA2 const f32 lbl_8047CF60 = 0.0f;
SDATA2 const f32 lbl_8047CF64 = 1.0f;
/*
 * 0x8047CF68-0x8047CF70 is GScolsys2Sun.c's literal pool (0.0f, 1.0f) and
 * 0x8047CF70-0x8047CFA0 is floor.c's, each owned by its unit.
 */
