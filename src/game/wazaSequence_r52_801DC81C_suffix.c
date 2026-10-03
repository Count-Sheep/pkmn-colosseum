/* _wazaSequenceEffectEntryLoad (0x801DC81C - 0x801DCAA0), built alone so the unit can link. */
#include "game/battle/battle_waza_types.h"

typedef struct WazaEffectLoadList {
    u8 pad_00[0x08];
    u32 size;
    u8 pad_0C[0x04];
    struct WazaEffectLoadList* next;
} WazaEffectLoadList;


void* _wazaSequenceEffectEntryLoad(void* entryPtr, void* dataPtr)
{
    extern s32 fn_800D37CC(void);
    extern u8* fn_801364A8(u8* work, u8* desc);
    extern u32 fn_8013AABC(void* callbacks);
    extern u32 electronStartEffect(void* callbacks);
    extern u32 fn_8013A42C(void* callbacks);
    extern u32 fn_801380D4(void* callbacks);
    extern u32 fn_8013796C(void* callbacks);
    extern u32 leaffxStartEffect(void* callbacks);
    extern u32 fn_8013C5A0(void* callbacks);
    extern u32 fn_8013B490(void* callbacks);
    extern u32 fn_8013D6B8(void* callbacks);
    extern u32 fn_8013DC18(void* callbacks);
    extern u32 fn_8013E4D4(void* callbacks);
    extern u32 patchiruTextureStart(void* callbacks);
    extern u32 fn_8013F000(void* callbacks);
    extern const f32 lbl_8047E3A0;
    u8 parsed[0xD8];
    u32 type;
    u32 handle = 0;
    WazaSequenceNode* entry = entryPtr;
    const WazaEffectLoadList* list;

    entry->resourceId = 0;
    entry->runtimeFlags =
        (s32)(((f32)(s32)fn_800D37CC() * (f32)(s32)*(u32*)((u8*)dataPtr + 4)) /
              lbl_8047E3A0);
    entry->resource = *(void**)dataPtr;
    entry->field_80 = 0;
    entry->animationMode = 0;

    dataPtr = fn_801364A8(parsed, dataPtr);
    type = *(u32*)parsed;
    switch (type) {
    case 0:
        list = *(const WazaEffectLoadList**)(parsed + 0x58);
        entry->resourceId = fn_8013AABC(parsed + 8);
        while (list != NULL) {
            handle += list->size;
            list = list->next;
        }
        break;
    case 1:
        entry->field_80 = *(u32*)(parsed + 0x58);
        entry->resourceId = electronStartEffect(parsed + 8);
        handle = *(u16*)(parsed + 0x52);
        break;
    case 2:
        entry->resourceId = fn_8013A42C(parsed + 8);
        handle = *(u16*)(parsed + 0x12);
        break;
    case 3:
        entry->field_80 = *(u16*)(parsed + 0x16);
        entry->resourceId = fn_801380D4(parsed + 8);
        handle = *(u16*)(parsed + 0x1A);
        break;
    case 4:
        entry->field_80 = *(u16*)(parsed + 0x30);
        entry->animationMode = *(u16*)(parsed + 0x32);
        entry->resourceId = fn_8013796C(parsed + 8);
        handle = *(u16*)(parsed + 0x28);
        break;
    case 5:
        entry->field_80 = *(s16*)(parsed + 0x52);
        entry->resourceId = leaffxStartEffect(parsed + 8);
        handle = *(u16*)(parsed + 0x46);
        break;
    case 6:
        entry->resourceId = fn_8013C5A0(parsed + 8);
        handle = *(u16*)(parsed + 0xAE);
        break;
    case 7:
        entry->resourceId = fn_8013B490(parsed + 8);
        handle = *(u16*)(parsed + 0xD6);
        break;
    case 8:
        entry->resourceId = fn_8013D6B8(parsed + 8);
        handle = 0;
        list = *(const WazaEffectLoadList**)(parsed + 0x14);
        while (list != NULL) {
            handle += list->size;
            list = list->next;
        }
        break;
    case 9:
        entry->resourceId = fn_8013DC18(parsed + 8);
        handle = 0;
        list = *(const WazaEffectLoadList**)(parsed + 0x1C);
        while (list != NULL) {
            handle += list->size;
            list = list->next;
        }
        break;
    case 10:
        entry->resourceId = fn_8013E4D4(parsed + 8);
        handle = *(u16*)(parsed + 0x3A);
        break;
    case 11:
        entry->resourceId = patchiruTextureStart(parsed + 8);
        /* RULE-EXCEPTION(user-approved): redundant reset retail performs here - see docs/RULE_EXCEPTIONS.md */
        handle = 0;
        break;
    case 12:
        entry->resourceId = fn_8013F000(parsed + 8);
        handle = *(u16*)(parsed + 0xBA);
        break;
    }
    if (entry->timing[entry->timingIndex - 1] == 0) {
        entry->timing[entry->timingIndex - 1] = handle;
    }
    return dataPtr;
}
