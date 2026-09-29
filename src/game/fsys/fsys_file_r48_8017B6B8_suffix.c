/**
 * @file fsys_file_r48_8017B6B8_suffix.c
 * @brief FSYS entry read start and group-done notification
 *        (0x8017B6B8 - 0x8017BC90).
 *
 * fn_8017B6B8 starts loading an archive entry: mark its trailer loading
 * (state 1, slot status 0x65), allocate its destination (compressed entries
 * from the top of the handle heap, plain ones through fsysAllocEntry, as in
 * fn_8017BD34), then either transfer the slot's current entry from the ARAM
 * cache (status 0xA1, fn_8017F25C completes it) or read it from the disc or
 * its external file (fn_8017F108 completes it). fn_8017BB80 flushes a loaded
 * entry and tells its group's done callback.
 *
 * Level-0 code like the rest of fsys (see configure.py; this object used to
 * be scored at -O1 from fsys_file_candidates.c). Stack homes follow the
 * frontend's numbering: function-level locals in declaration order, then
 * inline expansions breadth-first by depth (first-level helpers, then the
 * helpers they call). fsysReadCachedEntry is the first-level helper that
 * numbering shows for the cached branch. fsysAllocEntry, fsysUpdateTotalSize
 * and gsResAlloc are fn_8017BD34's helpers.
 */
#include "dolphin/types.h"
#include "game/fsys/fsys_entry.h"

extern void* GSresAllocResourceAlign(u32 size, u32 align, u32 group, u32 id, u32 param);
extern void* GSresGetResource(u32 fileHandle, u32 nameHash);
extern void DCFlushRange(void* addr, u32 nBytes);
extern u8 fn_80167EF8(const char* path);
extern u32 fn_80167F28(const char* path);
extern u32 fn_80167E5C(u32 fileInfo);
extern s32 fn_80167E64(u32 fileInfo);
extern u8 fn_80167E98(u32 fileInfo, void* dst, u32 length, u32 offset,
                      void (*callback)(s32 result));
extern u32 fn_8017F794(u32 fileHandle, u32 groupID, u32 nameHash);
extern void fn_80180584(void* dst, u32 src, u32 size, void (*callback)(void), void* userData);
extern u16 fn_800E2B00(u32 size, u32 alignment);
extern void* fn_800E27B0(u16 handle);
extern void fn_8017F25C(void);
extern void fn_8017F108(s32 result);

/* GSres allocation of an entry buffer, 32-byte aligned (as in fn_8017E30C). */
static inline void* gsResAlloc(u32 size, u32 group, u32 id)
{
    void* p;

    p = GSresAllocResourceAlign(size, 0x20, group, id, 0);
    return p;
}

/* Recompute the archive's total size after an entry changed size (as in
 * fn_8017E30C). */
static inline void fsysUpdateTotalSize(FSYSSlot* slot)
{
    u32 total;
    u32 k;
    FSYSFileEntry* e;
    u8* info;

    total = 0;
    for (k = 0; k < slot->numEntries; k++) {
        e = fsysGetEntry(slot, k);
        total += e->decompressedSize;
    }
    info = (u8*)slot->archiveData + slot->field_18;
    total += *(u32*)(info + 8);
    slot->totalDecompSize = total;
}

/*
 * Destination buffer of a plain entry (as in fn_8017E30C). Entries of
 * archives flagged external (flags bit 0) take their size from the file on
 * disc first.
 */
static inline void* fsysAllocEntry(FSYSSlot* slot, FSYSFileEntry* entry)
{
    u32 size;
    void* buffer;
    FSYSGroup* group;
    void* (*alloc)(u32 fileHandle, u32 nameHash, u32 size);
    FSYSArchiveHeader* archive;

    buffer = NULL;
    group = fsysFindGroup(entry->groupID);
    archive = slot->archiveData;
    size = entry->decompressedSize;
    if (archive->flags & 1) {
        FSYSSubEntry* sub;
        char* name;
        u32 fileSize;

        archive = slot->archiveData;
        name = (char*)slot->archiveData + entry->dataOffset;
        slot->tocBuffer = NULL;
        if (fn_80167EF8(name)) {
            slot->tocBuffer = (void*)fn_80167F28(name);
        }
        sub = (FSYSSubEntry*)entry->subEntry;
        if (slot->tocBuffer) {
            fileSize = fn_80167E5C((u32)slot->tocBuffer);
            entry->decompressedSize = fileSize;
            size = entry->decompressedSize;
            fsysUpdateTotalSize(slot);
            fn_80167E64((u32)slot->tocBuffer);
            slot->tocBuffer = NULL;
            sub->ready = 1;
        }
    }
    if (group->alloc) {
        alloc = group->alloc;
        buffer = alloc(slot->fileHandle, entry->nameHash, size);
    } else {
        buffer = gsResAlloc((size + 0x1F) & ~0x1F, slot->fileHandle, entry->nameHash);
    }
    return buffer;
}

