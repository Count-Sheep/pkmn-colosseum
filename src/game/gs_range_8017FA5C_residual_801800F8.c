/**
 * @file gs_range_8017FA5C_residual_801800F8.c
 * @brief gs heap / cache / ARQ-queue init and the ARQ transfer wrappers
 *        (0x801800F8 - 0x801808B4).
 *
 * CodeCandidate residual of the 0x8017FA5C - 0x80180C78 retail unit, built
 * at `-opt level=0`.
 *
 * Open differences (why this stays a candidate):
 * - fn_801800F8: retail stores the queue/cache/arena fields through a
 *   fresh base register per store (addi rX,r29,0x20; stw r0,0xc(rX)) and
 *   keeps the constants 0x400/0/0 in their own non-volatile registers;
 *   plain member stores fold the member offset into the store.
 * - ARQ wrappers: the three parameter copies land in rotated registers
 *   (retail copies the second parameter with `mr.` and ranks it above the
 *   first); control flow and stores already match.
 */
#include "game/gs_range_8017FA5C_shared.h"

void fn_801800F8(u32 entryCount, u8* arena, u32 arenaSize)
{
    GsRangeMemWork* work;
    GsRangeCacheNode* node;
    GsRangeMemNode* block;
    GsRangeARQEntry* entry;
    s32 i;
    u32 cacheSize;
    u32 entrySize;

    work = &lbl_80454018;
    work->cache.nodes = NULL;
    cacheSize = 0x8000;
    work->cache.nodes = memAlloc(cacheSize);
    work->cache.last = NULL;
    work->cache.capacity = 0x400;
    work->cache.count = 0;
    work->queue.field_00 = 0;
    work->queue.field_04 = 0;
    work->queue.field_08 = 0;
    work->queue.field_0C = 0;
    work->queue.field_10 = 0;
    work->queue.field_14 = 0;
    work->queue.field_18 = 0;
    node = work->cache.nodes;
    for (i = 0; i < 0x400; i++) {
        node->size = 0;
        node->data = node->prev = node->next = NULL;
        node->active = 0;
        node++;
    }
    node = work->cache.nodes;
    node->size = 0;
    node->next = node + 1;
    work->cache.last = node;
    block = lbl_80455070;
    for (i = 0; i < 0x1000; i++) {
        block->data = NULL;
        block++;
    }
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
    lbl_8047B1D0 = NULL;
    work->arena.cursorIndex = 0;
    work->arena.remaining = arenaSize;
    work->arena.cursor[0] = arena;
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
