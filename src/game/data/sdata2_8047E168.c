#include "dolphin/types.h"

#pragma section ".sdata2"
#define SDATA2 __declspec(section ".sdata2")

/*
 * 0x8047E160 - 0x8047E168 ("\200" and "\0") is emitted by the save-data
 * SHA1Final, src/game/field_exact_801CBF64.c.
 */

typedef union Sdata2AlignedString12 {
    u8 text[12];
    u32 align;
} Sdata2AlignedString12;

SDATA2 const u8 lbl_8047E168[8] = "GC6J";
SDATA2 const u16 lbl_8047E170 = 0x3031;
SDATA2 const Sdata2AlignedString12 lbl_8047E174 = { "GC6E" };
SDATA2 const u32 lbl_8047E180 = 0x808080FFu;
SDATA2 const f32 lbl_8047E184 = 1.5f;
SDATA2 const f32 lbl_8047E188 = 0.0f;
SDATA2 const f32 lbl_8047E18C = 2.0f;
