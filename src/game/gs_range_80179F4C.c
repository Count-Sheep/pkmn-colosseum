/**
 * .text 0x80179FA4 - 0x8017A5FC: fn_80179FA4. fn_80179F4C (0x80179F4C -
 * 0x80179FA4), the other function between camera.c (which ends at
 * 0x80179F4C with _cameraRestoreStateData) and gs_range_8017A5FC_prefix.c,
 * is exact at level 0 and carved into gs_range_80179F4C_exact_80179F4C.c.
 *
 * They are not camera.c code: both are built without optimization (the
 * parameter goes through the stack), they share no data with camera.c and
 * fn_80179FA4 works on 0x80453FEC / 0x8047B1B8, which the fsys units after
 * it also use. CodeCandidate research carried over unchanged from the old
 * camera.c residual; not accepted.
 */

#include "game/gs_scene_types.h"

#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_80179FA4(void) {
    /* TODO: match -- 1624 bytes at 0x80179FA4 */
}
#pragma pop
