/**
 * @file gs_range_8017FA5C_suffix.c
 * @brief gs-engine code, 0x8017FA5C - 0x80180C78 (13 fns): GS small-block
 *        heap (K&R-style free list over lbl_80455070 descriptors), its
 *        init, the ARQ transfer queue and the GSgapp job pool.
 *
 * CodeCandidate residual of the 0x8017F2C4 - 0x80180C78 range after the
 * exact island 0x8017F3F8 - 0x8017FA5C (gs_range_8017F3F8_middle.c) was
 * carved out. Built at `-opt level=0` like the rest of the range.
 *
 * Open differences (why this stays a candidate):
 * - fn_8017FA5C / fn_8017FB08: register priority. Retail ranks the
 *   free-list head above the walk counter (fn_8017FA5C) and the inline
 *   list heads above the prev-end/data pair (fn_8017FB08); the recovered
 *   bodies give those variables the reverse order.
 * - fn_801800F8: retail stores every queue/cache field through its own
 *   short-lived base register (addi rX,r29,0x20; stw r0,0xc(rX)), which
 *   plain member stores do not reproduce.
 * - ARQ wrappers: the three parameter copies land in rotated registers
 *   (retail copies the second parameter with `mr.` and ranks it above the
 *   first); control flow and stores already match.
 * - fn_8018094C: one extra variable copy before the aligned allocation.
 */
#include "dolphin/types.h"

extern void DCFlushRange(void* addr, u32 nBytes);
extern void* fn_800E27B0(u16 handle);
void fn_8017FB08(void* allocation);
void* fn_8017FDB0(u32 size);

typedef struct GsRangeCacheNode {
    void* data;
    struct GsRangeCacheNode* prev;
    struct GsRangeCacheNode* next;
    s32 size;
    u32 fileHandle;
    u32 key1;
    u32 key2;
    s32 active;
} GsRangeCacheNode;

typedef struct GsRangeCache {
    GsRangeCacheNode* nodes;
    GsRangeCacheNode* last;
    u32 capacity;
    s32 count;
} GsRangeCache;

typedef struct GsRangeMemNode {
    struct GsRangeMemNode* next;
    u32 size;
    void* data;
    struct GsRangeMemNode* previous;
} GsRangeMemNode;

typedef struct GsRangeArena {
    s32 cursorIndex;
    u8* cursor[8];
    u32 remaining;
} GsRangeArena;

extern GsRangeMemNode* lbl_8047B1D0;
extern GsRangeArena lbl_80455048;

u32 fn_8017FA5C(void)
{
    GsRangeMemNode* node;
    u32 sum;
    s32 count;
    GsRangeMemNode* head;

    sum = 0;
    head = lbl_8047B1D0;
    if (!lbl_8047B1D0) {
        sum = lbl_80455048.remaining;
    } else {
        count = 0;
        node = head->next;
        for (;;) {
            count++;
            if ((u32)node <= 0x80000000) {
                return sum;
            }
            if (node) {
                sum += node->size;
            }
            if (node == lbl_8047B1D0) {
                break;
            }
            node = node->next;
        }
        sum += lbl_80455048.remaining;
    }
    return sum;
}

extern GsRangeMemNode lbl_80455070[0x1000];

static inline GsRangeMemNode* memFindBlock(void* data)
{
    GsRangeMemNode* block;
    s32 i;

    block = lbl_80455070;
    for (i = 0; i < 0x1000; i++) {
        if (block->data == data) {
            return block;
        }
        block++;
    }
    return NULL;
}

static inline GsRangeMemNode* memFindPrev(GsRangeMemNode* target)
{
    GsRangeMemNode* a = NULL;
    GsRangeMemNode* b = NULL;
    GsRangeMemNode* head = lbl_8047B1D0;
    GsRangeMemNode* p;

    for (p = head->next;; p = p->next) {
        a = target;
        b = p->next;
        if (a == b) {
            return p;
        }
    }
}

static inline GsRangeMemNode* memAbsorbNext(GsRangeMemNode* block)
{
    u8* end = NULL;
    u8* data = NULL;
    GsRangeMemNode* head = lbl_8047B1D0;
    GsRangeMemNode* p;
    GsRangeMemNode* prev;

    if (!block) {
        return NULL;
    }
    for (p = head->next;; p = p->next) {
        end = (u8*)block->data + block->size;
        data = p->data;
        if (end == data) {
            block->size += p->size;
            p->data = NULL;
            prev = memFindPrev(p);
            prev->next = p->next;
            if (p == lbl_8047B1D0) {
                lbl_8047B1D0 = p->next;
            }
            p->previous = NULL;
            p->next = NULL;
            return p;
        }
        if (p == lbl_8047B1D0) {
            break;
        }
    }
    return NULL;
}

