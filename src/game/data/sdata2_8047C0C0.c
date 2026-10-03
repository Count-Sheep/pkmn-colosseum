#include "dolphin/types.h"

#pragma section ".sdata2"
#define SDATA2 __declspec(section ".sdata2")

typedef union Sdata2AlignedString3 {
    u8 text[3];
    f64 align;
} Sdata2AlignedString3;

typedef union Sdata2AlignedString7 {
    u8 text[7];
    f64 align;
} Sdata2AlignedString7;

/*
 * Mixed menu, Card-E, save, and GBA .sdata2 constants. The D.D.D0 byte string
 * and following u16 table are consumed as halfword lookup data in menu_tool2.c;
 * the aligned strings are assert labels referenced from save and GBA code.
 */
SDATA2 const f32 lbl_8047C0C0 = 0.833333313f;
SDATA2 const f32 lbl_8047C0C4 = 0.5f;
SDATA2 const f32 lbl_8047C0C8[2] = { 1.0f, 0.0f };
SDATA2 const u8 lbl_8047C0D0[7] = "D.D.D0";
SDATA2 const u16 lbl_8047C0D8[4] = { 0x442F, 0x442C, 0x442D, 0x4426 };
SDATA2 const f32 lbl_8047C0E0 = 0.0f;
SDATA2 const f32 lbl_8047C0E4 = 1.0f;
SDATA2 const u8 lbl_8047C0E8[7] = "celebi";
