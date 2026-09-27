/**
 * @file field_candidate_801D0090.c
 * @brief Memory-card error message (0x801D0090 - 0x801D0314).
 *
 * Shows the message for a memory-card task error in the task's window
 * (the byte kept at task +0x5C, passed to winMsgOpen sign-extended) and,
 * for the errors that ask a question (6, 7 and 8), opens a yes/no menu;
 * returns 1 only when the player answered no. Positive errors are the
 * task's own codes, negative ones CARD result codes (-2 .. -13). Some
 * messages depend on the task kind (5-7 and 13 get the "save" wording).
 *
 * The switch dispatches through jumptable_8036DFAC, whose entries fix the
 * case values: error -2 shows 0x3C5D and error 19 falls back to 0x3C30 in
 * its own block (the old candidate had 0x3C5D under case 19).
 *
 * Exact, but a candidate: the jump table sits 4-aligned inside the
 * memory-card unit's .data (after fn_801CDB04's and fn_801CF9C8's tables),
 * so a standalone object, whose .data starts 8-aligned, shifts all later
 * data by 0x20. It can only link as part of that whole unit.
 *
 * Built on the save-data SHA-1 / memory-card unit's flags (GC/2.5 -O4,p;
 * see configure.py).
 */
#include "dolphin/types.h"
#include "game/save/memcard_task.h"

extern void winMsgOpen(s32 window, u32 message, s32 arg2, s32 arg3);
extern s8 menuSubOpenYesNo(s32 port, s32 x, s32 y, s32 initial);
extern void msgctrlSetValue(u32 id, u32 value);

u8 fn_801D0090(s32 error)
{
    u32 message;
    u8 ask;
    s32 initial;

    ask = 0;
    switch (error) {
    case 0:
        return 0;
    case 1:
        message = 0x3C29;
        break;
    case 2:
        switch (lbl_8047B3D4->task_kind) {
        case 5:
        case 6:
        case 7:
        case 13:
            message = 0x3C2C;
            break;
        default:
            message = 0x3C04;
            break;
        }
        break;
    case 3:
        message = 0x3BFB;
        break;
    case 4:
        message = 0x3C55;
        break;
    case 5:
        message = 0x4421;
        break;
    case 6:
        message = 0x3BFA;
        ask = 1;
        initial = 0;
        break;
    case 7:
        message = 0x3C33;
        ask = 1;
        initial = 0;
        break;
    case 8:
        message = 0x3D45;
        ask = 1;
        initial = 1;
        break;
    case 9:
        message = 0x3C57;
        break;
    case 10:
        switch (lbl_8047B3D4->task_kind) {
        case 5:
        case 6:
        case 7:
        case 13:
            message = 0x3C2E;
            break;
        default:
            message = 0x3C06;
            break;
        }
        break;
    case 11:
        message = 0x3BFD;
        break;
    case 12:
        message = 0x44E5;
        break;
    case 13:
        message = 0x3D43;
        break;
    case 14:
        message = 0x44D8;
        break;
    case 15:
        switch (lbl_8047B3D4->task_kind) {
        case 5:
        case 6:
        case 7:
        case 13:
            message = 0x44D8;
            break;
        default:
            message = 0x4444;
            break;
        }
        break;
    case 16:
        message = 0x3C34;
        break;
    case 17:
        message = 0x4422;
        if (lbl_8047B3D4->task_kind != 2) {
            winMsgOpen((s8)lbl_8047B3D4->card_work_size, message, 1, 1);
            message = 0x44E4;
        }
        break;
    case 18:
        message = 0x4423;
        break;
    case -2:
        message = 0x3C5D;
        break;
    case -3:
        message = 0x3C32;
        break;
    case -4:
        message = 0x3C31;
        break;
    case -5:
        message = 0x3C5B;
        break;
    case -6:
    case -13:
        switch (lbl_8047B3D4->task_kind) {
        case 1:
        case 2:
            message = 0x4424;
            if (lbl_8047B3D4->next_state_after_delay == 0x32) {
                break;
            }
            /* fall through */
        default:
            message = 0x3C30;
            break;
        }
        break;
    case -8:
    case -9:
        message = 0x3C5F;
        msgctrlSetValue(0x2F, 0x30);
        break;
    case 19:
        message = 0x3C30;
        break;
    default:
        message = 0x3C30;
        break;
    }

    winMsgOpen((s8)lbl_8047B3D4->card_work_size, message, 1, 1);
    if (ask && menuSubOpenYesNo(0, 0x3C, 0xAA, initial)) {
        ask = 0;
    }
    return ask;
}
