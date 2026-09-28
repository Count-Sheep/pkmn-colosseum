/* Score instrumentation only; not evidence of a retail TU boundary. */
/* Link triage (2026-09-28): not carvable. fn_8019D9DC
 * (HSD_JObjSetupMatrixSub) reads jobj.c's float pool entries 0x8047DB44-
 * 0x8047DB60, which the resolveIKJoint1/2 candidates (0x8019DD00,
 * 0x8019E460) also read, so this range cannot own them, and a carve would
 * need extern stand-ins for its literals. It also both expands and calls
 * (fn_8019D980) HSD_JObjMtxIsDirty; see hsd_jobj_r51_8019D620_o2.c. */
#include "src/hsd/jobj.c"
