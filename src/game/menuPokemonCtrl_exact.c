/**
 * @file menuPokemonCtrl_exact.c
 * @brief Drive the party-menu entry and exit animations.
 */
#include "dolphin/types.h"

extern u8 lbl_803A1D40[];
extern u8 lbl_802E4E58[];
extern u32 lbl_8047A308;
extern s32 windowGetParam(s32 context, s32 index);
extern void menuItemBiosSetXY(s16 x, s16 y, s16 z);
extern void menuDataBiosSetXY(s16 x, s16 y, s16 z);
extern void menuDataBiosGetXY(s16 id, u16* x, u16* y);
extern void winSeqSetMenu(void* context, s32 state);
extern s32 menuOpenCustom(s32, s32, s32, s32, s32, s32, void*, ...);

s32 menuPokemonCtrl(s32 context)
{
    u32 result;
    s32 byte_offset;
    u8* iterator;
    s32 i;

    result = windowGetParam(context, 0);
    if (result == 0) {
        return 0;
    }

    if ((s8)*((u8*)context + 1) == 0) {
        *((s8*)context + 0x97) = -1;
        if ((s8)*((u8*)context + 2) == 0) {
            i = 0;
            byte_offset = 0;
            iterator = (u8*)result;
            for (; i < 6; i++) {
                s16 y;
                s16 x;
                s16 id;
                u8* slot;
                s32 state;

                slot = lbl_802E4E58 + (s32)(s8)lbl_803A1D40[4] * 0x30 +
                       byte_offset;
                menuItemBiosSetXY(*(s16*)(slot + 2), *(s16*)(slot + 4),
                                  *(s16*)(slot + 6));
                slot = lbl_802E4E58 + (s32)(s8)lbl_803A1D40[4] * 0x30 +
                       byte_offset;
                menuDataBiosSetXY(*(s16*)(slot + 0), *(s16*)(slot + 4),
                                  *(s16*)(slot + 6));

                slot = lbl_802E4E58 + (s32)(s8)lbl_803A1D40[4] * 0x30;
                menuOpenCustom((s32)*(s16*)(slot + byte_offset), 0x63, 0, 0,
                               0, 2, (void*)iterator, i);

                slot = lbl_802E4E58 + (s32)(s8)lbl_803A1D40[4] * 0x30;
                id = *(s16*)(slot + byte_offset);
                menuDataBiosGetXY(id, (u16*)&x, (u16*)&y);
                if (x > 0xFA) {
                    state = 0x116;
                } else {
                    state = 0x11E;
                }
                winSeqSetMenu((void*)(s32)id, state);

                byte_offset += 8;
                iterator += 0x30;
            }
            winSeqSetMenu(*(void**)((u8*)context + 4), 1);
            *((s8*)context + 2) = 1;
        }
    } else if ((s8)*((u8*)context + 1) == 3) {
        if ((s8)*((u8*)context + 2) == 0) {
            byte_offset = 0;
            for (i = byte_offset; byte_offset < 6; byte_offset++) {
                s16 y;
                s16 x;
                s16 id;
                u8* slot;
                s32 state;

                slot = lbl_802E4E58 + (s32)(s8)lbl_803A1D40[4] * 0x30;
                id = *(s16*)(slot + i);
                menuDataBiosGetXY(id, (u16*)&x, (u16*)&y);
                if (x > 0xFA) {
                    state = 0x11A;
                } else {
                    state = 0x122;
                }
                winSeqSetMenu((void*)(s32)id, state);

                i += 8;
            }
            winSeqSetMenu(*(void**)((u8*)context + 4), 7);
            *((s8*)context + 2) = 1;
        }
    }
    *(s16*)&lbl_8047A308 =
        (s16)(((s32)*(s16*)&lbl_8047A308 + 1) % 1000);
    return 0;
}
