/** Exact menuFight menuFightOpenTarget, 0x80011288 - 0x800114A4. */
#include "dolphin/types.h"

extern s32 menuOpenCustom(s32, ...);

s32 menuFightOpenTarget(u8* ctx, s32 arg1, s32 arg2) {
    extern void menuItemBiosSetSelectFlag(s32, s32);
    extern s32 menuGetCursorFromItemID(s32, s32);
    s32 cursor;
    u8* entry;
    s32 i;
    s32 item;
    s32 id;
    s32 result;

    if (ctx[0x21] == 0) {
        menuItemBiosSetSelectFlag(0x1258, 0);
        menuItemBiosSetSelectFlag(0x1259, 0);
        menuItemBiosSetSelectFlag(0x125A, 0);
        menuItemBiosSetSelectFlag(0x125B, 0);
        entry = ctx;
        for (i = 0; i < 4; i++) {
            switch (*(s32*)(entry + 4)) {
            case 0x45:
                item = 0x125B;
                break;
            case 0x46:
                item = 0x125A;
                break;
            case 0x47:
                item = 0x1258;
                break;
            case 0x48:
                item = 0x1259;
                break;
            default:
                item = 0;
                break;
            }
            if (item != 0) {
                menuItemBiosSetSelectFlag(item, 1);
            }
            entry += 8;
        }
        cursor = menuGetCursorFromItemID(0xFF, 0x125A);
        if (cursor == -1) {
            cursor = menuGetCursorFromItemID(0xFF, 0x125B);
            if (cursor == -1) {
                cursor = 0;
            }
        }
        switch (menuOpenCustom(0xFF, 0, &cursor, 0, arg2, 1, ctx)) {
        case 0x1258:
            id = 0x47;
            break;
        case 0x1259:
            id = 0x48;
            break;
        case 0x125A:
            id = 0x46;
            break;
        case 0x125B:
            id = 0x45;
            break;
        default:
            id = -1;
            break;
        }
        result = -1;
        for (i = 0; i < 4; i++) {
            if (id == *(s32*)(ctx + 4 + i * 8)) {
                result = i;
                break;
            }
        }
    } else {
        result = menuOpenCustom(0x104, 0, arg1, 0, arg2, 1, ctx);
    }
    return result;
}
