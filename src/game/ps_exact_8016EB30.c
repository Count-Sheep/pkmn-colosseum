/**
 * @file ps_exact_8016EB30.c
 * @brief HAL psdisp.c: psSetFog (fn_8016EB30), 0x8016EB30 - 0x8016EC1C,
 *        with its .rodata 0x80273968 - 0x8027399B.
 *
 * Function-boundary carve of psdisp.c (see src/game/psdisp.c for the TU
 * extent). psSetFog is psdisp.c's first definition, so deferred generation
 * emits it last: its code ends the TU's .text, and the two strings of the
 * object.h ref_INC assert it expands ("object.h" and
 * "HSD_OBJ(o)->ref_count != HSD_OBJ_NOREF") end the TU's .rodata. No other
 * function references those two strings, so the carve owns them. No jump
 * table and no pooled float; the fog pointer (lbl_8047B128, psdisp.c's
 * .sbss) stays extern. Built with the particle library flags (GC/1.3.2
 * -O4,p -inline auto,deferred -use_lmw_stmw on -sdata 8 -sdata2 8 -str
 * reuse,readonly), no local pragmas. The body is psdisp.c's; HSD_FogUnref
 * (Melee's static inline) comes from the shared psdisp_color.h.
 */
#include "dolphin/types.h"
#include "sysdolphin/baselib/object.h"
#include "sysdolphin/baselib/psstructs.h"
#include "sysdolphin/baselib/psdisp_color.h"

extern HSD_Fog* lbl_8047B128;
#define psFog lbl_8047B128

/* psSetFog */
void fn_8016EB30(HSD_Fog* fog)
{
    if (psFog != NULL) {
        HSD_FogUnref(psFog);
    }
    if (fog != NULL) {
        ref_INC(fog);
    }
    psFog = fog;
}
