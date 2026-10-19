/**
 * @file pda_exact_8003C2B8.c
 * @brief PDA party-list row drawer, 0x8003C2B8 - 0x8003C728.
 *
 * Standalone copy of fn_8003C2B8 from pda_range_80037158.c. Unit flag
 * -opt nopeephole replaces the shared file's local "peephole off".
 */
#include "dolphin/types.h"

typedef struct PdaSprite {
    u8 pad00[0x8B];
    u8 alphaByte;
} PdaSprite;

typedef struct PdaEvent {
    u8 pad00[0x6];
    s16 messageId;
} PdaEvent;

typedef struct PdaListEntry {
    u16 field_00;
    u16 battleId;
} PdaListEntry;

extern u8 lbl_803A6748[];
extern u8 lbl_802EF0A8[];
extern PdaListEntry* lbl_8047A4D4;
extern u32 lbl_8047A4D0;
extern s32 lbl_8047A4DC;

extern void fn_800FE35C(void);
extern void fn_800FE38C(s32 x1, s32 y1, s32 x2, s32 y2);
extern s32 fn_800FB680(s32 x, s32 y, s32 color, void* data);
extern u32 GSmsgGetGSchar(u32 msg);
extern void msgctrlSetValue(s32 id, u32 value);
extern void windowDrawSprite(s16 x, s16 y, PdaSprite* sprite, u16 id, s32 arg4);
extern s32 fn_8003CE1C(s32 index);
extern void* pokemonDataBiosGetPtr(u16 id);
extern void* pokemonDataBiosGetName(void* data);
extern void* fightTrainerPokemonDataBiosGetNickname(void* entry);
extern u8 fightTrainerPokemonDataBiosGetDarkPokemonFlag(void* entry);
extern u8 fn_801EED88(u16 id);
extern u16 fn_801EEFAC(u16 id, s32 side);
extern u16 fn_801EE248(u16 id);
extern u8* fn_801EE328(u16 id);
extern s32 fn_801EE614(u16 id);
extern u8 fn_801EE8F4(u16 id);
extern u8 fn_801EEAD0(u16 id);
extern u8 fn_801EEC74(u16 id);
extern u32 fn_801FCC7C(u8* ptr);
extern void* fightTrainerDataBiosGetPtr();
extern u32 fn_801EE544(u16 id, u8* variant);
extern u16 fn_801FCC3C(u8* ptr);
extern void* fightTrainerPokemonDataBiosGetPtr(u16 slot);

/* The owner caption of a party entry's battle: the trainer-kind or species
 * caption chosen by the entry's variant, or 0 without a battle list. */
/* RULE-EXCEPTION(user-approved): single-use inline helper pda_8003C2B8_OwnerText (fn_8003C2B8) - see docs/RULE_EXCEPTIONS.md */
static inline u32 pda_8003C2B8_OwnerText(s32 off, u8* slot)
{
    u16 kind;
    u32 caption;
    u32 text;
    u8 special;
    u16 trainerId;

    special = 0;
    if (lbl_8047A4D0 != 0) {
        kind = *(u16*)((u8*)lbl_8047A4D4 + off + 2);
        caption = (u32)fn_801EE544(kind, (void*)slot);
        switch (*slot) {
        case 0:
            caption = 0x371f;
            break;
        case 1:
        case 2:
            if (kind == 0x43) {
                caption = 0x12b0;
            } else if ((trainerId = fn_801EEFAC(kind, 0)) == 9) {
                text = (u32)fn_801EE328(kind);
                special = 1;
            } else {
                caption = fn_801FCC7C((void*)fightTrainerDataBiosGetPtr(trainerId));
            }
            break;
        }
        if (special != 0) {
            return text;
        }
        text = GSmsgGetGSchar(caption);
        return text;
    }
    return 0;
}

