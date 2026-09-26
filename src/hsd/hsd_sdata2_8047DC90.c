#include "dolphin/types.h"

#pragma section ".sdata2"
#define SDATA2 __declspec(section ".sdata2")

/*
 * Mixed HSD .sdata2 constants and assert strings. Source references and
 * symbolmap strings tie this range to HSD object/PObj/RObj code; sized string
 * declarations and scalar f32/f64/u32 constants preserve compiler layout.
 */
SDATA2 const f32 lbl_8047DC90[2] = { -1.5707963705062866f, 0.0f };
