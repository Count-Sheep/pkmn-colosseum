#include "dolphin/types.h"

#pragma section ".sdata2"
#define SDATA2 __declspec(section ".sdata2")

/*
 * The .sdata2 run after msgctrl.c's conversion literal (0x8047D0E0, owned by
 * game/msgctrl_candidate_80131FF4.c): the "movie" string the debug menu
 * reads, then the movie setup constants.
 */
SDATA2 const u8 lbl_8047D0E8[6] = "movie";
SDATA2 const f32 lbl_8047D0F0 = 0.0f;
SDATA2 const f32 lbl_8047D0F4 = 640.0f;
SDATA2 const f32 lbl_8047D0F8 = 480.0f;
SDATA2 const f32 lbl_8047D0FC = 100.0f;
SDATA2 const f32 lbl_8047D100 = 380.0f;
SDATA2 const f32 lbl_8047D104 = 200.0f;
