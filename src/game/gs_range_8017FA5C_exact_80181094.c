/**
 * @file gs_range_8017FA5C_exact_80181094.c
 * @brief GSgapp job-pool tasks (0x80181094 - 0x801812C4).
 *
 * The three background tasks the job pool starts (fn_80180C78 and
 * fn_8018094C pass them to GSgappCreate): fn_80181094 runs an archive
 * entry's group callback through fn_8017BB80, fn_8018114C decodes an
 * entry's LZSS image through fn_8017BC90, and fn_80181224 reads
 * "s1_out.fsys" (lbl_80273F80) into the pool's file buffer 30 times.
 * Each marks the current job (lbl_8047B1E4) done and ends its task.
 *
 * TU evidence: this code belongs to the 0x8017FA5C retail unit rather than
 * to people.c (whose code starts at 0x801812C4): it owns no data of its
 * own and works on that unit's job pool (.sbss lbl_8047B1E0/E4/E8, set up
 * by fn_80180B94), and fn_8018094C in that unit starts these very tasks.
 * It is built like the rest of the unit at `-opt level=0` with
 * `-inline deferred` and no local pragmas: every function here is exact
 * with those flags (the single-use-free, reload-per-access code of level
 * 0), and deferred inlining emits a unit's functions in reverse source
 * order, so they are written from the highest address down.
 */
#include "game/gs_range_8017FA5C_shared.h"
#include "game/fsys/fsys.h"

extern const char lbl_80273F80[]; /* "s1_out.fsys" */

extern void GSgappTerminate(void* app);
extern u32 fn_80167F28(const char* path);   /* DVDOpen wrapper */
extern u32 fn_80167E5C(u32 file);           /* file length */
extern s32 fn_80167E64(u32 file);           /* DVDClose wrapper */
extern void fn_80167ED0(u32 file, void* dst, u32 size, u32 offset);
extern s32 fn_8017BB80(GsRangeSlotInfo* slot, FSYSFileEntry* entry);
extern void* fn_8017BC90(GsRangeSlotInfo* slot, u32 nameHash,
                         void* compressed, FSYSSubEntry* sub);

/* Address: 0x80181224 | size: 0xA0 */
void fn_80181224(void)
{
    u32 file;
    s32 i;
    u32 size;
    void* buffer;

    buffer = lbl_8047B1E0;
    file = fn_80167F28(lbl_80273F80);
    size = fn_80167E5C(file);
    for (i = 0; i < 30; i++) {
        fn_80167ED0(file, buffer, size, 0);
    }
    fn_80167E64(file);
    lbl_8047B1E4->state = 0;
    GSgappTerminate(lbl_8047B1E4->app);
}

/* Address: 0x8018114C | size: 0xD8 */
void fn_8018114C(void)
{
    FSYSFileEntry* entry;
    u32* offsets;
    u32* tables;
    FSYSArchiveHeader* archive;

    archive = lbl_8047B1E4->slot->archiveData;
    tables = (u32*)((u8*)lbl_8047B1E4->slot->archiveData + archive->tableOffset);
    offsets = (u32*)((u8*)lbl_8047B1E4->slot->archiveData + tables[0]);
    entry = (FSYSFileEntry*)((u8*)lbl_8047B1E4->slot->archiveData +
                             offsets[lbl_8047B1E4->index]);
    fn_8017BC90(lbl_8047B1E4->slot, entry->nameHash, lbl_8047B1E4->compressed,
                lbl_8047B1E4->subEntry);
    lbl_8047B1E4->subEntry->state = 0;
    lbl_8047B1E4->state = 2;
    GSgappTerminate(lbl_8047B1E4->app);
}

/* Address: 0x80181094 | size: 0xB8 */
void fn_80181094(void)
{
    u32* offsets;
    u32* tables;
    FSYSArchiveHeader* archive;
    FSYSFileEntry* entry;

    archive = lbl_8047B1E4->slot->archiveData;
    tables = (u32*)((u8*)lbl_8047B1E4->slot->archiveData + archive->tableOffset);
    offsets = (u32*)((u8*)lbl_8047B1E4->slot->archiveData + tables[0]);
    entry = (FSYSFileEntry*)((u8*)lbl_8047B1E4->slot->archiveData +
                             offsets[lbl_8047B1E4->index]);
    fn_8017BB80(lbl_8047B1E4->slot, entry);
    lbl_8047B1E4->state = 2;
    GSgappTerminate(lbl_8047B1E4->app);
}
