#include "dolphin/types.h"

#pragma section ".sdata2"
#define SDATA2 __declspec(section ".sdata2")

/*
 * Mixed gs_render_util, gs_gfx, and gs_render constants. Assert strings are
 * referenced from cobj/aobj checks; numeric labels are render camera/light
 * constants and int-to-float conversion biases.
 */
SDATA2 const f64 lbl_8047C9A0 = 0.01;
