#ifndef HSD_OBJALLOC_H
#define HSD_OBJALLOC_H

#include "dolphin/types.h"

typedef struct HSD_ObjAllocLink {
    struct HSD_ObjAllocLink* next;
} HSD_ObjAllocLink;

typedef struct _HSD_ObjAllocData {
    u32 num_limit_flag : 1;  /* HSD_ObjAlloc: lbz + extrwi. of bit 7 */
    u32 heap_limit_flag : 1;
    HSD_ObjAllocLink* freeHead;
    u32 used;
    u32 free;
    u32 peak;
    u32 numLimit;
    u32 heapLimitSize;
    u32 heapLimitNum;
    u32 size;
    u32 align;
    struct _HSD_ObjAllocData* next;
} HSD_ObjAllocData;

void* HSD_ObjAlloc(HSD_ObjAllocData* data);

#endif /* HSD_OBJALLOC_H */
