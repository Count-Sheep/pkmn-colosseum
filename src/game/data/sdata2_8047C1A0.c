#include "dolphin/types.h"

#pragma section ".sdata2"
#define SDATA2 __declspec(section ".sdata2")

/*
 * Mixed GBA and Dolphin .sdata2 constants. GBA strings and constants are
 * referenced from gba_conv/gba_comm/gba_misc/late_game; the tail constants are
 * referenced by target relocations in OSSram and the 0x800A2D38..0x800A3744
 * Dolphin math/DVD area.
 */
SDATA2 const u8 lbl_8047C1A0[5] = "pWin";
SDATA2 const f32 lbl_8047C1A8 = 0.0f;
SDATA2 const f32 lbl_8047C1AC = 1.0f;
SDATA2 const f64 lbl_8047C1B0 = 4.503601774854144e+15;
SDATA2 const f64 lbl_8047C1B8 = 4.503599627370496e+15;
SDATA2 const u32 lbl_8047C1C0 = 0x00000001;
SDATA2 const u32 lbl_8047C1C4 = 0x00020003;
SDATA2 const f32 lbl_8047C1C8 = 0.5f;
SDATA2 const f32 lbl_8047C1CC = 0.0f;
SDATA2 const f32 lbl_8047C1D0 = 0.833333313f;
SDATA2 const f32 lbl_8047C1D4 = 0.0f;
SDATA2 const f32 lbl_8047C1D8 = 1.0f;
SDATA2 const f32 lbl_8047C1DC = 83.3333282f;
SDATA2 const f32 lbl_8047C1E0[2] = { 41.6666641f, 0.0f };
SDATA2 const u8 lbl_8047C1E8[7] = "handle";
SDATA2 const f32 lbl_8047C1F0 = 5.0f;
SDATA2 const f64 lbl_8047C1F8 = 4.503599627370496e+15;
SDATA2 const u32 lbl_8047C200 = 0x000078FF;
SDATA2 const u32 lbl_8047C204 = 0x780000FF;
SDATA2 const f32 lbl_8047C208 = 1.0f;
SDATA2 const f32 lbl_8047C20C = 0.800000012f;
SDATA2 const f32 lbl_8047C210 = 0.600000024f;
SDATA2 const f32 lbl_8047C214 = 0.400000006f;
SDATA2 const f32 lbl_8047C218 = 0.200000003f;
SDATA2 const f32 lbl_8047C21C = 5.0f;
SDATA2 const f64 lbl_8047C220 = 4.503599627370496e+15;
SDATA2 const f64 lbl_8047C228 = 4.503601774854144e+15;
SDATA2 const f32 lbl_8047C230 = 0.0f;
SDATA2 const f32 lbl_8047C234 = 0.25f;
SDATA2 const f32 lbl_8047C238[2] = { 0.5f, 0.0f };

/* 0x8047C240 - 0x8047C280 is OSFatal.c's pool, owned by the linked
 * dolphin/sdk_candidate_8009CD38.c carve; 0x8047C280 - 0x8047C2A0 is
 * sdata2_8047C280.c. */
