/**
 * @file gs_range_8017FA5C_suffix.c
 * @brief gs-engine code, 0x8017FA5C - 0x80180C78 (14 fns): GS small-block
 *        heap (K&R-style free list), ARQ transfer queue and GSgapp job pool.
 *
 * CodeCandidate residual of the 0x8017F2C4 - 0x80180C78 range after the
 * exact island 0x8017F3F8 - 0x8017FA5C (gs_range_8017F3F8_middle.c) was
 * carved out. Functions are kept in address order.
 */
#include "dolphin/types.h"

typedef struct GsRangeRequest {
    u8 _pad_0[0x20];
    s32 field_20;
    s32 field_24;
    void* field_28;
    s32 field_2C;
    u32 field_30;
    void* field_34;
    void (*callback)(void* arg0, void* arg1);
    void* field_3C;
} GsRangeRequest;

typedef struct GsRangeMemNode {
    struct GsRangeMemNode* next;
    u32 size;
    void* data;
    struct GsRangeMemNode* previous;
} GsRangeMemNode;

typedef struct GsRangeStats {
    u32 cursorIndex;
    u8* cursor[8];
    s32 totalBase;
} GsRangeStats;

extern GsRangeMemNode* lbl_8047B1D0;
extern GsRangeStats lbl_80455048;

s32 fn_8017FA5C(void)
{
    GsRangeMemNode* head = lbl_8047B1D0;
    GsRangeMemNode* node;
    s32 sum = 0;
    volatile s32 count;

    if (head == NULL) {
        return lbl_80455048.totalBase;
    }

    count = 0;
    node = head->next;
    for (;;) {
        count++;
        if ((u32)node <= 0x80000000u) {
            return sum;
        }
        if (node != NULL) {
            sum += node->size;
        }
        if (node == lbl_8047B1D0) {
            break;
        }
        node = node->next;
    }
    return sum + lbl_80455048.totalBase;
}

typedef struct GsRangeDVDQueueEntry {
    u8 _pad00[0x20];
    u32 state;
    s32 mode;
    void* srcPtr;
    void* dstPtr;
    u32 size;
    u32 flag34;
    void (*callback)(void* entry);
    u32 callbackArg;
    u32 index;
} GsRangeDVDQueueEntry;

extern u32 lbl_8047B1D4;
extern u32 lbl_8047B1D8;
extern u32 OSDisableInterrupts(void);
extern void OSRestoreInterrupts(u32 level);
extern void ARQPostRequest(void* request, u32 owner, u32 direction,
                           u32 priority, u32 source, u32 destination,
                           u32 size, void (*callback)(void*));
extern void fn_800AE630(void* request, void* owner, u32 direction, u32 offset,
                        void* callback, void* callbackArg, void* src,
                        void* dst, u32 size);
extern void DCFlushRange(void* addr, u32 nBytes);
extern u16 fn_800E2C04(u32 size, u32 align);
extern void* fn_800E27B0(u16 handle);
extern void fn_8017FB08(void*);
void fn_801808E4(volatile GsRangeRequest* req);

typedef struct GsRangeBufferEntry {
    u32 field00;
    u32 field04;
    struct GsRangeBufferEntry* field08;
    u32 field0C;
    u8 pad10[0xC];
    u32 field1C;
} GsRangeBufferEntry;

typedef struct GsRangeBufferPool {
    u32 field00;
    u32 field04;
    u32 field08;
    u32 field0C;
    u32 field10;
    u32 field14;
    u32 field18;
    u32 field1C;
    GsRangeBufferEntry* entries;
    GsRangeBufferEntry* tail;
    u32 count;
    u32 field2C;
} GsRangeBufferPool;

typedef GsRangeMemNode GsRangeDecompEntry;

extern GsRangeBufferPool lbl_80454018;
extern GsRangeDecompEntry lbl_80455070[];
extern GsRangeMemNode lbl_80465070;
extern void fn_8017D624(void);

static GsRangeMemNode* rangeFindFreeDescriptor(void* data)
{
    s32 i;

    for (i = 0; i < 0x1000; i++) {
        if (lbl_80455070[i].data == NULL) {
            lbl_80455070[i].data = data;
            return &lbl_80455070[i];
        }
    }
    return NULL;
}

