/**
 * @file win_msg_exact_80105A3C.c
 * @brief winMsg TU from winMsgCtrl to winMsgOpen (0x80105A3C - 0x80106F98,
 *        with its jump tables at .data 0x8035B0F8 - 0x8035B3F0).
 *
 * The message window used by battle, field, level-up and error text:
 * winMsgCtrl is the window's control callback, the Open family selects a
 * message-window menu by type, closes the one that is up, and opens the
 * new one through menuOpenCustom; Close/Check act on the current one.
 *
 * Flags: the winMsg TU (0x801058CC - 0x80106F98) is -O4,p with the peephole
 * pass off, built with the GC/2.5 compiler.
 * - "-opt nopeephole": retail keeps "clrlwi r0,r3,24; cmplwi r0,0" instead
 *   of the record form, leaves "bne; b" pairs uninverted and keeps
 *   "mr rX,r3; mr r3,rX" result copies. Without it every function here
 *   except winMsgButton differs.
 * - GC/2.5 (2.6 and 2.7 give the same code): with GC/1.3 or 1.3.2 every
 *   function in this file is exact except winMsgOpenFightNoWait, where
 *   GC/1.3 schedules the flags truncation for r10 after the r7/r9 argument
 *   moves instead of before them.
 * With those two unit-wide flags and no local pragmas all 19 functions here
 * are exact, and so is winMsgDraw (0x801058CC) in win_msg.c, which stays a
 * separate object starting the TU; this object starts at winMsgCtrl.
 *
 * Reconstructed inlines (repeated expansions in the target):
 * - winMsgResolveMenuId: the type -> menu ID switch; one jump table per
 *   expansion, 20 of them in the Close/Check/Open functions.
 * - winMsgOpenMessage: the whole Open body. It is expanded in
 *   winMsgOpenError (type 6), winMsgOpenFightNoWait / winMsgOpenFight
 *   (type 4), winMsgOpenFieldWithSE / winMsgOpenField (type 3),
 *   winMsgOpenWithSE and winMsgOpen (any type). The constant-type copies
 *   keep the inline's fingerprints: "li r5,ID; cmpwi r5,0" on a folded ID,
 *   the close switch reusing r5 for its own case, and the result variable
 *   copied through a saved register ("mr r29,r3; mr r3,r29").
 * - winMsgOpenLevelUp: the level-up variant (fixed flags 1, no result),
 *   expanded in winMsgOpenLevelUpFiledStatus and winMsgOpenLevelUpStatus.
 * - winMsgCloseMenu: close the current message window, expanded in
 *   winMsgCloseLevelUpStatus, winMsgCloseFight, winMsgCloseField and
 *   winMsgClose.
 */
#include "dolphin/types.h"

extern void fn_800FBD88(void* message);
extern u32 GSmsgGetRect(void* message);
extern void GSmsgExec(void* message, u8 a, u8 b);
extern s32 GSmsgIsCheck(void* message);

extern void* windowGetFreeWork(void* window);
extern u32 windowGetParam(void* window, u32 index);
extern void* windowGetKeyInfo(void);
extern u32 windowGetActiveID(void);
extern void* windowSearchID(s32 id);
extern s32 winSeqIsCheck(s32 menuId, u16 seq);
extern void winSeqSetMenu(s32 menuId, u32 seq);

extern s32 menuIsCheck(s32 menuId);
extern s32 menuCloseCustom(void* menuId, u32 mode, u32 wait);
extern s32 menuOpenCustom(void* menuId, u32 parent, s32 x, s32 y, void* wait,
                          s32 argc, ...);

extern u8 lbl_8047AD10; /* error window open */
extern s8 lbl_80478B30; /* current message window type, -1 when closed */

static inline s32 winMsgResolveMenuId(s8 type)
{
    s32 id = 0;

    switch (type) {
    case 0: id = 0x40; break;
    case 1: id = 0x0F; break;
    case 2: id = 0x10; break;
    case 3: id = 0x40; break;
    case 4: id = 0x50; break;
    case 5: id = 0x51; break;
    case 6: id = 0x10C; break;
    case 7: id = 0xE6; break;
    case 8: id = 0xE8; break;
    case 9: id = 0x107; break;
    }
    return id;
}

