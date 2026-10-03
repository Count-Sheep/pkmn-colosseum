#include "dolphin/types.h"

#pragma section ".sdata2"
#define SDATA2 __declspec(section ".sdata2")

/* 8-aligned so the unit starts on 0x8047C190 after the Card-e grid strings. */
typedef union Sdata2AlignedHalves4 {
    u16 values[4];
    f64 align;
} Sdata2AlignedHalves4;

/*
 * .sdata2 0x8047C190 - 0x8047C19F, split from sdata2_8047C128.c after the
 * Card-e grid unit's "series"/"lv" strings (0x8047C180 - 0x8047C18B).
 */
SDATA2 const Sdata2AlignedHalves4 lbl_8047C190 = { { 0x01A8, 0x01B1, 0x01B1, 0x01A8 } };
SDATA2 const u8 lbl_8047C198[7] = "handle";
