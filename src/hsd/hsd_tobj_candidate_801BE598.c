/*
 * tobj.c TObjLoad (0x801BE598 - 0x801BE800). Score instrumentation only: it
 * compiles the legacy tobj source under the old r58 prefix unit's flags.
 *
 * Under the sysdolphin library flags TObjLoad's code matches except that
 * MWCC swaps the callee-saved registers of tobj and td around the inlined
 * HSD_TObjLoadDesc (retail: tobj r31, td r30), the same wall as DObjLoad.
 */
#include "src/hsd/hsd_tobj_candidate_801BBDDC.c"
