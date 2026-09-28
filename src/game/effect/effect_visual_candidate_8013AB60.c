/* Link triage (2026-09-28): not carvable. fn_8013AB60 reads 0x8047D1E0 (its
 * own) and the int-to-float constant 0x8047D1E8, which the 0x8013AD9C and
 * 0x8013B268 chunks also read, so the range cannot own its pool entries. */
#include "src/game/effect/effect_visual.c"
