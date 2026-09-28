/**
 * @file floor_event_exact_80115E6C.c
 * @brief floorEventGetTresure, 0x80115E6C - 0x80116164.
 *
 * Function-boundary carve of the floor-event TU (see floor_event.c): give
 * the player the contents of a treasure (Poke dollars, an item, or a key
 * item with its explanation) and show the matching messages. No jump table
 * (the switches expand to compare trees), no pooled constant, no data.
 * GC/1.3 -O4,p with the TU's unit-wide -opt nopeephole, no pragmas.
 */
#include "dolphin/types.h"

extern void heroAddPokedoru(u8* ptr, u32 offset);
extern s32 heroItemAddItemDataId(u8* ptr, u32 arg2, u32 arg3, u32 arg4);
extern void fn_801653CC(u32, u32, u32);
extern void msgctrlSetValue(u32, u32);
extern void winMsgOpen(u32, u32, u32, u32);
extern void winMsgClose(u32);
extern u16 pcboxDelItem(u8*, u16, u16);
extern s8 fn_8001E184(void);

s32 floorEventGetTresure(u8 type, u32 item, s32 count)
{
    /* Retail leaves result unset on the Poke-dollar path and on unknown
     * types: those paths return r31 as found (no initialising
     * instruction in the target). */
    s32 result;
    u32 message = 0;

    fn_801653CC(0x3CA, 0, 0xFF);

    switch (type) {
    case 2:
        msgctrlSetValue(0x4B, item);
        winMsgOpen(3, 0x3CB5, 1, 0);
        winMsgClose(1);
        heroAddPokedoru(0, item);
        winMsgOpen(3, 0x3CB7, 1, 0);
        winMsgClose(1);
        break;

    case 1:
    case 4:
        if (count <= 0) {
            break;
        }

        msgctrlSetValue(0x2D, item);
        msgctrlSetValue(0x2F, count);
        if (type == 1) {
            if (count == 1) {
                message = 0x3CB4;
            } else {
                message = 0x3CB9;
            }
            winMsgOpen(3, message, 1, 0);
            winMsgClose(1);
        }

        result = heroItemAddItemDataId(0, (u16)item, (u16)count, -1);
        if (result == 0) {
            if (count == 1) {
                message = 0x3CB8;
            } else {
                message = 0x3CBD;
            }
        } else if (result > 0) {
            result = pcboxDelItem(0, (u16)item, (u16)result);
            if (count == 1) {
                message = 0x3CBA;
            } else {
                message = 0x3CBB;
            }
        }
        winMsgOpen(3, message, 1, 0);
        winMsgClose(1);
        break;

    case 3:
        msgctrlSetValue(0x2D, item);
        winMsgOpen(3, 0x3CBC, 1, 0);
        winMsgClose(1);
        heroItemAddItemDataId(0, (u16)item, 1, -1);
        winMsgOpen(3, 0x3CB6, 1, 0);
        winMsgClose(1);

        switch (item) {
        case 0x21A:
            message = 0x3B33;
            break;
        case 0x21D:
            message = 0x3B35;
            break;
        case 0x21B:
            message = 0x3B39;
            break;
        case 0x21C:
            message = 0x3B37;
            break;
        case 0x223:
            message = 0x44C4;
            break;
        }

        winMsgOpen(3, message, 1, 0);
        result = fn_8001E184();
        winMsgClose(1);
        if (result != 0) {
            return 0;
        }

        switch (item) {
        case 0x21A:
            message = 0x3B34;
            break;
        case 0x21D:
            message = 0x3B36;
            break;
        case 0x21B:
            message = 0x3B30;
            break;
        case 0x21C:
            message = 0x3B38;
            break;
        case 0x223:
            message = 0x44C5;
            break;
        }

        winMsgOpen(3, message, 1, 0);
        winMsgClose(1);
        break;
    }

    return result;
}
