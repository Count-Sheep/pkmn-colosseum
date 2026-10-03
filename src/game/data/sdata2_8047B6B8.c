#include "dolphin/types.h"

#pragma section ".sdata2"
#define SDATA2 __declspec(section ".sdata2")

/*
 * Mixed early game .sdata2 constants. main_tail references the error strings;
 * gs_task, gs_party_access, gs_npc_interact, gs_event_exec, and
 * gs_pokemon_summary reference the remaining UI/camera constants in this run.
 * The preceding 0x8047B6A0 - 0x8047B6B8 literals (0.0f, 359940.0f and the two
 * int-to-float conversion constants) are emitted by game/main.c itself.
 * 0x8047B6D8 (the int-to-float conversion constant) is
 * game/gs_range_800096B4.c's own literal; sdata2_8047B6E0.c continues after it.
 */
SDATA2 const u8 lbl_8047B6B8[8] = "error.c";
SDATA2 const u8 lbl_8047B6C0[5] = "%s:\n";
SDATA2 const u8 lbl_8047B6C8[3] = "%d";
#pragma push
#pragma force_active on
SDATA2 const u32 sdata2_padding_8047B6CC = 0;
#pragma pop
SDATA2 const f32 lbl_8047B6D0 = 25500.0f;
