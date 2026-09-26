/**
 * @file objalloc.h
 * @brief HAL sysdolphin objalloc.h: the object-pool accessors.
 *
 * The inline accessors come from the Melee decompilation (doldecomp/melee,
 * src/sysdolphin/baselib/objalloc.h). Their asserts name "objalloc.h"
 * through __FILE__; Colosseum's line numbers for them are not known (the
 * only user in this tree is initialize.c's dead-stripped HSD_ObjDumpStat),
 * so Melee's are kept.
 */
#ifndef SYSDOLPHIN_BASELIB_OBJALLOC_H
#define SYSDOLPHIN_BASELIB_OBJALLOC_H

#include "dolphin/types.h"
#include "hsd/hsd_objalloc.h"
#include "sysdolphin/baselib/debug.h"

void HSD_ObjSetHeap(u32 size, void* ptr);

static inline u32 HSD_ObjAllocGetUsing(HSD_ObjAllocData* data)
{
    HSD_ASSERT(205, data);
    return data->used;
}

static inline u32 HSD_ObjAllocGetFreed(HSD_ObjAllocData* data)
{
    HSD_ASSERT(221, data);
    return data->free;
}

static inline u32 HSD_ObjAllocGetPeak(HSD_ObjAllocData* data)
{
    HSD_ASSERT(237, data);
    return data->peak;
}

#endif /* SYSDOLPHIN_BASELIB_OBJALLOC_H */