static inline void winMsgCloseMenu(u32 wait)
{
    if (lbl_8047AD10 == 0) {
        s32 id = winMsgResolveMenuId(lbl_80478B30);
        if ((u8)menuIsCheck(id)) {
            menuCloseCustom((void*)id, 2, wait);
        }
        lbl_80478B30 = -1;
    }
}

static inline void winMsgOpenLevelUp(s8 type, u32 message, u32 wait)
{
    s32 id = winMsgResolveMenuId(type);

    if (id != 0) {
        if (type != lbl_80478B30 && lbl_8047AD10 == 0) {
            s32 oldId = winMsgResolveMenuId(lbl_80478B30);
            if ((u8)menuIsCheck(oldId)) {
                menuCloseCustom((void*)oldId, 2, wait);
            }
        }
        lbl_80478B30 = type;
        menuOpenCustom((void*)winMsgResolveMenuId(type), windowGetActiveID(), 0,
                       0, (void*)wait, 3, message, 1, 0);
    }
}

static inline s32 winMsgOpenMessage(s8 type, u32 message, u32 wait, u8 pause,
                                    u8 noWait, u8 sound)
{
    s32 result = 0;
    s32 parent;
    u8 flags = 0;
    s32 id = winMsgResolveMenuId(type);

    if (id == 0) {
        return result;
    }
    if (type != 6) {
        if (type != lbl_80478B30 && lbl_8047AD10 == 0) {
            s32 oldId = winMsgResolveMenuId(lbl_80478B30);
            if ((u8)menuIsCheck(oldId)) {
                menuCloseCustom((void*)oldId, 2, wait);
            }
        }
        lbl_80478B30 = type;
        parent = windowGetActiveID();
    } else {
        /* the error window: no parent, never waits, flagged until
         * winMsgCloseError */
        noWait = 1;
        parent = -1;
        lbl_8047AD10 = 1;
        wait = 0;
    }
    if (pause != 0) {
        flags |= 1;
    }
    if (noWait != 0) {
        flags |= 2;
    }
    result = menuOpenCustom((void*)winMsgResolveMenuId(type), parent, 0, 0,
                            (void*)wait, 3, message, flags, sound);
    return result;
}

/* 0x80105A3C | 0x1F4 */
s32 winMsgCtrl(u8* window)
{
    u8* work = windowGetFreeWork(window);
    u8 ready = 0;
    u32 rect;

    switch ((s8)window[1]) {
    case 0:
        if (*(void**)work != NULL) {
            fn_800FBD88(*(void**)work);
        }
        *(u32*)work = windowGetParam(window, 0);
        work[4] = (u8)windowGetParam(window, 1);
        work[5] = (u8)windowGetParam(window, 2);

        if ((s8)window[2] == 0) {
            ready = 1;
        }
        if ((u8)winSeqIsCheck(*(s32*)(window + 0x04), 0x2A) == 1) {
            ready = 1;
        }
        if (window[0x0A] != 0) {
            ready = 1;
        }
        if (ready != 0) {
            if (*(s32*)(window + 0x04) != 0x40) {
                winSeqSetMenu(*(s32*)(window + 0x04), 0x22);
            } else {
                winSeqSetMenu(*(s32*)(window + 0x04), 0x26);
            }
            window[2] = 1;
        }
        if (*(s32*)(window + 0x04) == 0x50) {
            /* message size, width in the high half, height in the low */
            rect = GSmsgGetRect(*(void**)work);
            *(s16*)(work + 0x0C) = rect >> 16;
            *(s16*)(work + 0x0E) = rect & 0xFFFF;
        }
        break;
    case 2:
        if ((s8)window[2] == 0) {
            GSmsgExec(*(void**)work, work[4], work[5]);
            window[2] = 1;
        }
        if (GSmsgIsCheck(*(void**)work) != 0) {
            window[0x98] = 0;
        } else {
            window[0x98] = 1;
        }
        break;
    case 3:
        if ((s8)window[2] == 0) {
            winSeqSetMenu(*(s32*)(window + 0x04), 0x2A);
            window[2] = 1;
        }
        break;
    case 5:
        fn_800FBD88(*(void**)work);
        *(void**)work = NULL;
        break;
    }
    return 0;
}

