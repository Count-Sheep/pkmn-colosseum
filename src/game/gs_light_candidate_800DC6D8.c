/*
 * GS light animation step, 0x800DC6D8 - 0x800DC874 (XD _lightPreUpdate
 * area). Advances each active light's animation frame and applies its
 * end-of-range mode. The unit owns the (f32)delta conversion's unsigned
 * 0x4330 bias at 0x8047CA80, which only this function uses.
 */
#include "dolphin/types.h"

extern u32 lbl_8047AAEC; /* light array */
extern u32 lbl_8047AAF0; /* light count */
extern f32 lbl_8047CA70;
extern f32 lbl_8047CA74;
extern f32 lbl_8047CA78;
extern void HSD_LObjReqAnimAll(void* lobj, f32 frame);
extern void HSD_LObjAnimAll(void* lobj);

void fn_800DC6D8(u32 delta) {
    u32 offset;
    u32 i;
    u8* obj;
    s32 animType;
    f32 step;
    f32 limit;
    f32 end;

    offset = 0;
    for (i = 0; i < lbl_8047AAF0; offset += 0x74, i++) {
        obj = (u8*)lbl_8047AAEC + offset;
        if (obj[0] != 1 || obj[3] != 1) {
            continue;
        }

        HSD_LObjReqAnimAll(*(void**)(obj + 0xc), *(f32*)(obj + 0x68));
        HSD_LObjAnimAll(*(void**)(obj + 0xc));

        step = *(f32*)(obj + 0x64) * (f32)delta;
        limit = *(f32*)(obj + 0x6c) - lbl_8047CA70;
        animType = *(s32*)(obj + 0x5c);
        if ((s8)obj[0x71] == -1) {
            *(f32*)(obj + 0x68) -= step;
        } else if ((s8)obj[0x71] == 1) {
            *(f32*)(obj + 0x68) += step;
        }

        switch (animType) {
            case 0:
                if (*(f32*)(obj + 0x68) >= (end = limit - lbl_8047CA74)) {
                    obj[0x70] = 1;
                    obj[0x71] = 0;
                    *(f32*)(obj + 0x68) = end;
                }
                break;
            case 1:
                if (*(f32*)(obj + 0x68) >= limit) {
                    *(f32*)(obj + 0x68) -= limit;
                }
                break;
            case 2:
                if (*(f32*)(obj + 0x68) >= limit) {
                    *(s8*)(obj + 0x71) = -1;
                } else if (*(f32*)(obj + 0x68) <= lbl_8047CA78) {
                    obj[0x71] = 1;
                }
                break;
        }
    }
}
