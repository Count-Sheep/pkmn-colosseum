#include "dolphin/types.h"

#pragma section ".sdata2"
#define SDATA2 __declspec(section ".sdata2")

/*
 * 0x8047E160 - 0x8047E180 belongs to the memory-card TU (src/game/memcard.c):
 * SHA1Final's padding bytes and its game-code strings.
 */

SDATA2 const u32 lbl_8047E180 = 0x808080FFu;
SDATA2 const f32 lbl_8047E184 = 1.5f;
SDATA2 const f32 lbl_8047E188 = 0.0f;
SDATA2 const f32 lbl_8047E18C = 2.0f;
