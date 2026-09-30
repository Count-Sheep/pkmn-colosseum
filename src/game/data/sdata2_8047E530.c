#include "dolphin/types.h"

#pragma section ".sdata2"
#define SDATA2 __declspec(section ".sdata2")

/* Split out of sdata2_8047E508.c when fn_8020DAD0's carve took its pool
 * (0x8047E528 - 0x8047E530). Read by fight_encount_exact_8020DD44.c. */
SDATA2 const f32 lbl_8047E530[2] = { 0.0f, 0.0f };
