/**
 * @file ps_exact_80169034.c
 * @brief psSetParticleVisibility (0x80169034 - 0x80169104).
 *
 * The source is ps_range_80168C64.c. It calls _psListGetFirst (pslist.c)
 * rather than inlining it; built with the particle units' flags.
 */
#define PR410_PS_SPLIT
#define PR410_PS_VISIBILITY
#include "src/game/ps_range_80168C64.c"
