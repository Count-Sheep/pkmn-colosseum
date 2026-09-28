/* Score instrumentation only; not evidence of a retail TU boundary. */
/* Link triage (2026-09-28): not linkable yet. fn_8013C670 (_envMapStart) is
 * data-free, but it is exact only under "#pragma global_optimizer off" with
 * the work pointer variable reused for the second texture handle. Natural
 * source at the chunk's GC/1.3 -O4,p gives work/model in r30/r31 instead of
 * r31/r30; at -O1 the registers match but the u16 handles get clrlwi
 * extensions and the second handle lands in r30 instead of r31. */
#include "src/game/effect/effect_visual_candidate_8013C670.c"