#pragma push
#pragma optimization_level 0
#pragma peephole off
void fn_8017FB08(void* allocation)
{
    GsRangeMemNode* block;
    GsRangeMemNode* previous;
    GsRangeMemNode* next;
    GsRangeMemNode* scan;
    GsRangeMemNode* scanPrevious;
    s32 i;

    if (allocation == NULL) {
        return;
    }

    block = NULL;
    for (i = 0; i < 0x1000; i++) {
        if (lbl_80455070[i].data == allocation) {
            block = &lbl_80455070[i];
            break;
        }
    }
    if (block == NULL || lbl_8047B1D0 == NULL) {
        return;
    }

    previous = lbl_8047B1D0;
    for (;;) {
        next = previous->next;
        if (block > previous && block < next) {
            break;
        }
        if (previous >= next &&
            (block > previous || block < next)) {
            break;
        }
        previous = next;
    }

    if (block->previous != NULL &&
        (u8*)block->previous->data + block->previous->size == block->data) {
        scan = block->previous;
        scan->size += block->size;
        block->data = NULL;
        block->next = NULL;
        block->previous = NULL;
        block = scan;

        for (;;) {
            scanPrevious = lbl_8047B1D0;
            scan = scanPrevious->next;
            while (scan != lbl_8047B1D0 &&
                   (u8*)block->data + block->size != scan->data) {
                scanPrevious = scan;
                scan = scan->next;
            }
            if ((u8*)block->data + block->size != scan->data) {
                break;
            }
            block->size += scan->size;
            scanPrevious->next = scan->next;
            if (scan == lbl_8047B1D0) {
                lbl_8047B1D0 = scan->next;
            }
            scan->data = NULL;
            scan->previous = NULL;
            scan->next = NULL;
        }
        return;
    }

    next = previous->next;
    if ((u8*)block->data + block->size == next->data) {
        block->size += next->size;
        block->next = next->next;
        next->data = NULL;
    } else {
        block->next = next;
    }

    if ((u8*)previous->data + previous->size == block->data) {
        previous->size += block->size;
        previous->next = block->next;
        block->data = NULL;
    } else {
        previous->next = block;
    }
    lbl_8047B1D0 = previous;
}

#pragma pop
#pragma push
#pragma optimization_level 3
#pragma peephole off
void* fn_8017FDB0(u32 size)
{
    GsRangeMemNode* block;
    GsRangeMemNode* previous;
    GsRangeMemNode* descriptor;
    void* allocation;
    u32 arenaSize;
    u32 alignedSize;
    u32 cursorIndex;

    alignedSize = (size + 0x1F) & ~0x1F;
    if (lbl_8047B1D0 == NULL) {
        lbl_80465070.next = &lbl_80465070;
        lbl_80465070.size = 0;
        lbl_8047B1D0 = &lbl_80465070;
    }

    for (;;) {
        previous = lbl_8047B1D0;
        block = previous->next;
        for (;;) {
            if ((u32)block <= 0x80000000) {
                return NULL;
            }
            if (block->size >= alignedSize) {
                if (block->size == alignedSize) {
                    previous->next = block->next;
                    descriptor = block;
                } else {
                    block->size -= alignedSize;
                    allocation =
                        (u8*)block->data + (block->size & ~0xF);
                    descriptor = rangeFindFreeDescriptor(allocation);
                    if (descriptor == NULL) {
                        fn_8017D624();
                        descriptor = rangeFindFreeDescriptor(allocation);
                    }
                    if (descriptor == NULL) {
                        return NULL;
                    }
                    descriptor->previous = block;
                    descriptor->size = alignedSize;
                }
                lbl_8047B1D0 = previous;
                return descriptor->data;
            }
            if (block == lbl_8047B1D0) {
                break;
            }
            previous = block;
            block = block->next;
        }

        arenaSize = alignedSize;
        if (arenaSize < 0x20) {
            arenaSize = 0x20;
        }
        cursorIndex = lbl_80455048.cursorIndex;
        if (cursorIndex >= 7 ||
            (u32)lbl_80455048.totalBase < arenaSize) {
            return NULL;
        }
        allocation = lbl_80455048.cursor[cursorIndex];
        lbl_80455048.cursorIndex = cursorIndex + 1;
        lbl_80455048.cursor[cursorIndex + 1] =
            (u8*)allocation + arenaSize;
        lbl_80455048.totalBase -= arenaSize;

        descriptor = rangeFindFreeDescriptor(allocation);
        if (descriptor == NULL) {
            fn_8017D624();
            descriptor = rangeFindFreeDescriptor(allocation);
        }
        if (descriptor == NULL) {
            return NULL;
        }
        descriptor->size = arenaSize;
        fn_8017FB08(descriptor->data);
    }
}
#pragma pop

