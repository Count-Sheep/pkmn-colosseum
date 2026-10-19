/* Standalone copy of fn_8003B478 from pda_range_80037158.c. */
#include "dolphin/types.h"

typedef struct PdaListEntry {
    u16 field_00;
    u16 battleId;
} PdaListEntry;

extern f32 lbl_8047BAC0;
extern u8 lbl_803A6748[];
extern u32 lbl_8047A4D0;
extern PdaListEntry* lbl_8047A4D4;

extern u32 GSmsgGetGSchar(u32 msg);
extern void msgctrlSetValue(s32 id, u32 value);
extern void fn_800FB680(s32 arg0, s32 arg1, s32 arg2, void* data);
extern u32 fn_801EE544(u16 id, u8* variant);
extern u16 fn_801EEFAC(u16 id, s32 side);
extern u8* fn_801EE328(u16 id);
extern u32 fn_801FCC7C(u8* ptr);
extern void* fightTrainerDataBiosGetPtr(u16 slot);
extern s32 fn_8003CE1C(s32 index);
extern s32 fn_801EE614(u16 id);
extern u32 fn_801EE750(u16 id);
extern u8 fn_801EE8F4(u16 id);
extern u8 fn_801EEAD0(u16 id);
extern u8 fn_801EEC74(u16 id);
extern u32 gamedataGetStatus(s32 a, s32 b);
extern u32 pokemonGetSex();
extern void pokemonCreate(u32 work, u16 species, s32 level, u32 trainer);
extern void pokemonBiosSetRnd(u32 work, u32 rnd);
extern void pokemonBiosSetDarkpokemonDataId(u32 work, s32 id);
extern void pokemonBiosSetDp(u32 work, s32 dp);

void fn_8003B478(u8* context)
{
    u32 message;
    u32 msg;
    u32 rnd;
    u32 model;
    s32 battleState;
    u32 battleId;
    u16 species;
    u32 liveModel;
    u8 caught;
    u8 seen;
    u16 curBattleId;
    u32 index;

    context[0x8B] = lbl_8047BAC0 * *(f32*)(lbl_803A6748 + 0x44);
    index = *(u32*)lbl_803A6748;
    species = lbl_8047A4D4[index].field_00;
    battleId = lbl_8047A4D4[index].battleId;
    curBattleId = lbl_8047A4D4[(u16)index].battleId;
    model = lbl_8047A4D0;
    fn_801EE614(curBattleId);
    fn_801EE8F4(curBattleId);
    seen = fn_801EEAD0(curBattleId);
    caught = fn_801EEC74(curBattleId);
    if (seen != 0) {
        if (caught != 0) {
            battleState = 1;
        } else {
            battleState = 0;
        }
    } else {
        battleState = 2;
    }

    pokemonCreate(model, species, 0xA, gamedataGetStatus(0, 1));
    rnd = *(u32*)(lbl_803A6748 + 0x98);
    fn_801EE750(battleId);
    pokemonBiosSetRnd(model, rnd);
    switch (battleState) {
    case 0:
    case 2:
        pokemonBiosSetDarkpokemonDataId(model, 1);
        pokemonBiosSetDp(model, 0xA);
        break;
    case 1:
        pokemonBiosSetDarkpokemonDataId(model, 0);
        pokemonBiosSetDp(model, 0);
        break;
    }

    if ((liveModel = lbl_8047A4D0) != 0) {
        if ((u16)battleId != 0) {
            if (fn_801EE8F4(battleId) != 0) {
                pokemonBiosSetRnd(liveModel, fn_801EE750(battleId));
                message = pokemonGetSex(liveModel);
                pokemonBiosSetRnd(liveModel, *(u32*)(lbl_803A6748 + 0x98));
            } else {
                message = 2;
            }
        } else {
            message = 0x80;
        }
    } else {
        message = 0x80;
    }
    message &= 0xFF;
    if (message == 0x80) {
        msg = GSmsgGetGSchar(1);
    } else {
        switch (message) {
        case 0:
            msg = GSmsgGetGSchar(0xD67);
            break;
        case 1:
            msg = GSmsgGetGSchar(0xD68);
            break;
        case 2:
            msg = GSmsgGetGSchar(0xD69);
            break;
        default:
            msg = GSmsgGetGSchar(0xD68);
            break;
        }
    }
    msgctrlSetValue(0x37, msg);
    {
        u32 alpha = context[0x8B];
        s32 color = alpha | -0x100LL;

        fn_800FB680(0, 0, color, (void*)0xCF);
    }
}
