#include "dolphin/types.h"

#pragma section ".sdata2"
#define SDATA2 __declspec(section ".sdata2")

/*
 * .sdata2 after the evolution code's 0.5f at 0x8047D020 (owned by
 * pokemon_evolution_exact_8012805C.c). lbl_8047D028 is a Shift-JIS
 * resource/key string. 0x8047D030-0x8047D0E0 is hero_move.c's literal pool.
 */
SDATA2 const u8 lbl_8047D028[8] = "レオ";
