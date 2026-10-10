#include "dolphin/types.h"

#pragma section ".sdata2"
#define SDATA2 __declspec(section ".sdata2")

/* Tail of the original 0x8047D098 small-data unit. */

SDATA2 const f32 lbl_8047D110[2] = { 0.0f, 0.0f };
/* 0x8047D118-0x8047D130 (60.0f, 0.0f, 1.0f and the int-to-float bias) is the
 * pool tracefx_r55_801364A8_gc13_o1 emits itself. */
/* 0x8047D130-0x8047D148 (1.0f, 0.0f and the two int-to-float biases) is the
 * pool tracefx_r55_8013796C_suffix emits itself. */
