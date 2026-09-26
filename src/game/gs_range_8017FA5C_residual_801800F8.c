/**
 * @file gs_range_8017FA5C_residual_801800F8.c
 * @brief gs heap / cache / ARQ-queue init and the ARQ transfer wrappers
 *        (0x801800F8 - 0x801808B4).
 *
 * CodeCandidate residual of the 0x8017FA5C - 0x80180C78 retail unit, built
 * at `-opt level=0`.
 *
 * fn_801800F8 addresses the queue (lbl_80454018), the cache header
 * (lbl_80454038) and the bump arena (lbl_80455048) through MWCC's pooled
 * .bss: when a function touches three or more .bss objects that the
 * translation unit has already defined, it loads the unit's .bss start once
 * (retail: lis/addi of lbl_80454018 into r29) and reaches each object with
 * a fresh `addi rX,r29,<object offset>` per access, the member offset
 * staying in the load/store (addi r10,r29,0x20; stw r0,0xc(r10) for
 * cache.count; the offset-0 cases fold to 0x20(r29) and 0(r29)). That is
 * why no pointer-member form ever reproduced it. Objects defined later
 * (the lbl_80455070 block array) keep their own relocation, and a
 * function that touches only one of them (the cache helpers in
 * gs_range_8017F3F8_middle.c, the allocator in _exact_8017FDB0) addresses
 * it directly. The r14-r16 "constants" are ordinary zero/0x400 temporaries
 * spilled into non-volatile registers once r3-r12 hold the pooled bases.
 * The three loops count in three distinct registers (r26, r25, r28), so
 * they use three separate counters.
 *
 * Open differences (why this stays a candidate):
 * - .bss order. MWCC lays a unit's pooled .bss out in the order in which
 *   the code first references each object after its definition (verified
 *   on GC/2.0: definition order and use counts do not matter; objects
 *   never referenced go last). Retail has the queue first (+0), then the
 *   cache (+0x20), then the arena (+0x1030). But the queue's only
 *   reference in the binary is here, after the pre-call
 *   `cache.nodes = NULL` (stw r0,0x20(r29)), so this body puts the cache
 *   first and shifts every pooled offset. Matching the order needs an
 *   earlier queue reference that emits no retail instruction (such as code
 *   the linker later stripped), which cannot be reconstructed.
 * - Register allocation. Retail passes entrySize straight to memAlloc
 *   (mr r3,r19), which this inline ARQ-table helper cannot (it copies it,
 *   mr r18,r19); it ranks the ARQ allocation's handle/result (r23/r24)
 *   above the cache allocation's (r21/r22), and it saves r20 without using
 *   it. With the ARQ code in the body instead, the copy goes but
 *   entryCount/arena are no longer spilled to 0x8/0xc(r1) as in retail.
 * - ARQ wrappers: the three parameter copies land in rotated registers
 *   (retail copies the second parameter with `mr.` and ranks it above the
 *   first); control flow and stores already match.
 * The pooled objects are defined here (not in bss_80452500) only because
 * pooling needs the definitions in this unit; this object is not linked.
 */
#include "game/gs_range_8017FA5C_shared.h"

GsRangeQueue lbl_80454018;
GsRangeCacheArea lbl_80454038;
GsRangeArena lbl_80455048;

static inline void memBlocksClear(void)
{
    GsRangeMemNode* block;
    s32 i;

    block = lbl_80455070;
    for (i = 0; i < 0x1000; i++) {
        block->data = NULL;
        block++;
    }
}

static inline void arqInit(u32 entryCount)
{
    GsRangeARQEntry* entry;
    u32 i;
    u32 entrySize;

    lbl_8047B1D8 = entryCount;
    entrySize = (entryCount * sizeof(GsRangeARQEntry) + 0x1F) & ~0x1F;
    lbl_8047B1D4 = memAlloc(entrySize);
    entry = lbl_8047B1D4;
    for (i = 0; i < lbl_8047B1D8; i++) {
        entry->state = 0;
        entry->mode = 0;
        entry->callback = NULL;
        entry->callbackArg = NULL;
        entry->index = i;
        entry++;
    }
}

