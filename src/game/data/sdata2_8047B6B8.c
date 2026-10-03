#include "dolphin/types.h"

#pragma section ".sdata2"
#define SDATA2 __declspec(section ".sdata2")

/*
 * Mixed early game .sdata2 constants. main_tail references the error strings;
 * gs_task, gs_party_access, gs_npc_interact, gs_event_exec, and
 * gs_pokemon_summary reference the remaining UI/camera constants in this run.
 * The preceding 0x8047B6A0 - 0x8047B6B8 literals (0.0f, 359940.0f and the two
 * int-to-float conversion constants) are emitted by game/main.c itself.
 * 0x8047B6F8 is game/gs_range_8000D290.c's own literal; the constants from
 * 0x8047B700 on are in sdata2_8047B700.c.
 */
SDATA2 const u8 lbl_8047B6B8[8] = "error.c";
SDATA2 const u8 lbl_8047B6C0[5] = "%s:\n";
SDATA2 const u8 lbl_8047B6C8[3] = "%d";
#pragma push
#pragma force_active on
SDATA2 const u32 sdata2_padding_8047B6CC = 0;
#pragma pop
SDATA2 const f32 lbl_8047B6D0 = 25500.0f;
SDATA2 const f64 lbl_8047B6D8 = 4.503601774854144e+15;
SDATA2 const f32 lbl_8047B6E0[2] = { 0.5f, 0.0f };
SDATA2 const f32 lbl_8047B6E8[2] = { 0.5f, 0.0f };
SDATA2 const f32 lbl_8047B6F0 = 20.0f;
