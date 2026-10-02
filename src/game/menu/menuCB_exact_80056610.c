/** Exact menuCB callback carve, 0x80056610 - 0x800566B4. */
#include "dolphin/types.h"

extern s32 lbl_8047A568;
extern void winSeqSetMenu(s32 param, u32 key);

/* RULE-EXCEPTION(user-approved): inherited nopeephole mode; see docs/RULE_EXCEPTIONS.md. */
u32 fn_80056610(u8* p)
{
    s8 state;

    state = (s8)p[1];
    switch (state) {
    case 0:
        if ((s8)p[2] == 0) {
            if (lbl_8047A568 != 0) {
                winSeqSetMenu(*(u32*)(p + 4), 0x107);
            }
            p[2] = 1;
        }
        break;
    case 3:
        if ((s8)p[2] == 0) {
            winSeqSetMenu(*(u32*)(p + 4), 0x10b);
            p[2] = 1;
        }
        break;
    }
    return 0;
}
