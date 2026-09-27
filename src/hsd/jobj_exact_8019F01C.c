/**
 * @file jobj_exact_8019F01C.c
 * @brief HAL jobj.c: HSD_JObjGetCurrent (fn_8019F01C), 0x8019F01C - 0x8019F024.
 *
 * A single exact function carved out of the jobj.c range, built with the
 * HSD library flags (GC/1.3.2 -O4,p -O1 -inline auto,deferred
 * -use_lmw_stmw on -str reuse,readonly) and no local pragmas. The body is
 * Melee's HSD_JObjGetCurrent (jobj.c). It touches no pooled data: its only
 * relocation is current_jobj (.sbss 0x8047B2AC), which stays extern and
 * keeps its address name. HSD_JObjSetCurrent (fn_8019F024) and
 * fn_8019F1C4 after it stay in hsd_jobj_candidate_8019F024.c:
 * HSD_JObjSetCurrent addresses jobj.c's .rodata string pool through its
 * base (lbl_80274AA0 + 0x54 / + 0xC4), which this carve does not own.
 */
#include "hsd/hsd_jobj.h"

extern HSD_JObj* lbl_8047B2AC; /* current_jobj */

HSD_JObj* HSD_JObjGetCurrent(void)
{
    return lbl_8047B2AC;
}
