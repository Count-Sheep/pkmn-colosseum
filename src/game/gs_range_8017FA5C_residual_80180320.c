/**
 * @file gs_range_8017FA5C_residual_80180320.c
 * @brief ARQ transfer wrappers (0x80180320 - 0x801808B4).
 *
 * CodeCandidate residual of the 0x8017FA5C - 0x80180C78 retail unit, built
 * like the rest of it at `-opt level=0` with `-inline deferred`. Deferred
 * inlining emits a unit's functions in reverse source order, so the
 * wrappers are written from the highest address down.
 *
 * Open difference: the three parameter copies land in rotated registers
 * (retail copies the second parameter with `mr.` and ranks it above the
 * first); control flow and stores already match.
 */
#include "game/gs_range_8017FA5C_shared.h"

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

GsRangeARQEntry* fn_801807A8(void* main, void* aram, u32 size)
{
    GsRangeARQEntry* entry = arqWrite(main, aram, size, NULL, NULL);
    return entry;
}

GsRangeARQEntry* fn_80180694(void* main, void* aram, u32 size,
                             void (*callback)(void*, void*), void* arg)
{
    GsRangeARQEntry* entry = arqWrite(main, aram, size, callback, arg);
    return entry;
}

GsRangeARQEntry* fn_80180584(void* main, void* aram, u32 size,
                             void (*callback)(void*, void*), void* arg)
{
    GsRangeARQEntry* entry = arqRead(main, aram, size, callback, arg);
    return entry;
}

void fn_80180450(void* main, void* aram, u32 size)
{
    GsRangeARQEntry* entry = arqWrite(main, aram, size, NULL, NULL);
    arqSync(entry);
}

void fn_80180320(void* main, void* aram, u32 size)
{
    GsRangeARQEntry* entry = arqRead(main, aram, size, NULL, NULL);
    arqSync(entry);
}
