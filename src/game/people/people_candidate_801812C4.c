/*
 * The start of the people TU: fn_801812C4 through fn_80181850.
 *
 * TU start: the functions before 0x801812C4 (fn_80180C78-fn_80181224) are
 * -opt level=0 code (they match with -O4,p -opt level=0, like the
 * 0x8017FA5C unit, and not at -O4), while this TU is GC/1.3 -O4,p. Its
 * pooled .sdata2 starts at 0x8047D798 (75.0f, first used by fn_801812C4),
 * right after the fsys TU's "%s.fsys" at 0x8047D790.
 *
 * fn_801812C4, fn_801812E8 and fn_80181478 are exact. fn_80181850 is 99.5%:
 * every instruction matches except callee-saved register choice (retail
 * gives the case-5 turn lookup's groupId/index r27/r28 and the range
 * lookup's r28/r27 and keeps the floor pointer in r29; this source mirrors
 * both pairs and puts the floor pointer in r26).
 *
 * Stays a CodeCandidate: these functions read the TU's pooled literals
 * (0x8047D798-0x8047D7D8), including compiler-generated ones (the
 * int-to-float biases, fn_80181478's folded (f32)0), which the rest of the
 * TU also reads by symbol. A text-only unit compiles private copies, and a
 * unit that owned the pool slice would leave those symbols undefined for the
 * TU's other, unlinked functions (tried: the link fails).
 */
#include "game/people/people_inline.h"

typedef struct GSvec {
    f32 x;
    f32 y;
    f32 z;
} GSvec;

extern f64 fmod(f64 x, f64 y);
extern s32 fn_800D37CC(void);
extern u32 fn_800D3088(void);
extern void fn_800E00AC(void* vector, void* direction, f32 distance);
extern void fn_800E0168(void* dst, void* srcA, void* srcB);
extern void GSvecCopy(void* dst, void* src);
extern f32 fn_800E0BA0(void);
extern u32 fn_800F7108(u16 flagId);
extern void* GSresGetResource(u32 group, u32 id);
extern void GSmodelSetShadowSurface(void* model, s32 count, void* surfaces);
extern u8 GScolsys2WalkGetLayer(void* position, u8* layer, u8* subLayer);
extern u32 fn_80113F48(void);
extern void* floorDataBiosGetCurrentPtr(void);
extern u32 floorDataBiosGetShadowReciveNum(void* floor);
extern u32 floorDataBiosGetShadowReciveID(void* floor, u32 index);

extern void fn_80184A90(PeopleEntry* entry);
extern void fn_80184D80(PeopleEntry* entry);
extern void fn_8018524C(PeopleEntry* entry, u8 loopPath);
extern void fn_801858C4(PeopleEntry* entry);
extern void fn_80185B90(PeopleEntry* entry, f32 amount);
extern void fn_8018ECEC(PeopleEntry* entry, f32 step);
extern void fn_8018F30C(void);
extern void fn_8018FC98(PeopleEntry* entry, void* pos);
extern void* fn_8018FCBC(PeopleEntry* entry);

/* The people movement step (auto-inlined into the per-frame update). */
void fn_801812C4(PeopleEntry* entry)
{
    fn_8018ECEC(entry, 75.0f);
}

/*
 * Suspend a person's scripted state (4 or 5) for an interaction, or restore
 * it afterwards.
 */
s32 fn_801812E8(u32 groupId, u32 index, u8 doInteract)
{
    PeopleEntry* entry;

    entry = peopleFindBySelf(peopleFindSelf(groupId, index));
    if (entry == NULL) {
        return 0;
    }
    if (doInteract) {
        entry->prevState = entry->state;
        switch (entry->state) {
        case 4:
        case 5:
            entry->state = 0;
            break;
        }
    } else {
        switch (entry->prevState) {
        case 4:
        case 5:
            entry->state = entry->prevState;
            entry->subState = 0;
            entry->animBlendFactor = 1.0f;
            break;
        }
    }
    return 1;
}

/* Lock a person for talking (playing motion slot 1), or release the lock. */
s32 fn_80181478(u32 groupId, u32 index, u8 doSetup)
{
    PeopleEntry* entry;

    entry = peopleFindBySelf(peopleFindSelf(groupId, index));
    if (entry == NULL) {
        return 0;
    }
    if (entry->state == 0) {
        return 1;
    }
    if (doSetup) {
        if (!entry->talkLock) {
            entry->talkLock = 1;
            peopleSetMotionIndex(entry, 1);
        }
        return 1;
    }
    if (entry->talkLock) {
        entry->talkLock = 0;
    }
    return 0;
}

