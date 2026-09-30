/**
 * @file OSCache_privileged_suffix.c
 * @brief Candidate-only locked-cache primitives, 0x8009B510 - 0x8009B628.
 *
 * LCDisable, LCStoreBlocks, LCQueueLength and LCQueueWait are hand-written
 * asm in the SDK (no asm evidence entry), so the unit stays unlinked;
 * LCStoreData is C. The C tail from L2GlobalInvalidate is
 * OSCache_l2_8009B628.c.
 */

#define OSCACHE_SPLIT_ACTIVE
#define OSCACHE_SUFFIX_ACTIVE
#include "src/dolphin/os/OSCache.c"