#pragma optimize_for_size on
void fn_801800F8(u32 queueCount, u32 field04, u32 initialSize)
{
    GsRangeBufferEntry* buffer;
    GsRangeDVDQueueEntry* queue;
    u16 handle;
    u32 size;
    s32 i;
    void* allocation;

    lbl_80454018.entries = 0;
    handle = fn_800E2C04(0x8000, 0x20);
    if (handle != 0) {
        allocation = fn_800E27B0(handle);
    } else {
        allocation = 0;
    }

    lbl_80454018.field0C = 0;
    lbl_80454018.tail = 0;
    lbl_80454018.field18 = 0;
    lbl_80454018.field00 = 0;
    lbl_80454018.entries = allocation;
    lbl_80454018.field04 = 0;
    lbl_80454018.field08 = 0;
    lbl_80454018.field10 = 0;
    lbl_80454018.field14 = 0;
    lbl_80454018.count = 0x400;
    lbl_80454018.field2C = 0;

    buffer = lbl_80454018.entries;
    i = 0;
    while (i < lbl_80454018.count) {
        buffer->field0C = 0;
        buffer->field08 = 0;
        buffer->field04 = 0;
        buffer->field00 = 0;
        buffer->field1C = 0;
        buffer++;
        i++;
    }

    buffer = lbl_80454018.entries;
    buffer->field0C = 0;
    buffer->field08 = buffer + 1;
    lbl_80454018.tail = buffer;

    i = 0;
    while (i < lbl_80454018.count * 4) {
        lbl_80455070[i].data = 0;
        i++;
    }

    lbl_8047B1D8 = queueCount;
    size = (queueCount * sizeof(GsRangeDVDQueueEntry) + 0x1F) & ~0x1F;
    handle = fn_800E2C04(size, 0x20);
    if (handle != 0) {
        queue = fn_800E27B0(handle);
    } else {
        queue = 0;
    }
    lbl_8047B1D4 = (u32)queue;

    i = 0;
    while (i < lbl_8047B1D8) {
        queue->state = 0;
        queue->mode = 0;
        queue->callback = 0;
        queue->callbackArg = 0;
        queue->index = i;
        queue++;
        i++;
    }

    lbl_8047B1D0 = 0;
    lbl_80455048.cursorIndex = 0;
    lbl_80455048.cursor[0] = (u8*)field04;
    lbl_80455048.totalBase = initialSize;
    allocation = fn_8017FDB0(initialSize);
    fn_8017FB08(allocation);
}
#pragma optimize_for_size reset

#pragma push
#pragma optimization_level 3
#pragma peephole off
void fn_80180320(void* dst, void* src, u32 size)
{
    GsRangeDVDQueueEntry* entry;
    GsRangeDVDQueueEntry* result;
    u32 i;
    u32 alignedSize;
    u32 savedIntr;

    if (size == 0) {
        return;
    }

    entry = (GsRangeDVDQueueEntry*)lbl_8047B1D4;
    result = NULL;
    for (i = 0; i < lbl_8047B1D8; i++, entry++) {
        if (entry->state == 0) {
            entry->state = 1;
            result = entry;
            break;
        }
    }

    entry = result;
    savedIntr = OSDisableInterrupts();
    alignedSize = (size + 0x1F) & ~0x1F;
    entry->flag34 = 1;
    entry->mode = 1;
    entry->callback = NULL;
    entry->callbackArg = 0;
    entry->srcPtr = dst;
    entry->dstPtr = src;
    entry->size = alignedSize;
    DCFlushRange(dst, size);
    ARQPostRequest(entry, (u32)entry, 1, 0, (u32)src, (u32)dst,
                   alignedSize, (void (*)(void*))fn_801808E4);
    OSRestoreInterrupts(savedIntr);

    while (entry->state != 0) {
        if (entry->mode != 1) {
            entry->state = 0;
        }
    }
}
#pragma pop

void* fn_80180450(void* src, void* dst, u32 size)
{
    GsRangeDVDQueueEntry* entry;
    GsRangeDVDQueueEntry* result;
    u32 i;
    u32 alignedSize;
    u32 savedIntr;
    u32 count;

    if (size == 0) {
        return NULL;
    }

    alignedSize = (size + 0x1F) & ~0x1F;
    entry = (GsRangeDVDQueueEntry*)lbl_8047B1D4;
    count = lbl_8047B1D8;
    result = NULL;
    for (i = 0; i < count; i++) {
        if ((s32)entry->state == 0) {
            entry->state = 1;
            result = entry;
            break;
        }
        entry++;
    }

    entry = result;
    savedIntr = OSDisableInterrupts();
    entry->flag34 = 0;
    entry->mode = 1;
    entry->callback = NULL;
    entry->callbackArg = 0;
    entry->srcPtr = src;
    entry->dstPtr = dst;
    entry->size = alignedSize;
    DCFlushRange(src, alignedSize);
    ARQPostRequest(entry, (u32)entry, 0, 0, (u32)src, (u32)dst,
                   alignedSize, (void (*)(void*))fn_801808E4);
    OSRestoreInterrupts(savedIntr);

    result = entry;
    while ((s32)result->state != 0) {
        if (result->mode != 1) {
            result->state = 0;
        }
    }
    return result;
}

