/** Exact menuFight menuFightOpenPokemon and menuFightCloseWaza, 0x800114A4 - 0x800117BC. */
#include "dolphin/types.h"

extern void* fightTypeDataBiosGetPtr();
extern u8 fightTypeDataBiosGetFightoutPokemonNum();
extern s32 menuPokemonOpenFight(u8, u32, u32, u32);
extern void menuCloseCustom();
extern void* fightTargetGetPtr();
extern u8 fightOutPokemonCheckFightOut();
extern u32 fightMenuGetFightOutPokemonPtrToStatusMenuId();
extern u32 menuPokemonCheckPokemonChange();
extern s32 menuOpenCustom(s32, ...);
extern s32 menuFightOpenTarget(u8* ctx, s32 arg1, s32 arg2);
extern u32 menuIsCheck(u32 a);
extern void winSeqSetMenu(void* ctx, s32 state);

static inline u32 menuFightTargetMenuId(s32 target, u32 a1, u32 a2) {
    void* ptr = fightTargetGetPtr(target, a1, a2);

    if (fightOutPokemonCheckFightOut(ptr) == 1) {
        return fightMenuGetFightOutPokemonPtrToStatusMenuId(ptr, a2, 0);
    }
    return 0;
}

s32 menuFightOpenPokemon(u32 a0, u32 a1, u32 a2, u32 a3, u8 a4) {
    u8 num;
    s32 sel;
    s32 target;
    u32 buf[9];

    num = fightTypeDataBiosGetFightoutPokemonNum(fightTypeDataBiosGetPtr(a2));
    if (a4 == 0) {
        sel = menuPokemonOpenFight(num, a3, a0, a1);
    } else {
        do {
            do {
                winSeqSetMenu((void*)0xF8, 0x1E);
                sel = menuOpenCustom(0xF8, 0, 0, 0, 1, 3, a0, a1, a3 & 0xFF);
                winSeqSetMenu((void*)0xF8, 0x20);
                if (sel == -1) {
                    menuCloseCustom(0xF8, 0, 1);
                    return -1;
                }
            } while ((u8)menuPokemonCheckPokemonChange(a1, a0, sel) == 0);
            if ((u8)a3 == 0 || num < 2) {
                break;
            }
            buf[1] = menuFightTargetMenuId(0xF, a1, a2);
            buf[3] = menuFightTargetMenuId(0x10, a1, a2);
            buf[5] = menuFightTargetMenuId(0xE, a1, a2);
            buf[7] = 0;
            ((u8*)buf)[0x21] = a4;
            target = menuFightOpenTarget((u8*)buf, 0, 1);
            if ((u8)menuIsCheck(0xFF) != 0) {
                menuCloseCustom(0xFF, 0, 1);
            }
            if ((u8)menuIsCheck(0x104) != 0) {
                menuCloseCustom(0x104, 0, 1);
            }
            menuIsCheck(0x100);
        } while (target == -1);
        menuCloseCustom(0xF8, 0, 1);
    }
    return sel;
}

s32 menuFightCloseWaza(s32 arg) {
    if ((u8)menuIsCheck(0x4c) != 0) menuCloseCustom(0x4c, 0, arg);
    if ((u8)menuIsCheck(0xf9) != 0) menuCloseCustom(0xf9, 0, arg);
    if ((u8)menuIsCheck(0xfa) != 0) menuCloseCustom(0xfa, 0, arg);
    if ((u8)menuIsCheck(0xf7) != 0) menuCloseCustom(0xf7, 0, arg);
    return 0;
}
