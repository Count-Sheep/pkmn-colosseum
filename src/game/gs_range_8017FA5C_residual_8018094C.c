/**
 * @file gs_range_8017FA5C_residual_8018094C.c
 * @brief GSgapp job pool update fn_8018094C (0x8018094C - 0x80180B94).
 *
 * CodeCandidate residual of the 0x8017FA5C - 0x80180C78 retail unit, built
 * at `-opt level=0` with `-inline deferred` like the rest of the unit.
 *
 * The default job type sizes "s1_out.fsys" (open / length / close) and
 * allocates a 32-byte-aligned buffer of that size. Retail copies the
 * length into a fresh register before aligning it (mr r20,r22; addi
 * r0,r20,31; clrrwi r21,r0,5). At optimisation level 0 an inline
 * parameter is only materialised as its own variable when the argument is
 * a local of another inline expansion; an argument that is a local of the
 * caller is substituted and gives no copy (compare memAlloc(cacheSize) in
 * fn_801800F8, which passes r18 straight to r3). So the length is the
 * result of an inline file-size helper whose value feeds memAllocAligned,
 * which fileGetSize reproduces with the same 147 instructions.
 *
 * Open difference: register numbering only. Retail saves r20-r31 and
 * leaves r23/r24 unused (two variables whose instructions were removed
 * after allocation), ranking the length (r22) above the aligned size (r21)
 * and the parameter copy (r20); this body allocates length r24, copy r23,
 * aligned r22 and saves r22-r31.
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

static inline u32 fileGetSize(const char* path)
{
    void* file;
    u32 length;

    file = fn_80167F28(path);
    length = fn_80167E5C(file);
    fn_80167E64(file);
    return length;
}

void fn_8018094C(void)
{
    GsRangePoolElem* entry;
    GsRangePoolElem* job;
    s32 i;
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
                            lbl_8047B1E0 = memAllocAligned(fileGetSize(lbl_80273F80));
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
