/**
 * @file fsys_file_candidate_8017CED8.c
 * @brief FSYS archive TOC stage done (0x8017CED8 - 0x8017D3A0).
 *
 * Once an archive's TOC is in memory: cache it in ARAM unless it is cached
 * already (making room first), make room in the ARAM cache for the whole
 * archive, move the archive's handle to the most recently used end of the
 * handle table (fn_8017D68C), reset every entry's runtime trailer, and set
 * the slot to status 100.
 *
 * The ARAM cache eviction helpers are those of
 * fsys_file_candidate_8017DB74_gc20.c (expanded there once and here twice).
 *
 * Built at optimisation level 0 with peephole and scheduling on, like the
 * rest of the fsys code (see configure.py): unit-wide "-opt level=0", no
 * local pragmas. The entry loop counts with an s32 against a saved count,
 * as retail's cmpw and 0x64(r1) show.
 *
 * Open wall (96.5%): the second eviction reloads slot->totalDecompSize at
 * each test, where an inline parameter bound to a memory load is copied
 * once (controlled compiles, GC/1.3 -opt level=0: every load argument is
 * copied, a register expression is substituted). The first eviction's
 * aligned tocSize is substituted as retail has it. In the entry loop
 * retail also copies the counter into fsysGetEntry's index (0x8) and homes
 * `entry` (0x68); here both stay in registers. Neither an s32 nor a u32
 * counter produces that copy in controlled compiles, so its source is
 * still unknown.
 */
#include "dolphin/types.h"
#include "game/fsys/fsys_entry.h"

extern FSYSFileHandle* lbl_8047B1B8; /* handle table, LRU order */
extern s32 lbl_8047B1BC;             /* handle table fill count */

extern void DCFlushRange(void* addr, u32 nBytes);
extern u32 fn_8017F794(u32 fileHandle, u32 key1, u32 key2);
extern void fn_8017F800(s32 handleID);
extern u32 fn_8017FA5C(void);
extern u32 fn_8017F928(u32 size, u32 fileHandle, u32 key1, u32 key2);
extern void fn_80180450(void* main, void* aram, u32 size);
extern FSYSFileHandle* fn_8017D68C(FSYSSlot* slot);

/* The slot's archive handle if the handle table holds it, else -1. */
static inline s32 fsysCacheFindHandle(FSYSSlot* slot)
{
    FSYSFileHandle* table;
    s32 i;

    table = lbl_8047B1B8;
    for (i = 0; i < lbl_8047B1BC; i++) {
        if (table->handleID == (s32)slot->field_08) {
            return table->handleID;
        }
        table++;
    }
    return -1;
}

/* The least recently used handle (the table's first entry). */
static inline s32 fsysCacheOldestHandle(void)
{
    FSYSFileHandle* table;

    table = lbl_8047B1B8;
    return table->handleID;
}

/* Drop handleID from the handle table; -1 when it is not there. */
static inline s32 fsysCacheRemoveHandle(s32 handleID)
{
    FSYSFileHandle* table;
    s32 i;
    s32 found;
    s32 index;

    found = 0;
    table = lbl_8047B1B8;
    for (i = 0; i < lbl_8047B1BC; i++) {
        if (table->handleID == handleID) {
            table->handleID = -1;
            found = 1;
            index = i;
            break;
        }
        table++;
    }
    if (found) {
        table = lbl_8047B1B8;
        for (i = 0; i < lbl_8047B1BC - 1; i++) {
            if (i >= index) {
                table[i] = table[i + 1];
            }
        }
        lbl_8047B1BC--;
        table[lbl_8047B1BC].handleID = -1;
        return 0;
    }
    return -1;
}

/*
 * Evict least recently used archives from the ARAM cache until `size`
 * bytes are free, unless the slot's own archive is cached already. `size`
 * is substituted into both tests, so an aligned size is recomputed at each
 * (fn_8017CED8 passes slot->totalDecompSize, which is reloaded instead).
 */
static inline void fsysCacheMakeRoom(FSYSSlot* slot, u32 size)
{
    u32 freeSize;
    s32 n;
    s32 handleID;

    freeSize = fn_8017FA5C();
    if (size > freeSize && fsysCacheFindHandle(slot) < 0) {
        for (n = 0;; n++) {
            handleID = fsysCacheOldestHandle();
            if (handleID < 0) {
                break;
            }
            fn_8017F800(handleID);
            if (fsysCacheRemoveHandle(handleID) < 0) {
                break;
            }
            freeSize = fn_8017FA5C();
            if (size <= freeSize) {
                break;
            }
        }
    }
}

/* Address: 0x8017CED8 | size: 0x4C8 */
s32 fn_8017CED8(FSYSSlot* slot)
{
    u32 tocSize;
    u32 cached;
    s32 i;
    s32 count;
    FSYSFileEntry* entry;

    tocSize = slot->field_1C;
    cached = fn_8017F794(slot->fileHandle, 0, 1);
    if (!cached) {
        DCFlushRange(slot->archiveData, (tocSize + 0x1F) & ~0x1F);
        fsysCacheMakeRoom(slot, (tocSize + 0x1F) & ~0x1F);
        cached = fn_8017F928((tocSize + 0x1F) & ~0x1F, slot->fileHandle, 0, 1);
        if (cached) {
            fn_80180450(slot->archiveData, (void*)cached, (tocSize + 0x1F) & ~0x1F);
        }
    }
    fsysCacheMakeRoom(slot, slot->totalDecompSize);
    fn_8017D68C(slot);
    count = slot->numEntries;
    for (i = 0; i < count; i++) {
        entry = fsysGetEntry(slot, i);
        ((FSYSSubEntry*)entry->subEntry)->buffer = NULL;
        ((FSYSSubEntry*)entry->subEntry)->state = 0;
        ((FSYSSubEntry*)entry->subEntry)->ready = 0;
    }
    slot->padding05C = 1;
    slot->status = 100;
    return 0;
}
