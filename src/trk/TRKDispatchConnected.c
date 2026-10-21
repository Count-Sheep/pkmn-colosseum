#include "dolphin/types.h"

extern u32 lbl_803FE7D0[];

/* NAME-T1 fn_800C04E8 */
void SetTRKConnected(u32 state) {
    lbl_803FE7D0[0] = state;
}

/* NAME-T1 fn_800C04F4 */
u32 GetTRKConnected(void) {
    return lbl_803FE7D0[0];
}