void fn_8017FB08(void* allocation)
{
    u8* end = NULL;
    u8* data = NULL;
    GsRangeMemNode* bp;
    GsRangeMemNode* p;
    GsRangeMemNode* prev;

    if (!allocation) {
        return;
    }
    bp = memFindBlock(allocation);
    if (!bp) {
        return;
    }
    for (p = lbl_8047B1D0; !(bp > p && bp < p->next); p = p->next) {
        if (p >= p->next && (bp > p || bp < p->next)) {
            break;
        }
    }
    if (bp->previous) {
        end = (u8*)bp->previous->data + bp->previous->size;
        prev = bp->previous;
    }
    data = bp->data;
    if (end == data) {
        bp->previous->size += bp->size;
        bp->data = NULL;
        bp->next = NULL;
        bp->previous = NULL;
        while (memAbsorbNext(prev)) {
        }
        return;
    }
    if (p->next->data == (u8*)bp->data + bp->size) {
        bp->size += p->next->size;
        bp->next = p->next->next;
        p->next->data = NULL;
    } else {
        bp->next = p->next;
    }
    if (bp->data == (u8*)p->data + p->size) {
        p->size += bp->size;
        p->next = bp->next;
        bp->data = NULL;
    } else {
        p->next = bp;
    }
    lbl_8047B1D0 = p;
}

extern GsRangeMemNode lbl_80465070;
extern void fn_8017D624(void);

static inline GsRangeMemNode* memNewBlock(void* data)
{
    GsRangeMemNode* block;
    s32 i;

    block = lbl_80455070;
    for (i = 0; i < 0x1000; i++) {
        if (!block->data) {
            block->data = data;
            return block;
        }
        block++;
    }
    return NULL;
}

static inline void* memArenaGrow(u32 size)
{
    u8* cursor = lbl_80455048.cursor[lbl_80455048.cursorIndex];
    u32 alignedSize = (size + 0x1F) & ~0x1F;

    if (lbl_80455048.remaining >= alignedSize && lbl_80455048.cursorIndex < 7) {
        lbl_80455048.cursorIndex++;
        lbl_80455048.cursor[lbl_80455048.cursorIndex] = cursor + alignedSize;
        lbl_80455048.remaining -= alignedSize;
        return cursor;
    }
    return NULL;
}

static inline GsRangeMemNode* memMoreCore(u32 size)
{
    u8* cursor;
    GsRangeMemNode* block;

    if (size < 0x20) {
        size = 0x20;
    }
    cursor = memArenaGrow(size);
    if (!cursor) {
        return NULL;
    }
    block = memNewBlock(cursor);
    if (!block) {
        fn_8017D624();
        block = memNewBlock(cursor);
    }
    block->size = size;
    fn_8017FB08(block->data);
    return lbl_8047B1D0;
}

void* fn_8017FDB0(u32 size)
{
    GsRangeMemNode* p;
    GsRangeMemNode* prevp;
    GsRangeMemNode* orig;
    u32 alignedSize;
    s32 count = 0;

    alignedSize = (size + 0x1F) & ~0x1F;
    if (!(prevp = lbl_8047B1D0)) {
        lbl_80465070.next = lbl_8047B1D0 = prevp = &lbl_80465070;
        lbl_80465070.size = 0;
    }
    for (count = 0, p = prevp->next;; prevp = p, p = p->next) {
        count++;
        if ((u32)p <= 0x80000000) {
            return NULL;
        }
        if (p->size >= alignedSize) {
            if (p->size == alignedSize) {
                prevp->next = p->next;
            } else {
                orig = p;
                p->size -= alignedSize;
                p = (GsRangeMemNode*)((u8*)p->data +
                                      p->size / sizeof(GsRangeMemNode) * sizeof(GsRangeMemNode));
                p = memNewBlock(p);
                if (!p) {
                    fn_8017D624();
                    p = memNewBlock(p);
                }
                p->previous = orig;
                p->size = alignedSize;
            }
            lbl_8047B1D0 = prevp;
            return p->data;
        }
        if (p == lbl_8047B1D0) {
            if (!(p = memMoreCore(alignedSize))) {
                return NULL;
            }
        }
    }
}

typedef struct GsRangeQueue {
    u32 field_00;
    u32 field_04;
    u32 field_08;
    u32 field_0C;
    u32 field_10;
    u32 field_14;
    u32 field_18;
    u32 field_1C;
} GsRangeQueue;

typedef struct GsRangeMemWork {
    GsRangeQueue queue;
    GsRangeCache cache;
    u8 _pad_30[0x1000];
    GsRangeArena arena;
} GsRangeMemWork;

typedef struct GsRangeARQEntry {
    u8 request[0x20];
    s32 state;
    s32 mode;
    void* src;
    void* dst;
    u32 size;
    u32 flush;
    void (*callback)(void* arg0, void* arg1);
    void* callbackArg;
    u32 index;
} GsRangeARQEntry;

extern GsRangeMemWork lbl_80454018;
extern GsRangeARQEntry* lbl_8047B1D4;
extern u32 lbl_8047B1D8;
extern u16 fn_800E2C04(u32 size, u32 align);

static inline void* memAlloc(u32 size)
{
    u16 h = fn_800E2C04(size, 0x20);
    if (h) {
        return fn_800E27B0(h);
    }
    return NULL;
}

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

