#include "dolphin/types.h"

#pragma section ".sdata2"
#define SDATA2 __declspec(section ".sdata2")

/*
 * 0x8047DFA0 - 0x8047DFA8, split from battle_sdata2_8047DF90.c when the
 * battle-grid prefix carve (0x801C3114 - 0x801C3A64) took its own
 * int-to-float literal at 0x8047DF98.
 */
SDATA2 const f32 lbl_8047DFA0[2] = { 1.71052635f, 0.0f };

/* 0x8047DFA8 - 0x8047DFD8 is emitted by game/effect/fade.c. */
