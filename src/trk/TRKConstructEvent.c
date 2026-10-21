#include "dolphin/types.h"

/* NAME-T1 fn_800BE464 */
void TRKConstructEvent(void* event, s32 type) {
    ((s32*)event)[0] = type;
    ((s32*)event)[1] = 0;
    ((s32*)event)[2] = -1;
}
