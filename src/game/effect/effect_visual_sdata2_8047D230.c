#include "dolphin/types.h"

#pragma section ".sdata2"
#define SDATA2 __declspec(section ".sdata2")

/*
 * effect_visual .sdata2 constants of seaEffectStart, between the
 * surfEffectStart - fn_8013BE04 pool (0x8047D200) and the fn_8013CA48 -
 * fn_8013CBF0 unit's (0x8047D240). Source references for this range are in
 * src/game/effect/effect_visual.c; the target slice has no relocations.
 */
SDATA2 const f32 lbl_8047D230 = 6.2831854820251465f;
SDATA2 const f32 lbl_8047D234 = 1.5707963705062866f;
SDATA2 const f32 lbl_8047D238 = 0.5f;
SDATA2 const f32 lbl_8047D23C = 0.0f;
