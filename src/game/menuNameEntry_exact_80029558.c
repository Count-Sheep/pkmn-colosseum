/** Exact name-entry call/open suffix, 0x80029558 - 0x80029850. */
#include "dolphin/types.h"

extern void* heroGetStatus(s32, s32, u32);
extern u8 pokemonCheckValid(void);
extern void menuNameEntry(void);
extern void GScharCpy(void*, u8*);
extern void fn_800FF730(s32);
extern void floorSetFadeScript(s32, u32);
extern void _threadSwitch(void);
extern u8 lbl_803A2068[];

#pragma optimization_level 4
#pragma peephole off
s32 menuNameEntryCall(s32 r3, s32 r4) {
    s32 r29;
    s32 r30;
    s32 r31;
    u8* ctx;
    r29 = r3;
    r30 = r4;
    r31 = 1;
    if (r29 == 2) goto _558_eq2;
    if (r29 >= 2) goto _558_ge3;
    if (r29 >= 0) goto _558_done;
    goto _558_fail;
    _558_ge3:
    if (r29 >= 4) goto _558_fail;
    goto _558_r30;
    _558_eq2:
    heroGetStatus(0, 3, (u16)r30);
    if ((u8)pokemonCheckValid() == 0) r31 = 0;
    goto _558_done;
    _558_r30:
    if (r30 < 0) goto _558_r30_fail;
    if (r30 < 3) goto _558_done;
    _558_r30_fail:
    r31 = 0;
    goto _558_done;
    _558_fail:
    r31 = 0;
    _558_done:;
    if (r31 == 0) return 0;
    ctx = lbl_803A2068;
    *(u16*)ctx = 0;
    *(s32*)(ctx + 0x18) = r29;
    *(s32*)(ctx + 0x1c) = r30;
    *(s32*)(ctx + 0x20) = 0;
    *(s32*)(ctx + 0x28) = 0;
    menuNameEntry();
    return *(s32*)(ctx + 0x20);
}
#pragma peephole on

#pragma scheduling off
#pragma optimization_level 4
void menuNameEntryGetLastName(void* r3) {
    GScharCpy(r3, lbl_803A2068);
}
#pragma scheduling on

#pragma optimization_level 4
#pragma peephole off
s32 menuNameEntryOpenNoFade(s32 r3, s32 r4) {
    s32 r29;
    s32 r30;
    s32 r31;
    u8* ctx;
    r29 = r3;
    r30 = r4;
    r31 = 1;
    if (r29 == 2) goto _660_eq2;
    if (r29 >= 2) goto _660_ge3;
    if (r29 >= 0) goto _660_done;
    goto _660_fail;
    _660_ge3:
    if (r29 >= 4) goto _660_fail;
    goto _660_r30;
    _660_eq2:
    heroGetStatus(0, 3, (u16)r30);
    if ((u8)pokemonCheckValid() == 0) r31 = 0;
    goto _660_done;
    _660_r30:
    if (r30 < 0) goto _660_r30_fail;
    if (r30 < 3) goto _660_done;
    _660_r30_fail:
    r31 = 0;
    goto _660_done;
    _660_fail:
    r31 = 0;
    _660_done:;
    if (r31 == 0) return 0;
    ctx = lbl_803A2068;
    *(u16*)ctx = 0;
    *(s32*)(ctx + 0x18) = r29;
    *(s32*)(ctx + 0x1c) = r30;
    *(s32*)(ctx + 0x20) = 0;
    *(s32*)(ctx + 0x24) = 0;
    *(s32*)(ctx + 0x28) = 1;
    fn_800FF730(0x390);
    floorSetFadeScript(0, 0x05960008);
    _threadSwitch();
    return *(s32*)(ctx + 0x20);
}

#pragma optimization_level 4
#pragma peephole off
s32 menuNameEntryOpen(s32 r3, s32 r4) {
    s32 r29;
    s32 r30;
    s32 r31;
    u8* ctx;
    r29 = r3;
    r30 = r4;
    r31 = 1;
    if (r29 == 2) goto _760_eq2;
    if (r29 >= 2) goto _760_ge3;
    if (r29 >= 0) goto _760_done;
    goto _760_fail;
    _760_ge3:
    if (r29 >= 4) goto _760_fail;
    goto _760_r30;
    _760_eq2:
    heroGetStatus(0, 3, (u16)r30);
    if ((u8)pokemonCheckValid() == 0) r31 = 0;
    goto _760_done;
    _760_r30:
    if (r30 < 0) goto _760_r30_fail;
    if (r30 < 3) goto _760_done;
    _760_r30_fail:
    r31 = 0;
    goto _760_done;
    _760_fail:
    r31 = 0;
    _760_done:;
    if (r31 == 0) return 0;
    ctx = lbl_803A2068;
    *(u16*)ctx = 0;
    *(s32*)(ctx + 0x18) = r29;
    *(s32*)(ctx + 0x1c) = r30;
    *(s32*)(ctx + 0x20) = 0;
    *(s32*)(ctx + 0x24) = 1;
    *(s32*)(ctx + 0x28) = 1;
    fn_800FF730(0x390);
    _threadSwitch();
    return *(s32*)(ctx + 0x20);
}
