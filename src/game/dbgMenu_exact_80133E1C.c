/** Exact debug-menu message helper, 0x80133E1C - 0x80133E6C. */
#include "dolphin/types.h"
#include "game/effect/effect_util_types.h"

/* RULE-EXCEPTION(user-approved): local peephole control for retail codegen;
 * see docs/RULE_EXCEPTIONS.md. */
#pragma peephole off
u32 _dbgMenuGetMsgID__FP14tagWINDOW_WORKl(void* obj, s32 offset)
{
    EffectUtilEntry* result;
    s32 index = _dbgMenuGetItemNo__FP14tagWINDOW_WORKl(obj, offset);
    EffectUtilEntryFunc fp = (EffectUtilEntryFunc)lbl_80478F8C;

    if (fp == NULL) {
        result = NULL;
    } else {
        result = fp(index);
    }
    if (result == NULL) {
        return 0;
    }
    return result->value;
}
#pragma peephole on
