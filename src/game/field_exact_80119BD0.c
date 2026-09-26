/**
 * @file field_exact_80119BD0.c
 * @brief Exact field function fn_80119BD0, 0x80119BD0 - 0x80119D90.
 *
 * The 0.0f/1.0f constants it loads (lbl_8047CFE8/lbl_8047CFEC in .sdata2)
 * are declared const in field_range_80117E58.c, which lets MWCC schedule
 * the first load ahead of the particle's field stores as retail does.
 */
#define FIELD_BANK_ACTIVE
#define FIELD_CANDIDATE_80119BD0_80119D90
#include "src/game/field_range_80117E58.c"
