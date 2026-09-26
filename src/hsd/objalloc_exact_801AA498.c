/**
 * @file objalloc_exact_801AA498.c
 * @brief sysdolphin objalloc.c, .text 0x801AA498-0x801AA568.
 *
 * HSD_ObjFree, HSD_ObjAlloc and HSD_ObjSetHeap. Colosseum's sysdolphin
 * (>= 1.3.0.0) no longer carves objects out of free lists: HSD_ObjAlloc
 * only enforces the object count limit, tracks used/peak and forwards to
 * the memory callbacks (fn_801A6928 / fn_801A6960, HSD_MemAlloc / HSD_Free),
 * and HSD_ObjFree just drops the count. HSD_ObjSetHeap still records the
 * obj_heap window exactly as in Melee.
 *
 * Built with the sysdolphin library flags (GC/1.3.2 -O4,p -O1
 * -inline auto,deferred -use_lmw_stmw on); deferred inlining emits
 * functions in reverse definition order, so they are listed from the last
 * address down. The level-1 optimizer does not CSE the four obj_heap
 * address loads in HSD_ObjSetHeap, which is what the retail code shows.
 *
 * Text-only unit carved from objalloc.c (0x801AA350-0x801AA568):
 * HSD_ObjAllocInit needs the TU's own .rodata string pool and .sdata2
 * "data" string, so it stays a candidate. obj_heap (.data 0x8036CBF0) stays
 * extern.
 */

#include "dolphin/types.h"
#include "hsd/hsd_objalloc.h"

typedef struct objheap {
    u32 top;
    u32 curr;
    u32 size;
    u32 remain;
} objheap;

extern void* fn_801A6928(u32 size); /* HSD_MemAlloc */
extern void fn_801A6960(void* ptr); /* HSD_Free */

extern objheap lbl_8036CBF0; /* obj_heap = { 0, 0, -1, -1 } */
#define obj_heap lbl_8036CBF0

void HSD_ObjSetHeap(u32 size, void* ptr)
{
    obj_heap.curr = (u32) ptr;
    obj_heap.top = (u32) ptr;
    obj_heap.remain = size;
    obj_heap.size = size;
}

void* HSD_ObjAlloc(HSD_ObjAllocData* data)
{
    if (data->num_limit_flag && data->used >= data->numLimit) {
        return NULL;
    }
    data->used += 1;
    if (data->used > data->peak) {
        data->peak = data->used;
    }
    return fn_801A6928(data->size);
}

void HSD_ObjFree(HSD_ObjAllocData* data, void* obj)
{
    data->used -= 1;
    fn_801A6960(obj);
}
