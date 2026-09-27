/**
 * @file window_exact_80104318.c
 * @brief windowGetCursorToItem, windowGetValue, fn_801044D0
 *        (0x80104318 - 0x80104530).
 *
 * Flags: the window TU is built with the project's -O4,p but with the
 * peephole pass off, like the winMsg TU that follows it (see
 * win_msg_exact_80105A3C.c). Retail keeps "clrlwi r0,rX,24; cmplwi r0,0"
 * pairs and uninverted "bne; b" branches. With "-opt nopeephole" and no
 * local pragmas, windowGetCursorToItem, fn_801044D0, windowCloseMain and
 * windowInit (all previously exact only under local optimization_level /
 * peephole pragmas) are exact, and so are windowGetValue and
 * windowCheckCursor. Carved at function boundaries so these can link
 * while the rest of the window TU stays a candidate; data stays extern.
 */
#include "dolphin/types.h"
#include "game/window_search.h"

extern void* menuDataBiosGetPtr(void* key);
extern void* menuItemBiosGetPtr(s16 index);
extern s32 menuGetCursorItemID(s32 windowId);

/* Menu item record from the menu item BIOS table. */
typedef struct WinMenuItem {
    u8 selectable : 1;
    u8 last : 1;
    u8 flags : 6;
    u8 pad_01[0x17];
    s16 next;
} WinMenuItem;

/* 0x80104318 | 0x8C */
WinMenuItem* windowGetCursorToItem(u8* window)
{
    WinMenuItem* item;
    s32 index;

    item = menuDataBiosGetPtr(*(void**)(window + 0x04));
    item = menuItemBiosGetPtr(*(s16*)((u8*)item + 0x04));
    index = 0;
    while (1) {
        if (item->selectable) {
            if ((s8)window[0x95] == index) {
                return item;
            }
            index++;
        }
        if (item->last) {
            break;
        }
        item = menuItemBiosGetPtr(item->next);
    }
    return NULL;
}

/* 0x801043A4 | 0x12C */
s32 windowGetValue(s32 id)
{
    u8* window = windowSearchIDInline(id);
    u8* menuData;
    s32 result;

    if (window != NULL) {
        if (window[0x99] != 0) {
            result = -1;
        } else {
            menuData = menuDataBiosGetPtr(*(void**)(window + 0x04));
            if (menuData != NULL) {
                /* The value kind is a 2-bit field, so every case is
                 * covered; retail has no default store either. */
                switch ((menuData[0] >> 6) & 3) {
                case 0:
                    result = 0;
                    break;
                case 1:
                    result = (s8)window[0x94] + (s8)window[0x95];
                    break;
                case 2:
                    result = *(s32*)(window + 0x80);
                    break;
                case 3:
                    result = menuGetCursorItemID(id);
                    break;
                }
            } else {
                result = (s8)window[0x94] + (s8)window[0x95];
            }
        }
    } else {
        result = -1;
    }
    return result;
}

/* 0x801044D0 | 0x60 */
s32 fn_801044D0(s32 id, u16* cursor)
{
    u8* window = windowSearchIDInline(id);

    if (window != NULL) {
        *(u16*)(window + 0x94) = *cursor;
        return 1;
    }
    return 0;
}
