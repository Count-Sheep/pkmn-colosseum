/*
 * dobj.c DObjLoad (0x801993A4 - 0x80199568). Score instrumentation only: it
 * compiles the legacy DObj/FObj source under the old unit's flags.
 *
 * With the sysdolphin library flags and the natural Melee shape
 * (dobj->next = HSD_DObjLoadDesc(desc->next); ... switch on
 * rendermode & RENDER_BLENDING with HSD_DObjModifyFlags) DObjLoad's code is
 * identical except that MWCC gives dobj r31 and desc r28 where retail uses
 * desc r31 / dobj r30 / inner desc r29 / inner dobj r28 (96.9%). TObjLoad
 * shows the same swap around its inlined HSD_TObjLoadDesc.
 */
#include "src/hsd/hsd_dobj.c"
