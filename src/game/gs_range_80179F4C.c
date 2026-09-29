/**
 * .text 0x80179FA4 - 0x8017A5FC: fn_80179FA4. fn_80179F4C (0x80179F4C -
 * 0x80179FA4), the other function between camera.c (which ends at
 * 0x80179F4C with _cameraRestoreStateData) and gs_range_8017A5FC_prefix.c,
 * is exact at level 0 and carved into gs_range_80179F4C_exact_80179F4C.c.
 *
 * They are not camera.c code: both are built without optimization (the
 * parameter goes through the stack), they share no data with camera.c and
 * fn_80179FA4 works on 0x80453FEC / 0x8047B1B8, which the fsys units after
 * it also use. The function sets up the archive's ARAM cache and asynchronous
 * DVD read, including the separate external-file path. This remains a
 * CodeCandidate: 91.41% at the unit-wide level-0 flags, not linked or usable
 * as accepted recomp evidence. Remaining differences include the packed-size
 * extraction, local stack layout, and external-file entry walk.
 */

#include "game/fsys/fsys.h"

extern FSYSManager lbl_80453FEC;
extern FSYSFileHandle* lbl_8047B1B8;
extern s32 lbl_8047B1BC;
extern void* lbl_8047B1C0[2];
extern u32 fn_8017FA5C(void);
extern void fn_8017F800(s32 handleID);
extern u32 fn_8017F928(u32 size, u32 fileHandle, u32 key1, u32 key2);
extern u8 fn_80167EF8(const char* path);
extern u32 fn_80167F28(const char* path);
extern u32 fn_80167E5C(u32 fileInfo);
extern u8 fn_80167E98(u32 fileInfo, void* dst, u32 length, u32 offset,
                      void (*callback)(s32 result));
extern void fn_8017A5FC(s32 result);

/* The same cache table walk and LRU removal expand in the later FSYS stages. */
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

static inline s32 fsysCacheOldestHandle(void)
{
    FSYSFileHandle* table;

    table = lbl_8047B1B8;
    return table->handleID;
}

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

static inline void fsysCacheMakeRoom(FSYSSlot* slot)
{
    u32 freeSize;
    s32 n;
    s32 handleID;

    freeSize = fn_8017FA5C();
    if (slot->totalDecompSize > freeSize && fsysCacheFindHandle(slot) < 0) {
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
            if (slot->totalDecompSize <= freeSize) {
                break;
            }
        }
    }
}

static inline FSYSFileEntry* fsysEntryAt(FSYSSlot* slot, u32 index)
{
    u8* archive;
    u32* entryTable;
    u32* firstTable;

    archive = (u8*)slot->archiveData;
    if (archive != NULL) {
        firstTable = (u32*)(archive + *(u32*)(archive + 0x18));
        entryTable = (u32*)(archive + firstTable[0]);
        return (FSYSFileEntry*)(archive + entryTable[index]);
    }
    return NULL;
}

/* Address: 0x80179FA4 | size: 0x658. Archive cache and DVD read setup. */
void fn_80179FA4(FSYSSlot* slot, u32 offset, u32 length, void* callbackA,
                 u32 callbackB, u32 callbackC, const char* externalPath,
                 FSYSFileEntry* entry)
{
    u32 packedSize;
    u32 cacheSize;
    u32 cacheAddress;
    u32 fileSize;
    u32 i;
    u32 total;
    FSYSFileEntry* member;
    void* dvdBuffer;

    packedSize = entry->decompressedSize;
    cacheSize = ((packedSize & 0x1FFFF) + 0x1F) & ~0x1F;
    cacheSize += packedSize & ~0x1FFFF;

    fsysCacheMakeRoom(slot);
    cacheAddress = fn_8017F928(cacheSize, slot->fileHandle, entry->groupID,
                                 entry->nameHash);
    fsysCacheMakeRoom(slot);

    if (length < 0x20000) {
        slot->dmaChunkSize = (length + 0x1F) & ~0x1F;
    } else {
        slot->dmaChunkSize = 0x20000;
    }
    slot->dmaBytesRemaining = length;
    slot->dmaSrcOffset = 0;
    slot->dmaDstOffset = offset;
    slot->callbackA = (u32)callbackA;
    slot->callbackB = callbackB;
    slot->callbackC = callbackC;
    slot->dmaCopyDst = (void*)cacheAddress;
    slot->dmaAsyncRequest = NULL;
    lbl_80453FEC.activeSlot = slot;
    lbl_80453FEC.currentSlot = slot;
    dvdBuffer = lbl_8047B1C0[lbl_80453FEC.field_24];

    if (slot->archiveData->flags & 1) {
        slot->tocBuffer = NULL;
        if (externalPath != NULL && fn_80167EF8(externalPath)) {
            slot->tocBuffer = (void*)fn_80167F28(externalPath);
        }
        if (slot->tocBuffer == NULL) {
            fn_80167E98(slot->fileInfo0, dvdBuffer, slot->dmaChunkSize,
                         offset, fn_8017A5FC);
            return;
        }
        fileSize = fn_80167E5C((u32)slot->tocBuffer);
        if (fileSize < 0x20000) {
            slot->dmaChunkSize = (fileSize + 0x1F) & ~0x1F;
        } else {
            slot->dmaChunkSize = 0x20000;
        }
        entry->decompressedSize = fileSize;
        slot->dmaBytesRemaining = fileSize;
        slot->dmaDstOffset = 0;
        total = 0;
        for (i = 0; i < slot->numEntries; i++) {
            member = fsysEntryAt(slot, i);
            total += member->decompressedSize;
        }
        total += *(u32*)((u8*)slot->archiveData + slot->field_18 + 8);
        slot->totalDecompSize = total;
        fn_80167E98((u32)slot->tocBuffer, dvdBuffer, slot->dmaChunkSize,
                     0, fn_8017A5FC);
    } else {
        fn_80167E98(slot->fileInfo0, dvdBuffer, slot->dmaChunkSize,
                     offset, fn_8017A5FC);
    }
}
