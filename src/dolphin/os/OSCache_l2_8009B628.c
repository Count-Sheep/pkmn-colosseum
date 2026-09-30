/**
 * @file OSCache_l2_8009B628.c
 * @brief OSCache.c tail: L2GlobalInvalidate, DMAErrorHandler and
 *        __OSCacheInit (0x8009B628 - 0x8009B914), with their report
 *        strings (.data 0x803105B0 - 0x803107E0).
 *
 * Split from OSCache_privileged_suffix.c, whose LCDisable, LCStoreBlocks,
 * LCQueueLength and LCQueueWait are hand-written asm in the SDK and stay
 * unlinked (no asm evidence entry).
 */

#define OSCACHE_SPLIT_ACTIVE
#define OSCACHE_L2_ACTIVE
#include "src/dolphin/os/OSCache.c"