void fn_801800F8(u32 entryCount, u8* arena, u32 arenaSize)
{
    GsRangeCacheNode* node;
    s32 i;
    u32 cacheSize;

    lbl_80454038.cache.nodes = NULL;
    cacheSize = 0x8000;
    lbl_80454038.cache.nodes = memAlloc(cacheSize);
    lbl_80454038.cache.last = NULL;
    lbl_80454038.cache.capacity = 0x400;
    lbl_80454038.cache.count = 0;
    lbl_80454018.field_00 = 0;
    lbl_80454018.field_04 = 0;
    lbl_80454018.field_08 = 0;
    lbl_80454018.field_0C = 0;
    lbl_80454018.field_10 = 0;
    lbl_80454018.field_14 = 0;
    lbl_80454018.field_18 = 0;
    node = lbl_80454038.cache.nodes;
    for (i = 0; i < 0x400; i++) {
        node->size = 0;
        node->data = node->prev = node->next = NULL;
        node->active = 0;
        node++;
    }
    node = lbl_80454038.cache.nodes;
    node->size = 0;
    node->next = node + 1;
    lbl_80454038.cache.last = node;
    memBlocksClear();
    arqInit(entryCount);
    lbl_8047B1D0 = NULL;
    lbl_80455048.cursorIndex = 0;
    lbl_80455048.remaining = arenaSize;
    lbl_80455048.cursor[0] = arena;
    fn_8017FB08(fn_8017FDB0(arenaSize));
}

extern BOOL OSDisableInterrupts(void);
extern BOOL OSRestoreInterrupts(BOOL level);
extern void ARQPostRequest(void* request, u32 owner, u32 type, u32 priority,
                           u32 source, u32 dest, u32 length,
                           void (*callback)(u32 request));
void fn_801808E4(GsRangeARQEntry* entry);

static inline GsRangeARQEntry* arqAlloc(void)
{
    GsRangeARQEntry* entry;
    u32 i;

    entry = lbl_8047B1D4;
    for (i = 0; i < lbl_8047B1D8; i++) {
        if (entry->state == 0) {
            entry->state = 1;
            return entry;
        }
        entry++;
    }
    return NULL;
}

static inline GsRangeARQEntry* arqRead(void* main, void* aram, u32 size,
                                       void (*callback)(void*, void*), void* arg)
{
    GsRangeARQEntry* entry;
    BOOL level;
    u32 alignedSize;

    if (size != 0) {
    entry = arqAlloc();
    level = OSDisableInterrupts();
    entry->flush = 1;
    entry->mode = 1;
    alignedSize = (size + 0x1F) & ~0x1F;
    entry->callback = callback;
    entry->callbackArg = arg;
    entry->src = main;
    entry->dst = aram;
    entry->size = alignedSize;
    DCFlushRange(main, size);
    ARQPostRequest(entry, (u32)entry, 1, 0, (u32)aram, (u32)main, alignedSize,
                   (void (*)(u32))fn_801808E4);
    OSRestoreInterrupts(level);
    return entry;
    }
    return NULL;
}

static inline GsRangeARQEntry* arqWrite(void* main, void* aram, u32 size,
                                        void (*callback)(void*, void*), void* arg)
{
    GsRangeARQEntry* entry;
    BOOL level;
    u32 alignedSize;

    if (size != 0) {
    entry = arqAlloc();
    level = OSDisableInterrupts();
    entry->flush = 0;
    entry->mode = 1;
    alignedSize = (size + 0x1F) & ~0x1F;
    entry->callback = callback;
    entry->callbackArg = arg;
    entry->src = main;
    entry->dst = aram;
    entry->size = alignedSize;
    DCFlushRange(main, size);
    ARQPostRequest(entry, (u32)entry, 0, 0, (u32)main, (u32)aram,
                   (size + 0x1F) & ~0x1F, (void (*)(u32))fn_801808E4);
    OSRestoreInterrupts(level);
    return entry;
    }
    return NULL;
}

static inline s32 arqIsBusy(void* handle)
{
    GsRangeARQEntry* entry = handle;

    if (entry->mode != 1) {
        entry->state = 0;
    }
    return entry->state;
}

static inline void arqWaitDone(void* handle)
{
    while (arqIsBusy(handle)) {
    }
}

static inline void arqSync(void* handle)
{
    GsRangeARQEntry* entry = handle;
    arqWaitDone(entry);
}

void fn_80180320(void* main, void* aram, u32 size)
{
    GsRangeARQEntry* entry = arqRead(main, aram, size, NULL, NULL);
    arqSync(entry);
}

void fn_80180450(void* main, void* aram, u32 size)
{
    GsRangeARQEntry* entry = arqWrite(main, aram, size, NULL, NULL);
    arqSync(entry);
}

GsRangeARQEntry* fn_80180584(void* main, void* aram, u32 size,
                             void (*callback)(void*, void*), void* arg)
{
    GsRangeARQEntry* entry = arqRead(main, aram, size, callback, arg);
    return entry;
}

GsRangeARQEntry* fn_80180694(void* main, void* aram, u32 size,
                             void (*callback)(void*, void*), void* arg)
{
    GsRangeARQEntry* entry = arqWrite(main, aram, size, callback, arg);
    return entry;
}

GsRangeARQEntry* fn_801807A8(void* main, void* aram, u32 size)
{
    GsRangeARQEntry* entry = arqWrite(main, aram, size, NULL, NULL);
    return entry;
}
