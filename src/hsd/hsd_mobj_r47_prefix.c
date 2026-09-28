/* Score instrumentation only; not evidence of a retail TU boundary. The
 * whole mtx.c TU is compiled; objdiff pairs the functions in this range. */
/* Link triage (2026-09-28): not carvable. The range reads mtx.c's pool
 * (0x8047DC58-0x8047DC90), whose first two literals the other mtx.c chunks
 * (0x801A86B4.., 0x801A9DF0) also read. */
#include "src/hsd/mtx.c"
