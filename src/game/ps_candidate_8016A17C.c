/* Candidate chunk of HAL's particle.c (see src/game/particle.c for the TU
 * extent). Scored from the whole reconstructed unit on its library flags;
 * the unit cannot be linked until psRemoveParticle is exact. */
/* Link triage (2026-09-28): not carvable. psInitDataBank keeps particle.c's
 * .rodata string pool base (lbl_80273820) and the pooled bank tables' .bss
 * base (lbl_804527C8) in registers and addresses both by offset, so it
 * links only with the whole unit. */
#include "src/game/particle.c"
