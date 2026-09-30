#include "dolphin/types.h"

#pragma section ".sdata2"
#define SDATA2 __declspec(section ".sdata2")

SDATA2 const f32 lbl_8047DF90 = -1.5707964f;

/*
 * 0x8047DF98 (the int-to-float double) belongs to the battle-grid prefix
 * carve, and 0x8047DFA0 - 0x8047DFA8 is battle_sdata2_8047DFA0.c.
 */
