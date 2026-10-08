#include "dolphin/types.h"
#include "game/effect/effect_util_types.h"

void dbgMenuClose(void)
{
    u8 disabled = 0;
    s32 menuId = lbl_80478848;

    lbl_8047AED1 = disabled;
    menuClose(menuId);
}
