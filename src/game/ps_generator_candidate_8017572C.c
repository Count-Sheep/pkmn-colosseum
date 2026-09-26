/**
 * @file ps_generator_candidate_8017572C.c
 * @brief Particle generator pool: psKillAllGenerator (0x8017572C - 0x801758D8).
 * psKillGenerator is built too so it can be inlined, as retail does.
 *
 * The source is ps_generator_range_8017572C.c (see there for the
 * compiler, source order and inlining evidence).
 */
#define PS_GENERATOR_SPLIT
#define PS_GENERATOR_KILL
#define PS_GENERATOR_KILLALL
#include "src/game/ps_generator_range_8017572C.c"
