/**
 * @file floor_event_exact_80115CB4.c
 * @brief floorEventGetTresureList, floorEventSetTresureDisp and
 *        floorEventChangeTresure, 0x80115CB4 - 0x80115E6C.
 *
 * Function-boundary carve of the floor-event TU (see floor_event.c): find
 * the n-th treasure placed on the current floor, show or hide a treasure by
 * its raw table index, and rewrite a treasure's contents. No jump table, no
 * pooled constant, no data of its own. GC/1.3 -O4,p with the TU's unit-wide
 * -opt nopeephole, no pragmas.
 */
#include "dolphin/types.h"

extern void GSlogWrite(const char* fmt, ...);
extern void* fn_800FF56C(void);
extern void* fn_80113F48(void);
extern void fn_8018C1E8(void*, u32, u32);

extern u32 lbl_80478EBC; /* treasure table */
extern u32 lbl_80478EB8; /* treasure count */

/* RULE-EXCEPTION(user-approved): extern named stand-ins for the TU's own
 * strings -- see docs/RULE_EXCEPTIONS.md. The format (.rodata, also read by
 * floor_character) and the function name (.data) stay with the unlinked
 * rest of the TU. */
extern u8 lbl_80272708[];
extern u8 lbl_8035BB70[];

/* 0x80115CB4 | 0xB0 */
void* floorEventGetTresureList(u32 param)
{
    u32 type;
    u8* entry;
    u32 index;
    u32 found;
    u32 target;

    entry = 0;
    found = 0;
    type = param & 0x7FFF0000;
    if (type != 0x7FFF0000) {
        return 0;
    }

    target = param & 0x1FF;
    for (index = 0; index < *(u32*)lbl_80478EB8; index++) {
        entry = (u8*)lbl_80478EBC + index * 0x1C;
        if (*(u16*)(entry + 4) == (u32)fn_800FF56C()) {
            if (target == found++) {
                break;
            }
        }
    }

    if (index == *(u32*)lbl_80478EB8) {
        return 0;
    }
    return entry;
}

/* 0x80115D64 | 0xA0 */
void floorEventSetTresureDisp(u32 rawIndex, u32 display)
{
    u8* entry;
    u32 index;
    u32 found = 0;
    u32 encoded;

    for (index = 0; index < *(u32*)lbl_80478EB8; index++) {
        entry = (u8*)lbl_80478EBC + index * 0x1C;
        if (*(u16*)(entry + 4) == (u32)fn_800FF56C()) {
            if (index == rawIndex) {
                encoded = found | 0x7FFF0000;
                break;
            }
            found++;
        }
    }

    if (index != *(u32*)lbl_80478EB8) {
        fn_8018C1E8(fn_80113F48(), encoded, display);
    }
}

/* 0x80115E04 | 0x68 */
s32 floorEventChangeTresure(u32 index, u16 val, u8 byte)
{
    u32 count;
    u8* entry;

    count = *(u32*)lbl_80478EB8;
    if (index >= count) {
        GSlogWrite((const char*)lbl_80272708, lbl_8035BB70);
        return -1;
    }
    entry = (u8*)lbl_80478EBC + index * 0x1c;
    *(u32*)(entry + 0xc) = val;
    *(u8*)(entry + 0x1) = byte;
    return 0;
}
