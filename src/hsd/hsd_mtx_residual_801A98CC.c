/* Score instrumentation only; not evidence of a retail TU boundary. The
 * whole mtx.c TU is compiled; objdiff pairs HSD_MtxGetRotation
 * (fn_801A98CC, 0x801A98CC-0x801A9DF0), exact in the whole TU. */
/* Link triage (2026-09-29): its only linkable carve reads mtx.c's pooled
 * floats (lbl_8047DC5C/60/78/80, shared with the other mtx.c chunks)
 * through named extern stand-ins, which the strict policy rejects and the
 * title-path exception does not cover (fn_801A98CC is not in the recomp
 * checker's closure). It links cleanly with the whole mtx.c TU, which waits
 * on HSD_MtxGetScale (src/hsd/mtx.c). */
#include "src/hsd/mtx.c"
