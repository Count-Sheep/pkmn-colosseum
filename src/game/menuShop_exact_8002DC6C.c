/* Shop travel request, 0x8002DC6C-0x8002DD24. */
#include "dolphin/types.h"

/* lbl_8047A3FC: the 8-byte travel request in .sbss (sda-addressed under -sdata 8). */
typedef struct ShopTravelState {
    u32 location;
    u32 active;
} ShopTravelState;

extern ShopTravelState lbl_8047A3FC;
extern void mailMainReceiveTerminate(void);
extern s32 fn_800D37CC(void);
extern void menuCreateOffScreen(f32 duration);
extern void _flagSet(s32 flagId, u32 value);
extern void fn_800FF730(u32 floorId);
extern void floorSetFadeScript(u32 a, u32 b);
extern void _threadSwitch(void);
extern void menuReleaseOffScreen(f32 duration);
extern f32 lbl_8047B9CC;

void menuShopOpen(u32 flag)
{
    f32 duration;

    mailMainReceiveTerminate();
    duration = (f32)fn_800D37CC();
    duration = lbl_8047B9CC / duration;
    menuCreateOffScreen(duration);
    lbl_8047A3FC.location = flag;
    lbl_8047A3FC.active = 1;
    _flagSet(1, 2);
    fn_800FF730(0x38F);
    floorSetFadeScript(0, 0);
    _threadSwitch();
    duration = (f32)fn_800D37CC();
    duration = lbl_8047B9CC / duration;
    menuReleaseOffScreen(duration);
}