/* Draw one row per party entry: nickname, owner and battle caption. */
void fn_8003C2B8(PdaSprite* alphaSprite, PdaEvent* event)
{
    s32 y;
    void* entry;
    u32 name;
    u32 caption;
    u32 owner;
    u32 msg;
    u32 tint;
    u16 id;
    s32 i;
    s32 off;
    s32 mode;
    u8* slot;
    void* nick;
    u16 trainerId;
    u8 registered;
    u8 cleared;
    f32 stride;
    f32 shifted;
    f32 firstX;
    f32 rowY;

    alphaSprite->alphaByte =
        255.0f * *(f32*)(lbl_803A6748 + 0x44);
    fn_800FE38C(*(s16*)(lbl_802EF0A8 + 0x4fa2) -
                    *(s16*)(lbl_802EF0A8 + event->messageId * 0x1c + 2),
                *(s16*)(lbl_802EF0A8 + 0x4fa4) -
                    *(s16*)(lbl_802EF0A8 + event->messageId * 0x1c + 4),
                *(s16*)(lbl_802EF0A8 + 0x4fa6),
                *(s16*)(lbl_802EF0A8 + 0x4fa8));
    slot = lbl_803A6748 + 0x94;
    firstX = *(f32*)(lbl_803A6748 + 0x14);
    rowY = *(f32*)(lbl_803A6748 + 0x18);
    shifted = 1.0f + (firstX - 67.0f - 8.0f);
    stride = 24.0f;
    for (i = 0, off = 0; i < (s32)lbl_8047A4DC; i++) {
        if (i >= *(s32*)(lbl_803A6748 + 0x8) - 1 &&
            i <= *(s32*)(lbl_803A6748 + 0xc) + 1) {
            id = *(u16*)((u8*)lbl_8047A4D4 + off + 2);
            trainerId = fn_801EEFAC(id, 0);
            if (trainerId == 9) {
                entry = pokemonDataBiosGetPtr(fn_801EE248(id));
                if (entry != NULL) {
                    name = GSmsgGetGSchar(
                        (u32)pokemonDataBiosGetName(entry));
                }
            } else {
                entry = fightTrainerPokemonDataBiosGetPtr(
                    fn_801FCC3C((u8*)fightTrainerDataBiosGetPtr(trainerId)));
                name = GSmsgGetGSchar(
                    (u32)fightTrainerPokemonDataBiosGetNickname(entry));
                for (;;) {
                    if (id == fightTrainerPokemonDataBiosGetDarkPokemonFlag(entry)) {
                        nick = fightTrainerPokemonDataBiosGetNickname(entry);
                        if (nick != NULL) {
                            name = GSmsgGetGSchar((u32)nick);
                        }
                        break;
                    }
                    entry = (u8*)entry + 0x50;
                }
            }
            msg = name;
            if (name == 0) {
                msg = GSmsgGetGSchar(1);
            }
            owner = pda_8003C2B8_OwnerText(off, slot);
            if (owner == 0) {
                owner = GSmsgGetGSchar(1);
            }
            caption = fn_8003CE1C(i);
            if (caption == 0) {
                caption = GSmsgGetGSchar(1);
            }
            id = lbl_8047A4D4[(u16)i].battleId;
            fn_801EE614(id);
            fn_801EE8F4(id);
            registered = fn_801EEAD0(id);
            cleared = fn_801EEC74(id);
            if (registered != 0) {
                if (cleared != 0) {
                    mode = 1;
                } else {
                    mode = 0;
                }
            } else {
                mode = 2;
            }
            switch (mode) {
            case 0:
                windowDrawSprite(shifted, rowY - 4.0f,
                                 alphaSprite, 0x161, 0);
                break;
            case 1:
                windowDrawSprite(firstX - 67.0f, rowY, alphaSprite, 0x160, 0);
                break;
            case 2:
                break;
            }
            if (fn_801EED88(*(u16*)((u8*)lbl_8047A4D4 + off + 2)) != 0) {
                tint = 0xe0e01800;
            } else {
                tint = -0x100;
            }
            msgctrlSetValue(0x37, msg);
            y = (s32)rowY;
            fn_800FB680(0, y, tint | alphaSprite->alphaByte, (void*)0xe7);
            msgctrlSetValue(0x37, caption);
            fn_800FB680(0x89, y, tint | alphaSprite->alphaByte, (void*)0xe7);
            msgctrlSetValue(0x37, owner);
            fn_800FB680(0x13d, y, tint | alphaSprite->alphaByte, (void*)0xe7);
        }
        rowY = rowY + stride;
        off += 4;
    }
    fn_800FE35C();
}
