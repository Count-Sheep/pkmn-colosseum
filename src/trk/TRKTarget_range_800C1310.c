#include "dolphin/types.h"

extern u8 gTRKState[]; /* large state structure */

/* TRKTargetStop - 0x800C1310 | size: 0x18 */
/* NAME-T1 fn_800C1310 */
s32 TRKTargetStop(void) {
    *(s32*)&gTRKState[0x98] = 1;
    return 0;
}
