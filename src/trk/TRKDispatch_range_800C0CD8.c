/**
 * MetroTRK flush_cache.c: TRK_flush_cache, .text 0x800C0D70 - 0x800C0DA8.
 * (TRKDoNotifyStopped and TRK_fill_mem_800D6430 are the
 * TRKDispatch_exact_800C0CD8 / _800C0DA8 objects.)
 */
#include "dolphin/types.h"

/* TRK_flush_cache - 0x800C0D70 | size: 0x38
 * Hand-written MetroTRK asm; evidence: docs/asm_evidence/trk_dispatch.md */
asm void TRK_flush_cache(register void* address, register u32 length) {
    nofralloc
    lis r5, 0xFFFF
    ori r5, r5, 0xFFF1
    and r5, r5, r3
    subf r3, r5, r3
    add r4, r4, r3
_flush_loop:
    dcbst r0, r5
    dcbf r0, r5
    sync
    icbi r0, r5
    addic r5, r5, 8
    subic. r4, r4, 8
    bge _flush_loop
    isync
    blr
}
