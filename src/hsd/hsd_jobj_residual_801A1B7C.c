/* Score instrumentation only; not evidence of a retail TU boundary. */
/* Link triage (2026-09-28): not carvable. JObjAnimAll (fn_801A1B7C) expands
 * HSD_JObjAnim and itself several levels deep and, where MWCC's auto-inline
 * budget runs out, calls jobj.c's out-of-line copies of HSD_JObjCheckDepend
 * (fn_801A3D04) and HSD_JObjMtxIsDirty (fn_8019D980), while expanding both
 * at the shallower levels. A carve must define those bodies to expand them,
 * and then MWCC emits its own (static or weak) out-of-line copies instead of
 * calling the linked ones, so the range only links with the whole jobj.c. */
#include "src/hsd/jobj.c"