/* 0x80105C30 | 0x38 */
void winMsgButton(void* window)
{
    void* work = windowGetFreeWork(window);
    void* keyInfo = windowGetKeyInfo();

    *(u32*)((u8*)work + 0x8) = *(u16*)((u8*)keyInfo + 0x4);
}

/* 0x80105C68 | 0xE0 */
void winMsgCloseLevelUpStatus(u32 wait)
{
    winMsgCloseMenu(wait);
}

/* 0x80105D48 | 0x134 */
void winMsgOpenLevelUpFiledStatus(u32 message, u32 wait)
{
    winMsgOpenLevelUp(9, message, wait);
}

/* 0x80105E7C | 0x134 */
void winMsgOpenLevelUpStatus(u32 message, u32 wait)
{
    winMsgOpenLevelUp(5, message, wait);
}

/* 0x80105FB0 | 0x48 */
void winMsgCloseError(void)
{
    if ((u8)menuIsCheck(0x10C)) {
        menuCloseCustom((void*)0x10C, 2, 0);
    }
    lbl_8047AD10 = 0;
}

/* 0x80105FF8 | 0x88 */
s32 winMsgOpenError(u32 message, u32 wait, u8 pause)
{
    return winMsgOpenMessage(6, message, wait, pause, 0, 0);
}

/* 0x80106080 | 0xE0 */
void winMsgCloseFight(u32 wait)
{
    winMsgCloseMenu(wait);
}

/* 0x80106160 | 0xE4 */
u8 winMsgCloseCheckFight(void)
{
    s32 id = winMsgResolveMenuId(lbl_80478B30);
    u8* window = windowSearchID(id);
    s8 result;

    if (window == NULL) {
        result = -1;
    } else if (window[0x98] != 0) {
        result = 0;
    } else if (window[0x99] != 0) {
        result = 0;
    } else {
        result = 1;
    }
    return result >= 0;
}

/* 0x80106244 | 0x150 */
s32 winMsgOpenFightNoWait(u32 message, u32 wait, u8 pause)
{
    return winMsgOpenMessage(4, message, wait, pause, 1, 0);
}

/* 0x80106394 | 0x14C */
s32 winMsgOpenFight(u32 message, u32 wait, u8 pause)
{
    return winMsgOpenMessage(4, message, wait, pause, 0, 0);
}

/* 0x801064E0 | 0xD8 */
s32 winMsgCheckField(void)
{
    s32 id = winMsgResolveMenuId(lbl_80478B30);
    u8* window = windowSearchID(id);
    s8 result;

    if (window == NULL) {
        result = -1;
    } else if (window[0x98] != 0) {
        result = 0;
    } else if (window[0x99] != 0) {
        result = 0;
    } else {
        result = 1;
    }
    return result;
}

/* 0x801065B8 | 0xE0 */
void winMsgCloseField(u32 wait)
{
    winMsgCloseMenu(wait);
}

/* 0x80106698 | 0x150 */
s32 winMsgOpenFieldWithSE(u32 message, u32 wait, u8 pause, u8 sound)
{
    return winMsgOpenMessage(3, message, wait, pause, 0, sound);
}

/* 0x801067E8 | 0x14C */
s32 winMsgOpenField(u32 message, u32 wait, u8 pause)
{
    return winMsgOpenMessage(3, message, wait, pause, 0, 0);
}

/* 0x80106934 | 0xC8 */
s32 winMsgCheck(void)
{
    u8* window = windowSearchID(winMsgResolveMenuId(lbl_80478B30));

    if (window == NULL) {
        return -1;
    }
    if (window[0x98] != 0) {
        return 0;
    }
    return window[0x99] == 0;
}

/* 0x801069FC | 0xE0 */
void winMsgClose(u32 wait)
{
    winMsgCloseMenu(wait);
}

/* 0x80106ADC | 0x260 */
s32 winMsgOpenWithSE(s8 type, u32 message, u32 wait, u8 pause, u8 sound)
{
    return winMsgOpenMessage(type, message, wait, pause, 0, sound);
}

/* 0x80106D3C | 0x25C */
s32 winMsgOpen(s8 type, u32 message, u32 wait, u8 pause)
{
    return winMsgOpenMessage(type, message, wait, pause, 0, 0);
}
