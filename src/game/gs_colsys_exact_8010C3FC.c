/**
 * @file gs_colsys_exact_8010C3FC.c
 * @brief Byte-exact GScolsys2 range, 0x8010C3FC - 0x8010C46C.
 */
#include "dolphin/types.h"

extern void* lbl_80478E70;
extern void* lbl_80478E74;
extern char lbl_80272000[];
extern char lbl_8035B4E8[];
extern void GSlogWrite(char*, char*, ...);

#pragma push
#pragma peephole off
void* _menuFaceBiosGetPtr__FUs(u16 idx)
{
    u8* entry;
    u16 i = idx;

    if (i >= *(u32*)lbl_80478E70) {
        GSlogWrite(lbl_80272000, lbl_8035B4E8);
        entry = NULL;
    } else {
        entry = (u8*)lbl_80478E74 + i * 8;
    }
    if (entry == NULL) {
        return NULL;
    }
    return *(void**)(entry + 4);
}
#pragma pop
