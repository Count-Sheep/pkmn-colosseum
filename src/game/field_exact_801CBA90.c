#include "dolphin/types.h"

extern void fn_800FF58C(u32);

#pragma push
// RULE-EXCEPTION(user-approved): scheduling off retains retail's call/return
// shape. See docs/RULE_EXCEPTIONS.md.
#pragma scheduling off
s32 fn_801CBA90(void)
{
    fn_800FF58C(0x395);
    return 0;
}
#pragma pop
