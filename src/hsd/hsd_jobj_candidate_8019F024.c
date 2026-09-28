/* Score instrumentation only; not evidence of a retail TU boundary. */
/* Link triage (2026-09-28): not carvable. HSD_JObjSetCurrent (fn_8019F024)
 * addresses jobj.c's .rodata string pool through its base (lbl_80274AA0 +
 * 0x54 / + 0xC4), which a carve does not own; see jobj_exact_8019F01C.c.
 * fn_8019F1C4 after it is linked from jobj_exact_8019F1C4.c. */
#include "src/hsd/jobj.c"
