/**
 * @file sdk_candidate_800B671C.c
 * @brief GXInit.c tail, 0x800B671C - 0x800B6FE0: __GXInitGX.
 *
 * The unit owns GXInit.c's .sdata2 pool 0x8047C2E0-0x8047C308: the GX state
 * pointer `gx` (0x8047C2E0), then __GXInitGX's clear/black/white colours,
 * the 1.0f/0.0f/0.1f literals and the u32-to-float bias. Nothing else reads
 * the literals. Body order and calls follow the SDK's GXInit.c as decompiled
 * in XD:
 * https://github.com/TeamOrre/xd-decomp/blob/4989794e6c6430684e033bc56f4bb97c9a921e73/src/dolphin/gx/GXInit.c
 */
#define SDK_EXACT_800B671C_800B6FE0
#include "src/dolphin/sdk_range_800AE3F0.c"
