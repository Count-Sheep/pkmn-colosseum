#ifndef GAME_FSYS_FSYS_ENTRY_H
#define GAME_FSYS_FSYS_ENTRY_H

#include "game/fsys/fsys.h"

/*
 * fsysGetEntry -- the archive entry index -> FSYSFileEntry lookup the fsys
 * code expands inline (fn_8017B4BC, fn_8017C074, fn_8017C1D8, fn_8017C39C,
 * fn_8017DAB8, ...): the archive's table-offset pair at +0x18 gives the
 * per-entry offset list, and entry `index` lives at base + offsets[index].
 *
 * Every expansion in the level-0 fsys code saves one non-volatile register
 * that no instruction uses: the typed header view `archive` of the byte
 * pointer `base` is a plain copy, which the peephole pass folds into its
 * uses after register allocation. Writing the lookup through `base` alone
 * (or through `archive` alone) leaves the other registers one slot off.
 */
static inline FSYSFileEntry* fsysGetEntry(FSYSSlot* slot, u32 index)
{
    u8* base;
    FSYSArchiveHeader* archive;
    u32* tables;
    u32* offsets;

    base = (u8*)slot->archiveData;
    if (base) {
        archive = (FSYSArchiveHeader*)base;
        tables = (u32*)(base + archive->tableOffset);
        offsets = (u32*)(base + tables[0]);
        return (FSYSFileEntry*)(base + offsets[index]);
    }
    return NULL;
}

#endif /* GAME_FSYS_FSYS_ENTRY_H */
