/** Exact e-Reader menu callbacks, 0x80075A34 - 0x80075DC8. */
#include "dolphin/types.h"

extern u32 fn_80113F48(void);
extern u32 fn_801CBA0C(u32 resourceId);
extern void GSresGetResource(u32 archive, u32 resource);
extern void cameraPlayAnime(s32 cameraId, u32 animationId, s32 frame, s32 loop);
extern void GSscene_SetMode(u32 mode);
extern s32 fn_80190528(s32);
extern u8 fn_801902E0(s32);
extern u32 fn_801906A0(s32);
extern void _flagSet(s32, u32);
extern s32 fadeCheck(s32);
extern s32 menuClose(s32);
extern void floorLink(s32, s32);
extern void msgctrlSetValue(s32 id, u32 value);
extern s32 menuOpenCustom(s32 slot, ...);
extern s32 fn_801D0748(u32, u32, u32);
extern void* gamedatasaveGetStatus(u32, u32);
extern void winMsgOpen(s32, s32, s32, s32);
extern void winMsgClose(s32);
extern u8 lbl_8047A5D0;

void fn_80075A34(void)
{
    u32 archive;

    archive = fn_80113F48();
    GSresGetResource(archive,
                     *(u32*)&lbl_8047A5D0 = fn_801CBA0C(0x10801000));
    cameraPlayAnime(0x5E0, 0x10821800, 0, 1);
    GSscene_SetMode(4);
}

s32 fn_80075A9C(void) { return fn_80190528(0xab5); }
u8 fn_80075AC0(void) { return fn_801902E0(0xab5); }
s32 fn_80075AE4(void) { return fn_80190528(0xab4); }
u8 fn_80075B08(void) { return fn_801902E0(0xab4); }
s32 fn_80075B2C(void) { return fn_80190528(0xab3); }
u8 fn_80075B50(void) { return fn_801902E0(0xab3); }

s32 fn_80075B74(void)
{
    s32 result;
    u32 value;

    value = fn_801906A0(0xab2) + 1;
    result = 1;
    if (value > 0x30) {
        value = 0x30;
        result = 0;
    }
    _flagSet(0xab2, value);
    return result;
}

s32 fn_80075BC4(void)
{
    u32 value;

    value = fn_801906A0(0xab2);
    if (value > 0x30) {
        return 0;
    } else {
        return 0x30 - value;
    }
}

s32 fn_80075BFC(void) { return fn_80190528(0xab1); }
u8 fn_80075C20(void) { return fn_801902E0(0xab1); }
u8 fn_80075C44(void) { return fn_801902E0(0xa14); }

s32 fn_80075C68(void)
{
    fadeCheck(1);
    return menuClose(0xe0);
}

void fn_80075C94(void)
{
    s32 result;

    for (;;) {
        msgctrlSetValue(0x37, 0);
        result = menuOpenCustom(0xE0, 0, 0, 0x10, 1, 0);
        fadeCheck(1);

        switch (result) {
        case 0:
            floorLink(0x322, 0);
            return;
        case 1:
            result = fn_801D0748(2, 2, 0);
            if (result != 3 || gamedatasaveGetStatus(0, 4) == 0) {
                if (result == -1) {
                    continue;
                }
                winMsgOpen(2, 0x44DB, 1, 0);
                winMsgClose(1);
                continue;
            }
            floorLink(0x323, 0);
            return;
        case -1:
            break;
        case 2:
            break;
        default:
            break;
        }

        floorLink(0x320, 0);
        return;
    }
}

void fn_80075D98(void)
{
}

s32 fn_80075D9C(void)
{
    fadeCheck(1);
    return menuClose(0xe2);
}
