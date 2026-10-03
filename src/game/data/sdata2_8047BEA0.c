#include "dolphin/types.h"

#pragma section ".sdata2"
#define SDATA2 __declspec(section ".sdata2")

/*
 * Scene-init and UI-core .sdata2 constants. References in scene_init.c own the
 * first camera/menu run; ui_core.c owns the assert strings, selection masks, and
 * transition constants. Ends where menuCB_80056C54.c's pool (0x8047BEE0-0x8047BF10)
 * begins; the run continues in sdata2_8047BF10.c.
 */
SDATA2 const f64 lbl_8047BEA0 = 4.503599627370496e+15;
SDATA2 const f64 lbl_8047BEA8 = 4.503601774854144e+15;
SDATA2 const f32 lbl_8047BEB0 = 2.0f;
SDATA2 const f32 lbl_8047BEB4 = 1.0f;
SDATA2 const f32 lbl_8047BEB8 = 0.5f;
SDATA2 const f32 lbl_8047BEBC = 255.0f;
SDATA2 const f32 lbl_8047BEC0 = 0.0f;
SDATA2 const f32 lbl_8047BEC4 = 0.8f;
SDATA2 const f32 lbl_8047BEC8 = -0.033333335f;
SDATA2 const f32 lbl_8047BECC = 0.033333335f;
SDATA2 const f32 lbl_8047BED0 = -1.0f;
SDATA2 const f32 lbl_8047BED4 = -422.0f;
SDATA2 const f32 lbl_8047BED8 = 0.016666668f;
