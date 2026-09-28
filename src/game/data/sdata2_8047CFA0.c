#include "dolphin/types.h"

#pragma section ".sdata2"
#define SDATA2 __declspec(section ".sdata2")

/*
 * floor_event's .sdata2 pool, 0x8047CFA0-0x8047CFC0 (floor.c's pool,
 * 0x8047CF70-0x8047CFA0, precedes it; the floorCharacterBios pool,
 * 0x8047CFC0-0x8047CFD0, is owned by floor_character_exact_80116F68.c).
 */
SDATA2 const f32 lbl_8047CFA0 = 0.0f;
SDATA2 const f32 lbl_8047CFA4 = 0.5f;
SDATA2 const f32 lbl_8047CFA8 = 25.0f;
SDATA2 const f32 lbl_8047CFAC = 1.0f;
SDATA2 const f32 lbl_8047CFB0 = 100.0f;
SDATA2 const f32 lbl_8047CFB4 = 2.0f;
SDATA2 const f64 lbl_8047CFB8 = 4.503601774854144e+15;