#pragma push
#pragma optimization_level 3
#pragma peephole off
void* fn_80180584(void* src, void* dst, u32 size, u32 cbA, u32 cbB)
{
    GsRangeDVDQueueEntry* entry;
    GsRangeDVDQueueEntry* result;
    u32 i;
    u32 alignedSize;
    u32 savedIntr;
    u32 count;

    if (size == 0) {
        return NULL;
    }

    alignedSize = (size + 0x1F) & ~0x1F;
    entry = (GsRangeDVDQueueEntry*)lbl_8047B1D4;
    count = lbl_8047B1D8;
    result = NULL;
    for (i = 0; i < count; i++) {
        if (entry->state == 0) {
            entry->state = 1;
            result = entry;
            break;
        }
        entry++;
    }

    entry = result;
    savedIntr = OSDisableInterrupts();
    entry->flag34 = 1;
    entry->mode = 1;
    entry->callback = (void (*)(void*))cbA;
    entry->callbackArg = cbB;
    entry->srcPtr = src;
    entry->dstPtr = dst;
    entry->size = alignedSize;
    DCFlushRange(src, size);
    ARQPostRequest(entry, (u32)entry, 1, 0, (u32)dst, (u32)src,
                   alignedSize, (void (*)(void*))fn_801808E4);
    OSRestoreInterrupts(savedIntr);
    return entry;
}
#pragma pop

#pragma push
#pragma optimization_level 3
#pragma peephole off
void* fn_80180694(void* src, void* dst, u32 size, u32 cbA, u32 cbB)
{
    GsRangeDVDQueueEntry* entry;
    GsRangeDVDQueueEntry* result;
    u32 i;
    u32 alignedSize;
    u32 savedIntr;
    u32 count;

    if (size == 0) {
        return NULL;
    }

    alignedSize = (size + 0x1F) & ~0x1F;
    entry = (GsRangeDVDQueueEntry*)lbl_8047B1D4;
    count = lbl_8047B1D8;
    result = NULL;
    for (i = 0; i < count; i++) {
        if (entry->state == 0) {
            entry->state = 1;
            result = entry;
            break;
        }
        entry++;
    }

    entry = result;
    savedIntr = OSDisableInterrupts();
    entry->flag34 = 0;
    entry->mode = 1;
    entry->callback = (void (*)(void*))cbA;
    entry->callbackArg = cbB;
    entry->srcPtr = src;
    entry->dstPtr = dst;
    entry->size = alignedSize;
    DCFlushRange(src, size);
    ARQPostRequest(entry, (u32)entry, 0, 0, (u32)src, (u32)dst,
                   alignedSize, (void (*)(void*))fn_801808E4);
    OSRestoreInterrupts(savedIntr);
    return entry;
}
#pragma pop

#pragma push
#pragma optimization_level 3
#pragma peephole off
void* fn_801807A8(void* src, void* dst, u32 size)
{
    GsRangeDVDQueueEntry* entry;
    GsRangeDVDQueueEntry* result;
    u32 i;
    u32 alignedSize;
    u32 savedIntr;
    u32 count;

    if (size == 0) {
        return NULL;
    }

    alignedSize = (size + 0x1F) & ~0x1F;
    entry = (GsRangeDVDQueueEntry*)lbl_8047B1D4;
    count = lbl_8047B1D8;
    result = NULL;
    for (i = 0; i < count; i++) {
        if ((s32)entry->state == 0) {
            entry->state = 1;
            result = entry;
            break;
        }
        entry++;
    }

    entry = result;
    savedIntr = OSDisableInterrupts();
    entry->flag34 = 0;
    entry->mode = 1;
    entry->callback = NULL;
    entry->callbackArg = 0;
    entry->srcPtr = src;
    entry->dstPtr = dst;
    entry->size = alignedSize;
    DCFlushRange(src, size);
    ARQPostRequest(entry, (u32)entry, 0, 0, (u32)src, (u32)dst,
                   alignedSize, (void (*)(void*))fn_801808E4);
    OSRestoreInterrupts(savedIntr);
    return entry;
}
#pragma pop