s32 fn_801808B4(void* handle)
{
    GsRangeARQEntry* entry = handle;

    if (entry->mode != 1) {
        entry->state = 0;
    }
    return entry->state;
}

void fn_801808E4(GsRangeARQEntry* entry)
{
    GsRangeARQEntry* e = entry;

    e->mode = 0;
    if (e->callback) {
        e->callback((void*)e->flush, e->callbackArg);
    }
    e->state = 0;
    DCFlushRange(e->src, e->size);
}

typedef struct GsRangeSlotInfo {
    u8 pad00[0xF8];
    void* taskParam;
} GsRangeSlotInfo;

typedef struct GsRangePoolElem {
    s32 active;
    s32 field_04;
    void (*callback)(void*, void*);
    s32 state;
    s32 field_10;
    s32 type;
    void* app;
    struct GsRangePoolElem* nextJob;
    GsRangeSlotInfo* slot;
    u32 index;
    void* subEntry;
    u8 _pad_2C[0x14];
} GsRangePoolElem;

typedef struct GsRangePoolInfo {
    s32 count;
    GsRangePoolElem* base;
} GsRangePoolInfo;

extern GsRangePoolInfo lbl_8047B1E8;
extern void* lbl_8047B1E0;
extern GsRangePoolElem* lbl_8047B1E4;

extern u16 fn_800E202C(void* ptr);
extern void fn_800E24B0(u16 handle);
extern void fn_800E209C(u16 handle);
extern void fn_8017C1D8(void*, void*, u32, void*);
extern void fn_8017C074(void*, void*, u32, void*);
extern s32 fn_8017AC30(void);
extern void* GSgappCreate(s32, s32, void*, void*);
extern void fn_8018114C(void);
extern void fn_80181224(void);
extern void* fn_80167F28(const char*);
extern s32 fn_80167E5C(void*);
extern void fn_80167E64(void*);
extern const char lbl_80273F80[];

static inline void* memAllocAligned(u32 size)
{
    u32 alignedSize = (size + 0x1F) & ~0x1F;
    u16 h = fn_800E2C04(alignedSize, 0x20);
    if (h) {
        return fn_800E27B0(h);
    }
    return NULL;
}

void fn_8018094C(void)
{
    GsRangePoolElem* entry;
    GsRangePoolElem* job;
    s32 i;
    void* file;
    s32 size;
    u16 h;

    entry = lbl_8047B1E8.base;
    for (i = 0; i < lbl_8047B1E8.count; i++) {
        if (entry->active == 1) {
            if (entry->callback) {
                entry->callback(entry->slot, entry->subEntry);
                return;
            }
            if (entry->app) {
                if (entry->state == 1) {
                    return;
                }
                if (entry->state == 2) {
                    if (entry->type == 0) {
                        fn_8017C1D8(entry->slot, entry->subEntry, entry->index, entry);
                    }
                    entry->state = 0;
                    return;
                }
                if (lbl_8047B1E0) {
                    h = fn_800E202C(lbl_8047B1E0);
                    if (h) {
                        fn_800E24B0(h);
                        fn_800E209C(h);
                    }
                    lbl_8047B1E0 = NULL;
                }
                entry->active = 0;
                entry->app = NULL;
                entry->state = 0;
                if (entry->nextJob) {
                    job = entry->nextJob;
                    switch (job->type) {
                    case 0:
                        lbl_8047B1E4 = job;
                        fn_8017C074(lbl_8047B1E4->slot, lbl_8047B1E4->subEntry,
                                    lbl_8047B1E4->index, lbl_8047B1E4);
                        job->app = GSgappCreate(fn_8017AC30(), 0xC8,
                                                job->slot->taskParam, fn_8018114C);
                        if (job->app) {
                            job->active = 1;
                            job->state = 1;
                            lbl_8047B1E4 = job;
                        }
                        break;
                    default:
                        job->app = GSgappCreate(2, 0x1E, NULL, fn_80181224);
                        if (job->app) {
                            job->active = 1;
                            job->state = 1;
                            file = fn_80167F28(lbl_80273F80);
                            size = fn_80167E5C(file);
                            fn_80167E64(file);
                            lbl_8047B1E0 = memAllocAligned(size);
                            lbl_8047B1E4 = job;
                        }
                        break;
                    }
                } else {
                    lbl_8047B1E4 = NULL;
                }
                return;
            }
        }
        entry++;
    }
}

void fn_80180B94(s32 count)
{
    u32 size;
    GsRangePoolElem* entry;
    s32 i;

    size = (count * sizeof(GsRangePoolElem) + 0x1F) & ~0x1F;
    lbl_8047B1E8.count = count;
    lbl_8047B1E8.base = memAlloc(size);
    lbl_8047B1E0 = NULL;
    entry = lbl_8047B1E8.base;
    lbl_8047B1E4 = NULL;
    for (i = 0; i < count; i++) {
        entry->active = 0;
        entry->field_04 = 0;
        entry->callback = NULL;
        entry->nextJob = NULL;
        entry->state = 0;
        entry->slot = NULL;
        entry->subEntry = NULL;
        entry->app = NULL;
        entry->field_10 = 0;
        entry++;
    }
}
