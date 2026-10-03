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
 * Tail of the mixed menu/Card-E/save/GBA .sdata2 constants (0x8047C128-
 * 0x8047C19F), split from sdata2_8047C0A0.c so the int->float biases at
 * 0x8047C118/0x8047C120 belong to menu_candidate_r47_80079C1C. The constants
 * move unchanged.
 */
SDATA2 const f32 lbl_8047C128[2] = { 0.100000001f, 0.0f };
SDATA2 const u32 lbl_8047C130[2] = { 0xFFFFFFFFu, 0xFFFFFF00u };
SDATA2 const u32 lbl_8047C138[2] = { 0xFFFFFF00u, 0xFFFFFFFFu };
SDATA2 const u8 lbl_8047C140[7] = "handle";
SDATA2 const f32 lbl_8047C148 = 1.0f;
SDATA2 const f32 lbl_8047C14C = 0.0625f;
SDATA2 const f32 lbl_8047C150 = 0.0f;
SDATA2 const f32 lbl_8047C154 = 48.0f;
SDATA2 const f32 lbl_8047C158 = 32.0f;
SDATA2 const f32 lbl_8047C15C = 10.0f;
SDATA2 const f32 lbl_8047C160 = 0.03125f;
SDATA2 const f64 lbl_8047C168 = 4.503601774854144e+15;
SDATA2 const f64 lbl_8047C170 = 4.503599627370496e+15;
SDATA2 const u8 lbl_8047C178[2] = "0";
SDATA2 const Sdata2AlignedString7 lbl_8047C180 = { "series" };
SDATA2 const Sdata2AlignedString3 lbl_8047C188 = { "lv" };
SDATA2 const u16 lbl_8047C190[4] = { 0x01A8, 0x01B1, 0x01B1, 0x01A8 };
SDATA2 const u8 lbl_8047C198[7] = "handle";
