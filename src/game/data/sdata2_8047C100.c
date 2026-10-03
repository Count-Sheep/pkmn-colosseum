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
 * Middle of the mixed menu .sdata2 constants (0x8047C100-0x8047C118), between
 * the int->float biases owned by the menu_candidate_r47_80077ED4 and
 * menu_candidate_r47_80079C1C carves. The constants move unchanged.
 */
SDATA2 const f32 lbl_8047C100 = 0.5f;
SDATA2 const f32 lbl_8047C104 = 0.300000012f;
SDATA2 const f32 lbl_8047C108 = 0.300000012f;
SDATA2 const u8 lbl_8047C10C[8] = "pikachu";
SDATA2 const f32 lbl_8047C114 = 0.0f;
