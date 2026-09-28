/* Candidate chunk of HAL's psinterpret.c (see src/game/psinterpret.c for the
 * TU extent). Scored from the whole reconstructed unit on its library flags;
 * the unit cannot be linked until every function is exact. */
/* Link triage (2026-09-28): not carvable. setVelToJObj reads psinterpret.c's
 * pool entries 0x8047D630/0x8047D650/0x8047D670/0x8047D678, which the
 * 0x8016F430, 0x80172BBC and 0x80172FA8 chunks also read. */
#include "src/game/psinterpret.c"
