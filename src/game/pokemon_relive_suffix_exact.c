#include "dolphin/types.h"

extern u32 lbl_8047B668;
extern void preReliveMain(void);

void preReliveInit(void)
{
    u32* p = &lbl_8047B668;
    p[0] = 0xFFFFFFFF;
    p[1] = 0xFFFFFFFF;
}

void preReliveSetParameter(int r3, u32 r4)
{
    u32* p = &lbl_8047B668;
    p[0] = r3;
    p[1] = r4;
    if ((r3 == 0) || (r3 == 1)) {
        preReliveMain();
    }
}
