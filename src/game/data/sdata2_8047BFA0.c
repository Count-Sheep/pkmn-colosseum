#include "dolphin/types.h"

#pragma section ".sdata2"
#define SDATA2 __declspec(section ".sdata2")

typedef union Sdata2AlignedString2 {
    u8 text[2];
    f64 align;
} Sdata2AlignedString2;

/*
 * UI-core and menu .sdata2 constants. ui_core.c owns the first transition and
 * rotation constants; menu_middle/menu_common_ext/menu_tool own the message-id
 * tables, assert strings, and menu angle/color constants. Ends where
 * menuCB_range_80063AD4.c's pool (0x8047BFC8-0x8047BFE8) begins; the run
 * continues in sdata2_8047BFE8.c.
 */
SDATA2 const f32 lbl_8047BFA0 = 255.0f;
SDATA2 const f32 lbl_8047BFA4 = 0.100000001f;
SDATA2 const f32 lbl_8047BFA8 = 0.200000003f;
SDATA2 const f32 lbl_8047BFAC = 100.0f;
SDATA2 const f32 lbl_8047BFB0 = 0.300000012f;
SDATA2 const f32 lbl_8047BFB4 = 0.400000006f;
SDATA2 const f32 lbl_8047BFB8 = 0.600000024f;
SDATA2 const f32 lbl_8047BFBC = 640.0f;
SDATA2 const f32 lbl_8047BFC0 = -640.0f;
SDATA2 const f32 lbl_8047BFC4 = 0.699999988f;
