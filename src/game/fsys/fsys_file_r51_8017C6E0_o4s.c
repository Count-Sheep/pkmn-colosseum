/**
 * @file fsys_file_r51_8017C6E0_o4s.c
 * @brief FSYS entry read completion (0x8017C6E0 - 0x8017C88C).
 *
 * Status handler for the entry the slot has just read (slot->entryIndex,
 * trailer slot->currentSub): flush the data it read. A compressed entry
 * (flags bit 31) goes on to the decode stage (slot status 103, entry
 * state 2); a plain entry is done (state 6), reported to its group and the
 * slot returns to status 100.
 *
 * Built at optimisation level 0 with peephole and scheduling on, like the
 * rest of the fsys code (see configure.py): unit-wide "-opt level=0", no
 * local pragmas.
 *
 * Open wall (89.4%): retail passes fsysEntryDone a copy of `entry`
 * (mr r31,r28, the helper's parameter in its own register), which pushes
 * fsysGetEntry's index (slot->entryIndex) onto the stack (0x18). Level-0
 * inlining copies a parameter only when the argument is another inline's
 * local, so in retail `entry` lived in a helper around this code; no
 * repeated expansion of such a helper was found (find_inline_expansions.py
 * block 0x8017C6F4 0x8017C774 and 0x8017C764 0x8017C7A0: no other site),
 * so none is reconstructed.
 */
#include "dolphin/types.h"
#include "game/fsys/fsys_entry.h"

extern void DCFlushRange(void* addr, u32 nBytes);
extern void* GSresGetResource(u32 fileHandle, u32 nameHash);

/*
 * fsysEntryDone of fsys_file_exact_8017C1D8.c and
 * fsys_file_candidate_8017E30C_o2.c: report a loaded entry to its group.
 */
static inline void fsysEntryDone(FSYSSlot* slot, FSYSFileEntry* entry)
{
    u32 size;
    u32 unused;
    FSYSGroup* group;
    void (*done)(u32 fileHandle, u32 nameHash, u32 size);
    void* resource;

    unused = 0;
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
}

/* Address: 0x8017C6E0 | size: 0x1AC */
s32 fn_8017C6E0(FSYSSlot* slot)
{
    FSYSFileEntry* entry;
    FSYSSubEntry* sub;

    entry = fsysGetEntry(slot, slot->entryIndex);
    sub = slot->currentSub;
    if (entry->flags & 0x80000000) {
        DCFlushRange(sub->buffer, entry->decompressedSize);
        slot->status = 103;
        sub->state = 2;
    } else {
        DCFlushRange(sub->buffer, entry->decompressedSize);
        sub->state = 6;
        fsysEntryDone(slot, entry);
        slot->status = 100;
    }
    return 0;
}
