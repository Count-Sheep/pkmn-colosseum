#include "dolphin/types.h"

#pragma section ".sdata2"
#define SDATA2 __declspec(section ".sdata2")

/*
 * Numeric .sdata2 run from 0x8047CF08 up to the GScolsys2Human pool at
 * 0x8047CF20 (owned by GScolsys2Human_range_8010FAF4.c). 0x8047CEF0-0x8047CF08
 * is the GScolsys2 sphere TU's own pool, emitted by
 * gs_colsys_candidate_8010E53C.c. lbl_8047CF08 (0.0001f and its pad) is that
 * TU's last literal, kept here by name (see the unit's file comment).
 */
SDATA2 const f32 lbl_8047CF08[2] = { 0.0001f, 0.0f };
SDATA2 const f32 lbl_8047CF10 = 0.0f;
SDATA2 const f32 lbl_8047CF14 = 1000000.0f;
SDATA2 const f32 lbl_8047CF18[2] = { -1000000.0f, 0.0f };
