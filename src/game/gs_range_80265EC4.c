/**
 * @file gs_range_80265EC4.c
 * @brief gs-engine code, 0x80265EC4 - 0x8026635C (6 fns).
 *
 * Range unit assigned from the propagated subsystem map
 * (tools/subsystem_propagation.py, >=80% single-label dominance;
 * campaign 2026-07-01). The range name stays honest until internal
 * TU structure is proven.
 */
#include "dolphin/types.h"

#include "game/colosseum.h"
#include "game/trainer.h"
#include "game/pokemon.h"

extern void* pokemonGetStatus();
extern u32 savedataGetStatus();
extern s8 fadeCheck(u32 wait);
extern void fadeSet(f32 duration, u32 mode);
extern void _threadSwitch();
extern void msgctrlSetValue();
extern void winMsgOpenField();
extern void winMsgCloseField();
extern void fn_801DB088(void);
extern void* fn_801DE190(u16 index, void* model, u8 variant);
extern BOOL fn_801DDD28(void* owner, u16 group, u16 index, u8 variant);
extern void fn_801DA4E8(void* effect, u32 visible);
extern void fn_801DA9E8(void* sequence, s32 moveID, s32 variant);
extern u8 fn_801DA94C(void* obj, s32 search_key1, s32 search_key2);
extern void floorSetFadeScript();

#if !defined(GS_RANGE_80265EC4_SPLIT) || defined(GS_RANGE_80265EC4_PREFIX)
/* Address: 0x80265EC4 | Size: 0x50 */
void exribbonSetNo(int no, u8 value)
{
    u8* data = (u8*)savedataGetStatus(0, 0x10);

    if (data[no] == 0 && value != 0) {
        data[no] = value;
    }
}

/* Address: 0x80265F14 | Size: 0x38 */
u8 exribbonGetNo(int r3)
{
    int iVar1;

    iVar1 = savedataGetStatus(0, 0x10);
    return *(u8 *)(iVar1 + r3);
}

/* Address: 0x80265F4C | Size: 0x48 */
void exribbonInit(u8* data)
{
    int i;

    if (data == NULL) {
        data = (u8*)savedataGetStatus(0, 0x10);
    }

    for (i = 0; i < 11; i++) {
        *data++ = 0;
    }
}
#endif

#if !defined(GS_RANGE_80265EC4_SPLIT) || defined(GS_RANGE_80265F94_SUFFIX)
extern s8 winMsgCheck(void);

static inline u8 fn_80265F94_msgOpen(void)
{
    if (winMsgCheck() == 1) {
        return 1;
    }
    return 0;
}

static inline void fn_80265F94_update(void* model, u16 id)
{
    fn_801DB088();
    if (fn_801DA94C(model, id, 4) == 0) {
        fn_801DA9E8(model, id, 4);
    }
    _threadSwitch();
}

/* Address: 0x80265F94 | Size: 0x2BC */
void fn_80265F94(int r3)
{
    extern int pokemonDataBiosGetVoice(void* data);
    extern void* pokemonDataBiosGetPtr(u16 species);
    extern s32 fn_801666BC(u16 voice);
    extern void fn_80166A28(u16 voice);
    extern const u16 lbl_8047E6F8;
    extern f32 lbl_8047E6FC;
    u16 voice;
    void* model;
    u16 status;
    u16 id;
    u16 species;
    void* data;
    u32 msg;

    status = (u16)(int)pokemonGetStatus(0, 0xfa, 0x66, 0);
    species = 0xFFFF;
    if (status != 0) {
        species = status;
    }
    model = fn_801DE190(species, 0, 0);
    fn_801DDD28(model, lbl_8047E6F8, 4, 0);
    fn_801DA4E8(model, 1);
    id = lbl_8047E6F8;
    fn_801DA9E8(model, id, 4);
    fadeSet(lbl_8047E6FC, 2);
    while (fadeCheck(0) != 0) {
        fn_80265F94_update(model, id);
    }
    data = pokemonDataBiosGetPtr(0xfa);
    if (data == NULL) {
        voice = 0;
    } else {
        voice = pokemonDataBiosGetVoice(data);
        fn_80166A28(voice);
    }
    while (fn_801666BC(voice) == 2) {
        fn_80265F94_update(model, id);
    }
    winMsgOpenField(0x44ba, 0, 0);
    while (fn_80265F94_msgOpen()) {
        fn_80265F94_update(model, id);
    }
    winMsgCloseField(1);
    switch (r3) {
    case 0:
        msgctrlSetValue(0x5d, 0x3d2);
        msg = 0x44bc;
        break;
    case 1:
        msgctrlSetValue(0x5d, 0x3d2);
        msg = 0x44bb;
        break;
    default:
        msg = 0x44b9;
        break;
    }
    winMsgOpenField(msg, 0, 0);
    while (fn_80265F94_msgOpen()) {
        fn_80265F94_update(model, id);
    }
    winMsgCloseField(1);
    fadeSet(lbl_8047E6FC, 3);
    while (fadeCheck(0) != 0) {
        fn_80265F94_update(model, id);
    }
}

#endif

#if !defined(GS_RANGE_80265EC4_SPLIT) || defined(GS_RANGE_80266250_EXACT)
/* RULE-EXCEPTION(user-approved): redundant masks preserve retail's saved-volume register moves; see docs/RULE_EXCEPTIONS.md. */
/* Address: 0x80266250 | Size: 0xD0 | Ghidra import */
void fn_80266250(void)
{
    extern int fn_800FF660();
    extern u32 lbl_8047B680;
    extern int fn_801653BC();
    extern int fn_801653C4();
    extern u32 fn_801656D8(u32 id);
    extern u32 fn_80165A20(u32 id, u32 volume, u32 priority);
    extern void scriptSoundStop();
    extern u32 fn_801659FC(u32 id, u32 volume, u32 priority);
    extern void fn_801DADC0();
    extern void wazaSequenceSysRelease();
    extern void fn_80265F94();
    u32 uVar5;
    u8 uVar4;
    u8 uVar3;
    u32 iVar2;
    u32 iVar1;

    uVar5 = lbl_8047B680;
    iVar1 = fn_801653C4();
    if (iVar1 != 0) {
        uVar3 = fn_801656D8(iVar1) & 0xFFFFFFFFFFFFFFFFu;
        fn_80165A20(1, 0x32, 0xff);
    } else {
        uVar3 = 0;
    }
    iVar2 = fn_801653BC();
    if (iVar2 != 0) {
        uVar4 = fn_801656D8(iVar2) & 0xFFFFFFFFFFFFFFFFu;
        scriptSoundStop(0x32);
    } else {
        uVar4 = 0;
    }
    fn_801DADC0(1);
    fn_80265F94(uVar5);
    wazaSequenceSysRelease();
    if (iVar1 != 0) {
        fn_80165A20(iVar1, 0x32, uVar3);
    }
    if (iVar2 != 0) {
        fn_801659FC(iVar2, 0x32, uVar4);
    }
    fn_800FF660();
    floorSetFadeScript(0, 0x5960008);
}

/* Address: 0x80266320 | Size: 0x3C */
void d2presentOpen(int r3)
{
    extern void fn_800FF730(u32);
    extern u32 lbl_8047B680;

    lbl_8047B680 = r3;
    fn_800FF730(0x8f);
    floorSetFadeScript((0x596 << 16) | 0x9, 0);
    _threadSwitch();
}
#endif
