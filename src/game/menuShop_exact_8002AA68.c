/** Exact shop phase callback, 0x8002AA68 - 0x8002AB00. */
#include "dolphin/types.h"

#pragma scheduling on
#pragma peephole off
#pragma optimization_level 4
s32 fn_8002AA68(void* r3)
{
    u8* r31;
    s8 state;
    r31 = (u8*)r3;
    state = (s8)r31[1];
    switch (state) {
    case 0:
        if ((s8)r31[2] == 0) {
            winSeqSetMenu((void*)0x61, 0x7e);
            r31[2] = 1;
        }
        break;
    case 3:
        if ((s8)r31[2] == 0) {
            winSeqSetMenu((void*)0x61, 0x82);
            r31[2] = 1;
        }
        break;
    }
    return 0;
}
