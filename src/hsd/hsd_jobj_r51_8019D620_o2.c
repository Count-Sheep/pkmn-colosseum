/* Score instrumentation only; not evidence of a retail TU boundary. */
/* Link triage (2026-09-28): not carvable. HSD_JObjSetMtxDirtySub
 * (fn_8019D620) expands itself six levels and HSD_JObjMtxIsDirty at each,
 * but calls the out-of-line HSD_JObjMtxIsDirty copy (fn_8019D980, the next
 * range) where the budget runs out. A carve that defines the inline for the
 * expansions also emits its own out-of-line copy, so the range only links
 * with the whole jobj.c (or together with 0x8019D980-0x8019DD00, which has
 * the same problem plus the shared float pool). */
#include "src/hsd/jobj.c"
