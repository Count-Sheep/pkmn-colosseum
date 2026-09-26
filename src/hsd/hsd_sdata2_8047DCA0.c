#include "dolphin/types.h"

#pragma section ".sdata2"
#define SDATA2 __declspec(section ".sdata2")

/*
 * Mixed HSD .sdata2 constants and assert strings. Source references and
 * symbolmap strings tie this range to HSD object/PObj/RObj code; sized string
 * declarations and scalar f32/f64/u32 constants preserve compiler layout.
 */
SDATA2 const u8 lbl_8047DCA0[8] = "hsd_obj";
SDATA2 const u8 lbl_8047DCA8[7] = "perf.c";
SDATA2 const u8 lbl_8047DCB0[7] = "n < 32";
