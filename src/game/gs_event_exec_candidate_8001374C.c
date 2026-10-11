/**
 * fn_8001374C, fn_800138B4, fn_80013A18, fn_80013DFC and fn_80013F80,
 * 0x8001374C - 0x800140FC. Function-boundary carve of gs_event_exec.c; the
 * unit's -opt nopeephole stands in for the file's per-function
 * `peephole off` push/pop pairs, which the carve skips.
 */
#define GS_EVENT_EXEC_ONLY_8001374C
#include "src/game/gs_event_exec.c"
