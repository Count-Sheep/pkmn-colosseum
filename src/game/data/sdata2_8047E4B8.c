#include "dolphin/types.h"

#pragma section ".sdata2"
#define SDATA2 __declspec(section ".sdata2")

/*
 * .sdata2 constants following the THP player sample's pools (THPDraw.c owns
 * 0x8047E480 - 0x8047E4A8, THPPlayer.c 0x8047E4A8 - 0x8047E4B0). These are
 * used by the THP decoder library; values are reproduced verbatim from the
 * shipped binary. 0x8047E4B0 (the int-to-float constant) is
 * dolphin/thp/THPDec_range_801E5A28.c's own literal.
 */
SDATA2 const f32 lbl_8047E4B8 = 1.4142135381698608f;
SDATA2 const f32 lbl_8047E4BC = 1.8477590084075928f;
SDATA2 const f32 lbl_8047E4C0 = 1.0823922157287598f;
SDATA2 const f32 lbl_8047E4C4 = -2.613126039505005f;
SDATA2 const f32 lbl_8047E4C8[2] = { 1024.0f, 0.0f };
