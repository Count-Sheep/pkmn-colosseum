#include "dolphin/types.h"
#include "game/data/sdata2_8047D690.h"

#pragma section ".sdata2"
#define SDATA2 __declspec(section ".sdata2")

/*
 * The tail of psinterpret.c's .sdata2 constants (generator.c owns
 * 0x8047D6B0..0x8047D720; the scene/camera constants after it are
 * sdata2_8047D720.c). The split starts at lbl_8047D690 so the following f64
 * constants keep their original address alignment.
 */
SDATA2 const f32 lbl_8047D690 = 255.0f;
SDATA2 const f32 lbl_8047D694 = 1.5707963705062866f;
SDATA2 const f32 lbl_8047D698 = -1.5707963705062866f;
SDATA2 const f64 lbl_8047D6A0 = 2.0;
SDATA2 const f64 lbl_8047D6A8 = 3.141592653589793;