/* Heap allocation from the top of the handle heap, 32-byte aligned. */
static inline void* fsysAllocTop(u32 size)
{
    u32 alignedSize;
    u16 handle;

    alignedSize = (size + 0x1F) & ~0x1F;
    handle = fn_800E2B00(alignedSize, 0x20);
    if (handle != 0) {
        return fn_800E27B0(handle);
    }
    return NULL;
}

/* The slot's current entry is already cached in ARAM: transfer it. */
/* RULE-EXCEPTION(title-path): single-use inline helper evidenced only by stack numbering — see docs/RULE_EXCEPTIONS.md */
static inline void fsysReadCachedEntry(FSYSSlot* slot)
{
    FSYSFileEntry* current;
    u32 cached;
    u32 size;
    FSYSSubEntry* sub;

    current = fsysGetEntry(slot, slot->entryIndex);
    sub = slot->currentSub;
    size = current->decompressedSize;
    cached = fn_8017F794(slot->fileHandle, current->groupID, current->nameHash);
    slot->status = 0xA1;
    fn_80180584(sub->buffer, cached, size, fn_8017F25C, slot);
}

/* Address: 0x8017B6B8 | size: 0x4C8 */
void fn_8017B6B8(FSYSSlot* slot, FSYSFileEntry* entry, u32 index)
{
    u32* offsets;
    u32* tables;
    u32 cached;
    char* name;
    FSYSSubEntry* sub;
    FSYSArchiveHeader* archive;
    FSYSFileEntry* current;
    u32 size;

    sub = slot->currentSub;
    /* RULE-EXCEPTION(title-path): no-op references used only for level-0 register priority — see docs/RULE_EXCEPTIONS.md */
    (void)sub;
    (void)sub;
    (void)size;
    (void)size;
    sub->state = 1;
    slot->status = 0x65;
    if (entry->flags & 0x80000000) {
        size = entry->decompressedSize;
        sub->buffer = fsysAllocTop(size);
        slot->padding100 = (u32)sub->buffer;
    } else {
        sub->buffer = fsysAllocEntry(slot, entry);
    }
    if (!sub->buffer) {
        slot->status = 100;
        sub->state = 7;
        return;
    }
    archive = slot->archiveData;
    tables = (u32*)((u8*)slot->archiveData + archive->tableOffset);
    offsets = (u32*)((u8*)slot->archiveData + tables[0]);
    size = entry->decompressedSize;
    size = (size + 0x1F) & ~0x1F;
    current = (FSYSFileEntry*)((u8*)slot->archiveData + offsets[index]);
    cached = fn_8017F794(archive->groupID, current->groupID, current->nameHash);
    if (cached) {
        fsysReadCachedEntry(slot);
    } else if (archive->flags & 1) {
        name = (char*)slot->archiveData + current->dataOffset;
        slot->tocBuffer = NULL;
        if (fn_80167EF8(name)) {
            slot->tocBuffer = (void*)fn_80167F28(name);
        }
        if (!slot->tocBuffer) {
            fn_80167E98(slot->fileInfo0, sub->buffer, size, current->padding04, fn_8017F108);
        } else {
            fn_80167E98((u32)slot->tocBuffer, sub->buffer, size, 0, fn_8017F108);
        }
    } else {
        fn_80167E98(slot->fileInfo0, sub->buffer, size, current->padding04, fn_8017F108);
    }
}

/* Address: 0x8017BB80 | size: 0x110 */
s32 fn_8017BB80(FSYSSlot* slot, FSYSFileEntry* entry)
{
    u32 size;
    /* RULE-EXCEPTION(title-path): unused locals kept for retail's dead stack stores — see docs/RULE_EXCEPTIONS.md */
    void* buffer = NULL;
    FSYSGroup* group;
    void* resource;
    void (*done)(u32 fileHandle, u32 nameHash, u32 size);

    group = fsysFindGroup(entry->groupID);
    size = entry->compressedSize;
    if (group->done) {
        resource = GSresGetResource(slot->fileHandle, entry->nameHash);
        if (resource) {
            if (entry->flags & 0x80000000) {
                DCFlushRange(resource, entry->compressedSize);
            } else {
                DCFlushRange(resource, entry->decompressedSize);
            }
        }
        done = group->done;
        if (entry->flags & 0x80000000) {
            done(slot->fileHandle, entry->nameHash, entry->compressedSize);
        } else {
            done(slot->fileHandle, entry->nameHash, entry->decompressedSize);
        }
    }
    return 0;
}
