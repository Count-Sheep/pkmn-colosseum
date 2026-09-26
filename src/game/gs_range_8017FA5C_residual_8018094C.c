/**
 * @file gs_range_8017FA5C_residual_8018094C.c
 * @brief GSgapp job pool update fn_8018094C (0x8018094C - 0x80180B94).
 *
 * CodeCandidate residual of the 0x8017FA5C - 0x80180C78 retail unit, built
 * at `-opt level=0`.
 *
 * Open difference: retail copies the file length into one more register
 * before the aligned allocation (mr r20,r22) and reserves two further
 * non-volatile registers (r23/r24) that no instruction uses, i.e. two more
 * compiler temporaries than the recovered body has.
 */
#include "game/gs_range_8017FA5C_shared.h"

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