#pragma optimize_for_size on
s32 fn_801808B4(volatile GsRangeRequest* req)
{
    volatile GsRangeRequest* ptr = req;
    s32 out;

    if (ptr->field_24 != 1) {
        ptr->field_20 = 0;
    }
    out = ptr->field_20;
    return out;
}
#pragma optimize_for_size reset

void fn_801808E4(volatile GsRangeRequest* req)
{
    void (*cb)(void*, void*);

    req->field_24 = 0;
    if (req->callback != NULL) {
        cb = req->callback;
        cb((void*)req->field_34, (void*)req->field_3C);
    }
    req->field_20 = 0;
    DCFlushRange((void*)req->field_28, req->field_30);
}

typedef struct GsRangeSlotInfo {
    u8 pad00[0xF8];
    void* taskParam;
} GsRangeSlotInfo;

typedef struct GsRangePoolElem {
    s32 active;
    s32 field_4;
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

extern u16 fn_800E2C04(u32 size, u32 align);
extern void* fn_800E27B0(u16 handle);
extern u16 fn_800E202C(void*);
extern void fn_800E24B0(u16);
extern void fn_800E209C(u16);
extern void fn_8017C1D8(void*, void*, u32, void*);
extern void fn_8017C074(void*, void*, u32, void*);
extern u32 fn_8017AC30(void);
extern void* GSgappCreate(s32, u8, void*, void*);
extern void fn_8018114C(void);
extern void fn_80181224(void);
extern void* fn_80167F28(const char*);
extern u32 fn_80167E5C(void*);
extern void fn_80167E64(void*);
extern const char lbl_80273F80[];

void fn_8018094C(void)
{
    GsRangePoolElem* entry = lbl_8047B1E8.base;
    GsRangePoolElem* job;
    void* file;
    void* allocation;
    u16 handle;
    u32 size;
    s32 i;

    for (i = 0; i < lbl_8047B1E8.count; i++, entry++) {
        if (entry->active != 1) {
            continue;
        }

        if (entry->callback != 0) {
            entry->callback(entry->slot, entry->subEntry);
            return;
        }
        if (entry->app == 0) {
            continue;
        }
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

        if (lbl_8047B1E0 != 0) {
            handle = fn_800E202C(lbl_8047B1E0);
            if (handle != 0) {
                fn_800E24B0(handle);
                fn_800E209C(handle);
            }
            lbl_8047B1E0 = 0;
        }

        entry->active = 0;
        entry->app = 0;
        entry->state = 0;
        if (entry->nextJob == 0) {
            lbl_8047B1E4 = 0;
            return;
        }

        job = entry->nextJob;
        if (job->type == 0) {
            lbl_8047B1E4 = job;
            fn_8017C074(job->slot, job->subEntry, job->index, job);
            job->app = GSgappCreate(fn_8017AC30(), 0xC8,
                                     job->slot->taskParam, fn_8018114C);
            if (job->app != 0) {
                job->active = 1;
                job->state = 1;
                lbl_8047B1E4 = job;
            }
            return;
        }

        job->app = GSgappCreate(2, 0x1E, 0, fn_80181224);
        if (job->app != 0) {
            job->active = 1;
            job->state = 1;
            file = fn_80167F28(lbl_80273F80);
            size = fn_80167E5C(file);
            fn_80167E64(file);
            handle = fn_800E2C04((size + 0x1F) & ~0x1F, 0x20);
            if (handle != 0) {
                allocation = fn_800E27B0(handle);
            } else {
                allocation = 0;
            }
            lbl_8047B1E0 = allocation;
            lbl_8047B1E4 = job;
        }
        return;
    }
}

#pragma optimize_for_size on
void fn_80180B94(s32 count)
{
    s32 size = count * 0x40;
    u32 alignedSize = (size + 0x1F) & ~0x1F;
    u16 handle;
    GsRangePoolElem* elem;
    s32 i;

    lbl_8047B1E8.count = count;
    handle = fn_800E2C04(alignedSize, 0x20);
    if (handle != 0) {
        lbl_8047B1E8.base = fn_800E27B0(handle);
    } else {
        lbl_8047B1E8.base = NULL;
    }
    lbl_8047B1E0 = 0;
    lbl_8047B1E4 = 0;

    elem = lbl_8047B1E8.base;
    for (i = 0; i < count; i++) {
        elem->active = 0;
        elem->field_4 = 0;
        elem->callback = 0;
        elem->state = 0;
        elem->field_10 = 0;
        elem->app = 0;
        elem->nextJob = 0;
        elem->slot = 0;
        elem->subEntry = 0;
        elem++;
    }
}
#pragma optimize_for_size reset
