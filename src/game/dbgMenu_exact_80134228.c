/** Exact debug-menu helper tail, 0x80134228 - 0x8013433C. */
#include "dolphin/types.h"
#include "game/effect/effect_util_types.h"

#pragma push
#pragma optimization_level 1
/* RULE-EXCEPTION(user-approved): local scheduling pragma for the retail
 * prologue order - see docs/RULE_EXCEPTIONS.md */
#pragma scheduling on
void* _dbgMenuGetWin__Fl(s32 offset)
{
    s32 mask = offset >> 31;
    return windowSearchID(((s32)lbl_80478848 + offset) & ~mask);
}
#pragma pop

s32 _dbgMenuGetIndex__FP14tagWINDOW_WORK(void* obj)
{
    u32 val = *(u32*)((u8*)obj + 0x04);
    s32 result = -1;

    if ((s32)val >= (s32)lbl_80478848) {
        result = (s32)(val - lbl_80478848);
    }
    return result;
}

#pragma optimization_level 4
#pragma push
#pragma scheduling on
u32 dbgMenuGetRootMenu(void)
{
    int new_var;
    extern u32 fn_800057A8(void);
    s32 val = (s32)fn_800057A8();
    if (val == 1) goto _ret2;
    if (val >= 1) goto _chk3;
    new_var = 2;
    goto _ret2;
_chk3:
    if (val >= 3) goto _ret2;
    return 0x115;
_ret2:
    return new_var;
}
#pragma pop

s32 dbgMenuGetLink__Fl(s32 idx)
{
    EffectUtilEntryFunc fp = (EffectUtilEntryFunc)lbl_80478F8C;
    EffectUtilEntry* result;
    if (fp == NULL) {
        result = NULL;
    } else {
        result = fp(idx);
    }
    if (result == NULL) {
        return 0;
    }
    return result->link;
}

u32 debugMenuGetNum__Fv(void)
{
    u32 (*fp)(void) = (u32 (*)(void))lbl_80478F88;
    if (fp == NULL) {
        return 0;
    }
    return fp();
}