void fn_80181850(void)
{
    s32 i;
    PeopleEntry* entry;
    GSvec currentPosition;
    GSvec modelRotation;
    GSvec modelPosition;
    GSvec floorPosition;
    void* shadowSurfaces[2];
    u8 subLayer;
    u8 layer;
    u8 visible;
    void* floor;
    u32 group;
    s32 receiverCount;
    s32 shadowCount;
    f32 frameCount;
    f32 angle;

    i = peopleGetMaxCount();
    while (i-- > 0) {
        entry = peopleGetEntry(i);
        if (!entry->active) {
            continue;
        }

        if (peopleTestFlags(entry, 0x40000000)) {
            GSvecCopy(&modelPosition, fn_8018FCBC(entry));
            GSvecCopy(&modelRotation, peopleGetTransform(entry));
            fn_800E0168(&entry->collisionX, &modelPosition, &modelRotation);
        }

        fn_8018FC98(entry, &currentPosition);
        peopleSetTransform(entry, &currentPosition);
        entry->talkRange = 0.0f;

        if (entry->visible) {
            visible = TRUE;
        } else if (!fn_800F7108(entry->flagId)) {
            visible = TRUE;
        } else {
            visible = FALSE;
        }

        if (visible && !entry->talkLock) {
            switch (entry->state) {
            case 1:
                fn_801858C4(entry);
                break;
            case 2:
                fn_8018524C(entry, 0);
                break;
            case 3:
                fn_8018524C(entry, 1);
                break;
            case 4:
                fn_80184D80(entry);
                break;
            case 5:
                switch (entry->subState) {
                case 0:
                    if (entry->animBlendFactor > 0.0f) {
                        frameCount = (f32)fn_800D37CC();
                        entry->animBlendFactor -= (f32)fn_800D3088() / frameCount;
                        if (entry->animBlendFactor < 0.0f) {
                            entry->animBlendFactor = 0.0f;
                        }
                        break;
                    }
                    entry->subState = 1;
                    /* fallthrough */
                case 1:
                    angle = 3.141592653589793 + entry->field_40 +
                            1.5707963267948966 * fn_800E0BA0();
                    angle = fmod(angle, 6.2831855f);
                    peopleStartTurn(entry->groupId, entry->index, angle, 1.0f);
                    entry->subState = 2;
                    /* fallthrough */
                case 2:
                    entry->animBlendFactor =
                        entry->field_8C * fn_800E0BA0() + entry->field_88;
                    entry->subState = 0;
                    break;
                }
                break;
            }

            if (fn_800D3088() != 0) {
                fn_800E0168(&currentPosition, fn_8018FCBC(entry), &currentPosition);
                fn_800E00AC(&currentPosition, &currentPosition, (f32)fn_800D3088());
                entry->talkRange = peopleCalcRange(entry->groupId, entry->index,
                                                   &currentPosition);
            }

            fn_80184A90(entry);
            fn_80185B90(entry, entry->talkRange);
            fn_801812C4(entry);

            floor = floorDataBiosGetCurrentPtr();
            if (floor != NULL) {
                group = fn_80113F48();
                fn_8018FC98(entry, &floorPosition);
                if (!GScolsys2WalkGetLayer(&floorPosition, &layer, &subLayer)) {
                    layer = 0;
                    subLayer = 0;
                }

                receiverCount = floorDataBiosGetShadowReciveNum(floor);
                if (layer < receiverCount && subLayer < receiverCount) {
                    shadowCount = 1;
                    shadowSurfaces[0] = GSresGetResource(
                        group, floorDataBiosGetShadowReciveID(floor, layer));
                    if (layer != subLayer) {
                        shadowCount = 2;
                        shadowSurfaces[1] = GSresGetResource(
                            group, floorDataBiosGetShadowReciveID(floor, subLayer));
                    }
                    GSmodelSetShadowSurface(entry->modelHandle, shadowCount, shadowSurfaces);
                }
            }
        }
    }

    fn_8018F30C();
}
