/** Exact save/card-state accessors, 0x801E1170 - 0x801E11F0. */
#include "dolphin/types.h"

void fn_801E1170(void)
{
    extern s32 lbl_8047B424;
    extern s32 lbl_8047B428;
    extern u32 lbl_8047B430;

    lbl_8047B424 = 4;
    lbl_8047B428 = 3;
    lbl_8047B430 = 0;
}

void fn_801E118C(void)
{
    extern s32 lbl_8047B424;
    extern s32 lbl_8047B428;

    lbl_8047B424 = 3;
    lbl_8047B428 = 3;
}

void fn_801E119C(void)
{
    extern s32 lbl_8047B424;
    extern s32 lbl_8047B428;

    lbl_8047B424 = 2;
    lbl_8047B428 = 3;
}

void fn_801E11B0(void)
{
    extern s32 lbl_8047B424;
    extern s32 lbl_8047B428;

    lbl_8047B424 = 1;
    if (lbl_8047B428 == 2) {
        return;
    }
    lbl_8047B428 = 1;
}

/* NAME-T1 fn_801E11CC */
u8 GSvtrGetInfoDisp(void)
{
    extern u8 lbl_8047B434;

    return lbl_8047B434;
}

/* NAME-T1 fn_801E11D4 */
void GSvtrSetInfoDisp(u8 index, u8 active)
{
    extern u8 lbl_8047B434;
    extern u8 lbl_8047B435;

    lbl_8047B434 = index;
    lbl_8047B435 = active;
}

s32 fn_801E11E0(void)
{
    extern s32 lbl_8047B424;

    return lbl_8047B424;
}

/* NAME-T1 fn_801E11E8 */
u8 GSvtrIsActive(void)
{
    extern u8 lbl_8047B420;

    return lbl_8047B420;
}
