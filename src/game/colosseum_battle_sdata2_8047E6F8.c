#include "dolphin/types.h"

#pragma section ".sdata2"
#define SDATA2 __declspec(section ".sdata2")

/* Constants following fight_timer.c's .sdata2 pool (0x8047E6D8-0x8047E6F8). */
SDATA2 const u16 lbl_8047E6F8 = 0x00A1;
SDATA2 const f32 lbl_8047E6FC = 0.5f;
