/* Synchronous name-entry dialog, 0x800281F4-0x80028444. */
#include "dolphin/types.h"

typedef struct NAME_ENTRY_ARG {
    u16 buffer[12];
    u16* name;
    s32 kind;
    s32 index;
    s32* row;
    s32* letter;
    s32* column;
    f32* fade;
    s32* state;
    u32* work0;
    u32* work1;
    u32* work2;
    u32* work3;
    u32* work4;
} NAME_ENTRY_ARG;

extern void GScharCpy(void*, u8*);
extern void* GSmsgGetGSchar(u32);
extern void msgctrlSetValue(s32, void*);
extern u8 lbl_8047A3D4[4], lbl_8047A3D0[4], lbl_8047A3CC[4];
extern u8 lbl_8047A3C8[4], lbl_8047A3C4[4];
extern u32 lbl_8047A3C0, lbl_8047A3BC, lbl_8047A3B8;
extern u32 lbl_8047A3B4, lbl_8047A3B0;
extern u8 lbl_803A2068[];
extern const f32 lbl_8047B930;

s32 inputName__FPUsPUsiii(u16* name, u16* defaultName, s32 kind, s32 index, s32 canCancel)
{
    extern void dbgMenuSetEnable(s32 enable);
    extern u32 windowGetActiveID(void);
    extern void menuOpenCustom(s32 menuId, u32 parentId, s32, s32, s32, s32, ...);
    extern void fn_80166A28(u32 se);
    extern void winMsgOpen(s32 slot, s32 msgId, s32, s32);
    extern s8 menuSubOpenYesNo(s32, s32, s32, s32);
    extern void winMsgClose(s32 slot);
    extern void menuClose(s32 menuId);
    extern void menuCloseSync(s32 menuId, s32 sync);

    NAME_ENTRY_ARG arg;
    s32 blanks;
    u16* letters;
    s32 length;
    u16* result;
    s32 valid;
    u16* blank;
    s32 accepted;
    s32 done;
    s32 yes;
    NAME_ENTRY_ARG* argp;
    s32 answer;

    done = 0;
    GScharCpy(arg.buffer, (u8*)defaultName);
    name[0] = 0;
    arg.name = name;
    arg.kind = kind;
    arg.index = index;
    *(s32*)lbl_8047A3D4 = 0;
    arg.row = (s32*)lbl_8047A3D4;
    *(s32*)lbl_8047A3D0 = 0;
    arg.letter = (s32*)lbl_8047A3D0;
    *(s32*)lbl_8047A3CC = 0;
    arg.column = (s32*)lbl_8047A3CC;
    *(f32*)lbl_8047A3C8 = lbl_8047B930;
    arg.fade = (f32*)lbl_8047A3C8;
    *(s32*)lbl_8047A3C4 = 0;
    arg.state = (s32*)lbl_8047A3C4;
    arg.work0 = &lbl_8047A3C0;
    arg.work1 = &lbl_8047A3BC;
    arg.work2 = &lbl_8047A3B8;
    arg.work3 = &lbl_8047A3B4;
    arg.work4 = &lbl_8047A3B0;
    dbgMenuSetEnable(0);
    argp = &arg;
    while (!done) {
        menuOpenCustom(0x6e, windowGetActiveID(), 0, 0, 1, 1, argp);

        letters = arg.name;
        for (length = 0; letters[length] != 0; length++) {
        }
        if (length <= 0) {
            valid = 0;
        } else {
            blank = (u16*)GSmsgGetGSchar(0x2ef9);
            for (blanks = 0; blanks < length; blanks++, letters++) {
                if (*letters != *blank) {
                    break;
                }
            }
            if (blanks >= length) {
                valid = 0;
            } else {
                valid = 1;
            }
        }
        if (valid) {
            result = arg.name;
        } else {
            result = arg.buffer;
        }

        fn_80166A28(0x440);
        msgctrlSetValue(0x4d, result);
        winMsgOpen(2, 0x2ef6, 1, 0);
        answer = menuSubOpenYesNo(0, -1, -1, 0);
        winMsgClose(1);
        if (answer == 1 || answer == -1) {
            yes = 0;
        } else {
            yes = 1;
        }
        accepted = yes;
        if (canCancel == 0) {
            done = 1;
        } else if (yes) {
            done = 1;
        }
    }
    dbgMenuSetEnable(1);
    menuClose(0x6e);
    menuCloseSync(0x6e, 1);
    if (accepted) {
        GScharCpy(lbl_803A2068, (u8*)result);
        return 1;
    }
    return 0;
}
