/**
 * @file hsd_memory_head_exact_801A6928.c
 * @brief Memory-callback dispatchers at the head of HAL's memory.c,
 *        0x801A6928 - 0x801A69C0.
 *
 * Split from hsd_lobj_tail_exact_801A68D0.c when lobj.c (which ends at
 * 0x801A6928) became its own translation unit; the code is unchanged.
 */
#include "dolphin/types.h"
#include "hsd/hsd_lobj.h"

extern u8 lbl_80465608[];

void fn_801A6928(HSD_LObj* lobj)
{
    void (*func)(HSD_LObj*, u32, u32);
    func = ((void (**)(HSD_LObj*, u32, u32))lbl_80465608)[0];
    func(lobj, 0x20, 0);
}

void fn_801A6960(HSD_LObj* lobj)
{
    void (*func)(HSD_LObj*);
    func = ((void (**)(HSD_LObj*))lbl_80465608)[1];
    func(lobj);
}

typedef void (*LObjDispatchFn)(void*);

void fn_801A6990(HSD_LObj* lobj)
{
    LObjDispatchFn func;
    func = ((LObjDispatchFn*)lbl_80465608)[4];
    func(lobj);
}
