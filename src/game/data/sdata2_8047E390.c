#include "dolphin/types.h"

#pragma section ".sdata2"
#define SDATA2 __declspec(section ".sdata2")

/*
 * Mixed battle .sdata2 constants. Target text relocations tie the first run to
 * battle_waza functions and the UI/status constants to battle_logic.c.
 */
/* The prefix stops before 0x8047E3A8: that int-to-float constant is the pool
 * entry wazaSequence_r52_801DC014_prefix emits itself. SDATA2_8047E3B0_ONLY
 * (sdata2_8047E3B0.c) builds the 0x8047E3B0-0x8047E3C0 rest. */
#if !defined(SDATA2_8047E390_SUFFIX) && !defined(SDATA2_8047E3B0_ONLY)
SDATA2 const f32 lbl_8047E390 = 1.33329999f;
SDATA2 const f32 lbl_8047E394 = 2.0f;
SDATA2 const f32 lbl_8047E398 = 3.25f;
SDATA2 const f32 lbl_8047E39C = 1.0f;
SDATA2 const f32 lbl_8047E3A0 = 60.0f;
#endif
#if !defined(SDATA2_8047E390_SUFFIX) && !defined(SDATA2_8047E390_PREFIX)
SDATA2 const f32 lbl_8047E3B0[2] = { 0.0f, 0.0f };
SDATA2 const f32 lbl_8047E3B8 = 60.0f;
#endif

#if !defined(SDATA2_8047E390_PREFIX) && !defined(SDATA2_8047E3B0_ONLY)
SDATA2 const f32 lbl_8047E3C8 = 0.5f;
SDATA2 const f32 lbl_8047E3CC = 0.0f;
SDATA2 const f64 lbl_8047E3D0 = 4.503599627370496e+15;
#endif
