#include "dolphin/types.h"

typedef struct AbilityItemWeight {
    u16 item;
    u16 weight;
} AbilityItemWeight;

typedef struct AbilityVec {
    f32 x;
    f32 y;
    f32 z;
} AbilityVec;

extern u32* lbl_80478EB0;
extern AbilityItemWeight* lbl_80478EB4;
extern f32 lbl_8047E3F0;

extern u16 fn_800E0C54(void);
extern void* fn_8018D998(s32, s32);
extern void* peopleSearchID(void*);
extern void* peopleGetPosition(void*);
extern void heroMoveGetHeroPos(void*);
extern void* fn_8018FCBC(void*);
extern void fn_800E0168(void*, void*, void*);
extern f64 atan2(f64, f64);
extern void fn_8018805C(s32, s32, f32, f32);
extern void peopleMoveCheck(s32, s32, u8);
extern u32 fn_801906A0(u32);
extern void _flagSet(u32, s32);
extern u8 fn_801902E0(u32);
extern void winMsgOpenFieldWithSE(s32, s32, s32, s32);
extern void fn_80165668(s32, s32, s32);
extern void msgctrlSetValue(s32, u32);
extern void winMsgOpen(s32, s32, s32, s32);
extern s32 heroItemAddItemDataId(void*, u32, u32, s32);
extern void pcboxDelItem(s32, u32, u32);

void fn_801DF474(s32 slot, s32 abilityID) {
    AbilityItemWeight* weights;
    AbilityVec direction;
    AbilityVec heroPosition;
    u32 state;
    s32 running;
    u32 totalWeight;
    u32 i;
    s32 selectedItem;
    u32 selectedData;
    u32 cumulative;
    u32 randomValue;
    u32 count;
    s32 result;
    void* person;

    state = 0;
    running = 1;
    totalWeight = 0;
    count = *lbl_80478EB0;
    weights = lbl_80478EB4;
    for (i = 0; i < count; i++) {
        totalWeight += (weights++)->weight;
    }

    randomValue = (u16)fn_800E0C54() % totalWeight;
    cumulative = 0;
    {
        AbilityItemWeight* entry = lbl_80478EB4;

        for (i = 0; i < count; i++, entry++) {
            cumulative += entry->weight;
            if (randomValue < cumulative) {
                selectedItem = entry->item;
                selectedData = entry->item;
                break;
            }
        }
    }

    do {
        switch (state) {
        case 0:
            person = peopleSearchID(fn_8018D998(slot, abilityID));
            if (person != NULL) {
                peopleGetPosition(person);
                heroMoveGetHeroPos(&heroPosition);
                fn_800E0168(&direction, &heroPosition,
                            fn_8018FCBC(person));
                fn_8018805C(slot, abilityID,
                            (f32)atan2(direction.x, direction.z),
                            lbl_8047E3F0);
                peopleMoveCheck(slot, abilityID, 1);
            }

            if (fn_801906A0(0xD0) != 0) {
                if (fn_801906A0(0xD1) >= 500) {
                    _flagSet(0xD1, 0);
                    result = 1;
                } else if (fn_801902E0(0xAFE) != 0) {
                    _flagSet(0xAFE, 0);
                    result = -1;
                } else {
                    result = 0;
                }
            } else {
                _flagSet(0xD0, 1);
                result = 1;
            }

            if (result > 0) {
                state = 1;
            } else if (result < 0) {
                state = 3;
            } else {
                state = 4;
            }
            break;
        case 1:
            winMsgOpenFieldWithSE(0x5571, 1, 0, 1);
            state = 2;
            break;
        case 2:
            fn_80165668(0x3CA, 0, 0xFF);
            msgctrlSetValue(0x2D, selectedItem);
            winMsgOpen(3, 0x3CB8, 1, 0);
            result = heroItemAddItemDataId(NULL, selectedData, 1, -1);
            if (result != 0 && result > 0) {
                /* Masks rather than casts keep MWCC's preheader order retail. */
                pcboxDelItem(0, selectedData & 0xFFFF, result & 0xFFFF);
            }
            state = 7;
            break;
        case 3:
            winMsgOpenFieldWithSE(0x5573, 1, 0, 1);
            state = 7;
            break;
        case 4:
            winMsgOpenFieldWithSE(0x5574, 1, 0, 1);
            state = 7;
            break;
        case 7:
            running = 0;
            break;
        case 5:
        case 6:
        default:
            break;
        }
    } while (running != 0);
}
