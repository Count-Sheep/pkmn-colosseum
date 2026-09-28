/* Candidate owner of its split range; built from the whole memory-card TU. */
/* Link triage (2026-09-28): not carvable. fn_801CBCDC reaches the SHA-1
 * digest buffers off the memory-card unit's .bss base (see memcard.c), so
 * it links only with the whole unit, i.e. once fn_801CDB04 and fn_801CF9C8
 * are exact. */
#include "src/game/memcard.c"
