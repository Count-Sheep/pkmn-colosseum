/**
 * @file people_bios.c
 * @brief The people core unit (.text 0x8018F470-0x8018FE30): the people
 *        array, entry accessors, info-bios getters and the floor
 *        save/restore hooks.
 *
 * A separate unit from people.c: it owns .sdata2 0x8047D8A8-0x8047D8B8
 * (0.0f, the degree factor and 1.0f, all of which people.c's pool also
 * holds), .sbss 0x8047B1F8-0x8047B208 and fn_8018F4C8's switch table at
 * .data 0x8036C540, and people.c calls even its eight-byte getters.
 *
 * Most of its functions are linked from their own carved units; this file
 * holds the save/restore hooks the candidate unit people_candidate_8018F730
 * scores (research source carried over from the old combined people.c).
 */
#include "dolphin/types.h"
#include "game/people/people.h"

extern void* memset(void* dst, int val, u32 size);
extern void* memcpy(void* dst, const void* src, u32 size);
extern void* GSmodelGetPositionPtr(void*);
extern void* GSmodelGetRotationPtr(void*);
extern void GSmodelPushState(void* model, void* state);
extern void GSmodelPopState(void* model, void* state);
extern void GSmodelSetVisibility(void*, u8);
extern void GScolsys2HumanEnable(s32, u8);
extern void GSvecCopy(void*, void*);
extern void GSmodelSetPosition(void*, void*);
extern void GSmodelSetRotation(void*, void*);
extern int peopleOpenSub(void* entry, u32 groupId, u32 indexId, s32 objectId);
extern void fn_8018F08C(void* entry, u32 motionIndex);
extern void fn_801848D0(void* model, s32 group, s32 id, s32 part);
extern void fn_80188AF4(u32 groupId, u32 index);
extern void fn_80188FA0(u32 groupId, u32 index, u32 targetGroupId, u32 targetIndex);
extern void fn_80188CA0(u32 groupId, u32 index, u32 x, u32 y, u32 z);
extern void fn_801812C4(void* entry);

extern s32 lbl_8047B1F8;          /* maximum people count */
extern PeopleEntry* lbl_8047B200; /* people array (heap-allocated) */
extern f32 lbl_8047D8B0;          /* default moveSpeed */

#define gPeopleMaxCount lbl_8047B1F8
#define gPeopleArray    lbl_8047B200

u32 peopleBiosGetPushDataSize(void) {
    PeopleEntry* current;
    PeopleEntry* entry = lbl_8047B200;
    s32 count = lbl_8047B1F8;
    u32 total;
    s32 i;

    total = 0;
    for (i = 0; i < count; i++) {
        if (i < 0 || count <= i) {
            current = NULL;
        } else {
            current = entry;
        }
        if (current->active != 0) {
            total += PEOPLE_SPAWN_DATA_SIZE;
        }
        entry = (PeopleEntry*)((u8*)entry + PEOPLE_ENTRY_SIZE);
    }

    return total;
}
void peopleBiosPushData(u8* dst, u32 size) {
    u32 offset;
    u8* end;
    u8* current;
    void* model;
    s32 i;

    i = 0;
    current = dst;
    offset = 0;
    end = dst + size;
    while (i < gPeopleMaxCount) {
        PeopleEntry* entry;

        if (i < 0 || gPeopleMaxCount <= i) {
            entry = NULL;
        } else {
            entry = (PeopleEntry*)((u8*)gPeopleArray + offset);
        }
        if (entry->active) {
            model = entry->modelHandle;
            memcpy(current, (u8*)entry + 0x20, 0xBC);
            memcpy(current + 0xBC, GSmodelGetPositionPtr(model), 0xC);
            memcpy(current + 0xC8, GSmodelGetRotationPtr(model), 0xC);
            memcpy(current + 0xD4, (u8*)model + 0x120, 0xC);
            memcpy(current + 0xE0, (u8*)model + 0x12C, 0xC);
            GSmodelPushState(model, current + 0xEC);
            current += PEOPLE_SPAWN_DATA_SIZE;
            if (current > end) {
                break;
            }
        }
        offset += PEOPLE_ENTRY_SIZE;
        i++;
    }
}
void peopleBiosPopData(u8* src, u32 size) {
    u8* restored[64];
    u8* saved;
    u8* entry;
    u8* model;
    u32 count;
    u32 i;
    u32 j;

    memset(gPeopleArray, 0, gPeopleMaxCount * PEOPLE_ENTRY_SIZE);
    count = size / PEOPLE_SPAWN_DATA_SIZE;
    if (count * PEOPLE_SPAWN_DATA_SIZE != size || count > 64) {
        return;
    }

    saved = src;
    for (i = 0; i < count; i++, saved += PEOPLE_SPAWN_DATA_SIZE) {
        entry = NULL;
        for (j = 0; j < (u32)gPeopleMaxCount; j++) {
            u8* candidate = (u8*)gPeopleArray + j * PEOPLE_ENTRY_SIZE;
            if (candidate[0] == 0) {
                memset(candidate, 0, PEOPLE_ENTRY_SIZE);
                candidate[0] = 1;
                *(void**)(candidate + 4) = candidate;
                *(s32*)(candidate + 0x50) = -1;
                *(f32*)(candidate + 0x58) = lbl_8047D8B0;
                entry = candidate;
                break;
            }
        }
        peopleOpenSub(entry, *(u32*)(saved + 8), *(u32*)(saved + 0x0C),
                    *(u32*)(saved + 0x10));
        memcpy(entry + 0x20, saved, 0xBC);
        restored[i] = entry;
    }

    saved = src;
    for (i = 0; i < count; i++, saved += PEOPLE_SPAWN_DATA_SIZE) {
        entry = restored[i];
        model = *(u8**)(entry + 8);
        if (model != NULL) {
            GSmodelSetVisibility(model, entry[0x21]);
        }
        if (*(s32*)(entry + 0x50) >= 0) {
            GScolsys2HumanEnable(*(s32*)(entry + 0x50), entry[0x23]);
        }
        fn_8018F08C(entry, *(u32*)(entry + 0x90));
        GSmodelPopState(model, saved + 0xEC);

        if (*(s32*)(entry + 0xC8) != -1 &&
            *(s32*)(entry + 0xCC) != -1) {
            fn_801848D0(model, *(s32*)(entry + 0xC8),
                        *(s32*)(entry + 0xCC), *(s32*)(entry + 0xD0));
            GSvecCopy(GSmodelGetPositionPtr(model), saved + 0xBC);
            GSvecCopy(GSmodelGetRotationPtr(model), saved + 0xC8);
            GSmodelSetPosition(model, saved + 0xD4);
            GSmodelSetRotation(model, saved + 0xE0);
        }

        switch (entry[0x96]) {
        case 0:
            fn_80188AF4(*(u32*)(entry + 0x28), *(u32*)(entry + 0x2C));
            break;
        case 1:
            fn_80188FA0(*(u32*)(entry + 0x28), *(u32*)(entry + 0x2C),
                        *(u32*)(entry + 0xC0), *(u32*)(entry + 0xC4));
            break;
        case 2:
            fn_80188CA0(*(u32*)(entry + 0x28), *(u32*)(entry + 0x2C),
                        (s32)*(f32*)(entry + 0xA8),
                        (s32)*(f32*)(entry + 0xAC),
                        (s32)*(f32*)(entry + 0xB0));
            break;
        }
        if (entry[0x96] != 0) {
            for (j = 0; j < 60; j++) {
                fn_801812C4(entry);
            }
        }
    }
}
