/**
 * .text 0x80179F4C - 0x8017A5FC: fn_80179F4C and fn_80179FA4, the two
 * functions between camera.c (which ends at 0x80179F4C with
 * _cameraRestoreStateData) and gs_range_8017A5FC_prefix.c.
 *
 * They are not camera.c code: both are built without optimization (the
 * parameter goes through the stack), they share no data with camera.c and
 * fn_80179FA4 works on 0x80453FEC / 0x8047B1B8, which the fsys units after
 * it also use. CodeCandidate research carried over unchanged from the old
 * camera.c residual; not accepted.
 */

#include "game/gs_scene_types.h"

#pragma push
#pragma optimization_level 1
void fn_80179F4C(u32 param) {
    volatile u32* saved = &param;

    lbl_80478C4C = *saved;
    if ((u8)menuIsCheck(0xFE) == 0) {
        menuOpenCustom(0xFE, 0, 0, 0, 0, 0);
    }
}
#pragma pop

#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_80179FA4(void) {
    /* TODO: match -- 1624 bytes at 0x80179FA4 */
}
#pragma pop
