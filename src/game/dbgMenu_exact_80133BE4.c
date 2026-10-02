/** Exact debug-menu termination helper, 0x80133BE4 - 0x80133C3C. */
#include "dolphin/types.h"
#include "game/effect/effect_util_types.h"

u32 _dbgMenuCheckTerminate__FP14tagWINDOW_WORKl(void* obj, s32 offset)
{
    EffectUtilEntry* result;
    u32 ret;
    s32 index = _dbgMenuGetItemNo__FP14tagWINDOW_WORKl(obj, offset);
    EffectUtilEntryFunc fp = (EffectUtilEntryFunc)lbl_80478F8C;

    if (fp == NULL) {
        result = NULL;
    } else {
        result = fp(index);
    }
    if (result == NULL) {
        ret = 1;
    } else {
        ret = (result->flags >> 7) & 1;
    }
    return (u8)ret;
}
