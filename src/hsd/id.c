/**
 * @file id.c
 * @brief HAL sysdolphin id.c: the u32 id -> data hash table used to
 *        resolve descriptor references, 0x8019C0F8-0x8019C3C4.
 *
 * Adapted from the Melee decompilation (doldecomp/melee,
 * src/sysdolphin/baselib/id.c). Listed in HAL's order; deferred inlining
 * emits the functions in reverse (the retail address order).
 *
 * The table and the entry pool keep their dtk names (lbl_804653A8 is
 * default_table, lbl_8046553C is hsd_iddata).
 */
#include "dolphin/types.h"
#include "hsd/hsd_id.h"
#include "hsd/hsd_objalloc.h"
#include "sysdolphin/baselib/debug.h"

void* memset(void* dst, int c, u32 n);
void HSD_ObjAllocInit(HSD_ObjAllocData* data, u32 size, u32 align);
void HSD_ObjFree(HSD_ObjAllocData* data, void* obj);

/* hsd_iddata */
HSD_ObjAllocData lbl_8046553C;

/* default_table */
HSD_IDTable lbl_804653A8;

HSD_ObjAllocData* HSD_IDGetAllocData(void)
{
    return &lbl_8046553C;
}

void HSD_IDInitAllocData(void)
{
    HSD_ObjAllocInit(HSD_IDGetAllocData(), sizeof(IDEntry), 4);
}

void HSD_IDSetup(void)
{
    memset(&lbl_804653A8, 0, sizeof(HSD_IDTable));
}

static inline u32 hash(u32 id)
{
    return id % 0x65;
}

static inline IDEntry* IDEntryAlloc(void)
{
    IDEntry* entry;

    entry = HSD_ObjAlloc(HSD_IDGetAllocData());
    HSD_ASSERT(67, entry);
    memset(entry, 0, sizeof(IDEntry));

    return entry;
}

void HSD_IDInsertToTable(HSD_IDTable* table, u32 id, void* data)
{
    IDEntry* entry;
    u32 idx;

    if (table == NULL) {
        table = &lbl_804653A8;
    }

    idx = hash(id);
    entry = table->table[idx];
    while (entry != NULL) {
        if (entry->id == id) {
            break;
        }
        entry = entry->next;
    }

    if (entry != NULL) {
        entry->id = id;
        entry->data = data;
    } else {
        entry = IDEntryAlloc();
        entry->id = id;
        entry->data = data;
        entry->next = table->table[idx];
        table->table[idx] = entry;
    }
}

static inline void IDEntryFree(IDEntry* entry)
{
    HSD_ObjFree(HSD_IDGetAllocData(), entry);
}

void HSD_IDRemoveByIDFromTable(HSD_IDTable* table, u32 id)
{
    IDEntry* entry;
    IDEntry* prev;
    u32 idx;

    if (table == NULL) {
        table = &lbl_804653A8;
    }

    idx = hash(id);
    prev = NULL;
    for (entry = table->table[idx]; entry != NULL; entry = entry->next) {
        if (entry->id == id) {
            if (prev != NULL) {
                prev->next = entry->next;
            } else {
                table->table[idx] = entry->next;
            }
            IDEntryFree(entry);
            return;
        }
        prev = entry;
    }
}

void* HSD_IDGetDataFromTable(HSD_IDTable* table, u32 id, s32* success)
{
    IDEntry* entry;

    if (table == NULL) {
        table = &lbl_804653A8;
    }

    entry = table->table[hash(id)];
    while (entry != NULL) {
        if (entry->id == id) {
            if (success != NULL) {
                *success = 1;
            }
            return entry->data;
        }
        entry = entry->next;
    }

    if (success != NULL) {
        *success = 0;
    }
    return NULL;
}

void _HSD_IDForgetMemory(void)
{
    memset(&lbl_804653A8, 0, sizeof(HSD_IDTable));
}
