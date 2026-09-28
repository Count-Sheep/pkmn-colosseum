/** Candidate chunk 0x80176C78 - 0x80176F68 (cameraPlayOffsetAnime, cameraPlayAnime): scores the whole-TU candidate camera.c. */
/* Link triage (2026-09-28): not carvable. cameraPlayAnime /
 * cameraPlayOffsetAnime read camera.c's pool entries 0x8047D724 and
 * 0x8047D738, which the 0x801786F4 and 0x80177A64 chunks also read, so the
 * range cannot own them; whole TU only (see camera.c). */
#include "src/game/camera.c"
