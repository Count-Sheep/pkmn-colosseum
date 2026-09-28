/* Candidate chunk of HAL's particle.c (see src/game/particle.c for the TU
 * extent). Scored from the whole reconstructed unit on its library flags;
 * the unit cannot be linked until psRemoveParticle is exact. */
/* Link triage (2026-09-28): not carvable yet. psSetGeneratorAngleRadiusScale
 * owns its switch table (.data 0x8036BF80, alone) and particle.c's first
 * literal 3.0f (0x8047D5B0), but 0.0f/1.0f after it (0x8047D5B4-0x8047D5C0)
 * belong to psRemoveParticle's chunk, and a data object cannot start at
 * 0x8047D5B4 (MWCC aligns .sdata2 to 8). Links with the whole unit. */
#include "src/game/particle.c"
