/**
 * @file ps_generator_candidate_80175B94.c
 * @brief Particle generator pool: psRemoveGenerator (0x80175B94 - 0x80175DF0).
 * psKillAllGenerator and psKillGenerator are built too so they can be
 * inlined, as retail does.
 *
 * The source is ps_generator_range_8017572C.c (see there for the
 * compiler, source order and inlining evidence).
 */
#define PS_GENERATOR_SPLIT
#define PS_GENERATOR_REMOVE
#define PS_GENERATOR_KILL
#define PS_GENERATOR_KILLALL
#include "src/game/ps_generator_range_8017572C.c"
