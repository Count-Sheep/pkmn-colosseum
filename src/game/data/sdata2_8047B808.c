#include "dolphin/types.h"

#pragma section ".sdata2"
#define SDATA2 __declspec(section ".sdata2")

/* Follows the menuSub TU pool (0x8047B7C8-0x8047B808, src/game/menuSub.c). */
SDATA2 const f32 lbl_8047B808 = 180.0f;
SDATA2 const f32 lbl_8047B80C = 199.0f;
SDATA2 const f32 lbl_8047B810 = 0.5f;
SDATA2 const f32 lbl_8047B814 = 0.0f;
SDATA2 const f32 lbl_8047B818 = 1.0f;
SDATA2 const f32 lbl_8047B81C = 4.0f;
SDATA2 const f32 lbl_8047B820 = 6.0f;
SDATA2 const f32 lbl_8047B824 = -6.0f;
/* 0x8047B828-0x8047B838 (the int-to-float conversion biases) is compiled by
 * src/game/gs_pcbox_exact_8001EF78.c; the pool continues in
 * sdata2_8047B838.c. */
