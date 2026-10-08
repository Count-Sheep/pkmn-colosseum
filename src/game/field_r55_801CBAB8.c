#include "dolphin/types.h"

extern void winMsgOpen(s32, s32, s32, s32);
extern s32 fn_8001E184(void);
extern void winMsgClose(s32);
extern s32 fn_800889E4(s32);
extern s32 fn_800FF58C(s32);

/* RULE-EXCEPTION(user-approved): scoped compiler controls preserve the retail
 * initialization copies and epilogue order. See docs/RULE_EXCEPTIONS.md. */
#pragma push
#pragma scheduling off
#pragma peephole off
#pragma optimization_level 2
s32 fn_801CBAB8(void)
{
    s32 state;
    s32 input;
    s32 done;
    s32 result;

    done = state = result = 0;
    while (done == 0) {
        switch (state) {
        case 0:
            winMsgOpen(2, 0x3C46, 1, 1);
            input = (s8)fn_8001E184();
            winMsgClose(1);
            if (input != 0) {
                done = 1;
            } else {
                state = 2;
            }
            break;
        case 2:
            if (fn_800889E4(1) == 0) {
                state = 3;
                result = 1;
            } else {
                state = 4;
            }
            break;
        case 3:
            fn_800FF58C(0x395);
            state = 4;
            break;
        case 4:
            done = 1;
            break;
        }
    }

#pragma scheduling on
    return result;
}
#pragma pop
