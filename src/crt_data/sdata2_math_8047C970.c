#include "dolphin/types.h"

#pragma section ".sdata2"
#define SDATA2 __declspec(section ".sdata2")

/* The libm pool after __ieee754_sqrt's 1.0 (0x8047C968, owned by
 * crt/math_range_800CE378.c): the inline sqrtf constants the linked
 * math_exact_800CE59C.c reads by name. */
SDATA2 const f32 lbl_8047C970 = 0.0f;
SDATA2 const f64 lbl_8047C978 = 5.00000000000000000000e-01;
SDATA2 const f64 lbl_8047C980 = 3.00000000000000000000e+00;
SDATA2 const f64 lbl_8047C988 = 0.0;
