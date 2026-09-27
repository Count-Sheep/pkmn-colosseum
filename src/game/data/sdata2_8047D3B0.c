#include "dolphin/types.h"

#pragma section ".sdata2"
#define SDATA2 __declspec(section ".sdata2")

/*
 * JAudio/SAL numeric .sdata2 constants. Text references tie this run to
 * synth/mcmd/adsr/SAL helpers currently mapped into src/game/people/people_field.c.
 */
/* musyx/runtime/synth.c owns 0x8047D370 - 0x8047D3B0. */
SDATA2 const f32 lbl_8047D3B0 = 0.007874015718698502f;
