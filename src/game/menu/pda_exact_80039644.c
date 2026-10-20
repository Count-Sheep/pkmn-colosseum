/* Standalone copy of fn_80039644 from pda_range_80037158.c, linked on its own. */
#include "dolphin/types.h"

typedef struct PdaSprite PdaSprite;

extern s32 lbl_8047A4A8;
extern s32 lbl_8047A4BC;
extern f32 lbl_8047A4C0;
extern f32 lbl_8047BAB0;

extern void* pcboxGetItem(s32 box, s16 slot);
extern u16 pcboxGetNbItemSlot(s32 box);
extern u8 fn_801429E8(void* item);
extern s32 itemBiosGetItemDataId(void* item);
extern s32 itemBiosGetNum(void* item);
extern void* itemDataBiosGetPtr(s32 id);
extern void* itemDataBiosGetName(void* data);
extern u8 itemDataBiosGetKind(void* data);
extern void msgctrlSetValue(s32 id, u32 value);
extern u32 GSmsgGetRect(s32 id);
extern void fn_800FB680(s32 arg0, s32 arg1, s32 arg2, void* data);

static inline s32 pda_80039644_Slots(void)
{
    return pcboxGetNbItemSlot(0);
}

/* The data id of the target-th listed item in PC box 0. */
static inline s32 pda_80039644_ItemId(s32 target)
{
    s32 slots;
    void* item;
    s32 i;
    s32 found;

    slots = pda_80039644_Slots();
    found = -1;
    for (i = 0; i < slots; i++) {
        item = pcboxGetItem(0, (s16)i);
        if ((u8)fn_801429E8(item) != 0) {
            found++;
            if (found >= target) {
                return itemBiosGetItemDataId(item);
            }
        }
    }
    return 0;
}

/* The count of the target-th listed item in PC box 0. */
static inline s32 pda_80039644_ItemNum(s32 target)
{
    void* item;
    s32 i;
    s32 found;
    s32 slots;

    slots = pda_80039644_Slots();
    found = -1;
    for (i = 0; i < slots; i++) {
        item = pcboxGetItem(0, (s16)i);
        if ((u8)fn_801429E8(item) != 0) {
            found++;
            if (found >= target) {
                return itemBiosGetNum(item);
            }
        }
    }
    return 0;
}

static inline s32 pda_80039644_Count(void)
{
    s32 slots;
    s32 i;
    s32 total;

    total = 0;
    slots = pda_80039644_Slots();
    for (i = 0; i < slots; i++) {
        if ((u8)fn_801429E8(pcboxGetItem(0, (s16)i)) != 0) {
            total++;
        }
    }
    return total;
}

s32 fn_80039644(void* work, PdaSprite* sprite)
{
    s32 start;
    s32 rowY;
    s32 slots;
    s32 j;
    s32 found;
    s32 total;
    s32 nameX;
    s32 y;
    s32 i;
    s32 width;
    s32 itemId;
    u16 count;
    void* item;
    void* name;
    s32 index;
    s32 rows;
    s32 yAdj;
    s16 w2;

    (void)work;
    start = 0;
    rows = 8;
    yAdj = 0;
    total = 0;
    total = pda_80039644_Count();
    index = lbl_8047A4A8;
    msgctrlSetValue(0x34, 0x3e7);
    width = (s16)(GSmsgGetRect(0xca) >> 16);
    width += (s16)(GSmsgGetRect(0x12e) >> 16);
    if (lbl_8047BAB0 != lbl_8047A4C0 && lbl_8047A4BC != 0) {
        if (lbl_8047A4C0 < lbl_8047BAB0) {
            index--;
            start = -1;
        } else {
            rows = 9;
        }
        yAdj = (s32)lbl_8047A4C0;
    }
    rowY = start * 0x1f;
    i = start;
    nameX = 0x11a - width;
    for (; i < rows && index < total; rowY += 0x1f, i++, index++) {
        if (index < 0) {
            continue;
        }
        y = rowY - yAdj;
        itemId = pda_80039644_ItemId(index);
        name = itemDataBiosGetName(itemDataBiosGetPtr((u16)itemId));
        if (name != NULL) {
            fn_800FB680(0, y, (s32)sprite, name);
        }
        if (itemDataBiosGetKind(itemDataBiosGetPtr(itemId)) == 5) {
            continue;
        }
        fn_800FB680(nameX, y, (s32)sprite, (void*)0x12e);
        count = pda_80039644_ItemNum(index);
        msgctrlSetValue(0x34, count);
        w2 = GSmsgGetRect(0xca) >> 16;
        fn_800FB680(0x11a - w2, y, (s32)sprite, (void*)0xca);
    }
    if (i < rows) {
        fn_800FB680(0, i * 0x1f - yAdj, (s32)sprite, (void*)0x1b67);
    }
    return 0;
}
