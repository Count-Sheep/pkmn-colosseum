/** Candidate chunk 0x801786F4 - 0x80179DFC: scores the whole-TU candidate camera.c. */
/* Link triage (2026-09-28): not carvable. These functions read camera.c's
 * .sdata2 float pool (0x8047D720-0x8047D790), whose entries are shared with
 * the other camera chunks (e.g. 0x8047D728/0x8047D72C with cameraSetFov,
 * 0x8047D738 with 0x80177A64 and 0x80176C78), and cameraInit copies its
 * initialiser from camera.c's .rodata (0x80273D98), which cannot link
 * because of the stripped function's strings (see camera.c). A carve would
 * need extern stand-ins for its literals: whole TU only. */
#include "src/game/camera.c"
