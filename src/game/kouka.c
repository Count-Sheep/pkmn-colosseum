/**
 * @file kouka.c
 * @brief Decompiled functions.
 *
 * Address range: 0x80136078 - 0x80136368
 *
 * Split out of the former game/effect/effect_util.c CodeCandidate
 * bucket (0x8013151C - 0x80137114); see effect_util_types.h for
 * shared cross-TU declarations.
 */

#include "dolphin/types.h"
#include "game/effect/effect_util_types.h"

/* 0x80136078 | 0xC4 */
void koukaExec(u32 index, void* arg1, void* arg2, s32* out) {
    u32 linkedIndex;
    u16 sub;

    if (out != NULL) {
        _koukaOneExec__FUlPvPvPl(index, arg1, arg2, out);
    } else {
        _koukaOneExec__FUlPvPvPl(index, arg1, arg2, NULL);
    }

    linkedIndex = koukaDataBiosGetLink(index) & 0xFFFF;
    if (linkedIndex == 0) {
        return;
    }

    for (sub = 0; sub < 8; sub++) {
        if ((u16)koukaLinkDataBiosGetKouka(linkedIndex, sub) != 0) {
            if (out != NULL) {
                _koukaOneExec__FUlPvPvPl(index, arg1, arg2, out + ((sub & 0xFFFF) + 1));
            } else {
                _koukaOneExec__FUlPvPvPl(index, arg1, arg2, NULL);
            }
        }
    }
}


/* 0x8013613C | 0x22C */
/* RULE-EXCEPTION(user-approved): single-use inline helper - the s16 return gives the first value its own web, joined to the reassigned amount by a copy the allocator merges, so the call result goes straight into r24 as in retail (found by the permuter farm) - see docs/RULE_EXCEPTIONS.md */
static inline s16 koukaGetBaseValue(u32 index)
{
    return koukaDataBiosGetValue(index, 0);
}

s32 _koukaOneExec__FUlPvPvPl(u32 index, void* arg1, void* arg2, s32* out) {
    /* RULE-EXCEPTION(user-approved): block-scope statusGetStatus/statusSetStatus prototypes with a void* target and u16 sub/value parameters (the header says u32) so arg1 needs no cast copy and the conversions follow the argument moves as in retail - see docs/RULE_EXCEPTIONS.md */
    extern u32 statusGetStatus(u32, void*, u32, u16, u16);
    extern u32 statusSetStatus(u32, void*, u32, u16, u32, u32);
    s32 current;
    s16 amount;
    s16 divisor;
    u8 mode;
    s32 result;
    u32 extra;
    u16 statusSub;
    u32 statusKind;

    if (index == 0) {
        return 0;
    }

    statusKind = koukaDataBiosGetStatusKind(index);
    statusSub = koukaDataBiosGetStatus(index);
    amount = koukaGetBaseValue(index);
    divisor = koukaDataBiosGetValue(index, 1);
    mode = koukaDataBiosGetVar(index);

    current = statusGetStatus(statusKind, arg1, 0, statusSub, (u16)amount);
    if (mode == 0 || mode == 2 || mode == 3) {
        if (amount == -1) {
            amount = (s32)statusGetStatus(statusKind, arg1, 0, (u16)divisor, 0) / 2;
        } else if (amount == -2) {
            amount = statusGetStatus(statusKind, arg1, 0, (u16)divisor, 0);
        } else if (amount < 0 || divisor < 0) {
            return 0;
        }
    }

    switch (mode) {
    case 0:
        result = amount;
        break;
    case 1:
        result = (current * amount) / divisor;
        break;
    case 2:
        result = current + amount;
        break;
    case 3:
        result = current - amount;
        break;
    case 4:
        result = current + ((current * amount) / divisor);
        break;
    case 5:
        result = current - ((current * amount) / divisor);
        break;
    default:
        return 0;
    }

    if (arg2 != NULL) {
        extra = fightTargetGetTragetPtrToRelativeHostSideFightTargetId(arg2, (u16)fightFloorGetStatus(0, 0, 0x14, 0));
    } else {
        extra = 0;
    }

    statusSetStatus(statusKind, arg1, 0, statusSub, extra, result);
    if (out != NULL) {
        *out = result;
    }
}
