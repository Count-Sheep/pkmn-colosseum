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
 *
 * 2026-09-27 carve attempt (standalone DObjLoad, library flags, data
 * extern, HSD_DObjLoadDesc / HSD_DObjAlloc / HSD_DObjModifyFlags as
 * inline copies): 95.4% in every arrangement tried (helpers defined
 * before or after DObjLoad, static or global LoadDesc, TObjLoadDesc-style
 * `if (desc != NULL) { ... }` body (91.9%), a rendermode local). Retail
 * colours desc r31, dobj r30, inner desc r29, inner dobj r28; the carve
 * gets dobj r31, inner desc r30, inner dobj r29, desc r28. Only the
 * legacy pragma and param-copy shape in hsd_dobj.c reproduces retail, which is not
 * admissible.
 */
#include "src/hsd/hsd_dobj.c"
