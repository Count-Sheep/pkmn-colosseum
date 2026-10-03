/**
 * @file menu_middle.c
 * @brief Menu middle code between battle and common (0x80069C0C-0x8007109C)
 *
 * Address range: 0x80069C0C - 0x8007109C
 * Total functions: 100
 */

#include "dolphin/types.h"
#include "game/menu/menu_middle.h"

#if !defined(MENU_MIDDLE_RESIDUAL_80069C0C_ONLY) && \
    !defined(MENU_MIDDLE_EXACT_8006A65C_ONLY) && \
    !defined(MENU_MIDDLE_EXACT_8006A824_ONLY) && \
    !defined(MENU_MIDDLE_RESIDUAL_8006A990_ONLY) && \
    !defined(MENU_MIDDLE_EXACT_8006AC28_ONLY) && \
    !defined(MENU_MIDDLE_RESIDUAL_8006ACCC_ONLY) && \
    !defined(MENU_MIDDLE_EXACT_8006ADB4_ONLY) && \
    !defined(MENU_MIDDLE_RESIDUAL_8006AE18_ONLY) && \
    !defined(MENU_MIDDLE_EXACT_8006AEEC_ONLY) && \
    !defined(MENU_MIDDLE_RESIDUAL_8006AF44_ONLY) && \
    !defined(MENU_MIDDLE_EXACT_8006AFC4_ONLY) && \
    !defined(MENU_MIDDLE_RESIDUAL_8006AFE4_ONLY) && \
    !defined(MENU_MIDDLE_EXACT_8006B09C_ONLY) && \
    !defined(MENU_MIDDLE_RESIDUAL_8006B154_ONLY) && \
    !defined(MENU_MIDDLE_EXACT_8006B1C0_ONLY) && \
    !defined(MENU_MIDDLE_RESIDUAL_8006B2A4_ONLY) && \
    !defined(MENU_MIDDLE_EXACT_8006B354_ONLY) && \
    !defined(MENU_MIDDLE_RESIDUAL_8006B420_ONLY) && \
    !defined(MENU_MIDDLE_EXACT_8006B4AC_ONLY) && \
    !defined(MENU_MIDDLE_RESIDUAL_8006B5D0_ONLY) && \
    !defined(MENU_MIDDLE_EXACT_8006B8E8_ONLY) && \
    !defined(MENU_MIDDLE_RESIDUAL_8006B9B8_ONLY) && \
    !defined(MENU_MIDDLE_EXACT_8006B9B8_ONLY) && \
    !defined(MENU_MIDDLE_EXACT_8006BB34_ONLY) && \
    !defined(MENU_MIDDLE_EXACT_8006FBFC_ONLY) && \
    !defined(MENU_MIDDLE_EXACT_8006FCF8_ONLY) && \
    !defined(MENU_MIDDLE_RESIDUAL_8006FEE4_ONLY) && \
    !defined(MENU_MIDDLE_EXACT_80070274_ONLY) && \
    !defined(MENU_MIDDLE_RESIDUAL_80070318_ONLY) && \
    !defined(MENU_MIDDLE_EXACT_800704A4_ONLY) && \
    !defined(MENU_MIDDLE_RESIDUAL_800704AC_ONLY) && \
    !defined(MENU_MIDDLE_RESIDUAL_80070D84_ONLY)
#define MENU_MIDDLE_ALL
#endif

/* ===== External function declarations ===== */
extern void menuSubGetPokemonSexForDisp();
extern void* menuSeqBiosGetPtr();
extern void menuSpriteBiosGetPtr();
extern void menuItemBiosSetSelectFlag();
extern void fn_80071160();
extern void fn_80071208();
extern void fn_80071318();
extern s32 fn_8007162C(void);
extern void menuCB_InitMenu();
extern void menuCBRule_CheckPokemonErrorAll();
extern void fn_80076398();
extern void fn_800767B8();
extern void fn_80076A8C();
extern void fn_80076F2C();
extern void fn_800772AC();
extern void fn_800774D4();
extern void fn_80077A5C();
extern u8 fn_80077BD0(void);
extern void menuCBRule_CheckValidItem();
extern void fn_80077C68();
extern void fn_80077D88();
extern s32 fn_80077DB8();
extern void menuCBRule_ConstantRule();
extern void fn_80088EA8();
extern void sprintf();
extern void fmod();
extern void fn_800D5648();
extern void fn_800D5BA0();
extern void fn_800D61E4();
extern void fn_800D6728();
extern void fn_800D67BC();
extern void fn_800D6A00();
extern void fn_800D7820();
extern void fn_800D888C();
extern void fn_800D88DC();
extern u16 fn_800E0C54();
extern void _threadSwitch();
extern void GScharMakeFromSJIS();
extern u32 GSmsgGetGSchar();
extern void GSmsgGetRect();
extern void fn_800FB680();
extern void fn_800FE35C();
extern void fn_800FE38C();
extern void fn_800FF730();
extern s32 menuGetCursorFromItemID(s32 id, s32 itemId);
extern s32 menuGetCursorItemID(s32 id);
extern void menuSetCursor(s32 id, s32 cursor);
extern void menuSetPosition();
extern void menuButtonNormal();
extern void menuCursorNormal(void* menu);
extern void* windowGetParam(void* menu, s32 idx);
extern void fn_801044D0(s32 id, void* arg);
extern s32 windowGetActiveID(void);
extern void windowSearchItemID();
extern void* windowSearchID(s32 id);
extern void windowCreateCursorSprite();
extern void* windowGetKeyInfo(void);
extern void fn_80107F38();
extern void fn_801081F8();
extern void winSetSequence();
/* ... and 50 more external functions */
extern void* memset(void* dst, int val, u32 size);
extern void* memcpy(void* dst, const void* src, u32 size);

/* ===== SDA globals ===== */
extern u8 lbl_80478938;
extern u8 lbl_80478F20;
extern u8 lbl_8047A5A4;
extern u8 lbl_8047A5D8;
extern u8 lbl_8047A5E0;
extern u32 lbl_8047A5E8;
extern u32 lbl_8047A5EC;
extern s32 lbl_8047A5F0;
extern u32 lbl_8047A5F4;
extern u8 lbl_8047A5F8;
extern u8 lbl_8047A5FC;
extern u8 lbl_8047C028;
extern u8 lbl_8047C030;
extern u8 lbl_8047C038;
extern u8 lbl_8047C040;
extern u8 lbl_8047C048;
extern u8 lbl_8047C050;
extern u8 lbl_8047C058;
extern u8 lbl_8047C060;
extern u8 lbl_8047C064;
extern u8 lbl_8047C068;
extern u8 lbl_8047C070;
extern u8 lbl_8047C078;
extern u8 lbl_8047C080;
extern u8 lbl_8047C088;
extern u8 lbl_8047C08C;
extern u8 lbl_8047E708;

/* ===== Rodata / data labels ===== */
extern u8 jumptable_802EDE78[];
extern u8 jumptable_802EDEFC[];
extern u8 jumptable_802EDF20[];
extern u8 jumptable_802EDFB0[];
extern u8 jumptable_802EDFCC[];
extern u8 jumptable_802EE06C[];
extern u8 jumptable_802EE0F0[];
extern u8 jumptable_802EE20C[];
extern u8 jumptable_802EE31C[];
extern u8 lbl_80267C18[];
extern u8 lbl_80267DD8[];
extern u8 lbl_80267DE8[];
extern u8 lbl_80267E70[];
extern u8 lbl_80267EA8[];
extern u8 lbl_80267F68[];
extern u8 lbl_80267FE8[];
extern u8 lbl_80268184[];
extern u8 lbl_802681B4[];
extern u8 lbl_80268234[];
extern u8 lbl_80268424[];
extern u8 lbl_80268560[];
extern u8 lbl_80268574[];
extern u8 lbl_8026858C[];
extern u8 lbl_8026860C[];
extern u8 lbl_8026864C[];
extern u8 lbl_80268674[];
extern u8 lbl_80268680[];
extern u8 lbl_802686D0[];
extern u8 lbl_802EDE58[];
extern u8 lbl_802EE618[];
extern u8 lbl_80314E08[];
extern u8 lbl_803B6D68[];

/* ===== Forward declarations ===== */
s32 fn_80069C0C(void* arg0);
u16 fn_8006A65C(void);
u16 fn_8006A718(s32 idx);
u8 fn_8006A76C(void);
void fn_8006A79C(u8* p);
void fn_8006A7AC(u8* p);
u8 fn_8006A7BC(u8* p);
u32 fn_8006A7C8(u32 r3);
u16 fn_8006A7D0(u32 r3);
u16 fn_8006A7D8(u32 r3);
void fn_8006A7E0(void* r3, u32 r4);
u32 fn_8006A7E8(u32 r3);
void fn_8006A7F0(void* dst, const void* src);
u32 fn_8006A814(u32 r3);
void fn_8006A81C(void* r3, u32 r4);
void fn_8006A824(u32 r3, u32 r4);
void fn_8006A990(void* destination, const void* heroSource, u16 trainerId);
void fn_8006AABC(void* destination, u16 trainerId);
void menuCBBios_InitTrainer(void* p, u16 value);
s32 fn_8006AC6C(u32 id);
u8* fn_8006ACCC(s32 id);
void fn_8006ADB4(s32 value);
s32 fn_8006ADEC(void);
s32 fn_8006AE18(void);
u8* fn_8006AEEC(void);
void fn_8006AF44(u8* base, void* src);
u8* fn_8006AFC4(u8* p);
u8* fn_8006AFE4(s32 id);
u8* fn_8006B09C(s32 index);
u8* fn_8006B0F8(s32 index);
s32 menuCBBios_ControlerIDtoPortID(s32 id);
u32 fn_8006B1C0(s32 i);
void fn_8006B1D4(void);
u32 fn_8006B1F4(s32 index, s32 slot);
void fn_8006B2A4(s32 idx, s32 sub);
void fn_8006B354(s32 index);
u32 fn_8006B3C8(s32 index);
void* fn_8006B420(void);
void fn_8006B4AC(s32 value);
u8* fn_8006B51C(s32 index);
s32 fn_8006B57C(void);
s32 fn_8006B5A8(void);
void fn_8006B5D0(MenuMiddleWork* work);
void fn_8006B6B4(void* saveSection);
u8 fn_8006B8E8(void);
void fn_8006B8F0(void);
void fn_8006B8FC(void);
void fn_8006B908(u32 r3);
void fn_8006B930(void* menu);
void fn_8006B9B8(void* menu);
void fn_8006BB34(void* menu);
void fn_8006C018(void* menu);
void fn_8006C0DC(void* menu);
void fn_8006C164(void* menu);
void fn_8006C5D8(void* window, void* sprite);
void fn_8006C7D4(void* arg0, void* item);
void fn_8006CCC0(void* window, struct MenuMiddleSprite_8006CCC0* sprite);
void fn_8006D550(void* window, struct MenuMiddleSprite_8006CCC0* sprite);
void fn_8006D940(void* menu);
void fn_8006D98C(void* menu);
void fn_8006DAE4(void* arg0);
void fn_8006DC28(void* menu);
void fn_8006E0CC(void);
u32 fn_8006E128(u8* p);
void fn_8006E160(u32 r3);
void fn_8006E188(void);
void fn_8006E18C(void* menu);
void fn_8006E258(void* menu);
void fn_8006E338(void* obj);

typedef struct KeyInfo_8006BB34 {
    u8 pad0[4];
    u16 flags4;
    u16 flags6;
    u8 padA[2];
} KeyInfo_8006BB34;
void fn_8006E798(void* menu);
void fn_8006E9A4(void* window, void* sprite);
void fn_8006EE7C(void* menu);
void fn_8006EF24(void* menu);
void fn_8006EFF8(void* menu);
void fn_8006F284(void* menu);
void fn_8006F720(void* menu);
void fn_8006FBFC(void* menu);
void fn_8006FCF8(u32 r3);
void fn_8006FD24(u32 r3);
void fn_8006FD4C(u32 r3);
void fn_8006FD74(u32 r3);
void fn_8006FD9C(u32 r3);
void fn_8006FDC4(u32 r3);
void fn_8006FDEC(u32 r3);
void fn_8006FE14(u32 r3);
void fn_8006FE3C(u32 r3);
void fn_8006FE64(void* menu);
void fn_8006FEE4(void* menu);
void fn_80070274(u32 r3);
void fn_8007029C(u32 r3);
void fn_800702C8(u32 r3);
void fn_800702F0(u32 r3);
void fn_80070318(void* menu);
void fn_80070428(void* arg0, void* menu);
void fn_800704A4(void);
void fn_800704A8(void);
void fn_800704AC(void* menu, void* sprite);
void fn_800706C4(void* menu, void* sprite);
void fn_80070A9C(void* menu, void* sprite);
typedef struct MenuMiddleSeqPair {
    u16 open;
    u16 close;
} MenuMiddleSeqPair;

typedef struct MenuMiddleItem {
    void* next;        /* 0x00 */
    u8 pad_04[8];
    u32 window;        /* 0x0C */
    u8 pad_10[0x36];
    u8 disabled;       /* 0x46 */
    u8 pad_47[9];
    s16 x;             /* 0x50 */
    s16 y;             /* 0x52 */
} MenuMiddleItem;

typedef struct MenuMiddleEntry {
    u16 id;            /* 0x00 */
    u8 pad_02[2];
    u32 kind;          /* 0x04 */
} MenuMiddleEntry;

typedef struct MenuMiddleMenu {
    u8 pad_00;
    s8 state;          /* 0x01 */
    s8 done;           /* 0x02 */
    u8 pad_03[0x19];
    MenuMiddleItem* items;    /* 0x1C */
    MenuMiddleItem* windows;  /* 0x20 */
} MenuMiddleMenu;

s32 fn_80070D84(MenuMiddleMenu* mm, MenuMiddleEntry* list, u32 count);

/* ===== Function implementations ===== */


#if defined(MENU_MIDDLE_ALL) || defined(MENU_MIDDLE_RESIDUAL_80069C0C_ONLY)
/* menuCB_Battle.c */

typedef struct MenuCBPlayer {
    u16 trainerId;      /* 0x0000 */
    u16 side;           /* 0x0002 */
    u8 pad04[0x20];
    s32 inputDevice;    /* 0x0024 */
    s32 slot;           /* 0x0028 */
    u8 hero[0xB18];     /* 0x002C */
    u8 rental[0xB1C];   /* 0x0B44 */
} MenuCBPlayer;

typedef struct MenuCBBattle {
    s32 m_eBattleMode;  /* 0x00 */
    s32 m_eBattleType;  /* 0x04 */
    s32 unk08;
    s32 m_eColosseum;   /* 0x0C */
    s32 cpuRank;        /* 0x10 */
    u32 m_nBattleCount; /* 0x14 */
    u8 pad18[4];
    u8 ready;           /* 0x1C */
    u8 pad1D[7];
    MenuCBPlayer trainers[4]; /* 0x0024 */
    u8 pad59A4[4];
    MenuCBPlayer players[4];  /* 0x59A8 */
} MenuCBBattle;

static inline u16 menuCB_GetColosseumBattleTrainerID(u8* data, s32 type, s32 colosseum, u32 count) {
    extern void __assert(const char* file, s32 line, const char* expr);
    u32 n;

    if (type != 0 && type != 1) {
        return 0;
    }
    switch (colosseum) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
        if (count >= 8) {
            return 0;
        }
        n = colosseum * 16 + type * 8 + count;
        break;
    case 6:
        if (count >= 100) {
            return 0;
        }
        n = 0x60 + type * 100 + count;
        break;
    }
    if (!(*(u32*)&lbl_80478938 > n)) {
        __assert((char*)(data + 0x7C), 0xCA, (char*)(data + 0x174));
    }
    return ((u16*)lbl_802EE618)[n];
}

/* 0x80069C0C | size: 0xA50 */
s32 fn_80069C0C(void* arg0) {
    extern void* fightEncountDataBiosGetPtr(u16 id);
    extern void fightEncountDataBiosSetBgmSndId(void* enc, s32 id);
    extern void fightEncountDataBiosSetFightFloorDataId(void* enc, s32 id);
    extern void fightEncountDataBiosSetFightKind(void* enc, s32 kind);
    extern void fightEncountDataBiosSetSyoukaiWzxDataId(void* enc, s32 id);
    extern void fightEncountDataBiosSetTrainer(void* enc, s32 trainer);
    extern void fightEncountDataBiosSetFightTrainerDataId(void* enc, s32 slot, u16 id);
    extern void fightEncountDataBiosSetGSInputDevice(void* enc, s32 slot, s32 device);
    extern void fightTrainerCreateFightTrainerDataIdToHero(u16 id, u32 arg1, void* hero);
    extern u8* heroBiosGetPokemonPtr(void* hero, u16 index);
    extern void heroBiosSetNamePtr(void* hero, u16* name);
    extern void heroBiosCopy(void* dst, void* src);
    extern void pokemonBiosSetItemDataId(u8* pokemon, u16 item);
    extern u8 pokemonCheckValid(u8* pokemon);
    extern u8 pokemonBiosGetLevel(u8* pokemon);
    extern u16 pokemonBiosGetPokemonDataId(u8* pokemon);
    extern void* pokemonDataBiosGetPtr(u16 id);
    extern u8 pokemonDataBiosGetGrowDataId(void* data);
    extern void* pokemonGrowDataBiosGetPtr(u8 id);
    extern u32 pokemonGrowDataBiosGetExp(void* grow, u8 level);
    extern void pokemonBiosSetExp(u8* pokemon, u32 exp);
    extern void pokemonResetBasisStatus(u8* pokemon);
    extern void __assert(const char* file, s32 line, const char* expr);
    u16 name[0x40];
    u8 hero[0xB18];
    MenuCBBattle* p;
    u8* data;
    void* enc;
    u16 encountId;
    s32 bgm;
    s32 floor;
    s32 nquant;
    u8* cpu;
    u16 first;
    u16 second;
    u16 i;
    u32 maxLevel;
    u8* pokemon;
    u8* rental;
    u16* src;
    u16* dst;
    s32 j;

    p = arg0;
    data = lbl_80267C18;
    encountId = 0;
    switch (p->m_eBattleType) {
    case 0:
        encountId = 0x20A;
        break;
    case 1:
        encountId = 0x20B;
        break;
    case 2:
        encountId = 0x20C;
        break;
    }
    if (!(encountId != 0)) {
        __assert((char*)(data + 0x7C), 0xF8, (char*)(data + 0x8C));
    }
    enc = fightEncountDataBiosGetPtr(encountId);

    switch (p->m_eBattleMode) {
    case 0:
        switch (p->m_nBattleCount) {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
            bgm = 0x1F;
            break;
        case 7:
        default:
            switch (p->m_eColosseum) {
            case 0:
            case 1:
            case 2:
            case 4:
                bgm = 0x20;
                break;
            case 3:
            case 5:
            case 6:
            default:
                bgm = 0x21;
                break;
            }
            break;
        }
        break;
    case 1:
        if (p->m_nBattleCount < 30) {
            bgm = 0x1A;
        } else if (p->m_nBattleCount < 60) {
            bgm = 0x17;
        } else if (p->m_nBattleCount < 99) {
            bgm = 0x18;
        } else {
            bgm = 0x3D5;
        }
        break;
    default:
        bgm = ((u32*)data)[(*(u32*)&lbl_8047A5D8)++];
        *(u32*)&lbl_8047A5D8 = *(u32*)&lbl_8047A5D8 % 3;
        break;
    }
    fightEncountDataBiosSetBgmSndId(enc, bgm);

    if (p->m_eBattleMode == 1 && p->m_eColosseum == 6) {
        if (p->m_nBattleCount < 30) {
            floor = 0x28;
        } else if (p->m_nBattleCount < 60) {
            floor = 0x29;
        } else if (p->m_nBattleCount < 99) {
            floor = 0x2A;
        } else {
            floor = 0x2E;
        }
    } else {
        if (!(7u > p->m_eColosseum)) {
            __assert((char*)(data + 0x7C), 0x166, (char*)(data + 0xB8));
        }
        floor = ((u16*)(data + 0xC))[p->m_eColosseum];
    }
    fightEncountDataBiosSetFightFloorDataId(enc, floor);

    switch (p->m_eBattleMode) {
    case 0:
        if (!(8 > p->m_nBattleCount)) {
            __assert((char*)(data + 0x7C), 0x17F, (char*)(data + 0xE0));
        }
        if (p->m_nBattleCount < 7) {
            fightEncountDataBiosSetFightKind(enc, 0xD);
        } else {
            fightEncountDataBiosSetFightKind(enc, 0xE);
        }
        fightEncountDataBiosSetSyoukaiWzxDataId(enc, ((u32*)(data + 0x1C))[p->m_nBattleCount]);
        break;
    case 1:
        if (p->m_nBattleCount < 99) {
            fightEncountDataBiosSetFightKind(enc, 0xF);
        } else {
            fightEncountDataBiosSetFightKind(enc, 0x12);
        }
        fightEncountDataBiosSetSyoukaiWzxDataId(enc, 0);
        break;
    case 2:
        fightEncountDataBiosSetFightKind(enc, 0x10);
        fightEncountDataBiosSetSyoukaiWzxDataId(enc, 0);
        break;
    case 3:
    default:
        fightEncountDataBiosSetFightKind(enc, 0xC);
        fightEncountDataBiosSetSyoukaiWzxDataId(enc, 0);
        break;
    }

    nquant = fn_80077DB8();
    if (nquant == 6) {
        switch (p->m_eBattleType) {
        case 1:
            fightEncountDataBiosSetTrainer(enc, 1);
            break;
        case 2:
            fightEncountDataBiosSetTrainer(enc, 2);
            break;
        case 0:
        default:
            fightEncountDataBiosSetTrainer(enc, 0);
            break;
        }
    } else {
        switch (p->m_eBattleType) {
        case 1:
            if (!(4 == nquant)) {
                __assert((char*)(data + 0x7C), 0x1C0, (char*)(data + 0x108));
            }
            fightEncountDataBiosSetTrainer(enc, 5);
            break;
        case 2:
            if (!(2 == nquant)) {
                __assert((char*)(data + 0x7C), 0x1C5, (char*)(data + 0x114));
            }
            fightEncountDataBiosSetTrainer(enc, 6);
            break;
        case 0:
        default:
            if (!(3 == nquant)) {
                __assert((char*)(data + 0x7C), 0x1CB, (char*)(data + 0x120));
            }
            fightEncountDataBiosSetTrainer(enc, 4);
            break;
        }
    }

    switch (p->m_eBattleMode) {
    case 3:
        if (!(2u > p->m_eBattleType)) {
            __assert((char*)(data + 0x7C), 0x221, (char*)(data + 0x12C));
        }
        if (!(4u > p->m_eBattleMode)) {
            __assert((char*)(data + 0x7C), 0x222, (char*)(data + 0x150));
        }
        cpu = ((u8(*)[4][8])(data + 0x3C))[p->m_eBattleType][p->cpuRank];
        first = menuCB_GetColosseumBattleTrainerID(data, p->m_eBattleType, 6, cpu[fn_800E0C54() & 7] - 1);
        do {
            second = menuCB_GetColosseumBattleTrainerID(data, p->m_eBattleType, 6, cpu[fn_800E0C54() & 7] - 1);
        } while (first == second);
        fn_8006AABC(&p->trainers[1], first);
        fn_8006A81C(&p->trainers[1], 0);
        for (i = 0; i < 6; i++) {
            pokemonBiosSetItemDataId(heroBiosGetPokemonPtr(p->trainers[1].hero, i), 0);
        }
        for (i = 0; i < 6; i++) {
            pokemonBiosSetItemDataId(heroBiosGetPokemonPtr(p->trainers[1].rental, i), 0);
        }
        src = (u16*)GSmsgGetGSchar(((u16*)&lbl_8047C028)[(u32)fn_800E0C54() % 3]);
        dst = name;
        for (; *src != 0; src++, dst++) {
            *dst = *src;
        }
        *dst = 0;
        fightTrainerCreateFightTrainerDataIdToHero(second, fn_8006B1C0(0), hero);
        heroBiosSetNamePtr(hero, name);
        for (i = 0; i < 6; i++) {
            pokemonBiosSetItemDataId(heroBiosGetPokemonPtr(hero, i), 0);
        }
        fn_8006A990(&p->trainers[0], hero, 1);
        fn_8006A81C(&p->trainers[0], fn_8006B1C0(0));
        fn_8006A7E0(&p->trainers[0], 0);
        break;
    case 0:
    case 1:
        fn_8006AABC(&p->players[1],
                    menuCB_GetColosseumBattleTrainerID(data, p->m_eBattleType, p->m_eColosseum, p->m_nBattleCount));
        fn_8006A7F0(&p->players[0], fn_8006AFC4((u8*)p));
        if (p->m_eBattleMode == 1) {
            maxLevel = 0;
            for (j = 0; j < 6; j++) {
                pokemon = heroBiosGetPokemonPtr(p->players[0].rental, j);
                if (pokemonCheckValid(pokemon) && maxLevel < pokemonBiosGetLevel(pokemon)) {
                    maxLevel = pokemonBiosGetLevel(pokemon);
                }
            }
            if (maxLevel > 100) {
                maxLevel = 100;
            }
            rental = p->players[1].rental;
            for (j = 0; j < 6; j++) {
                pokemon = heroBiosGetPokemonPtr(rental, j);
                if (pokemonCheckValid(pokemon) && maxLevel > pokemonBiosGetLevel(pokemon)) {
                    pokemonBiosSetExp(pokemon,
                                      pokemonGrowDataBiosGetExp(
                                          pokemonGrowDataBiosGetPtr(pokemonDataBiosGetGrowDataId(
                                              pokemonDataBiosGetPtr(pokemonBiosGetPokemonDataId(pokemon)))),
                                          maxLevel));
                    pokemonResetBasisStatus(pokemon);
                }
            }
            heroBiosCopy(p->players[1].hero, rental);
        }
        fn_8006A7F0(&p->trainers[0], &p->players[0]);
        fn_8006A7F0(&p->trainers[1], &p->players[1]);
        fn_8006A81C(&p->trainers[0], fn_8006B1C0(0));
        fn_8006A7E0(&p->trainers[0], 0);
        p->players[0].side = 0;
        p->trainers[0].side = 0;
        p->players[1].side = 1;
        p->trainers[1].side = 1;
        break;
    case 2:
        switch (p->m_eBattleType) {
        case 0:
        case 1:
            p->players[0].slot = 0;
            p->players[1].slot = 1;
            p->players[2].slot = 2;
            p->players[3].slot = 3;
            fn_8006A7F0(&p->trainers[0], &p->players[0]);
            fn_8006A7F0(&p->trainers[1], &p->players[1]);
            break;
        case 2:
            fn_8006A7F0(&p->trainers[0], &p->players[p->players[0].slot]);
            fn_8006A7F0(&p->trainers[1], &p->players[p->players[1].slot]);
            fn_8006A7F0(&p->trainers[2], &p->players[p->players[2].slot]);
            fn_8006A7F0(&p->trainers[3], &p->players[p->players[3].slot]);
            break;
        default:
            __assert((char*)(data + 0x7C), 0x291, (char*)&lbl_8047C030);
            break;
        }
        break;
    }

    switch (p->m_eBattleType) {
    case 2:
        fightEncountDataBiosSetFightTrainerDataId(enc, 2, p->trainers[2].trainerId);
        fightEncountDataBiosSetFightTrainerDataId(enc, 3, p->trainers[3].trainerId);
        fightEncountDataBiosSetGSInputDevice(enc, 2, p->trainers[2].inputDevice);
        fightEncountDataBiosSetGSInputDevice(enc, 3, p->trainers[3].inputDevice);
    case 0:
    case 1:
        fightEncountDataBiosSetFightTrainerDataId(enc, 0, p->trainers[0].trainerId);
        fightEncountDataBiosSetFightTrainerDataId(enc, 1, p->trainers[1].trainerId);
        fightEncountDataBiosSetGSInputDevice(enc, 0, p->trainers[0].inputDevice);
        fightEncountDataBiosSetGSInputDevice(enc, 1, p->trainers[1].inputDevice);
        break;
    }
    p->ready = 1;
    return 0;
}
#endif


#if defined(MENU_MIDDLE_ALL) || defined(MENU_MIDDLE_EXACT_8006A65C_ONLY)
/* 0x8006A65C | size: 0xBC */
u16 fn_8006A65C(void) {
    extern void* savedataGetStatus(int, int);
    extern void scriptSoundStop(int);
    extern void fn_80088EA8(void*);
    extern u32 fn_801906A0(int);
    extern s32 fn_80069C0C(void*);
    extern void fn_800FF730(int);
    extern void _threadSwitch(void);
    void* menuPtr;
    u32 x;

    menuPtr = (u8*)savedataGetStatus(0, 0xe) + 0xC9A8;
    scriptSoundStop(0x3e8);
    fn_80088EA8(menuPtr);
    x = fn_801906A0(0xb59);
    MENU_MIDDLE_U32_0014(savedataGetStatus(0, 0xe))->unk_0014 = x;
    MENU_MIDDLE_U32_000C(savedataGetStatus(0, 0xe))->unk_000C = 6;
    MENU_MIDDLE_U32_0000(savedataGetStatus(0, 0xe))->unk_0000 = 1;
    fn_80069C0C(savedataGetStatus(0, 0xe));
    fn_800FF730(0x397);
    _threadSwitch();
    return (u16) * (u32*)((u8*)savedataGetStatus(0, 0xe) + 0x20);
}


/* 0x8006A718 | size: 0x54 */
u16 fn_8006A718(s32 idx) {
    extern u8* savedataGetStatus(s32 idx, s32 type);
    u8* p;
    u16 value;

    p = savedataGetStatus(idx, 0xE) + 0x10000;
    if (MENU_MIDDLE_NEG_U8_C988(p)->unk_C988 != 0) {
        p -= 0x4cd8;
    } else {
        p = 0;
    }
    if (p != 0) {
        value = MENU_MIDDLE_U16_0000(p)->unk_0000;
    } else {
        value = 0;
    }
    return value;
}


/* 0x8006A76C | size: 0x30 */
u8 fn_8006A76C(void) {
    extern u8 fn_801D04E8(void);
    return (u8)((fn_801D04E8() & 0xFF) == 0);
}

/* 0x8006A79C | size: 0x10 */
void fn_8006A79C(u8* p) {
    p[0xC98B] = 0;
}

/* 0x8006A7AC | size: 0x10 */
void fn_8006A7AC(u8* p) {
    p[0xC98B] = 1;
}

/* 0x8006A7BC | size: 0xC */
u8 fn_8006A7BC(u8* p) {
    return p[0xC98B];
}

/* 0x8006A7C8 | size: 0x8 */
u32 fn_8006A7C8(u32 r3) {
    return r3 + 0xb44;
}

/* 0x8006A7D0 | size: 0x8 */
u16 fn_8006A7D0(u32 r3) {
    return MENU_MIDDLE_U16_0000(r3)->unk_0000;
}

/* 0x8006A7D8 | size: 0x8 */
u16 fn_8006A7D8(u32 r3) {
    return MENU_MIDDLE_U16_0002(r3)->unk_0002;
}

/* 0x8006A7E0 | size: 0x8 */
void fn_8006A7E0(void* r3, u32 r4) {
    MENU_MIDDLE_U32_0004(r3)->unk_0004 = r4;
}

/* 0x8006A7E8 | size: 0x8 */
u32 fn_8006A7E8(u32 r3) {
    return MENU_MIDDLE_U32_0004(r3)->unk_0004;
}

/* 0x8006A7F0 | size: 0x24 */
void fn_8006A7F0(void* dst, const void* src) {
    memcpy(dst, src, 0x1660);
}

/* 0x8006A814 | size: 0x8 */
u32 fn_8006A814(u32 r3) {
    return MENU_MIDDLE_U32_0024(r3)->unk_0024;
}

/* 0x8006A81C | size: 0x8 */
void fn_8006A81C(void* r3, u32 r4) {
    MENU_MIDDLE_U32_0024(r3)->unk_0024 = r4;
}
#endif

#if defined(MENU_MIDDLE_ALL) || defined(MENU_MIDDLE_EXACT_8006A824_ONLY)
/* 0x8006A824 | size: 0x16C */
void fn_8006A824(u32 r28, u32 r29) {
    extern void fn_8006A990();
    extern u8 heroBiosGetHomePlace(u32);
    extern u8 heroBiosGetSexDataId(u32);
    extern void __assert(u32, u32, u32);
    u32 r0 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    r31 = (u32)&lbl_80267DD8;
    r30 = 0x1;
    r0 = heroBiosGetHomePlace(r29) & 0xFF;
    switch ((s32)r0) {
        case 0:
            r30 = 0x1;
            r0 = heroBiosGetSexDataId(r29) & 0xFF;
            if (r0 != (u32)0x0) {
                __assert(r31 + 0x10, 0x281, r31 + 0x20);
            }
            break;
        case 1:
            r0 = heroBiosGetSexDataId(r29) & 0xFF;
            switch ((s32)r0) {
                case 0: r30 = 0x2; break;
                case 1: r30 = 0x3; break;
                case 2:
                default:
                    __assert(r31 + 0x10, 0x28a, r31 + 0x4c);
                    break;
            }
            break;
        case 2:
            r0 = heroBiosGetSexDataId(r29) & 0xFF;
            switch ((s32)r0) {
                case 0: r30 = 0x309; break;
                case 1: r30 = 0x308; break;
                case 2:
                default:
                    __assert(r31 + 0x10, 0x294, r31 + 0x4c);
                    break;
            }
            break;
        default:
            __assert(r31 + 0x10, 0x299, r31 + 0x60);
            break;
    }
    fn_8006A990(r28, r29, r30);
    return;
}
#endif

#if defined(MENU_MIDDLE_ALL) || defined(MENU_MIDDLE_RESIDUAL_8006A990_ONLY)
/* 0x8006A990 | size: 0x12C */
void fn_8006A990(void* destination, const void* heroSource, u16 trainerId) {
    extern void pokemonAllKaihuku(void* pokemon);
    extern void heroBiosSetHizukiFlag(void* hero, s32 flag);
    extern void* heroBiosGetPokemonPtr(void* hero, u16 index);
    extern void heroBiosCopy(void* destination, const void* source);
    extern u8 fn_80077A5C(void* pokemon);
    u8 hero[0xB18];
    void* pokemon;
    u16 savedId;
    u32 kind;
    s32 i;

    heroBiosCopy(hero, heroSource);
    heroBiosSetHizukiFlag(hero, 0);
    for (i = 0; (u16)i < 6; i++) {
        pokemon = heroBiosGetPokemonPtr(hero, (u16)i);
        if (fn_80077A5C(pokemon) == 0) {
            pokemonAllKaihuku(pokemon);
        }
    }

    savedId = *(u16*)((u8*)destination + 2);
    memset(destination, 0, 0x1660);
    *(u16*)((u8*)destination + 2) = savedId;
    heroBiosCopy((u8*)destination + 0x2C, hero);
    heroBiosCopy((u8*)destination + 0xB44, hero);
    *(u16*)destination = trainerId;

    if (*(u32*)*(u32*)&lbl_80478F20 <= trainerId) {
        kind = -1;
    } else if (trainerId >= 1 && trainerId < 9) {
        kind = 1;
    } else if (trainerId >= 0x308 && trainerId < 0x30A) {
        kind = 2;
    } else {
        kind = 0;
    }
    *(u32*)((u8*)destination + 4) = kind;
}


/* 0x8006AABC | size: 0x16C */
void fn_8006AABC(void* destination, u16 trainerId) {
    extern void pokemonAllKaihuku(void* pokemon);
    extern u8 heroCheckValid(void* hero);
    extern void heroBiosSetHizukiFlag(void* hero, s32 flag);
    extern void* heroBiosGetPokemonPtr(void* hero, u16 index);
    extern void heroBiosCopy(void* destination, const void* source);
    extern void fightTrainerCreateFightTrainerDataIdToHero(
        u16 trainerId, u32 fightTrainerId, void* hero);
    extern u8 fn_80077A5C(void* pokemon);
    u8 hero[0xB18];
    void* pokemon;
    u16 savedId;
    u32 kind;
    s32 i;

    heroBiosCopy(hero, (u8*)destination + 0xB44);
    fightTrainerCreateFightTrainerDataIdToHero(
        trainerId, *(u32*)lbl_80267DD8, hero);
    if (heroCheckValid(hero) == 0) {
        __assert(lbl_80267DD8 + 0x10, 0x258, lbl_80267DD8 + 0x7C);
    }
    heroBiosSetHizukiFlag(hero, 0);

    for (i = 0; (u16)i < 6; i++) {
        pokemon = heroBiosGetPokemonPtr(hero, (u16)i);
        if (fn_80077A5C(pokemon) == 0) {
            pokemonAllKaihuku(pokemon);
        }
    }

    savedId = *(u16*)((u8*)destination + 2);
    memset(destination, 0, 0x1660);
    *(u16*)((u8*)destination + 2) = savedId;
    heroBiosCopy((u8*)destination + 0x2C, hero);
    heroBiosCopy((u8*)destination + 0xB44, hero);
    *(u16*)destination = trainerId;

    if (*(u32*)*(u32*)&lbl_80478F20 <= trainerId) {
        kind = -1;
    } else if (trainerId >= 1 && trainerId < 9) {
        kind = 1;
    } else if (trainerId >= 0x308 && trainerId < 0x30A) {
        kind = 2;
    } else {
        kind = 0;
    }
    *(u32*)((u8*)destination + 4) = kind;
}
#endif

#if defined(MENU_MIDDLE_ALL) || defined(MENU_MIDDLE_EXACT_8006AC28_ONLY)
/* 0x8006AC28 | size: 0x44 */
void menuCBBios_InitTrainer(void* p, u16 value) {
    memset(p, 0, 0x1660);
    MENU_MIDDLE_U16_0002(p)->unk_0002 = value;
}


/* 0x8006AC6C | size: 0x60 */
s32 fn_8006AC6C(u32 r3) {
    u32 r0;
    u32 r4;

    r4 = *(u32*)&lbl_80478F20;
    r3 = r3 & 0xffff;
    r0 = MENU_MIDDLE_U32_0000(r4)->unk_0000;
    if (r0 <= r3) {
        return -1;
    }
    if ((s32)r3 < 9) {
        if ((s32)r3 == 1) {
            goto ret0;
        }
        if ((s32)r3 >= 1) {
            goto ret1;
        }
        goto ret0;
    }
    if ((s32)r3 >= 0x30a) {
        goto ret0;
    }
    if ((s32)r3 >= 0x308) {
        goto ret2;
    }
    goto ret0;
ret1:
    return 1;
ret2:
    return 2;
ret0:
    return 0;
}
#endif

#if defined(MENU_MIDDLE_ALL) || defined(MENU_MIDDLE_RESIDUAL_8006ACCC_ONLY)
/* 0x8006ACCC | size: 0xE8 */
u8* fn_8006ACCC(s32 id) {
    extern u8* savedataGetStatus(s32 idx, s32 type);
    s32 ruleType;
    u32 offset;
    s32 i;
    u8* status;

    ruleType = MENU_MIDDLE_U32_0004(savedataGetStatus(0, 0xE))->unk_0004;
    switch (ruleType) {
    case 0:
    case 1:
        if (id >= 0 && id <= 1) {
            status = savedataGetStatus(0, 0xE);
            return status + id * 0x1660 + 0x24;
        }
        goto ret0;
    case 2:
    default:
        goto search;
    }

search:
    i = 0;
    offset = 0;
    while (i < 4) {
        s32 trainerId = *(s32*)(savedataGetStatus(0, 0xE) + offset + 0x4c);
        if (id == trainerId) {
            status = savedataGetStatus(0, 0xE);
            return status + i * 0x1660 + 0x24;
        }
        offset += 0x1660;
        i++;
    }

ret0:
    return 0;
}
#endif

#if defined(MENU_MIDDLE_ALL) || defined(MENU_MIDDLE_EXACT_8006ADB4_ONLY)
/* 0x8006ADB4 | size: 0x38 */
void fn_8006ADB4(s32 value) {
    extern u8* savedataGetStatus(s32 idx, s32 type);

    *(s32*)(savedataGetStatus(0, 0xe) + 0x59a4) = value;
}


/* 0x8006ADEC | size: 0x2C */
s32 fn_8006ADEC(void) {
    extern u8 *savedataGetStatus(s32 idx, s32 type);
    return *(s32*)(savedataGetStatus(0x0, 0xe) + 0x59a4);
}

#endif
#if defined(MENU_MIDDLE_ALL) || defined(MENU_MIDDLE_RESIDUAL_8006AE18_ONLY)
/* 0x8006AE18 | size: 0xD4 */
s32 fn_8006AE18(void) {
    extern u8* savedataGetStatus(s32 idx, s32 type);
    extern u32 fn_801906A0(u32 flag);
    extern void __assert(const char* file, s32 line, const char* expr);
    u8* p;
    s32 state;

    if (fn_801906A0(0x8AE) != 0) {
        p = savedataGetStatus(0, 0xE) + 0x10000;
        if (MENU_MIDDLE_NEG_U8_C988(p)->unk_C988 != 0) {
            p -= 0x4cd8;
        } else {
            p = 0;
        }

        if (p != 0) {
            state = MENU_MIDDLE_U16_0000(p)->unk_0000;
            switch (state) {
            case 1:
                return 0;
            case 2:
                return 1;
            case 3:
                return 2;
            case 0x309:
                return 3;
            case 0x308:
                return 4;
            default:
                __assert((const char*)&lbl_80267DE8, 0x1c2, (const char*)&lbl_8047C040);
                break;
            }
        }
    }
    return 0;
}
#endif

#if defined(MENU_MIDDLE_ALL) || defined(MENU_MIDDLE_EXACT_8006AEEC_ONLY)
/* 0x8006AEEC | size: 0x58 */
u8* fn_8006AEEC(void) {
    extern u8* savedataGetStatus(s32 idx, s32 type);
    u8* p;

    p = savedataGetStatus(0, 0xe) + 0x10000;
    if (*(u8*)(p - 0x3678) != 0) {
        p -= 0x4cd8;
    } else {
        p = NULL;
    }
    if (p == NULL) {
        return NULL;
    }
    return p + 0xb44;
}
#endif

#if defined(MENU_MIDDLE_ALL) || defined(MENU_MIDDLE_RESIDUAL_8006AF44_ONLY)
/* 0x8006AF44 | size: 0x80 */
void fn_8006AF44(u8* base, void* src) {
    if (src != 0) {
        memcpy(base + 0x10000 - 0x4cd8, src, 0x1660);
        MENU_MIDDLE_NEG_U32_B34C(base + 0x10000)->unk_B34C = MENU_MIDDLE_U32_0000(&lbl_80267DD8)->unk_0000;
        MENU_MIDDLE_NEG_U16_B32A(base + 0x10000)->unk_B32A = 0;
        MENU_MIDDLE_NEG_U8_C988(base + 0x10000)->unk_C988 = 1;
    } else {
        MENU_MIDDLE_NEG_U8_C988(base + 0x10000)->unk_C988 = 0;
    }
    MENU_MIDDLE_NEG_U8_C98B(base + 0x10000)->unk_C98B = 0;
}
#endif

#if defined(MENU_MIDDLE_ALL) || defined(MENU_MIDDLE_EXACT_8006AFC4_ONLY)
/* 0x8006AFC4 | size: 0x20 */
u8* fn_8006AFC4(u8* p) {
    p += 0x10000;
    if (*(u8*)(p - 0x3678) != 0) {
        return p - 0x4cd8;
    }
    return 0;
}
#endif

#if defined(MENU_MIDDLE_ALL) || defined(MENU_MIDDLE_RESIDUAL_8006AFE4_ONLY)
/* 0x8006AFE4 | size: 0xB8 */
u8* fn_8006AFE4(s32 id) {
    extern u8* savedataGetStatus(s32 side, s32 type);
    s32 index;
    s32* cursor = (s32*)lbl_80267DD8;
    u32 status;
    u32 offset;

    if (id == *cursor++) {
        index = 0;
    } else if (id == *cursor++) {
        index = 1;
    } else if (id == *cursor++) {
        index = 2;
    } else if (id == *cursor) {
        index = 3;
    } else {
        index = -1;
    }

    if (index < 0) {
        return NULL;
    }
    status = (u32)savedataGetStatus(0, 0xe);
    offset = index * 0x1660;
    return (u8*)(status + offset + 0x24);
}
#endif

#if defined(MENU_MIDDLE_ALL) || defined(MENU_MIDDLE_EXACT_8006B09C_ONLY)
/* 0x8006B09C | size: 0x5C */
u8* fn_8006B09C(s32 index) {
    extern u8* savedataGetStatus(s32 idx, s32 type);

    if (index < 0 || index >= 4) {
        return 0;
    }

    return savedataGetStatus(0, 0xE) + index * 0x1660 + 0x24;
}


/* 0x8006B0F8 | size: 0x5C */
u8* fn_8006B0F8(s32 index) {
    extern u8* savedataGetStatus(s32 idx, s32 type);

    if (index < 0 || (u32)index >= 4) {
        return 0;
    }

    return savedataGetStatus(0, 0xE) + index * 0x1660 + 0x50;
}
#endif

#if defined(MENU_MIDDLE_ALL) || defined(MENU_MIDDLE_RESIDUAL_8006B154_ONLY)
/* 0x8006B154 | size: 0x6C */
s32 menuCBBios_ControlerIDtoPortID(s32 id) {
    s32* cursor = (s32*)lbl_80267DD8;
    s32 i;

    for (i = 0; i < 4; i++, cursor++) {
        if (id == *cursor) {
            return i;
        }
    }
    return -1;
}
#endif

#if defined(MENU_MIDDLE_ALL) || defined(MENU_MIDDLE_EXACT_8006B1C0_ONLY)
/* 0x8006B1C0 | size: 0x14 */
u32 fn_8006B1C0(s32 i) {
    return ((u32*)lbl_80267DD8)[i];
}

/* 0x8006B1D4 | size: 0x20 */
void fn_8006B1D4(void) {
    fn_80077DB8();
}

/* 0x8006B1F4 | size: 0xB0 */
u32 fn_8006B1F4(s32 index, s32 slot) {
    extern u8* savedataGetStatus(s32 idx, s32 type);
    s32 r30;
    s32 r31;
    u32 r0;
    u8* r3;

    r30 = index;
    r31 = slot;
    if (r30 < 0) {
        goto invalid_index;
    }
    if (r30 < 7) {
        goto valid_index;
    }
invalid_index:
    r0 = 0;
    goto check_enabled;
valid_index:
    r3 = savedataGetStatus(0, 0xe);
    r0 = *(u8*)(r3 + (r30 + (1 << 16)) - 0x342c);
check_enabled:
    r0 = (u8)r0;
    if (r0 != 0) {
        goto enabled;
    }
    return 0;
enabled:
    if (r31 < 0) {
        goto ret0;
    }
    if ((u32)r31 < 2) {
        goto valid_slot;
    }
ret0:
    return 0;
valid_slot:
    r3 = savedataGetStatus(0, 0xe);
    return *(u8*)(r3 + (r31 + (1 << 16)) + r30 * 2 - 0x3425);
}
#endif

#if defined(MENU_MIDDLE_ALL) || defined(MENU_MIDDLE_RESIDUAL_8006B2A4_ONLY)
/* 0x8006B2A4 | size: 0xB0 */
void fn_8006B2A4(s32 idx, s32 sub) {
    extern u8* savedataGetStatus(s32 side, s32 type);
    u8 flag;

    flag = 0;
    if (idx >= 0 && idx < 7) {
        u8* status = savedataGetStatus(0, 0xE);
        flag = *(u8*)(status + idx + 0x10000 - 0x342C);
    }

    if (flag == 0) {
        return;
    }
    if (sub < 0 || (u32)sub >= 2) {
        return;
    }
    *(u8*)(savedataGetStatus(0, 0xE) + sub + 0x10000 + idx * 2 - 0x3425) = 1;
}
#endif

#if defined(MENU_MIDDLE_ALL) || defined(MENU_MIDDLE_EXACT_8006B354_ONLY)
/* 0x8006B354 | size: 0x74 */
void fn_8006B354(s32 index) {
    extern u8* savedataGetStatus(s32 idx, s32 type);
    extern void __assert(char* file, s32 line, char* expr);
    s32 r30;
    u32 r31;

    r30 = index;
    if (r30 >= 0) {
        if (r30 < 7) {
            goto valid_index;
        }
    }
    __assert((char*)&lbl_80267DE8, 0xe4, (char*)&lbl_8047C040);
    return;
valid_index:
    r31 = 1;
    *(u8*)(savedataGetStatus(0, 0xe) + (r30 + (1 << 16)) - 0x342c) = r31;
}


/* 0x8006B3C8 | size: 0x58 */
u32 fn_8006B3C8(s32 index) {
    extern u8* savedataGetStatus(s32 idx, s32 type);
    s32 r31;

    r31 = index;
    if (r31 < 0) {
        goto ret0;
    }
    if (r31 < 7) {
        goto valid_index;
    }
ret0:
    return 0;
valid_index:
    return *(u8*)(savedataGetStatus(0, 0xe) + (r31 + (1 << 16)) - 0x342c);
}
#endif

#if defined(MENU_MIDDLE_ALL) || defined(MENU_MIDDLE_RESIDUAL_8006B420_ONLY)
/* 0x8006B420 | size: 0x8C */
void* fn_8006B420(void) {
    extern u8* savedataGetStatus(s32 idx, s32 type);
    extern void* menuCBRule_ConstantRule(s32 index);
    s32 ruleId;
    s32 index;
    void* value;
    u8* record;

    ruleId = MENU_MIDDLE_U32_0008(savedataGetStatus(0, 0xE))->unk_0008;
    value = menuCBRule_ConstantRule(ruleId);
    if (value != 0) {
        return value;
    }

    index = MENU_MIDDLE_U32_0008(savedataGetStatus(0, 0xE))->unk_0008;
    if (index < 0 || (u32)index >= 6) {
        record = 0;
    } else {
        record = savedataGetStatus(0, 0xE) + index * 0x54 + 0xC9DC;
    }
    return record;
}
#endif

#if defined(MENU_MIDDLE_ALL) || defined(MENU_MIDDLE_EXACT_8006B4AC_ONLY)
/* 0x8006B4AC | size: 0x70 */
void fn_8006B4AC(s32 value) {
    extern u8* savedataGetStatus(s32 idx, s32 type);
    extern void __assert(const char* file, s32 line, const char* expr);
    s32 valid;

    valid = 0;
    if (value >= 0 && (u32)value < 6) {
        valid = 1;
    }
    if (valid == 0) {
        __assert((const char*)&lbl_80267DE8, 0xB9, (const char*)&lbl_80267E70);
    }
    MENU_MIDDLE_U32_0008(savedataGetStatus(0, 0xE))->unk_0008 = value;
}


/* 0x8006B51C | size: 0x60 */
u8* fn_8006B51C(s32 index) {
    extern u8* savedataGetStatus(s32 idx, s32 type);

    if (index < 0 || (u32)index >= 6) {
        return 0;
    }

    return savedataGetStatus(0, 0xE) + index * 0x54 + 0xC9DC;
}


/* 0x8006B57C | size: 0x2C */
s32 fn_8006B57C(void) {
    extern u8 *savedataGetStatus(s32 idx, s32 type);
    return savedataGetStatus(0x0, 0xe)[0x1c];
}

/* 0x8006B5A8 | size: 0x28 */
s32 fn_8006B5A8(void) {
    extern s32 savedataGetStatus(s32 idx, s32 type);
    return savedataGetStatus(0x0, 0xe);
}

#endif
#if defined(MENU_MIDDLE_ALL) || defined(MENU_MIDDLE_RESIDUAL_8006B5D0_ONLY)
/* 0x8006B5D0 | size: 0xE4 */
void fn_8006B5D0(MenuMiddleWork* work) {
    extern void fn_8006AABC();
    extern void* savedataGetStatus(s32 index, s32 kind);
    u16* trainerIds;
    u8* slot;
    u32* trainerKinds;
    u32 i;

    *(void**)&lbl_8047A5A4 = savedataGetStatus(0, 0xE);
    if (lbl_8047A5E0 == 0) {
        slot = (u8*)work;
        trainerKinds = (u32*)lbl_80267DD8;
        i = 0;
        trainerIds = (u16*)&lbl_8047C038;
        do {
            u32 trainerKind;
            u32 controllerId;

            fn_8006AABC(slot + 0x24, *trainerIds);
            trainerKind = *trainerKinds;
            controllerId = i & 0xFFFF;
            *(u32*)(slot + 0x48) = trainerKind;
            *(u16*)(slot + 0x26) = controllerId;
            memcpy(slot + 0x59A8, slot + 0x24, 0x1660);
            trainerIds++;
            slot += 0x1660;
            trainerKinds++;
            i++;
        } while (i < 4);

        switch (work->ruleMode) {
        case 3:
            menuCB_InitMenu(0xAF);
            work->randomTableIndex = 0;
            break;
        case 0:
        default:
            menuCB_InitMenu(0xA8);
            work->randomTableIndex = 4;
            break;
        }
    }
    lbl_8047A5E0 = 0;
}


/* Retail copies each fixed 0x54-byte rule as a value record. */
typedef struct MenuRuleCopy {
    u32 word[0x15];
} MenuRuleCopy;

/* 0x8006B6B4 | size: 0x234 */
void fn_8006B6B4(void* saveSection)
{
    extern void* menuCBRule_ConstantRule(s32 index);
    u8* status = (u8*)saveSection;
    s32 i;

    lbl_8047A5E0 = 0;
    memset(status, 0, 0xCC2C);
    status[0x1C] = 0;

    *(MenuRuleCopy*)(status + 0xC9DC) = *(MenuRuleCopy*)menuCBRule_ConstantRule(0);
    *(MenuRuleCopy*)(status + 0xCA30) = *(MenuRuleCopy*)menuCBRule_ConstantRule(1);
    *(MenuRuleCopy*)(status + 0xCA84) = *(MenuRuleCopy*)menuCBRule_ConstantRule(2);
    *(MenuRuleCopy*)(status + 0xCAD8) = *(MenuRuleCopy*)menuCBRule_ConstantRule(0);
    *(MenuRuleCopy*)(status + 0xCB2C) = *(MenuRuleCopy*)menuCBRule_ConstantRule(0);
    *(MenuRuleCopy*)(status + 0xCB80) = *(MenuRuleCopy*)menuCBRule_ConstantRule(0);

    *(u16*)(status + 0xCB86) = 6;
    *(u16*)(status + 0xCB32) = 6;
    *(u16*)(status + 0xCADE) = 6;
    status[0xCBD4] = 1;
    status[0xCBD5] = 1;
    status[0xCBD6] = 1;
    status[0xCBD7] = 0;
    status[0xCBD8] = 1;
    status[0xCBD9] = 0;
    {
        u8* p = status;
        for (i = 0; i < 7; i++, p += 2) {
            p[0xCBDB] = 0;
            p[0xCBDC] = 0;
        }
    }
}
#endif

#if defined(MENU_MIDDLE_ALL) || defined(MENU_MIDDLE_EXACT_8006B8E8_ONLY)
/* 0x8006B8E8 | size: 0x8 */
u8 fn_8006B8E8(void) {
    return lbl_8047A5E0;
}

/* 0x8006B8F0 | size: 0xC */
void fn_8006B8F0(void) {
    lbl_8047A5E0 = 0;
}

/* 0x8006B8FC | size: 0xC */
void fn_8006B8FC(void) {
    lbl_8047A5E0 = 1;
}

/* 0x8006B908 | size: 0x28 */
#pragma push
#pragma scheduling off
void fn_8006B908(u32 r3) {
    extern void fn_80070D84(u32 r3, u32 r4, u32 r5);
    fn_80070D84(r3, 0x0, 0x0);
}
#pragma pop

/* 0x8006B930 | size: 0x88 */
#pragma peephole off
void fn_8006B930(void* menu) {
    extern u8* savedataGetStatus(s32 idx, s32 type);
    extern s32 fn_80071160(void);
    extern u32 fn_80071208(u32 flags);
    u32 flags;

    if (fn_80071160() != 0) {
        MENU_MIDDLE_U8_0098(menu)->unk_0098 = 1;
        MENU_MIDDLE_U8_0099(menu)->unk_0099 = 1;
        return;
    }

    flags = MENU_MIDDLE_U32_59CC(savedataGetStatus(0, 0xE))->unk_59CC;
    flags = fn_80071208(flags);
    if ((flags & 0x1000) != 0) {
        MENU_MIDDLE_U8_0098(menu)->unk_0098 = 1;
        return;
    }
    if ((flags & 0x200) == 0) {
        return;
    }
    MENU_MIDDLE_U8_0098(menu)->unk_0098 = 1;
    MENU_MIDDLE_U8_0099(menu)->unk_0099 = 1;
}
#pragma peephole reset
#endif

#if defined(MENU_MIDDLE_ALL) || defined(MENU_MIDDLE_RESIDUAL_8006B9B8_ONLY) || \
    defined(MENU_MIDDLE_EXACT_8006B9B8_ONLY)
/* 0x8006B9B8 | size: 0x17C */
/* RULE-EXCEPTION(user-approved): local peephole-control pragma — see docs/RULE_EXCEPTIONS.md */
#pragma peephole off
void fn_8006B9B8(void* menu) {
    extern u8* savedataGetStatus(s32 index, s32 type);
    extern s32 fn_80071160(void);
    extern u32 fn_80071208(u32 flags);
    extern void fn_80166A28(s32 sound);
    s32 player;
    s32 playerCount;
    u32 flags;

    switch (*(s32*)(savedataGetStatus(0, 0xE) + 4)) {
    case 0:
    case 1:
        playerCount = 2;
        break;
    case 2:
    default:
        playerCount = 4;
        break;
    }

    switch ((s8)MENU_MIDDLE_U8_0001(menu)->unk_0001) {
    case 0:
    case 1:
        break;
    case 2:
        if (fn_80071160() != 0) {
            MENU_MIDDLE_U8_0098(menu)->unk_0098 = 1;
            MENU_MIDDLE_U8_0099(menu)->unk_0099 = 1;
            return;
        }

        for (player = 0; player < playerCount; player++) {
            flags = fn_80071208(
                fn_8006A814((u32)(savedataGetStatus(0, 0xE) + player * 0x1660 + 0x59A8)));

            if ((flags & 0x100) != 0) {
                if (savedataGetStatus(0, 0xE)[player * 0x1660 + 0x7005] == 0) {
                    savedataGetStatus(0, 0xE)[player * 0x1660 + 0x7005] = 1;
                    fn_80166A28(0x24);
                }
            } else if ((flags & 0x200) != 0) {
                if (savedataGetStatus(0, 0xE)[player * 0x1660 + 0x7005] != 0) {
                    savedataGetStatus(0, 0xE)[player * 0x1660 + 0x7005] = 0;
                    fn_80166A28(0x25);
                } else {
                    MENU_MIDDLE_U8_0098(menu)->unk_0098 = 1;
                    MENU_MIDDLE_U8_0099(menu)->unk_0099 = 1;
                    return;
                }
            }
        }
        break;
    case 3:
    case 4:
    case 5:
        break;
    }
}
#pragma peephole reset
#endif

#if defined(MENU_MIDDLE_ALL) || defined(MENU_MIDDLE_RESIDUAL_8006B9B8_ONLY) || \
    defined(MENU_MIDDLE_EXACT_8006BB34_ONLY)
/* 0x8006BB34 | size: 0x4E4 */
extern void fn_80166A28(s32 sndId);
extern void __assert(const void* file, s32 line, const void* expr);

typedef struct CursorArg_8006BB34 {
    u8 pad;
    s8 cursor;
} CursorArg_8006BB34;

typedef struct MenuState_8006BB34 {
    u8 pad0[4];
    s32 menuId;   /* 0x04 */
    u8 pad8[2];
    u8 disabled;  /* 0x0A */
    u8 padB[0x89];
    CursorArg_8006BB34 cursor; /* 0x94 */
} MenuState_8006BB34;

typedef struct Param_8006BB34 {
    u8 pad0[0x11];
    u8 sel11;
    u8 sel12;
    u8 sel13;
    s16 val14;
    s16 val16;
} Param_8006BB34;

/* RULE-EXCEPTION(user-approved): local peephole-control pragma — see docs/RULE_EXCEPTIONS.md */
#pragma peephole off
void fn_8006BB34(void* menu) {
    s32 cur;
    Param_8006BB34* p;
    s32 signOld;
    KeyInfo_8006BB34* ki;
    u8 pressed;
    u8 inc;
    u8 dec;
    u8* asrt;
    u8 confirm;
    u8 dir;
    s32 delta;
    MenuState_8006BB34* m;
    CursorArg_8006BB34 srcB;
    CursorArg_8006BB34 srcA;
    CursorArg_8006BB34 argA;
    CursorArg_8006BB34 argB;

    m = (MenuState_8006BB34*)menu;
    asrt = lbl_80267EA8;
    ki = (KeyInfo_8006BB34*)windowGetKeyInfo();
    if (m->disabled != 0) return;

    confirm = (ki->flags6 & 1) != 0;
    inc = (ki->flags6 & 4) != 0;
    dec = (ki->flags6 & 8) != 0;

    pressed = (inc != 0 || dec != 0);
    dir = (inc != 0 || dec == 0);

    cur = menuGetCursorItemID(m->menuId);
    if (fn_80077BD0()) {
        s32 c;
        if (!confirm) return;
        c = menuGetCursorFromItemID(m->menuId, 0xE35);
        srcA.cursor = (s8)c;
        srcA.pad = 0;
        argA = srcA;
        fn_801044D0(m->menuId, &argA);
        return;
    }

    p = (Param_8006BB34*)windowGetParam(menu, 0);

    delta = 0;
    if (ki->flags6 & 2) delta = -1;
    else if (ki->flags6 & 1) delta = 1;

    switch (cur) {
    case 0xA0C:
        delta = delta * 10;
        /* fallthrough */
    case 0xA0D:
        if (p->val14 >= 0) {
            p->val14 = (p->val14 + delta < 1) ? 1 : ((p->val14 + delta > 99) ? 99 : p->val14 + delta);
        }
        break;
    case 0xE34:
        delta = delta * 10;
        /* fallthrough */
    case 0xE33:
        if (p->val16 >= 0) {
            p->val16 = (p->val16 + delta < 1) ? 1 : ((p->val16 + delta > 99) ? 99 : p->val16 + delta);
        }
        break;
    default:
        delta = 0;
        break;
    }
    if (delta != 0) return;

    srcB = m->cursor;

    switch (cur) {
    case 0x9F7:
        if (!pressed) break;
        if (p->sel11 != dir) fn_80166A28(0x24);
        p->sel11 = dir;
        return;
    case 0x9F8:
        if (!pressed) break;
        if (p->sel12 != dir) fn_80166A28(0x24);
        p->sel12 = dir;
        return;
    case 0x9F9:
        if (!pressed) break;
        if (p->sel13 != dir) fn_80166A28(0x24);
        p->sel13 = dir;
        return;
    case 0x9FA:
        if (pressed) {
            s16 vv = p->val14;
            s32 a;
            signOld = (u32)vv >> 31;
            a = ((s32)vv >> 31) ^ vv;
            a = a - ((s32)vv >> 31);
            if (signOld == dir) fn_80166A28(0x24);
            p->val14 = (s16)(dir ? a : -a);
            return;
        }
        if (p->val14 < 0) break;
        if (!(ki->flags4 & 0x10)) break;
        menuSetCursor(m->menuId, menuGetCursorFromItemID(m->menuId, 0xA0C));
        return;
    case 0x9FB:
        if (pressed) {
            s16 vv = p->val16;
            s32 a;
            signOld = (u32)vv >> 31;
            a = ((s32)vv >> 31) ^ vv;
            a = a - ((s32)vv >> 31);
            if (signOld == dir) fn_80166A28(0x24);
            p->val16 = (s16)(dir ? a : -a);
            return;
        }
        if (p->val16 < 0) break;
        if (!(ki->flags4 & 0x10)) break;
        menuSetCursor(m->menuId, menuGetCursorFromItemID(m->menuId, 0xE34));
        return;
    case 0xA0C:
        if (inc) return;
        /* fallthrough */
    case 0xA0D:
        if (p->val14 < 0) {
            __assert(asrt + 0x7d8, 0xE73, asrt + 0x7e8);
        }
        if (!(ki->flags4 & 0x30)) break;
        menuSetCursor(m->menuId, menuGetCursorFromItemID(m->menuId, 0x9FA));
        return;
    case 0xE34:
        if (inc) return;
        /* fallthrough */
    case 0xE33:
        if (p->val16 < 0) {
            __assert(asrt + 0x7d8, 0xE7F, asrt + 0x808);
        }
        if (!(ki->flags4 & 0x30)) break;
        menuSetCursor(m->menuId, menuGetCursorFromItemID(m->menuId, 0x9FB));
        return;
    case 0x9FD:
        if (confirm) {
            s32 c = menuGetCursorFromItemID(m->menuId, 0x9FB);
            srcB.cursor = (s8)c;
            argB = srcB;
            fn_801044D0(m->menuId, &argB);
            return;
        }
        if (dec) return;
        break;
    default:
        break;
    }

    menuCursorNormal(menu);
}
/* RULE-EXCEPTION(user-approved): local peephole-control pragma — see docs/RULE_EXCEPTIONS.md */
#pragma peephole reset



/* 0x8006C018 | size: 0xC4 */
#pragma peephole off
void fn_8006C018(void* menu) {
    typedef struct MenuButton_8006C018 {
        u8 pad0;
        u8 state;
        u8 pad2[2];
        s32 menuId;
    } MenuButton_8006C018;

    MenuButton_8006C018* button = menu;
    KeyInfo_8006BB34* keyInfo;
    s32 itemId;
    u32 value;

    value = button->state;
    value = (s8)value;
    switch ((s32)value) {
    case 2:
        break;
    default:
        goto end;
    }

    keyInfo = windowGetKeyInfo();
    itemId = menuGetCursorItemID(button->menuId);

    switch (itemId) {
    case 0x9F7:
    case 0x9F8:
    case 0x9F9:
    case 0x9FA:
    case 0x9FB:
        value = keyInfo->flags4 & 0x10;
        switch ((s32)value) {
        case 0:
            break;
        default:
            return;
        }
        goto normal;
    case 0xA0C:
    case 0xA0D:
    case 0xE33:
    case 0xE34:
        value = keyInfo->flags4 & 0x30;
        switch ((s32)value) {
        case 0:
            break;
        default:
            return;
        }
        goto normal;
    default:
        goto normal;
    }
end:
    return;
normal:
    menuButtonNormal(button);
}
/* RULE-EXCEPTION(user-approved): local peephole-control pragma — see docs/RULE_EXCEPTIONS.md */
#pragma peephole reset


/* 0x8006C0DC | size: 0x88 */
#pragma peephole off
void fn_8006C0DC(void* menu) {
    typedef struct MenuButton_8006C0DC {
        u8 pad0;
        u8 state;
        u8 pad2[2];
        s32 menuId;
    } MenuButton_8006C0DC;

    MenuButton_8006C0DC* button = menu;
    KeyInfo_8006BB34* keyInfo;
    s32 itemId;
    u32 value;

    value = button->state;
    value = (s8)value;
    switch ((s32)value) {
    case 2:
        break;
    default:
        goto end;
    }

    keyInfo = windowGetKeyInfo();
    itemId = menuGetCursorItemID(button->menuId);

    switch (itemId) {
    case 0x9CA:
    case 0x9CB:
    case 0x9CC:
    case 0x9CD:
    case 0x9CE:
    case 0x9CF:
    case 0x9D0:
    case 0x9D1:
        value = keyInfo->flags4 & 0x10;
        switch ((s32)value) {
        case 0:
            break;
        default:
            return;
        }
        goto normal;
    default:
        goto normal;
    }
end:
    return;
normal:
    menuButtonNormal(button);
}
#pragma peephole reset
#endif

#if defined(MENU_MIDDLE_ALL) || defined(MENU_MIDDLE_RESIDUAL_8006B9B8_ONLY)


/* 0x8006C164 | size: 0x474 */
void fn_8006C164(void* menu) {
    extern void fn_80166A28();
    extern u8 jumptable_802EDE78[];
    extern u8 jumptable_802EDEFC[];
    u8 sp[0x40];
    u32 r0 = 0;
    u32 r1 = (u32)sp;
    u32 r3 = (u32)menu;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r24 = 0;
    u32 r25 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    void (*ctr_fn)(void) = 0;
    u32 ctr = 0;

    
    r24 = r3;
    r0 = MENU_MIDDLE_U8_000A(r24)->unk_000A;
    if (r0 != (u32)0x0) return;
    r3 = (u32)windowGetKeyInfo();
    r0 = MENU_MIDDLE_U16_0006(r3)->unk_0006;
    r3 = r0 & 0x1;
    r0 = -r3;
    r0 = r0 | r3;
    r27 = (u32)r0 >> 31;
    r3 = (u32)windowGetKeyInfo();
    r0 = MENU_MIDDLE_U16_0006(r3)->unk_0006;
    r3 = r0 & 0x00000002;
    r0 = -r3;
    r0 = r0 | r3;
    r28 = (u32)r0 >> 31;
    r3 = (u32)windowGetKeyInfo();
    r0 = MENU_MIDDLE_U16_0006(r3)->unk_0006;
    r3 = r0 & 0x00000004;
    r0 = -r3;
    r0 = r0 | r3;
    r29 = (u32)r0 >> 31;
    r3 = (u32)windowGetKeyInfo();
    r4 = MENU_MIDDLE_U16_0006(r3)->unk_0006;
    r0 = r29 & 0xFF;
    r3 = 0x0;
    r4 = r4 & 0x00000008;
    r0 = -r4;
    r0 = r0 | r4;
    r30 = (u32)r0 >> 31;
    if (r0 == (u32)0x0) {
        r0 = r30 & 0xFF;
        if (r0 != (u32)0x0) {
        }
        r3 = 0x1;
        }
    r0 = r29 & 0xFF;
    r26 = r3 & 0xFF;
    r3 = 0x0;
    if (r0 == (u32)0x0) {
        r0 = r30 & 0xFF;
        if (r0 == (u32)0x0) {
        }
        r3 = 0x1;
        }
    r25 = r3 & 0xFF;
    r3 = (u32)fn_80077BD0();
    r0 = r3 & 0xFF;
    if (r0 != (u32)0x0) {
        r0 = r28 & 0xFF;
        if (r0 == (u32)0x0) return;
        r3 = MENU_MIDDLE_U32_0004(r24)->unk_0004;
        r4 = 0x9d2;
        r3 = (u32)menuGetCursorFromItemID((s32)r3, (s32)r4);
        r3 = (s8)r3;
        r0 = 0x0;
        *(u8*)(sp + 0x11) = r3;
        r4 = (u32)sp + 0xc;
        *(u8*)(sp + 0x10) = r0;
        r0 = *(u16*)(sp + 0x10);
        *(u16*)(sp + 0xC) = r0;
        r3 = MENU_MIDDLE_U32_0004(r24)->unk_0004;
        ((void(*)(void))fn_801044D0)();
        return;
    }
    r3 = r24;
    r4 = 0x0;
    r3 = (u32)windowGetParam((void*)r3, (s32)r4);
    r31 = r3;
    r3 = MENU_MIDDLE_U32_0004(r24)->unk_0004;
    r3 = (u32)menuGetCursorItemID((s32)r3);
    r0 = r28 & 0xFF;
    r5 = 0x0;
    if (r0 != (u32)0x0) {
        r5 = -0x1;
    } else {
        r0 = r27 & 0xFF;
        if (r0 != (u32)0x0) {
            r5 = 0x1;
        }
    }
    r0 = r3 - 0x9e2;
    switch (r0) {
    case 0:
        r5 = r5 * 0xa;
        /* fall through */
    case 1:
        r5 = r5 * 0xa;
        /* fall through */
    case 2:
        r0 = MENU_MIDDLE_S16_0000(r31)->unk_0000;
        r0 = r0 + r5;
        r0 = (s16)r0;
        MENU_MIDDLE_U16_0000(r31)->unk_0000 = r0;
        r0 = MENU_MIDDLE_S16_0002(r31)->unk_0002;
        r4 = MENU_MIDDLE_S16_0000(r31)->unk_0000;
        if ((s32)r0 < (s32)r4) {
            MENU_MIDDLE_U16_0002(r31)->unk_0002 = r4;
        }
        break;
    case 3:
        r5 = r5 * 0xa;
        /* fall through */
    case 4:
        r5 = r5 * 0xa;
        /* fall through */
    case 5:
        r0 = MENU_MIDDLE_S16_0002(r31)->unk_0002;
        r0 = r0 + r5;
        r0 = (s16)r0;
        MENU_MIDDLE_U16_0002(r31)->unk_0002 = r0;
        r4 = MENU_MIDDLE_S16_0002(r31)->unk_0002;
        r0 = MENU_MIDDLE_S16_0000(r31)->unk_0000;
        if ((s32)r4 < (s32)r0) {
            MENU_MIDDLE_U16_0000(r31)->unk_0000 = r4;
        }
        break;
    case 6:
        r5 = r5 * 0xa;
        /* fall through */
    case 7:
        r5 = r5 * 0xa;
        /* fall through */
    case 8:
        r0 = MENU_MIDDLE_S16_0004(r31)->unk_0004;
        r0 = r0 + r5;
        r0 = (s16)r0;
        MENU_MIDDLE_U16_0004(r31)->unk_0004 = r0;
        break;
    default:
        r5 = 0;
        break;
    }
    if ((s32)r5 != (s32)0x0) {
        r0 = MENU_MIDDLE_S16_0000(r31)->unk_0000;
        if ((s32)r0 < (s32)0x1) {
            r0 = 0x1;
        } else if ((s32)r0 > (s32)0x64) {
            r0 = 0x64;
        }
        r0 = (s16)r0;
        MENU_MIDDLE_U16_0000(r31)->unk_0000 = r0;
        r0 = MENU_MIDDLE_S16_0002(r31)->unk_0002;
        if ((s32)r0 < (s32)0x1) {
            r0 = 0x1;
        } else if ((s32)r0 > (s32)0x64) {
            r0 = 0x64;
        }
        r0 = (s16)r0;
        MENU_MIDDLE_U16_0002(r31)->unk_0002 = r0;
        r0 = MENU_MIDDLE_S16_0000(r31)->unk_0000;
        r3 = MENU_MIDDLE_S16_0004(r31)->unk_0004;
        r0 = r0 * 0x6;
        if ((s32)r0 <= (s32)r3) {
            r0 = MENU_MIDDLE_S16_0002(r31)->unk_0002;
            r0 = r0 * 0x6;
            if ((s32)r0 >= (s32)r3) {
                r0 = r3;
            }
        }
        r0 = (s16)r0;
        MENU_MIDDLE_U16_0004(r31)->unk_0004 = r0;
        return;
    }
    r0 = r3 - 0x9ca;
    r3 = MENU_MIDDLE_U16_0094(r24)->unk_0094;
    *(u16*)(sp + 0x14) = r3;
    switch (r0) {
    case 0:
        if (r27 != 0) {
            return;
        }
        break;
    case 2:
        if (r26 == 0) {
            break;
        }
        r0 = MENU_MIDDLE_U8_000C(r31)->unk_000C;
        if (r0 != r25) {
            fn_80166A28(0x24);
        }
        MENU_MIDDLE_U8_000C(r31)->unk_000C = r25;
        return;
    case 3:
        if (r29 != 0) {
            r0 = MENU_MIDDLE_U32_0008(r31)->unk_0008 - 1;
            MENU_MIDDLE_U32_0008(r31)->unk_0008 = r0;
            if ((s32)r0 < 0) {
                MENU_MIDDLE_U32_0008(r31)->unk_0008 = 0;
                return;
            }
            fn_80166A28(0x24);
            return;
        }
        if (r30 != 0) {
            r0 = MENU_MIDDLE_U32_0008(r31)->unk_0008 + 1;
            MENU_MIDDLE_U32_0008(r31)->unk_0008 = r0;
            if ((s32)r0 >= 3) {
                MENU_MIDDLE_U32_0008(r31)->unk_0008 = 2;
                return;
            }
            fn_80166A28(0x24);
            return;
        }
        break;
    case 4:
        if (r26 == 0) {
            break;
        }
        r0 = MENU_MIDDLE_U8_000D(r31)->unk_000D;
        if (r0 != r25) {
            fn_80166A28(0x24);
        }
        MENU_MIDDLE_U8_000D(r31)->unk_000D = r25;
        return;
    case 5:
        if (r26 == 0) {
            break;
        }
        r0 = MENU_MIDDLE_U8_000E(r31)->unk_000E;
        if (r0 != r25) {
            fn_80166A28(0x24);
        }
        MENU_MIDDLE_U8_000E(r31)->unk_000E = r25;
        return;
    case 6:
        if (r26 == 0) {
            break;
        }
        r0 = MENU_MIDDLE_U8_000F(r31)->unk_000F;
        if (r0 != r25) {
            fn_80166A28(0x24);
        }
        MENU_MIDDLE_U8_000F(r31)->unk_000F = r25;
        return;
    case 7:
        if (r26 == 0) {
            break;
        }
        r0 = MENU_MIDDLE_U8_0010(r31)->unk_0010;
        if (r0 != r25) {
            fn_80166A28(0x24);
        }
        MENU_MIDDLE_U8_0010(r31)->unk_0010 = r25;
        return;
    case 24:
    case 30:
        goto L_8006C43C;
    case 25:
    case 26:
    case 27:
    case 28:
    case 31:
        goto L_8006C448;
    case 29:
    case 32:
        if (r30 != 0) {
            return;
        }
    L_8006C43C:
        if (r30 != 0) {
            return;
        }
        if (r29 != 0) {
            break;
        }
    L_8006C448:
        if (r29 != 0) {
            (*(u8*)(sp + 0x15))--;
        }
        if (r30 != 0) {
            (*(u8*)(sp + 0x15))++;
        }
        *(u16*)(sp + 0x8) = *(u16*)(sp + 0x14);
        r3 = MENU_MIDDLE_U32_0004(r24)->unk_0004;
        r4 = (u32)sp + 0x8;
        ((void(*)(void))fn_801044D0)();
        return;
    default:
        break;
    }
    r3 = r24;
    ((void(*)(void))menuCursorNormal)();
    return;

    if (r0 <= (u32)0x20) {
        r3 = (u32)jumptable_802EDE78;
        r0 = r0 << 2;
        r3 = (u32)jumptable_802EDE78;
        r0 = *(u32*)(r3 + r0);
        ctr_fn = (void(*)(void))r0;
        /* indirect jump via ctr */;
        r0 = r27 & 0xFF;
        if (r0 != (u32)0x0) {
            if (r0 != (u32)0x0) return;
            r0 = r29 & 0xFF;
            if (r0 == (u32)0x0) {
                r0 = r29 & 0xFF;
                if (r0 != (u32)0x0) {
                    r3 = *(u8*)(sp + 0x15);
                    /* subi r0, r3, 0x1 */;
                    *(u8*)(sp + 0x15) = r0;
                }
                r0 = r30 & 0xFF;
                if (r0 != (u32)0x0) {
                    r3 = *(u8*)(sp + 0x15);
                    r0 = r3 + 0x1;
                    *(u8*)(sp + 0x15) = r0;
                }
                r0 = *(u16*)(sp + 0x14);
                r4 = (u32)sp + 0x8;
                *(u16*)(sp + 0x8) = r0;
                r3 = MENU_MIDDLE_U32_0004(r24)->unk_0004;
                ((void(*)(void))fn_801044D0)();
                if (r26 != (u32)0x0) {
                    r0 = MENU_MIDDLE_U8_000C(r31)->unk_000C;
                    if (r0 != (u32)r25) {
                        r3 = 0x24;
                        fn_80166A28();
                    }
                    MENU_MIDDLE_U8_000C(r31)->unk_000C = r25;
                    return;

                    if (r0 != (u32)0x0) {
                        r3 = MENU_MIDDLE_U32_0008(r31)->unk_0008;
                        /* subi r0, r3, 0x1 */;
                        MENU_MIDDLE_U32_0008(r31)->unk_0008 = r0;
                        r0 = MENU_MIDDLE_U32_0008(r31)->unk_0008;
                        if ((s32)r0 < (s32)0x0) {
                            r0 = 0x0;
                            MENU_MIDDLE_U32_0008(r31)->unk_0008 = r0;
                            return;
                        }
                        r3 = 0x24;
                        fn_80166A28();
                        return;
                    }
                    r0 = r30 & 0xFF;
                    if (r0 != (u32)0x0) {
                        r3 = MENU_MIDDLE_U32_0008(r31)->unk_0008;
                        r0 = r3 + 0x1;
                        MENU_MIDDLE_U32_0008(r31)->unk_0008 = r0;
                        r0 = MENU_MIDDLE_U32_0008(r31)->unk_0008;
                        if ((s32)r0 >= (s32)0x3) {
                            r0 = 0x2;
                            MENU_MIDDLE_U32_0008(r31)->unk_0008 = r0;
                            return;
                        }
                        r3 = 0x24;
                        fn_80166A28();
                        return;
                        if (r26 != (u32)0x0) {
                            r0 = MENU_MIDDLE_U8_000D(r31)->unk_000D;
                            if (r0 != (u32)r25) {
                                r3 = 0x24;
                                fn_80166A28();
                            }
                            MENU_MIDDLE_U8_000D(r31)->unk_000D = r25;
                            return;
                            if (r26 != (u32)0x0) {
                                r0 = MENU_MIDDLE_U8_000E(r31)->unk_000E;
                                if (r0 != (u32)r25) {
                                    r3 = 0x24;
                                    fn_80166A28();
                                }
                                MENU_MIDDLE_U8_000E(r31)->unk_000E = r25;
                                return;
                                if (r26 != (u32)0x0) {
                                    r0 = MENU_MIDDLE_U8_000F(r31)->unk_000F;
                                    if (r0 != (u32)r25) {
                                        r3 = 0x24;
                                        fn_80166A28();
                                    }
                                    MENU_MIDDLE_U8_000F(r31)->unk_000F = r25;
                                    return;
                                    if (r26 != (u32)0x0) {
                                        r0 = MENU_MIDDLE_U8_0010(r31)->unk_0010;
                                        if (r0 != (u32)r25) {
                                            r3 = 0x24;
                                            fn_80166A28();
                                        }
                                        MENU_MIDDLE_U8_0010(r31)->unk_0010 = r25;
                                        return;
    }
    }
    }
    }
    }
    }
    }
    }
    }
    r3 = r24;
    ((void(*)(void))menuCursorNormal)();

    return;
}


/* 0x8006C5D8 | size: 0x1FC */
void fn_8006C5D8(void* window, void* sprite) {
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = 0;
    u32 r4 = (u32)sprite;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f1 = 0.0f;

    
    r31 = r4;
    r3 = (u32)&lbl_80268674;
    r5 = MENU_MIDDLE_S16_0054(r31)->unk_0054;
    r9 = (u32)&lbl_80268674;
    r6 = MENU_MIDDLE_S16_0056(r31)->unk_0056;
    r8 = MENU_MIDDLE_U32_0000(r9)->unk_0000;
    r3 = 0x0;
    r7 = MENU_MIDDLE_U32_0004(r9)->unk_0004;
    r4 = 0x0;
    r0 = MENU_MIDDLE_U32_0008(r9)->unk_0008;
    *(u32*)(sp + 0x10) = r0;
    ((void(*)(void))fn_800FE38C)();
    r3 = 0x1;
    ((void(*)(void))fn_800D88DC)();
    r3 = 0x6;
    ((void(*)(void))fn_800D888C)();
    r5 = *(u8*)(sp + 0xF);
    r3 = (0x8081 << 16);
    r4 = MENU_MIDDLE_U8_0067(r31)->unk_0067;
    /* subi r6, r3, 0x7f7f */;
    r0 = *(u8*)(sp + 0x13);
    r3 = 0x6;
    r5 = r5 * r4;
    r0 = r0 * r4;
    r7 = (s32)((s64)r6 * (s64)r5 >> 32);
    r4 = (s32)((s64)r6 * (s64)r0 >> 32);
    r5 = r7 + r5;
    r5 = (s32)r5 >> 7;
    r0 = r4 + r0;
    r6 = (u32)r5 >> 31;
    r0 = (s32)r0 >> 7;
    r4 = (u32)r0 >> 31;
    r5 = r5 + r6;
    r0 = r0 + r4;
    r4 = r5 & 0xFF;
    r0 = r0 & 0xFF;
    *(u8*)(sp + 0xF) = r4;
    *(u8*)(sp + 0x13) = r0;
    ((void(*)(void))fn_800D6A00)();
    r3 = (u32)&lbl_80314E08;
    r3 = (u32)&lbl_80314E08;
    ((void(*)(void))fn_800D7820)();
    r3 = 0x4;
    ((void(*)(void))fn_800D67BC)();
    r3 = 0x0;
    r4 = 0x0;
    ((void(*)(void))fn_800D61E4)();
    r3 = 0x0;
    ((void(*)(void))fn_800D5BA0)();
    r3 = MENU_MIDDLE_S16_0054(r31)->unk_0054;
    r4 = 0x0;
    ((void(*)(void))fn_800D61E4)();
    r3 = 0x0;
    ((void(*)(void))fn_800D5BA0)();
    r3 = MENU_MIDDLE_S16_0054(r31)->unk_0054;
    r4 = MENU_MIDDLE_S16_0056(r31)->unk_0056;
    ((void(*)(void))fn_800D61E4)();
    r3 = 0x0;
    ((void(*)(void))fn_800D5BA0)();
    r4 = MENU_MIDDLE_S16_0056(r31)->unk_0056;
    r3 = 0x0;
    ((void(*)(void))fn_800D61E4)();
    r3 = 0x0;
    ((void(*)(void))fn_800D5BA0)();
    ((void(*)(void))fn_800D6728)();
    f1 = *(f32*)&lbl_8047C060;
    ((void(*)(void))fn_800D5648)();
    r3 = 0x1;
    ((void(*)(void))fn_800D6A00)();
    r3 = (u32)&lbl_80314E08;
    r3 = (u32)&lbl_80314E08;
    ((void(*)(void))fn_800D7820)();
    r0 = MENU_MIDDLE_U8_0067(r31)->unk_0067;
    r4 = 0xff;
    r3 = (0x8081 << 16);
    *(u8*)(sp + 0x8) = r4;
    r0 = r0 * 0x38;
    r30 = 0x0;
    /* subi r3, r3, 0x7f7f */;
    *(u8*)(sp + 0x9) = r4;
    r3 = (s32)((s64)r3 * (s64)r0 >> 32);
    *(u8*)(sp + 0xA) = r4;
    r0 = r3 + r0;
    r0 = (s32)r0 >> 7;
    r3 = (u32)r0 >> 31;
    r0 = r0 + r3;
    r0 = r0 & 0xFF;
    *(u8*)(sp + 0xB) = r0;

    while ((s32)r3 < (s32)r0) {
        r3 = 0x2;
        ((void(*)(void))fn_800D67BC)();
        r4 = r30;
        r3 = 0x0;
        ((void(*)(void))fn_800D61E4)();
        r3 = 0x0;
        ((void(*)(void))fn_800D5BA0)();
        r3 = MENU_MIDDLE_S16_0054(r31)->unk_0054;
        r4 = r30;
        ((void(*)(void))fn_800D61E4)();
        r3 = 0x0;
        ((void(*)(void))fn_800D5BA0)();
        ((void(*)(void))fn_800D6728)();
        r30 = r30 + 0x4;

    r0 = MENU_MIDDLE_S16_0056(r31)->unk_0056;
    r3 = (s16)r30;
    }
    ((void(*)(void))fn_800FE35C)();
    return;
}


/* 0x8006C7D4 | size: 0x4EC */
typedef struct MenuMiddleTrainer_8006C7D4 {
    u8 pad0000[0xB44];
    u8 hero[0xB1C];     /* 0x0B44 */
} MenuMiddleTrainer_8006C7D4;

typedef struct MenuMiddleStatus_8006C7D4 {
    u8 pad0[0x59A8];
    MenuMiddleTrainer_8006C7D4 trainers[4]; /* 0x59A8 */
} MenuMiddleStatus_8006C7D4;

static inline u8* fn_8006C7D4_GetHero(s32 index) {
    extern MenuMiddleStatus_8006C7D4* savedataGetStatus(s32 idx, s32 type);

    return savedataGetStatus(0, 0xE)->trainers[index].hero;
}

static inline s32 fn_8006C7D4_GetKind(MenuMiddleTrainer_8006C7D4* trainer) {
    extern u8 heroBiosGetSexDataId(void* hero);

    switch ((s32)fn_8006A7E8((u32)trainer)) {
    case 0:
        return 0;
    case 1:
        switch (heroBiosGetSexDataId(trainer->hero)) {
        case 0:
            return 1;
        case 1:
            return 2;
        case 2:
            break;
        }
        break;
    case 2:
        switch (heroBiosGetSexDataId(trainer->hero)) {
        case 0:
            return 3;
        case 1:
            return 4;
        case 2:
            break;
        }
        break;
    }
    return 1;
}

void fn_8006C7D4(void* arg0, void* item) {
    extern MenuMiddleStatus_8006C7D4* savedataGetStatus(s32 idx, s32 type);
    extern u16 heroBiosGetRnd(void* hero);
    extern void* heroBiosGetNamePtr(void* hero);
    extern void msgctrlSetValue(s32 id, void* value);
    extern void* menuSpriteBiosGetPtr(u32 spriteId);
    extern void fn_80071318(void* widget, void* sprite);
    extern void fn_800FB680(s32 a, s32 b, u32 window, s32 c);
    extern s32 sprintf(char* buffer, const char* format, ...);
    extern void GScharMakeFromSJIS(u16* dst, const char* src);
    u8 digits[5];
    char text[0x80];
    u16 message[0x80];
    u32 window;
    s32 index;
    MenuMiddleTrainer_8006C7D4* trainer;
    u32 spriteId;
    s32 type;
    s32 kind;
    u32 id;
    u8* d;

    switch (*(s16*)((u8*)item + 6)) {
    case 0xEC2:
        type = 2;
        index = 0;
        break;
    case 0xEC3:
        type = 2;
        index = 1;
        break;
    case 0xEC4:
        type = 2;
        index = 2;
        break;
    case 0xEC5:
        type = 2;
        index = 3;
        break;
    case 0xEC6:
        type = 2;
        index = 0;
        break;
    case 0xEC7:
        type = 2;
        index = 1;
        break;
    case 0xEC8:
        type = 2;
        index = 2;
        break;
    case 0xEC9:
        type = 2;
        index = 3;
        break;
    case 0xECA:
        type = 2;
        index = 0;
        break;
    case 0xECB:
        type = 2;
        index = 1;
        break;
    case 0xECC:
        type = 2;
        index = 2;
        break;
    case 0xECD:
        type = 2;
        index = 3;
        break;
    case 0xECE:
        type = 1;
        index = 0;
        break;
    case 0xECF:
        type = 1;
        index = 1;
        break;
    case 0xED0:
        type = 1;
        index = 2;
        break;
    case 0xED1:
        type = 1;
        index = 3;
        break;
    case 0xED2:
        type = 1;
        index = 0;
        break;
    case 0xED3:
        type = 1;
        index = 1;
        break;
    case 0xED4:
        type = 1;
        index = 2;
        break;
    case 0xED5:
        type = 1;
        index = 3;
        break;
    case 0xED6:
        type = 1;
        index = 0;
        break;
    case 0xED7:
        type = 1;
        index = 1;
        break;
    case 0xED8:
        type = 1;
        index = 2;
        break;
    case 0xED9:
        type = 1;
        index = 3;
        break;
    case 0xEDA:
        type = 3;
        index = 0;
        break;
    case 0xEDB:
        type = 3;
        index = 1;
        break;
    case 0xEDC:
        type = 3;
        index = 2;
        break;
    case 0xEDD:
        type = 3;
        index = 3;
        break;
    case 0xEDE:
        type = 3;
        index = 0;
        break;
    case 0xEDF:
        type = 3;
        index = 1;
        break;
    case 0xEE0:
        type = 3;
        index = 2;
        break;
    case 0xEE1:
        type = 3;
        index = 3;
        break;
    case 0xEE2:
        type = 3;
        index = 0;
        break;
    case 0xEE3:
        type = 3;
        index = 1;
        break;
    case 0xEE4:
        type = 3;
        index = 2;
        break;
    case 0xEE5:
        type = 3;
        index = 3;
        break;
    default:
        return;
    }

    switch (type) {
    case 0:
        break;
    case 1:
        window = *(u32*)((u8*)item + 0x64);
        msgctrlSetValue(0x37, heroBiosGetNamePtr(fn_8006C7D4_GetHero(index)));
        fn_800FB680(0, 0, window, 0xD0);
        break;
    case 2:
        spriteId = 0;
        trainer = &savedataGetStatus(0, 0xE)->trainers[index];
        kind = fn_8006C7D4_GetKind(trainer);
        switch (kind) {
        case 0:
            spriteId = 0x29F;
            break;
        case 1:
            spriteId = 0x2A1;
            break;
        case 2:
            spriteId = 0x2A2;
            break;
        case 3:
            spriteId = 0x2A3;
            break;
        case 4:
            spriteId = 0x2A0;
            break;
        }
        if (spriteId != 0) {
            void* sprite = menuSpriteBiosGetPtr(spriteId);

            fn_80071318(item, sprite);
        }
        break;
    case 3:
        window = *(u32*)((u8*)item + 0x64);
        id = heroBiosGetRnd(fn_8006C7D4_GetHero(index));
        d = digits;
        d[0] = id % 10;
        d[1] = id / 10 % 10;
        d[2] = id / 100 % 10;
        d[3] = id / 1000 % 10;
        d[4] = id / 10000 % 10;
        sprintf(text, (const char*)lbl_802686D0, digits[4], digits[3], digits[2], digits[1], digits[0]);
        GScharMakeFromSJIS(message, text);
        msgctrlSetValue(0x37, message);
        fn_800FB680(0, 0, window, 0xD0);
        break;
    }
}


/* 0x8006CCC0 | size: 0x890 */
typedef struct MenuMiddleSprite_8006CCC0 {
    u8 pad00[6];
    s16 itemId;         /* 0x06 */
    u8 pad08[0x44];
    u32 spriteId;       /* 0x4C */
    s16 x;              /* 0x50 */
    u8 pad52[0x12];
    u32 color;          /* 0x64 */
} MenuMiddleSprite_8006CCC0;

typedef struct MenuMiddleBattleInfo_8006CCC0 {
    s32 mode;           /* 0x00 */
    s32 type;           /* 0x04 */
    s32 unk08;
    s32 colosseum;      /* 0x0C */
    s32 unk10;
    s32 count;          /* 0x14 */
    s32 wins;           /* 0x18 */
} MenuMiddleBattleInfo_8006CCC0;

void fn_8006CCC0(void* window, MenuMiddleSprite_8006CCC0* sprite) {
    extern void* windowSearchItemID(void* menu, s32 itemId);
    extern u32 GSmsgGetRect(u32 msg);
    extern void msgctrlSetValue(s32 slot, u32 value);
    extern void fn_800FB680(s16 x, s16 y, u32 color, u32 msg);
    extern u16* heroBiosGetNamePtr(void* hero);
    extern u8* heroBiosGetPokemonPtr(void* hero, u16 index);
    extern u8 fn_80077A5C(void);
    extern u8 pokemonCheckValid(u8* pokemon);
    extern u8 pokemonBiosGetTamagoFlag(u8* pokemon);
    extern void fn_8010B718(void* window, void* sprite, u8* pokemon);
    extern u16* pokemonBiosGetNicknamePtr(u8* pokemon);
    extern u8 menuSubGetPokemonSexForDisp(u8* pokemon);
    extern u32 pokemonGetStatus(u8* pokemon, s32 a, s32 b, s32 c);
    extern u8 fn_800774D4(u8* pokemon, void* item, s32 flag);
    extern u8 fn_80076F2C(void* hero, void* item, s32 flag);
    extern void __assert(const char* file, s32 line, const char* expr);
    s32 kind = 0;
    void* hero = windowGetParam(window, 0);
    s32 rank = (s32)windowGetParam(window, 1);
    void* item = windowGetParam(window, 2);
    MenuMiddleBattleInfo_8006CCC0* battle = windowGetParam(window, 3);
    u16 index = 0;
    u8 isEgg = 0;
    u8 invalid = 0;
    u16* name = NULL;
    u8* pokemon;
    u32 msg;
    s16 x;
    u32 color;
    u16* nickname;
    void* base;

    switch (*(s8*)((u8*)window + 1)) {
    case 0:
        break;
    case 1:
    case 2:
    case 3:
        switch (sprite->itemId) {
        case 0xE83:
        case 0xFC9:
            switch (rank) {
            case 0:
                sprite->spriteId = 0x3D8D;
                break;
            case 1:
            case 2:
                sprite->spriteId = 0x3D8F;
                break;
            case -1:
            default:
                sprite->spriteId = 0;
                break;
            }
            break;
        case 0xE80:
            if (battle != NULL) {
                switch (battle->colosseum) {
                case 0:
                    sprite->spriteId = 0x3D91;
                    break;
                case 1:
                    sprite->spriteId = 0x3D93;
                    break;
                case 2:
                    sprite->spriteId = 0x3D94;
                    break;
                case 3:
                    sprite->spriteId = 0x3D95;
                    break;
                case 4:
                    sprite->spriteId = 0x3D9B;
                    break;
                case 5:
                    sprite->spriteId = 0x3D9C;
                    break;
                case 6:
                    sprite->spriteId = 0x3DAF;
                    break;
                }
            }
            break;
        case 0xE81:
            if (battle != NULL) {
                switch (battle->type) {
                case 0:
                    sprite->spriteId = 0x3D9D;
                    break;
                case 1:
                    sprite->spriteId = 0x3D9E;
                    break;
                default:
                    __assert((char*)lbl_80268680, 0xB47, (char*)&lbl_8047C064);
                    break;
                }
                base = windowSearchItemID(window, 0xE80);
                sprite->x = ((MenuMiddleSprite_8006CCC0*)base)->x +
                            (GSmsgGetRect(((MenuMiddleSprite_8006CCC0*)base)->spriteId) >> 16);
            }
            break;
        case 0xE82:
            break;
        case 0xE85:
        case 0xFCB:
            sprite->spriteId = 0x3D8B;
            break;
        case 0xE5C:
            kind = 1;
            index = 0;
            break;
        case 0xE5D:
            kind = 1;
            index = 1;
            break;
        case 0xE5E:
            kind = 1;
            index = 2;
            break;
        case 0xE5F:
            kind = 1;
            index = 3;
            break;
        case 0xE60:
            kind = 1;
            index = 4;
            break;
        case 0xE61:
            kind = 1;
            index = 5;
            break;
        case 0xE74:
            kind = 2;
            index = 0;
            break;
        case 0xE75:
            kind = 2;
            index = 1;
            break;
        case 0xE76:
            kind = 2;
            index = 2;
            break;
        case 0xE77:
            kind = 2;
            index = 3;
            break;
        case 0xE78:
            kind = 2;
            index = 4;
            break;
        case 0xE79:
            kind = 2;
            index = 5;
            break;
        case 0xE7A:
            kind = 3;
            index = 0;
            break;
        case 0xE7B:
            kind = 3;
            index = 1;
            break;
        case 0xE7C:
            kind = 3;
            index = 2;
            break;
        case 0xE7D:
            kind = 3;
            index = 3;
            break;
        case 0xE7E:
            kind = 3;
            index = 4;
            break;
        case 0xE7F:
            kind = 3;
            index = 5;
            break;
        case 0xF2C:
            kind = 1;
            index = 0;
            break;
        case 0xF2D:
            kind = 1;
            index = 1;
            break;
        case 0xF2E:
            kind = 1;
            index = 2;
            break;
        case 0xF2F:
            kind = 1;
            index = 3;
            break;
        case 0xF30:
            kind = 1;
            index = 4;
            break;
        case 0xF31:
            kind = 1;
            index = 5;
            break;
        case 0xF44:
            kind = 2;
            index = 0;
            break;
        case 0xF45:
            kind = 2;
            index = 1;
            break;
        case 0xF46:
            kind = 2;
            index = 2;
            break;
        case 0xF47:
            kind = 2;
            index = 3;
            break;
        case 0xF48:
            kind = 2;
            index = 4;
            break;
        case 0xF49:
            kind = 2;
            index = 5;
            break;
        case 0xF4A:
            kind = 3;
            index = 0;
            break;
        case 0xF4B:
            kind = 3;
            index = 1;
            break;
        case 0xF4C:
            kind = 3;
            index = 2;
            break;
        case 0xF4D:
            kind = 3;
            index = 3;
            break;
        case 0xF4E:
            kind = 3;
            index = 4;
            break;
        case 0xF4F:
            kind = 3;
            index = 5;
            break;
        case 0xE84:
        case 0xFCA:
            if (hero != NULL) {
                name = heroBiosGetNamePtr(hero);
                kind = 4;
            }
            break;
        case 0x12BC:
            if (battle != NULL) {
                msg = 0;
                switch (battle->mode) {
                case 0:
                    switch (battle->count) {
                    case 0:
                    case 1:
                    case 2:
                    case 3:
                    case 4:
                    case 5:
                        msgctrlSetValue(0x2F, battle->count + 1);
                        msg = 0x3D9F;
                        break;
                    case 6:
                        msg = 0x3DA0;
                        break;
                    case 7:
                        msg = 0x3DA1;
                        break;
                    }
                    break;
                case 1:
                    msgctrlSetValue(0x2F, battle->count);
                    msg = 0x3DA2;
                    break;
                }
                fn_800FB680(0, 0, sprite->color, msg);
                x = (GSmsgGetRect(msg) >> 16) + 0x24;
                msgctrlSetValue(0x2F, battle->wins);
                fn_800FB680(x, 0, sprite->color, 0x3DA4);
            }
            break;
        }

        if (kind != 0) {
            pokemon = heroBiosGetPokemonPtr(hero, index);
            if (fn_80077A5C()) {
                pokemon = NULL;
            } else if (!pokemonCheckValid(pokemon)) {
                pokemon = NULL;
                invalid = 1;
                isEgg = 1;
            } else {
                isEgg = pokemonBiosGetTamagoFlag(pokemon);
            }

            switch (kind) {
            case 1:
                if (pokemon != NULL) {
                    fn_8010B718(window, sprite, pokemon);
                }
                break;
            case 2:
                if (invalid) {
                    fn_800FB680(0, 0, sprite->color, 0x56C);
                } else if (isEgg) {
                    fn_800FB680(0, 0, sprite->color, 0x56B);
                } else if (pokemon != NULL) {
                    nickname = pokemonBiosGetNicknamePtr(pokemon);
                    if (nickname != NULL && *nickname != 0) {
                        msgctrlSetValue(0x37, (u32)nickname);
                        fn_800FB680(0, 0, sprite->color, 0xE7);
                    }
                    switch (menuSubGetPokemonSexForDisp(pokemon)) {
                    case 0:
                        msg = 0xD67;
                        break;
                    case 1:
                        msg = 0xD68;
                        break;
                    case 2:
                    default:
                        msg = 0;
                        break;
                    }
                    if (msg != 0 && ((u8*)&sprite->color)[3] == 0xFF) {
                        fn_800FB680(0x5C, 0, sprite->color, msg);
                    }
                }
                break;
            case 3:
                if (pokemon != NULL && !isEgg) {
                    color = sprite->color;
                    msgctrlSetValue(0x2F, pokemonGetStatus(pokemon, 0, 0x7A, 0));
                    if (item != NULL &&
                        (!fn_800774D4(pokemon, item, 0) || !fn_800774D4(pokemon, item, 1) ||
                         !fn_80076F2C(hero, item, 0))) {
                        color |= 0xFF000000;
                        color &= 0xFF0000FF;
                    }
                    fn_800FB680(0, 0, color, 0x41FA);
                }
                break;
            case 4:
                if (name != NULL) {
                    msgctrlSetValue(0x37, (u32)name);
                    fn_800FB680(0, 0, sprite->color, 0xCF);
                }
                break;
            }
        }
        break;
    case 4:
    case 5:
        break;
    }
}


/* 0x8006D550 | size: 0x3F0 */
typedef struct MenuMiddleTrainer_8006D550 {
    u8 pad0000[0xB44];
    u8 hero[0xB1C];     /* 0x0B44 */
} MenuMiddleTrainer_8006D550;

static inline s32 fn_8006D550_GetTrainerType(MenuMiddleTrainer_8006D550* trainer) {
    extern u8 heroBiosGetSexDataId(void* hero);

    switch ((s32)fn_8006A7E8((u32)trainer)) {
    case 0:
        return 0;
    case 1:
        switch (heroBiosGetSexDataId(trainer->hero)) {
        case 0:
            return 1;
        case 1:
            return 2;
        case 2:
            break;
        }
        break;
    case 2:
        switch (heroBiosGetSexDataId(trainer->hero)) {
        case 0:
            return 3;
        case 1:
            return 4;
        case 2:
            break;
        }
        break;
    }
    return 1;
}

void fn_8006D550(void* window, MenuMiddleSprite_8006CCC0* sprite) {
    typedef struct MenuMiddlePlayer_8006D550 {
        u8 pad0[0x24];
        s32 controllerId;   /* 0x24 */
        u8 pad28[0x1638];
    } MenuMiddlePlayer_8006D550;
    typedef struct MenuMiddleStatus_8006D550 {
        u8 pad0[0x59A8];
        MenuMiddlePlayer_8006D550 players[4]; /* 0x59A8 */
    } MenuMiddleStatus_8006D550;
    extern u8 winSpriteGetDisp(void* sprite);
    extern MenuMiddleStatus_8006D550* savedataGetStatus(s32 idx, s32 type);
    extern u16* heroBiosGetNamePtr(void* hero);
    extern u16 heroBiosGetRnd(void* hero);
    extern void msgctrlSetValue(s32 slot, u32 value);
    extern void fn_800FB680(s16 x, s16 y, u32 color, u32 msg);
    extern int sprintf(char* buf, const char* fmt, ...);
    extern void GScharMakeFromSJIS(u16* dst, char* src);
    extern void* menuSpriteBiosGetPtr(u32 spriteId);
    extern void fn_80071318(void* sprite, void* data);
    extern void __assert(const char* file, s32 line, const char* expr);
    u8 digits[5];
    char text[0x80];
    u16 message[0x84];
    s32 kind;
    MenuMiddleTrainer_8006D550* trainer;
    u32 color;
    u32 i;
    s32 port;
    u32 id;
    u32 spriteId;
    u8* d;

    if (!winSpriteGetDisp(sprite)) {
        return;
    }

    trainer = NULL;
    switch (sprite->itemId) {
    case 0xA73:
        kind = 2;
        port = 0;
        break;
    case 0xA74:
        kind = 2;
        port = 1;
        break;
    case 0xA75:
        kind = 2;
        port = 2;
        break;
    case 0xA76:
        kind = 2;
        port = 3;
        break;
    case 0xA6F:
        kind = 1;
        port = 0;
        break;
    case 0xA70:
        kind = 1;
        port = 1;
        break;
    case 0xA71:
        kind = 1;
        port = 2;
        break;
    case 0xA72:
        kind = 1;
        port = 3;
        break;
    case 0xA4F:
        kind = 3;
        port = 0;
        break;
    case 0xA50:
        kind = 3;
        port = 1;
        break;
    case 0xA51:
        kind = 3;
        port = 2;
        break;
    case 0xA52:
        kind = 3;
        port = 3;
        break;
    default:
        return;
    }

    for (i = 0; i < 4; i++) {
        if (port == menuCBBios_ControlerIDtoPortID(savedataGetStatus(0, 0xE)->players[i].controllerId)) {
            trainer = (MenuMiddleTrainer_8006D550*)&savedataGetStatus(0, 0xE)->players[i];
            break;
        }
    }
    if (trainer == NULL) {
        return;
    }

    switch (kind) {
    case 1:
        color = sprite->color;
        msgctrlSetValue(0x37, (u32)heroBiosGetNamePtr(trainer->hero));
        fn_800FB680(0, 0, color, 0xCF);
        break;
    case 2:
        color = sprite->color;
        id = heroBiosGetRnd(trainer->hero);
        d = digits;
        d[0] = id % 10;
        d[1] = id / 10 % 10;
        d[2] = id / 100 % 10;
        d[3] = id / 1000 % 10;
        d[4] = id / 10000 % 10;
        sprintf(text, (char*)lbl_802686D0, digits[4], digits[3], digits[2], digits[1], digits[0]);
        GScharMakeFromSJIS(message, text);
        msgctrlSetValue(0x37, (u32)message);
        fn_800FB680(0, 0, color, 0xCF);
        break;
    case 3:
        spriteId = 0;
        switch (fn_8006D550_GetTrainerType(trainer)) {
        case 0:
            spriteId = 0x2BA;
            break;
        case 1:
            spriteId = 0x2BC;
            break;
        case 2:
            spriteId = 0x2B5;
            break;
        case 3:
            spriteId = 0x2BB;
            break;
        case 4:
            spriteId = 0x2B4;
            break;
        }
        if (spriteId != 0) {
            fn_80071318(sprite, menuSpriteBiosGetPtr(spriteId));
        }
        break;
    default:
        __assert((char*)lbl_80268680, 0xAE6, (char*)&lbl_8047C064);
        break;
    }
}


/* 0x8006D940 | size: 0x4C */
#pragma push
#pragma peephole off
void fn_8006D940(void* menu) {
    extern void* windowSearchItemID(void* menu, s32 itemId);
    void* value;
    void* target;

    value = windowGetParam(menu, 0);
    target = windowSearchItemID(menu, 0xE8E);
    MENU_MIDDLE_U32_004C(target)->unk_004C = (u32)value;
}
#pragma pop


/* 0x8006D98C | size: 0x158 */
void fn_8006D98C(void* menu) {
    typedef struct MenuDisplayEntry {
        u16 itemId;
        u16 padding;
        u32 spriteId;
    } MenuDisplayEntry;
    extern void* windowGetParam(void* menu, s32 index);
    extern void* windowSearchItemID(void* menu, s32 itemId);
    extern void fn_801081F8(void* menu, u16 itemId, u16 messageId);
    extern void fn_80070D84(void* menu, void* table, s32 count);
    MenuDisplayEntry* entry;
    s32 messageIndex;
    void* node;
    u8 option;
    u32 i;

    option = (u8)(u32)windowGetParam(menu, 0);
    messageIndex = option != 0 ? 6 : 3;

    switch (*(s8*)((u8*)menu + 1)) {
    case 0:
        if (*(s8*)((u8*)menu + 2) == 0) {
            *(s16*)((u8*)menu + 0x84) = option != 0 ? 0x152 : 0;
            entry = (MenuDisplayEntry*)lbl_8026864C;
            for (i = 0; i < 5; i++) {
                MENU_MIDDLE_U32_004C(windowSearchItemID(menu, entry[i].itemId))->unk_004C = entry[i].spriteId;
            }

            node = (void*)MENU_MIDDLE_U32_001C(menu)->unk_001C;
            while (node != NULL) {
                fn_801081F8(menu, *(s16*)((u8*)node + 6),
                           *(u16*)(lbl_80267EA8 + messageIndex * 4));
                node = *(void**)node;
            }
        }
        break;
    case 3:
        if (*(s8*)((u8*)menu + 2) == 0) {
            void* n = (void*)MENU_MIDDLE_U32_001C(menu)->unk_001C;
            while (n != NULL) {
                fn_801081F8(menu, *(s16*)((u8*)n + 6),
                           *(u16*)(lbl_80267EA8 + messageIndex * 4 + 2));
                n = *(void**)n;
            }
        }
        break;
    }

    fn_80070D84(menu, NULL, 0);
}


/* 0x8006DAE4 | size: 0x144 */
#pragma peephole off
void fn_8006DAE4(void* arg0) {
    extern void* windowGetParam(void*, int);
    extern void* windowSearchItemID(void*, int);
    extern void menuSetPosition(int, int, s16);
    extern u32 GSmsgGetRect(void*);
    extern void winSpriteSetDisp(void*, int);
    extern void fn_80070D84(void*, void*, int);

    typedef struct {
        u8 unk0;
        s8 unk1;
        s8 unk2;
    } arg_state_DAE4;

    typedef struct {
        u8 unk0[0x4c];
        u32 field_4c;
        u8 pad50[2];
        s16 field_52;
    } slot_obj_DAE4;

    void* a;
    void* b;
    void* c;
    slot_obj_DAE4* slot;
    s32 d29;
    s32 d27;
    s16 cmp;

    if (((arg_state_DAE4*)arg0)->unk2 == 0) {
        switch (((arg_state_DAE4*)arg0)->unk1) {
        case 0:
        {
            a = windowGetParam(arg0, 0);
            b = windowGetParam(arg0, 1);
            c = windowGetParam(arg0, 2);
            menuSetPosition(0xd6, 0, (s16)(s32)windowGetParam(arg0, 3));
            slot = (slot_obj_DAE4*)windowSearchItemID(arg0, 0xe8c);
            slot->field_4c = (u32)a;
            slot = (slot_obj_DAE4*)windowSearchItemID(arg0, 0xe8d);
            slot->field_4c = (u32)b;
            cmp = (s16)(u16)GSmsgGetRect(c);
            d29 = 0;
            d27 = 0;
            if (cmp > 0x32) {
                d29 = -0x14;
                d27 = -0xa;
            }
            slot = (slot_obj_DAE4*)windowSearchItemID(arg0, 0xe8a);
            slot->field_4c = (u32)c;
            slot->field_52 = slot->field_52 + d29;
            slot = (slot_obj_DAE4*)windowSearchItemID(arg0, 0xe87);
            winSpriteSetDisp(slot, c != 0);
            slot->field_52 = slot->field_52 + d27;
            break;
        }
        }
    }
    fn_80070D84(arg0, lbl_8026860C, 8);
}
#pragma peephole reset


/* 0x8006DC28 | size: 0x4A4 */
/* RULE-EXCEPTION(user-approved): local peephole-control pragma — see docs/RULE_EXCEPTIONS.md */
#pragma peephole off
void fn_8006DC28(void* menu) {
    extern void fn_80070D84(void* menu, s32 a, s32 b);
    extern void winSpriteSetDisp(void* widget, u8 flag);
    extern void fn_8010B01C(s32 kind, u32 (*compare)(u8*));
    extern s32 pokemonBiosGetItemDataId(void* pokemon);
    extern u8 pokemonIsDarkPokemon(void* pokemon);
    extern u8 pokemonCheckValid(void* pokemon);
    extern void* heroBiosGetPokemonPtr(void* hero, u16 index);
    extern u32 fn_8006E128(u8* p);
    extern void* windowSearchItemID(void* menu, s32 itemId);
    extern void* menuSpriteBiosGetPtr(s32 spriteId);
    extern void fn_80071318(void* widget, void* sprite);
    extern u8 fn_80076398(void* pokemon, s32 kind);
    extern u8 menuCBRule_CheckPokemonErrorAll(void* pokemon);
    extern u8 fn_800772AC(void* pokemon, void* rule);
    extern u8 fn_80076A8C(void* hero, void* pokemon, void* rule, s32 kind);
    extern u8 menuCBRule_CheckValidItem(s32 itemId);
    extern u8 fn_80077C68(s32 itemId);
    extern void winSetSequence(void* widget, s32 sequence);
    void* rule;
    void* hero;
    void* widget;

    hero = windowGetParam(menu, 0);
    windowGetParam(menu, 1);
    rule = windowGetParam(menu, 2);

    if ((s8)MENU_MIDDLE_U8_0002(menu)->unk_0002 == 0) {
    switch ((s8)MENU_MIDDLE_U8_0001(menu)->unk_0001) {
    case 0: {
        s32 i;
        void* pokemon;
        u8* node;
        u8 empty;
        s32 sequence;
        void** list;
        s32 slot;
        s32 count;
        u8 error;
        u16 (*itemIds)[4];

        slot = count = 0;
        do {
            void* pokemon = heroBiosGetPokemonPtr(hero, slot);
            if (pokemonCheckValid(pokemon) != 0) {
                ((void**)lbl_803B6D68)[count++] = pokemon;
            }
            slot++;
        } while (slot < 6);
        list = (void**)lbl_803B6D68;
        list[count] = NULL;
        list[7] = NULL;
        fn_8010B01C(0, fn_8006E128);

        itemIds = (u16 (*)[4])lbl_8026858C;
        for (i = 0; (u32)i < 12; i++) {
            pokemon = heroBiosGetPokemonPtr(hero, i % 6);
            empty = (fn_80076398(pokemon, 0) == 0);
            error = (empty != 0 || menuCBRule_CheckPokemonErrorAll(pokemon) == 0);
            if (rule != NULL) {
                if (pokemon != NULL && pokemonCheckValid(pokemon) != 0) {
                    error |= (fn_800772AC(pokemon, rule) == 0);
                }
                error |= (fn_80076A8C(hero, pokemon, rule, 1) == 0);
                error |= (fn_80076A8C(hero, pokemon, rule, 2) == 0);
                error |= (fn_80076A8C(hero, pokemon, rule, 3) == 0);
            }

            widget = windowSearchItemID(menu, itemIds[i][0]);
            if (widget != NULL) {
                if (empty != 0) {
                    fn_80071318(widget, menuSpriteBiosGetPtr(0x375));
                } else if (error != 0) {
                    fn_80071318(widget, menuSpriteBiosGetPtr(0x25B));
                }
            }

            widget = windowSearchItemID(menu, itemIds[i][1]);
            if (widget != NULL) {
                s32 spriteId;
                spriteId = 0x274;
                if (error != 0) {
                    spriteId = 0x25C;
                }
                if (pokemonIsDarkPokemon(pokemon) != 0) {
                    spriteId = 0x341;
                }
                fn_80071318(widget, menuSpriteBiosGetPtr(spriteId));
            }

            widget = windowSearchItemID(menu, itemIds[i][2]);
            if (widget != NULL) {
                winSpriteSetDisp(widget, error);
            }
        }

        node = (u8*)MENU_MIDDLE_U32_001C(menu)->unk_001C;
        sequence = ((u16*)&lbl_80267EA8)[6];
        while (node != NULL) {
            winSetSequence(node + 0xC, sequence);
            node = *(u8**)node;
        }
        break;
    }
    case 1:
    case 2: {
        u16 (*itemIds)[4];
        void* pokemon;
        s32 i;
        s32 itemId;
        u8 usable;

        itemIds = (u16 (*)[4])lbl_8026858C;
        for (i = 0; (u32)i < 12; i++) {
            widget = windowSearchItemID(menu, itemIds[i][3]);
            if (widget != NULL) {
                pokemon = heroBiosGetPokemonPtr(hero, i % 6);
                if (pokemon != NULL && pokemonCheckValid(pokemon) != 0) {
                    itemId = pokemonBiosGetItemDataId(pokemon);
                    usable = (menuCBRule_CheckValidItem(itemId) != 0 && fn_80077C68(itemId) != 0);
                    if (rule != NULL) {
                        usable &= fn_80076A8C(hero, pokemon, rule, 2);
                    }
                    winSpriteSetDisp(widget, (u16)itemId != 0);
                    MENU_MIDDLE_U32_0064(widget)->unk_0064 =
                        (MENU_MIDDLE_U32_0064(widget)->unk_0064 & 0xFF) |
                        (usable != 0 ? 0xFFFFFF00 : 0xFF000000);
                } else {
                    winSpriteSetDisp(widget, 0);
                }
            }
        }
        break;
    }
    case 3: {
        u8* node;
        s32 sequence;

        node = (u8*)MENU_MIDDLE_U32_001C(menu)->unk_001C;
        sequence = ((u16*)&lbl_80267EA8)[13];
        while (node != NULL) {
            winSetSequence(node + 0xC, sequence);
            node = *(u8**)node;
        }
        break;
    }
    }
    }

    fn_80070D84(menu, 0, 0);
}
#pragma peephole reset


/* 0x8006E0CC | size: 0x5C */
#pragma peephole off
void fn_8006E0CC(void) {
    extern void fn_8010BBB8(void* ptr);
    extern s8 fn_8010BCE4(void);
    extern void _threadSwitch(void);
    void** list = (void**)&lbl_803B6D68;
    s32 i = 0;

    while (list[i] != NULL) {
        fn_8010BBB8(list[i]);
        if (fn_8010BCE4() == 0) {
            _threadSwitch();
        } else {
            i++;
        }
    }
}
#pragma peephole reset


/* 0x8006E128 | size: 0x38 */
u32 fn_8006E128(u8* p) {
    u32 index;

    if (p == NULL) {
        return 0;
    }
    index = *(u32*)(p + 0x1C);
    if (index >= 7) {
        return 0;
    }
    *(u32*)(p + 0x1C) = index + 1;
    return *(u32*)(p + index * 4);
}


/* 0x8006E160 | size: 0x28 */
#pragma push
#pragma scheduling off
void fn_8006E160(u32 r3) {
    extern void fn_80070D84(u32 r3, u32 r4, u32 r5);
    fn_80070D84(r3, 0x0, 0x0);
}
#pragma pop

/* 0x8006E188 | size: 0x4 */
void fn_8006E188(void) {
}

/* 0x8006E18C | size: 0xCC */
#pragma push
#pragma peephole off
void fn_8006E18C(void* menu) {
    extern void fn_80070D84(void* menu, s32 arg1, s32 arg2);
    extern void winSpriteSetDisp(void* widget, s32 flag);
    extern void* windowSearchItemID(void* menu, s32 itemId);
    extern s32 fn_80071160(void);
    extern void fn_80107F38(s32 param, u32 key);
    typedef struct Entry_8006E18C {
        u16 itemId;
        u8 threshold;
        u8 pad3;
    } Entry_8006E18C;

    Entry_8006E18C* table = (Entry_8006E18C*)&lbl_80268574;
    void* widget;
    s32 flags;
    u32 i;

    for (i = 0; i < 6; i++) {
        s32 diff;
        s32 threshold;
        u32 active;

        widget = windowSearchItemID(menu, table[i].itemId);
        threshold = MENU_MIDDLE_U8_0095(menu)->unk_0095;
        threshold = (s8)threshold;
        diff = table[i].threshold - threshold;
        active = (u32)__cntlzw(diff) >> 5;
        active = active & 0xFF;
        winSpriteSetDisp(widget, active);
    }

    flags = MENU_MIDDLE_U8_0001(menu)->unk_0001;
    flags = (s8)flags;
    switch (flags) {
    case 2:
        if (fn_80071160() != 0) {
            fn_80107F38(MENU_MIDDLE_U32_0004(menu)->unk_0004, 0x1CE);
            MENU_MIDDLE_U8_0098(menu)->unk_0098 = 1;
            MENU_MIDDLE_U8_0099(menu)->unk_0099 = 1;
            return;
        }
        break;
    }

    fn_80070D84(menu, 0, 0);
}
#pragma pop


/* 0x8006E258 | size: 0xE0 */
#pragma peephole off
void fn_8006E258(void* menu) {
    extern s32 menuCBBios_ControlerIDtoPortID(u32 flags);
    extern void* windowSearchItemID(void* menu, s32 itemId);
    extern void winSpriteSetDisp(void* widget, u8 flag);
    extern u8* savedataGetStatus(s32 idx, s32 type);
    void* widget[5];
    u16 (*itemIds)[5];
    s32 portId;
    u32 slot;
    u8 active;
    u32 i;

    portId = menuCBBios_ControlerIDtoPortID(MENU_MIDDLE_U32_59CC(savedataGetStatus(0, 0xE))->unk_59CC);
    itemIds = (u16 (*)[5])lbl_80268560;
    for (slot = 0; slot < 2; slot++) {
        active = (slot == portId);

        for (i = 0; i < 5; i++) {
            void* w = windowSearchItemID(menu, itemIds[slot][i]);
            winSpriteSetDisp(w, active);
            widget[i] = w;
        }

        MENU_MIDDLE_U32_004C(widget[4])->unk_004C = active ? 0x424B : 0;
        MENU_MIDDLE_U32_004C(widget[3])->unk_004C = active ? 0x3F40 : 0;
    }
}
#pragma peephole reset


/* 0x8006E338 | size: 0x460 */
static inline void fn_8006E338_Update(void* obj) {
    typedef struct MenuMiddlePlayer_8006E338 {
        u8 pad0[0x24];
        s32 controllerId;   /* 0x24 */
        u8 pad28[0x1635];
        u8 ready;           /* 0x165D */
        u8 pad165E[2];
    } MenuMiddlePlayer_8006E338;
    typedef struct MenuMiddleStatus_8006E338 {
        u8 pad0[0x59A8];
        MenuMiddlePlayer_8006E338 players[4]; /* 0x59A8 */
    } MenuMiddleStatus_8006E338;
    extern void* windowSearchItemID(void* menu, s32 itemId);
    extern void winSetSequence(void* widget, s32 sequence);
    extern MenuMiddleStatus_8006E338* savedataGetStatus(s32 idx, s32 type);
    extern void* menuSpriteBiosGetPtr(s32 spriteId);
    extern void fn_80071318(void* widget, void* sprite);
    extern void winSpriteSetDisp(void* widget, u8 flag);
    extern void fn_80070D84(void* menu, void* table, s32 count);
    extern void __assert(const char* file, s32 line, const char* expr);
    u8* data;
    u8 used[4] = {0, 0, 0, 0};
    void* widgets[14];
    void* playerWidgets[14];
    s32 notReady;
    s32 player;
    s32 port;
    s32 row;
    s32 col;
    s32 j;
    u32 color;
    u32 index;
    u8 ready;
    void* sprite;
    void* widget;
    u8 allReady;

    data = lbl_80267EA8;
    allReady = 1;
    if (*(s8*)((u8*)obj + 2) == 0) {
        switch (*(s8*)((u8*)obj + 1)) {
        case 0:
            for (row = 0; row < 4u; row++) {
                index = ((u32*)(data + 0x698))[row];
                for (col = 0; col < 14u; col++) {
                    winSetSequence((u8*)windowSearchItemID(obj, ((u16(*)[14])(data + 0x628))[row][col]) + 0xC,
                                   ((u16(*)[2])(data + 0x0))[index][0]);
                }
            }
            break;
        case 3:
            for (row = 0; row < 4u; row++) {
                index = ((u32*)(data + 0x698))[row];
                for (col = 0; col < 14u; col++) {
                    winSetSequence((u8*)windowSearchItemID(obj, ((u16(*)[14])(data + 0x628))[row][col]) + 0xC,
                                   ((u16(*)[2])(data + 0x0))[index][1]);
                }
            }
            break;
        }
    }

    for (player = 0; player < 4; player++) {
        port = menuCBBios_ControlerIDtoPortID(savedataGetStatus(0, 0xE)->players[player].controllerId);
        if (port >= 0) {
            used[port] = 1;
            for (col = 0; col < 14u; col++) {
                playerWidgets[col] = windowSearchItemID(obj, ((u16(*)[14])(data + 0x628))[port][col]);
            }
            fn_80071318(playerWidgets[4],
                        menuSpriteBiosGetPtr((s32)fn_8006A7E8((u32)&savedataGetStatus(0, 0xE)->players[player]) != 0 ? 0x2B2 : 0x2AE));
            fn_80071318(playerWidgets[5], menuSpriteBiosGetPtr(((u16*)&lbl_8047C058)[player]));
            color = ((u32*)(data + 0x6A8))[player];
            MENU_MIDDLE_U32_0064(playerWidgets[0])->unk_0064 =
                (MENU_MIDDLE_U32_0064(playerWidgets[0])->unk_0064 & 0xFF) | color;
            MENU_MIDDLE_U32_0064(playerWidgets[1])->unk_0064 =
                (MENU_MIDDLE_U32_0064(playerWidgets[1])->unk_0064 & 0xFF) | color;
            MENU_MIDDLE_U32_0064(playerWidgets[2])->unk_0064 =
                (MENU_MIDDLE_U32_0064(playerWidgets[2])->unk_0064 & 0xFF) | color;
            allReady &= savedataGetStatus(0, 0xE)->players[player].ready;
        }
    }

    notReady = allReady == 0;
    for (row = 0; row < 4u; row++) {
        for (col = 0; col < 14u; col++) {
            widgets[col] = windowSearchItemID(obj, ((u16(*)[14])(data + 0x628))[row][col]);
        }
        if (used[row] != 0) {
            for (j = 0; j < 4; j++) {
                if (row == menuCBBios_ControlerIDtoPortID(savedataGetStatus(0, 0xE)->players[j].controllerId)) {
                    break;
                }
            }
            if (!(4u > j)) {
                __assert((char*)(data + 0x7D8), 0x8A1, (char*)(data + 0x83C));
            }
            ready = savedataGetStatus(0, 0xE)->players[j].ready;
            MENU_MIDDLE_U32_004C(widgets[10])->unk_004C = ready ? 0 : 0x3F3F;
            MENU_MIDDLE_U32_004C(widgets[9])->unk_004C = ready ? 0 : 0x3F40;
            winSpriteSetDisp(widgets[2], notReady);
            winSpriteSetDisp(widgets[8], ready == 0);
            winSpriteSetDisp(widgets[7], ready == 0);
            winSpriteSetDisp(widgets[13], ready != 0 && notReady != 0);
        } else {
            for (col = 0; col < 14u; col++) {
                winSpriteSetDisp(widgets[col], 0);
            }
        }
    }

    if (savedataGetStatus(0, 0xE)->players[0].controllerId != 1) {
        sprite = menuSpriteBiosGetPtr(0x2AE);
        widget = windowSearchItemID(obj, ((u16(*)[14])(data + 0x628))[0][4]);
        fn_80071318(widget, sprite);
        winSpriteSetDisp(widget, 1);
        winSpriteSetDisp(windowSearchItemID(obj, ((u16(*)[14])(data + 0x628))[0][6]), 1);
    }

    fn_80070D84(obj, NULL, 0);
    MENU_MIDDLE_U8_0098(obj)->unk_0098 = allReady;
}

void fn_8006E338(void* obj) {
    fn_8006E338_Update(obj);
}


/* 0x8006E798 | size: 0x20C */
void fn_8006E798(void* menu) {
    extern u8* savedataGetStatus(s32 index, s32 type);
    extern void* windowSearchItemID(void* menu, s32 itemId);
    extern void winSetSequence(void* sprite, u16 sequence);
    extern void winSpriteSetDisp(void* sprite, u8 visible);
    extern void fn_80070D84(void* menu, void* table, s32 count);
    u8* data = lbl_80267EA8;
    void* sprite;
    void* node;
    u16 sequence;
    s32 displaySet;
    s32 rule;
    s32 i;

    if (*(s8*)((u8*)menu + 2) == 0) {
        if (*(s8*)((u8*)menu + 1) == 0) {
            sequence = *(u16*)(data + 0x18);
            for (i = 0; i < 7; i++) {
                sprite = windowSearchItemID(
                    menu, *(u16*)(data + 0x5F4 + i * 2));
                winSetSequence((u8*)sprite + 0xC, sequence);
            }
        } else if (*(s8*)((u8*)menu + 1) == 3) {
            sequence = *(u16*)(data + 0x1A);
            for (i = 0; i < 7; i++) {
                sprite = windowSearchItemID(
                    menu, *(u16*)(data + 0x5F4 + i * 2));
                winSetSequence((u8*)sprite + 0xC, sequence);
            }
        }
    }

    rule = *(s32*)(savedataGetStatus(0, 0xE) + 4);
    displaySet = rule >= 0 && rule < 2 ? 0 : 2;
    if (fn_8006A7E8(
            (u32)(savedataGetStatus(0, 0xE) + 0x59A8)) != 0) {
        displaySet++;
    }

    node = windowSearchItemID(menu, 0x99B);
    for (i = 0; i < 8; i++) {
        winSpriteSetDisp(node, data[0x5D4 + displaySet * 8 + i]);
        node = *(void**)node;
    }

    sprite = windowSearchItemID(menu, 0x9A7);
    MENU_MIDDLE_U32_004C(sprite)->unk_004C = 0x3D2C;
    sprite = windowSearchItemID(menu, 0x9A9);
    MENU_MIDDLE_U32_004C(sprite)->unk_004C = 0x3D26;
    sprite = windowSearchItemID(menu, 0x9A6);
    MENU_MIDDLE_U32_004C(sprite)->unk_004C =
        *(u32*)(data + 0x604 +
                *(u32*)(savedataGetStatus(0, 0xE) + 8) * 4);
    sprite = windowSearchItemID(menu, 0x9A8);
    MENU_MIDDLE_U32_004C(sprite)->unk_004C =
        *(u32*)(data + 0x61C +
                *(u32*)(savedataGetStatus(0, 0xE) + 4) * 4);
    fn_80070D84(menu, NULL, 0);
}


/* 0x8006E9A4 | size: 0x4D8 */
void fn_8006E9A4(void* window, void* sprite) {
    extern void winSpriteGetDisp();
    extern void msgctrlSetValue();
    u8 sp[0x910];
    u32 r0 = 0;
    u32 r1 = (u32)sp;
    u32 r3 = (u32)window;
    u32 r4 = (u32)sprite;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    
    r30 = r3;
    r31 = r4;
    r0 = MENU_MIDDLE_U8_0001(r30)->unk_0001;
    r0 = (s8)r0;
    if ((s32)r0 == (s32)0x0) return;


    r3 = r31;
    winSpriteGetDisp();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x0) return;
    r3 = r30;
    r4 = 0x0;
    ((void(*)(void))windowGetParam)();
    r0 = MENU_MIDDLE_S16_0006(r31)->unk_0006;
    r5 = 0x0;
    if ((s32)r0 != (s32)0xd8e) {
        if ((s32)r0 < (s32)0xd8e) {
            if ((s32)r0 != (s32)0x969) {
                if ((s32)r0 < (s32)0x969) {
                    if ((s32)r0 != (s32)0x966) {
                        if ((s32)r0 < (s32)0x966) {
                            if ((s32)r0 != (s32)0x964) {
                                if ((s32)r0 < (s32)0x964) {
                                    goto L_8006EE30;
                                }
                                if ((s32)r0 < (s32)0x968) {
                                    goto L_8006ECA0;
                                }
                                if ((s32)r0 != (s32)0xa0f) {
                                    if ((s32)r0 < (s32)0xa0f) {
                                        if ((s32)r0 < (s32)0xa0e) {
                                            goto L_8006EE30;
                                        }
                                        if ((s32)r0 < (s32)0xd8d) {
                                            goto L_8006EE30;
                                        }
                                        if ((s32)r0 != (s32)0xd94) {
                                            if ((s32)r0 < (s32)0xd94) {
                                                if ((s32)r0 != (s32)0xd91) {
                                                    if ((s32)r0 < (s32)0xd91) {
                                                        if ((s32)r0 < (s32)0xd90) {
                                                            goto L_8006EAF8;
                                                        }
                                                        if ((s32)r0 >= (s32)0xd93) goto L_8006EB48;
                                                        goto L_8006EB40;
                                                    }
                                                    if ((s32)r0 == (s32)0xda0) goto L_8006EBFC;
                                                    if ((s32)r0 < (s32)0xda0) {
                                                        if ((s32)r0 >= (s32)0xd96) goto L_8006EE30;
                                                        goto L_8006EB88;
                                                    }
                                                    if ((s32)r0 >= (s32)0xda2) goto L_8006EE30;
                                                    goto L_8006EBD0;
                                                    }
                                                r4 = (0x51ec << 16);
                                                r0 = MENU_MIDDLE_S16_0000(r3)->unk_0000;
                                                r3 = r4 - 0x7ae1;
                                                r0 = (s32)((s64)r3 * (s64)r0 >> 32);
                                                r0 = (s32)r0 >> 5;
                                                r3 = (u32)r0 >> 31;
                                                r5 = r0 + r3;
                                                goto L_8006EE30;
                                            }
                                            r4 = (0x6666 << 16);
                                            r0 = MENU_MIDDLE_S16_0000(r3)->unk_0000;
                                            r3 = r4 + 0x6667;
                                            r0 = (s32)((s64)r3 * (s64)r0 >> 32);
                                            r0 = (s32)r0 >> 2;
                                            r3 = (u32)r0 >> 31;
                                            r5 = r0 + r3;
                                            goto L_8006EE30;
                                            L_8006EAF8: ;
                                            r5 = MENU_MIDDLE_S16_0000(r3)->unk_0000;
                                            goto L_8006EE30;
                                                        }
                                        r4 = (0x51ec << 16);
                                        r0 = MENU_MIDDLE_S16_0002(r3)->unk_0002;
                                        /* subi r3, r4, 0x7ae1 */;
                                        r0 = (s32)((s64)r3 * (s64)r0 >> 32);
                                        r0 = (s32)r0 >> 5;
                                        r3 = (u32)r0 >> 31;
                                        r5 = r0 + r3;
                                        goto L_8006EE30;
                                                }
                                    r4 = (0x6666 << 16);
                                    r0 = MENU_MIDDLE_S16_0002(r3)->unk_0002;
                                    r3 = r4 + 0x6667;
                                    r0 = (s32)((s64)r3 * (s64)r0 >> 32);
                                    r0 = (s32)r0 >> 2;
                                    r3 = (u32)r0 >> 31;
                                    r5 = r0 + r3;
                                    goto L_8006EE30;
                                    L_8006EB40: ;
                                    r5 = MENU_MIDDLE_S16_0002(r3)->unk_0002;
                                    goto L_8006EE30;
                                    L_8006EB48: ;
                                    r4 = (0x51ec << 16);
                                    r0 = MENU_MIDDLE_S16_0004(r3)->unk_0004;
                                    /* subi r3, r4, 0x7ae1 */;
                                    r0 = (s32)((s64)r3 * (s64)r0 >> 32);
                                    r0 = (s32)r0 >> 5;
                                    r3 = (u32)r0 >> 31;
                                    r5 = r0 + r3;
                                    goto L_8006EE30;
                                        }
                                r4 = (0x6666 << 16);
                                r0 = MENU_MIDDLE_S16_0004(r3)->unk_0004;
                                r3 = r4 + 0x6667;
                                r0 = (s32)((s64)r3 * (s64)r0 >> 32);
                                r0 = (s32)r0 >> 2;
                                r3 = (u32)r0 >> 31;
                                r5 = r0 + r3;
                                goto L_8006EE30;
                                L_8006EB88: ;
                                r5 = MENU_MIDDLE_S16_0004(r3)->unk_0004;
                                goto L_8006EE30;
                                    }
                            r0 = MENU_MIDDLE_S16_0014(r3)->unk_0014;
                            r3 = (0x6666 << 16);
                            r4 = r3 + 0x6667;
                            r3 = (s32)r0 >> 31;
                            r0 = r3 ^ r0;
                            r0 = r0 - r3;
                            r0 = (s32)((s64)r4 * (s64)r0 >> 32);
                            r0 = (s32)r0 >> 2;
                            r3 = (u32)r0 >> 31;
                            r5 = r0 + r3;
                            goto L_8006EE30;
                                        }
                        r3 = MENU_MIDDLE_S16_0014(r3)->unk_0014;
                        r0 = (s32)r3 >> 31;
                        r5 = r0 ^ r3;
                        r5 = r5 - r0;
                        goto L_8006EE30;
                        L_8006EBD0: ;
                        r0 = MENU_MIDDLE_S16_0016(r3)->unk_0016;
                        r3 = (0x6666 << 16);
                        r4 = r3 + 0x6667;
                        r3 = (s32)r0 >> 31;
                        r0 = r3 ^ r0;
                        r0 = r0 - r3;
                        r0 = (s32)((s64)r4 * (s64)r0 >> 32);
                        r0 = (s32)r0 >> 2;
                        r3 = (u32)r0 >> 31;
                        r5 = r0 + r3;
                        goto L_8006EE30;
                        L_8006EBFC: ;
                        r3 = MENU_MIDDLE_S16_0016(r3)->unk_0016;
                        r0 = (s32)r3 >> 31;
                        r5 = r0 ^ r3;
                        r5 = r5 - r0;
                        goto L_8006EE30;
                            }
                    r31 = MENU_MIDDLE_U32_0064(r31)->unk_0064;
                    r3 = (u32)sp + 0x288;
                    r4 = (u32)&lbl_8047C068;
                    r5 = 0x32;
                    /* crclr cr1eq */;
                    ((void(*)(void))sprintf)();
                    r3 = (u32)sp + 0x808;
                    r4 = (u32)sp + 0x288;
                    ((void(*)(void))GScharMakeFromSJIS)();
                    r4 = (u32)sp + 0x808;
                    r3 = 0x37;
                    msgctrlSetValue();
                    r5 = r31;
                    r3 = 0xa;
                    r4 = 0x0;
                    r6 = 0xcf;
                    ((void(*)(void))fn_800FB680)();
                    return;
                                }
                r31 = MENU_MIDDLE_U32_0064(r31)->unk_0064;
                r3 = (u32)sp + 0x208;
                r4 = (u32)&lbl_8047C068;
                r5 = 0x32;
                /* crclr cr1eq */;
                ((void(*)(void))sprintf)();
                r3 = (u32)sp + 0x708;
                r4 = (u32)sp + 0x208;
                ((void(*)(void))GScharMakeFromSJIS)();
                r4 = (u32)sp + 0x708;
                r3 = 0x37;
                msgctrlSetValue();
                r5 = r31;
                r3 = 0xa;
                r4 = 0x0;
                r6 = 0xcf;
                ((void(*)(void))fn_800FB680)();
                return;
                L_8006ECA0: ;
                r31 = MENU_MIDDLE_U32_0064(r31)->unk_0064;
                r3 = (u32)sp + 0x188;
                r4 = (u32)&lbl_8047C068;
                r5 = 0x32;
                /* crclr cr1eq */;
                ((void(*)(void))sprintf)();
                r3 = (u32)sp + 0x608;
                r4 = (u32)sp + 0x188;
                ((void(*)(void))GScharMakeFromSJIS)();
                r4 = (u32)sp + 0x608;
                r3 = 0x37;
                msgctrlSetValue();
                r5 = r31;
                r3 = 0xa;
                r4 = 0x0;
                r6 = 0xcf;
                ((void(*)(void))fn_800FB680)();
                return;
                            }
            r31 = MENU_MIDDLE_U32_0064(r31)->unk_0064;
            r0 = fn_8006B3C8(3) & 0xFF;
            if (r0 != (u32)0x0) {
                r5 = 0x32;
            } else {

                r5 = -0x1;
            }
            if ((s32)r5 >= (s32)0x0) {
                r3 = (u32)sp + 0x108;
                r4 = (u32)&lbl_8047C068;
                /* crclr cr1eq */;
                ((void(*)(void))sprintf)();
            } else {

                r3 = (u32)sp + 0x108;
                r4 = (u32)&lbl_8047C070;
                /* crclr cr1eq */;
                ((void(*)(void))sprintf)();
            }
            r3 = (u32)sp + 0x508;
            r4 = (u32)sp + 0x108;
            ((void(*)(void))GScharMakeFromSJIS)();
            r4 = (u32)sp + 0x508;
            r3 = 0x37;
            msgctrlSetValue();
            r5 = r31;
            r3 = 0xa;
            r4 = 0x0;
            r6 = 0xcf;
            ((void(*)(void))fn_800FB680)();
            return;
                                }
        r31 = MENU_MIDDLE_U32_0064(r31)->unk_0064;
        r3 = (u32)sp + 0x88;
        r4 = (u32)&lbl_8047C068;
        r5 = 0x64;
        /* crclr cr1eq */;
        ((void(*)(void))sprintf)();
        r3 = (u32)sp + 0x408;
        r4 = (u32)sp + 0x88;
        ((void(*)(void))GScharMakeFromSJIS)();
        r4 = (u32)sp + 0x408;
        r3 = 0x37;
        msgctrlSetValue();
        r5 = r31;
        r3 = 0xa;
        r4 = 0x0;
        r6 = 0xcf;
        ((void(*)(void))fn_800FB680)();
        return;
                            }
    r31 = MENU_MIDDLE_U32_0064(r31)->unk_0064;
    r0 = fn_8006B3C8(5) & 0xFF;
    if (r0 != (u32)0x0) {
        r5 = 0x64;
    } else {

        r5 = -0x1;
    }
    if ((s32)r5 >= (s32)0x0) {
        r3 = (u32)sp + 0x8;
        r4 = (u32)&lbl_8047C068;
        /* crclr cr1eq */;
        ((void(*)(void))sprintf)();
    } else {

        r3 = (u32)sp + 0x8;
        r4 = (u32)&lbl_8047C070;
        /* crclr cr1eq */;
        ((void(*)(void))sprintf)();
    }
    r3 = (u32)sp + 0x308;
    r4 = (u32)sp + 0x8;
    ((void(*)(void))GScharMakeFromSJIS)();
    r4 = (u32)sp + 0x308;
    r3 = 0x37;
    msgctrlSetValue();
    r5 = r31;
    r3 = 0xa;
    r4 = 0x0;
    r6 = 0xcf;
    ((void(*)(void))fn_800FB680)();
    return;
    L_8006EE30: ;
    r4 = (0xcccd << 16);
    r3 = 0x34;
    /* subi r0, r4, 0x3333 */;
    r0 = (u32)((u64)r0 * (u64)r5 >> 32);
    r0 = (u32)r0 >> 3;
    r0 = r0 * 0xa;
    r4 = r5 - r0;
    msgctrlSetValue();
    r5 = MENU_MIDDLE_U32_0064(r31)->unk_0064;
    r3 = 0x0;
    r4 = 0x0;
    r6 = 0xc9;
    ((void(*)(void))fn_800FB680)();

    return;
}


/* 0x8006EE7C | size: 0xA8 */
#pragma peephole off
void fn_8006EE7C(void* menu) {
    typedef struct MenuState_8006EE7C {
        u8 pad0;
        u8 state;
        u8 pad2[0x92];
        s8 cursor;
        s8 offset;
        u8 pad96[2];
        u8 dirty;
    } MenuState_8006EE7C;

    MenuState_8006EE7C* state;
    KeyInfo_8006BB34* keyInfo;
    u8* params;
    u8 toggled;
    s32 index;
    u32 value;

    state = menu;
    value = state->state;
    value = (s8)value;
    if ((s32)value != 2) {
        if (((!menu) && (!menu)) && (!menu)) {
            /* Preserve MWCC register allocation. */
        }
    } else {
        keyInfo = windowGetKeyInfo();
        value = keyInfo->flags4;
        value = value & 0x10;
        toggled = value == 0;
        if ((s32)value != 0) {
            index = state->offset + state->cursor;
            if (index < 60) {
                params = windowGetParam(menu, 0);
                value = params[index];
                toggled = value == 0;
                params[index] = toggled;
                state->dirty = 0;
                return;
            }
        }
    }

    menuButtonNormal(menu);
}
#pragma peephole reset


/* 0x8006EF24 | size: 0xD4 */
typedef struct MenuState_8006EF24 {
    u8 pad0;
    s8 state;
    u8 pad2[0x92];
    s8 cursor;
    s8 mode;
} MenuState_8006EF24;

#pragma peephole off
void fn_8006EF24(void* menu) {
    MenuState_8006EF24* state = (MenuState_8006EF24*)menu;
    KeyInfo_8006BB34* keyInfo;
    s32 currentState;

    currentState = state->state;
    if (currentState != 2) {
        if ((state && state) && state) {
            /* Preserve MWCC register allocation. */
        }
        goto normal;
    }

    if (state->mode == 0) {
        keyInfo = (KeyInfo_8006BB34*)windowGetKeyInfo();
        if (keyInfo->flags6 & 1) {
            state->cursor--;
            if (state->cursor < 0) {
                state->cursor = 0;
            }
            return;
        }
    } else if (state->mode == 10) {
        keyInfo = (KeyInfo_8006BB34*)windowGetKeyInfo();
        if (keyInfo->flags6 & 2) {
            state->cursor++;
            if (state->cursor > 50) {
                state->cursor = 50;
            }
            return;
        }
    }

normal:
    menuCursorNormal(menu);
}
#pragma peephole reset


/* 0x8006EFF8 | size: 0x28C */
void fn_8006EFF8(void* menu) {
    extern void winSpriteSetDisp();
    extern void fn_80142984();
    extern void itemDataBiosGetName();
    extern void itemDataBiosGetPtr();
    u8 sp[0x30];
    u32 r0 = 0;
    u32 r1 = (u32)sp;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r22 = 0;
    u32 r23 = 0;
    u32 r24 = 0;
    u32 r25 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    
    r31 = (u32)menu;
    r4 = 0x0;
    ((void(*)(void))windowGetParam)();
    r28 = r3;
    r3 = r31;
    r4 = 0xa40;
    ((void(*)(void))windowSearchItemID)();
    r0 = MENU_MIDDLE_U8_0094(r31)->unk_0094;
    r4 = (s8)r0;
    r0 = -r4;
    r0 = r0 & ~r4;
    r4 = (u32)r0 >> 31;
    winSpriteSetDisp();
    r3 = r31;
    r4 = 0xa3f;
    ((void(*)(void))windowSearchItemID)();
    r0 = MENU_MIDDLE_U8_0094(r31)->unk_0094;
    r5 = 0x32;
    r0 = (s8)r0;
    r0 = r5 ^ r0;
    r4 = (s32)r0 >> 1;
    r0 = r0 & r5;
    r0 = r4 - r0;
    r4 = (u32)r0 >> 31;
    winSpriteSetDisp();
    r3 = (u32)&lbl_80268424;
    r25 = 0x0;
    r27 = (u32)&lbl_80268424;
    do {
        r4 = MENU_MIDDLE_U16_0000(r27)->unk_0000;
        r3 = r31;
        ((void(*)(void))windowSearchItemID)();
        r4 = MENU_MIDDLE_U16_0002(r27)->unk_0002;
        r29 = r3;
        r3 = r31;
        ((void(*)(void))windowSearchItemID)();
        r4 = MENU_MIDDLE_U16_0004(r27)->unk_0004;
        r30 = r3;
        r3 = r31;
        ((void(*)(void))windowSearchItemID)();
        r4 = MENU_MIDDLE_U16_0006(r27)->unk_0006;
        r24 = r3;
        r3 = r31;
        ((void(*)(void))windowSearchItemID)();
        r0 = MENU_MIDDLE_U8_0094(r31)->unk_0094;
        r23 = r3;
        r0 = (s8)r0;
        r26 = r25 + r0;
        if ((s32)r26 == (s32)0x3c) {
            r3 = 0x43e6;
            r0 = 0x0;
            MENU_MIDDLE_U32_004C(r29)->unk_004C = r3;
            MENU_MIDDLE_U32_004C(r30)->unk_004C = r0;

        } else {
        r3 = r26;
        ((void(*)(void))fn_80077D88)();
        r22 = r3;
        fn_80142984();
        r0 = r3 & 0xFF;
        if (r0 != (u32)0x0) {
            r3 = r22;
            itemDataBiosGetPtr();
            itemDataBiosGetName();
            MENU_MIDDLE_U32_004C(r29)->unk_004C = r3;
        } else {

            r0 = 0x12e;
            MENU_MIDDLE_U32_004C(r29)->unk_004C = r0;
        }
        r0 = *(u8*)(r28 + r26);
        if (r0 != (u32)0x0) {
            if (r0 != (u32)0x0) {
                r0 = 0x3d6f;
            } else {

                r0 = 0x3d68;
            }
            r3 = (0xff00 << 16);
            MENU_MIDDLE_U32_004C(r30)->unk_004C = r0;
            r0 = r3 + 0xff;
            MENU_MIDDLE_U32_0064(r30)->unk_0064 = r0;

        } else {
        if (r0 != (u32)0x0) {
            r0 = 0x3d6f;
        } else {

            r0 = 0x3d68;
        }
        MENU_MIDDLE_U32_004C(r30)->unk_004C = r0;
        r0 = -0x1;
        MENU_MIDDLE_U32_0064(r30)->unk_0064 = r0;
        }
        }
        r4 = *(u8*)(r28 + r26);
        r3 = r24;
        winSpriteSetDisp();
        r0 = MENU_MIDDLE_U8_0095(r31)->unk_0095;
        r3 = r23;
        r0 = (s8)r0;
        r0 = r0 - r25;
        r0 = __cntlzw(r0);
        r0 = (u32)r0 >> 5;
        r4 = r0 & 0xFF;
        winSpriteSetDisp();
        r27 = r27 + 0x8;
        r25 = r25 + 0x1;
    } while (r25 < (u32)0xb);
    r0 = MENU_MIDDLE_U8_0001(r31)->unk_0001;
    r0 = (s8)r0;
    if ((s32)r0 != (s32)0x3) {
        if ((s32)r0 >= (s32)0x3) return;
        if ((s32)r0 != (s32)0x0) {
            return;


        }
        r0 = MENU_MIDDLE_U8_0002(r31)->unk_0002;
        r0 = (s8)r0;
        if ((s32)r0 != (s32)0x0) return;
        r3 = (u32)&lbl_80267EA8;
        r24 = MENU_MIDDLE_U32_001C(r31)->unk_001C;
        r3 = (u32)&lbl_80267EA8;
        r23 = MENU_MIDDLE_U16_0018(r3)->unk_0018;
        while (r24 != (u32)0x0) {

            r0 = MENU_MIDDLE_S16_0006(r24)->unk_0006;
            r3 = r31;
            r5 = r23;
            r4 = r0 & 0xFFFF;
            ((void(*)(void))fn_801081F8)();
            r24 = MENU_MIDDLE_U32_0000(r24)->unk_0000;

        }
        r3 = r31;
        r23 = 0x424a;
        r4 = 0xe4c;
        ((void(*)(void))windowSearchItemID)();
        MENU_MIDDLE_U32_004C(r3)->unk_004C = r23;
        return;
    }
    r0 = MENU_MIDDLE_U8_0002(r31)->unk_0002;
    r0 = (s8)r0;
    if ((s32)r0 != (s32)0x0) return;
    r3 = (u32)&lbl_80267EA8;
    r24 = MENU_MIDDLE_U32_001C(r31)->unk_001C;
    r3 = (u32)&lbl_80267EA8;
    r23 = MENU_MIDDLE_U16_001A(r3)->unk_001A;
    while (r24 != (u32)0x0) {

        r0 = MENU_MIDDLE_S16_0006(r24)->unk_0006;
        r3 = r31;
        r5 = r23;
        r4 = r0 & 0xFFFF;
        ((void(*)(void))fn_801081F8)();
        r24 = MENU_MIDDLE_U32_0000(r24)->unk_0000;

    }
    r0 = 0x1;
    MENU_MIDDLE_U8_0002(r31)->unk_0002 = r0;

    return;
}


/* 0x8006F284 | size: 0x49C */
/* RULE-EXCEPTION(user-approved): local peephole-control pragma — see docs/RULE_EXCEPTIONS.md */
#pragma peephole off
void fn_8006F284(void* menu) {
    extern void* windowSearchItemID(void* menu, s32 itemId);
    extern void winSpriteSetDisp(void* widget, u8 flag);
    extern void menuItemBiosSetSelectFlag(s32 itemId, u8 flag);
    extern void windowCreateCursorSprite(void* menu);
    extern void fn_801081F8(void* menu, u16 itemId, u16 sequence);
    typedef struct Entry_8006F284 {
        u16 itemId;
        u8 pad2[2];
        u32 value;
    } Entry_8006F284;
    typedef struct CursorArg_8006F284 {
        u8 pad;
        s8 cursor;
    } CursorArg_8006F284;
    u8 val16Valid;
    u8 sel11;
    u8 locked;
    s32 selected;
    Param_8006BB34* param;
    u8 val14Valid;
    u8 sel12;
    u8* data;
    Entry_8006F284* entries;
    u16* itemIds;
    void* widget;
    u32 i;
    u8 sel13;

    data = lbl_80267EA8;
    switch ((s8)MENU_MIDDLE_U8_0001(menu)->unk_0001) {
    case 0: {
        CursorArg_8006F284 src;
        CursorArg_8006F284 arg;
        u8 unlocked;

        entries = (Entry_8006F284*)(data + 0x4EC);
        for (i = 0; i < 16; i++) {
            widget = windowSearchItemID(menu, entries[i].itemId);
            MENU_MIDDLE_U32_004C(widget)->unk_004C = entries[i].value;
        }
        unlocked = (fn_80077BD0() == 0);
        menuItemBiosSetSelectFlag(0x9FC, unlocked);
        if (unlocked == 0) {
            src.cursor = (s8)menuGetCursorFromItemID(MENU_MIDDLE_U32_0004(menu)->unk_0004, 0x9FD);
            src.pad = 0;
            arg = src;
            fn_801044D0(MENU_MIDDLE_U32_0004(menu)->unk_0004, &arg);
            windowCreateCursorSprite(menu);
        }
        break;
    }
    case 2:
        if (menuGetCursorItemID(MENU_MIDDLE_U32_0004(menu)->unk_0004) == 0xE35) {
            MENU_MIDDLE_U8_0098(menu)->unk_0098 = 1;
        }
        break;
    case 3:
    case 4:
    case 5:
        break;
    }

    switch (menuGetCursorItemID(MENU_MIDDLE_U32_0004(menu)->unk_0004)) {
    case 0x9F7:
        selected = 0;
        break;
    case 0x9F8:
        selected = 1;
        break;
    case 0x9F9:
        selected = 2;
        break;
    case 0x9FA:
    case 0xA0C:
    case 0xA0D:
        selected = 3;
        break;
    case 0x9FB:
    case 0xE33:
    case 0xE34:
        selected = 4;
        break;
    case 0x9FC:
    case 0x9FD:
    case 0xE35:
    default:
        selected = 5;
        break;
    }

    itemIds = (u16*)(data + 0x4E0);
    for (i = 0; i < 5; i++) {
        widget = windowSearchItemID(menu, itemIds[i]);
        winSpriteSetDisp(widget, selected == (s32)i && fn_80077BD0() == 0);
        if (*(void**)((u8*)widget + 0xC) != menuSeqBiosGetPtr(0x191)) {
            fn_801081F8(menu, itemIds[i], 0x191);
        }
    }

    locked = (fn_80077BD0() == 0);
    param = (Param_8006BB34*)windowGetParam(menu, 0);
    val14Valid = !(param->val14 < 0);
    val16Valid = !(param->val16 < 0);
    sel11 = param->sel11;
    sel12 = param->sel12;
    sel13 = param->sel13;

    winSpriteSetDisp(windowSearchItemID(menu, 0xA06), sel11);
    winSpriteSetDisp(windowSearchItemID(menu, 0xA07), sel11 == 0);
    winSpriteSetDisp(windowSearchItemID(menu, 0xA08), sel12);
    winSpriteSetDisp(windowSearchItemID(menu, 0xA09), sel12 == 0);
    winSpriteSetDisp(windowSearchItemID(menu, 0xA0A), sel13);
    winSpriteSetDisp(windowSearchItemID(menu, 0xA0B), sel13 == 0);

    widget = windowSearchItemID(menu, 0x11A2);
    winSpriteSetDisp(widget, val14Valid != 0 &&
                                 menuGetCursorItemID(MENU_MIDDLE_U32_0004(menu)->unk_0004) != 0xA0C &&
                                 menuGetCursorItemID(MENU_MIDDLE_U32_0004(menu)->unk_0004) != 0xA0D);
    winSpriteSetDisp(windowSearchItemID(menu, 0x11A3), val14Valid == 0);
    widget = windowSearchItemID(menu, 0x11A4);
    winSpriteSetDisp(widget, val16Valid != 0 &&
                                 menuGetCursorItemID(MENU_MIDDLE_U32_0004(menu)->unk_0004) != 0xE34 &&
                                 menuGetCursorItemID(MENU_MIDDLE_U32_0004(menu)->unk_0004) != 0xE33);
    winSpriteSetDisp(windowSearchItemID(menu, 0x11A5), val16Valid == 0);

    winSpriteSetDisp(windowSearchItemID(menu, 0xA16), locked);
    winSpriteSetDisp(windowSearchItemID(menu, 0xD9F), locked);
    widget = windowSearchItemID(menu, 0xA15);
    MENU_MIDDLE_U32_004C(widget)->unk_004C = locked ? 0x3D7A : 0x3D79;

    widget = windowSearchItemID(menu, 0x11A6);
    winSpriteSetDisp(widget, locked != 0 && val14Valid != 0 &&
                                 menuGetCursorItemID(MENU_MIDDLE_U32_0004(menu)->unk_0004) == 0x9FA);
    widget = windowSearchItemID(menu, 0x11A7);
    winSpriteSetDisp(widget, locked != 0 && val16Valid != 0 &&
                                 menuGetCursorItemID(MENU_MIDDLE_U32_0004(menu)->unk_0004) == 0x9FB);

    fn_80070D84(menu, (MenuMiddleEntry*)(data + 0x56C), 2);
}
#pragma peephole reset


/* 0x8006F720 | size: 0x4DC */
void fn_8006F720(void* menu) {
    extern void fn_80070D84();
    extern void winSpriteSetDisp();
    extern void savedataGetStatus();
    extern u8 jumptable_802EE06C[];
    u8 sp[0x30];
    u32 r0 = 0;
    u32 r1 = (u32)sp;
    u32 r3 = (u32)menu;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r25 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    void (*ctr_fn)(void) = 0;
    u32 ctr = 0;

    
    r29 = r3;
    r5 = (u32)&lbl_80267EA8;
    r4 = 0x0;
    r30 = (u32)&lbl_80267EA8;
    ((void(*)(void))windowGetParam)();
    r31 = r3;
    r28 = r30 + 0x3dc;
    r26 = 0x0;
    do {
        r4 = MENU_MIDDLE_U16_0000(r28)->unk_0000;
        r3 = r29;
        ((void(*)(void))windowSearchItemID)();
        r0 = MENU_MIDDLE_U32_0004(r28)->unk_0004;
        r28 = r28 + 0x8;
        r26 = r26 + 0x1;
        MENU_MIDDLE_U32_004C(r3)->unk_004C = r0;
    } while (r26 < (u32)0x1a);
    r3 = MENU_MIDDLE_U32_0004(r29)->unk_0004;
    ((void(*)(void))menuGetCursorItemID)();
    r0 = r3 - 0x9CA;
    switch (r0) {
    case 0:
        r25 = 0;
        break;
    case 1:
        r25 = 1;
        break;
    case 2:
        r25 = 2;
        break;
    case 3:
        r25 = 3;
        break;
    case 4:
        r25 = 4;
        break;
    case 5:
        r25 = 5;
        break;
    case 6:
        r25 = 6;
        break;
    case 7:
        r25 = 7;
        break;
    default:
        r25 = 8;
        break;
    }
    ((void(*)(void))fn_80077BD0)();
    r0 = r3 & 0xFF;
    if (r0 != (u32)0x0) {
        r28 = r30 + 0x3cc;
        r26 = 0x0;
        do {
            r4 = MENU_MIDDLE_U16_0000(r28)->unk_0000;
            r3 = r29;
            ((void(*)(void))windowSearchItemID)();
            r4 = 0x0;
            winSpriteSetDisp();
            r28 = r28 + 0x2;
            r26 = r26 + 0x1;
        } while (r26 < (u32)0x8);
    } else {

        r27 = r30 + 0x3cc;
        r26 = 0x0;
        do {
            r4 = MENU_MIDDLE_U16_0000(r27)->unk_0000;
            r3 = r29;
            ((void(*)(void))windowSearchItemID)();
            r0 = r26 - r25;
            r28 = r3;
            r0 = __cntlzw(r0);
            r0 = (u32)r0 >> 5;
            r4 = r0 & 0xFF;
            winSpriteSetDisp();
            r3 = 0x191;
            ((void(*)(void))menuSeqBiosGetPtr)();
            r0 = MENU_MIDDLE_U32_000C(r28)->unk_000C;
            if (r0 != (u32)r3) {
                r4 = MENU_MIDDLE_U16_0000(r27)->unk_0000;
                r3 = r29;
                r5 = 0x191;
                ((void(*)(void))fn_801081F8)();
            }
            r27 = r27 + 0x2;
            r26 = r26 + 0x1;
        } while (r26 < (u32)0x8);
    }
    r3 = r29;
    r4 = 0xd80;
    ((void(*)(void))windowSearchItemID)();
    r4 = MENU_MIDDLE_U8_000C(r31)->unk_000C;
    winSpriteSetDisp();
    r3 = r29;
    r4 = 0x9eb;
    ((void(*)(void))windowSearchItemID)();
    r0 = MENU_MIDDLE_U8_000C(r31)->unk_000C;
    r0 = __cntlzw(r0);
    r0 = (u32)r0 >> 5;
    r4 = r0 & 0xFF;
    winSpriteSetDisp();
    r3 = r29;
    r4 = 0x9ec;
    ((void(*)(void))windowSearchItemID)();
    r0 = MENU_MIDDLE_U32_0008(r31)->unk_0008;
    r0 = __cntlzw(r0);
    r0 = (u32)r0 >> 5;
    r4 = r0 & 0xFF;
    winSpriteSetDisp();
    r3 = r29;
    r4 = 0x9ed;
    ((void(*)(void))windowSearchItemID)();
    r0 = MENU_MIDDLE_U32_0008(r31)->unk_0008;
    r0 = 0x1 - r0;
    r0 = __cntlzw(r0);
    r0 = (u32)r0 >> 5;
    r4 = r0 & 0xFF;
    winSpriteSetDisp();
    r3 = r29;
    r4 = 0x9ee;
    ((void(*)(void))windowSearchItemID)();
    r0 = MENU_MIDDLE_U32_0008(r31)->unk_0008;
    r0 = 0x2 - r0;
    r0 = __cntlzw(r0);
    r0 = (u32)r0 >> 5;
    r4 = r0 & 0xFF;
    winSpriteSetDisp();
    r3 = r29;
    r4 = 0x9ef;
    ((void(*)(void))windowSearchItemID)();
    r4 = MENU_MIDDLE_U8_000D(r31)->unk_000D;
    winSpriteSetDisp();
    r3 = r29;
    r4 = 0x9f0;
    ((void(*)(void))windowSearchItemID)();
    r0 = MENU_MIDDLE_U8_000D(r31)->unk_000D;
    r0 = __cntlzw(r0);
    r0 = (u32)r0 >> 5;
    r4 = r0 & 0xFF;
    winSpriteSetDisp();
    r3 = r29;
    r4 = 0x9f1;
    ((void(*)(void))windowSearchItemID)();
    r4 = MENU_MIDDLE_U8_000E(r31)->unk_000E;
    winSpriteSetDisp();
    r3 = r29;
    r4 = 0x9f2;
    ((void(*)(void))windowSearchItemID)();
    r0 = MENU_MIDDLE_U8_000E(r31)->unk_000E;
    r0 = __cntlzw(r0);
    r0 = (u32)r0 >> 5;
    r4 = r0 & 0xFF;
    winSpriteSetDisp();
    r3 = r29;
    r4 = 0x9f3;
    ((void(*)(void))windowSearchItemID)();
    r4 = MENU_MIDDLE_U8_000F(r31)->unk_000F;
    winSpriteSetDisp();
    r3 = r29;
    r4 = 0x9f4;
    ((void(*)(void))windowSearchItemID)();
    r0 = MENU_MIDDLE_U8_000F(r31)->unk_000F;
    r0 = __cntlzw(r0);
    r0 = (u32)r0 >> 5;
    r4 = r0 & 0xFF;
    winSpriteSetDisp();
    r3 = r29;
    r4 = 0x9f5;
    ((void(*)(void))windowSearchItemID)();
    r4 = MENU_MIDDLE_U8_0010(r31)->unk_0010;
    winSpriteSetDisp();
    r3 = r29;
    r4 = 0x8a2;
    ((void(*)(void))windowSearchItemID)();
    r0 = MENU_MIDDLE_U8_0010(r31)->unk_0010;
    r0 = __cntlzw(r0);
    r0 = (u32)r0 >> 5;
    r4 = r0 & 0xFF;
    winSpriteSetDisp();
    r3 = r29;
    r4 = 0x9d4;
    ((void(*)(void))windowSearchItemID)();
    r0 = MENU_MIDDLE_S16_0002(r31)->unk_0002;
    r6 = MENU_MIDDLE_S16_0004(r31)->unk_0004;
    r0 = r0 * 0x6;
    r5 = (s32)r6 >> 31;
    r4 = (u32)r0 >> 31;
    r0 = r6 - r0;
    r0 = r5 + r4; /* +carry */;
    r4 = r0 & 0xFF;
    winSpriteSetDisp();
    r3 = r29;
    r4 = 0xfb0;
    ((void(*)(void))windowSearchItemID)();
    r28 = 0x0;
    r26 = r3;
    ((void(*)(void))fn_80077BD0)();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x0) {
        r3 = MENU_MIDDLE_U32_0004(r29)->unk_0004;
        ((void(*)(void))menuGetCursorItemID)();
        if ((s32)r3 == (s32)0x9cd) {
            r0 = MENU_MIDDLE_U32_0008(r31)->unk_0008;
            if ((s32)r0 == (s32)0x2) {
                r28 = 0x1;
    }
    }
    }
    r4 = r28 & 0xFF;
    r3 = r26;
    winSpriteSetDisp();
    r3 = 0x0;
    r4 = 0xe;
    savedataGetStatus();
    r0 = MENU_MIDDLE_U32_0000(r3)->unk_0000;
    if ((s32)r0 == (s32)0x0) {
        r27 = r30 + 0x4ac;
        r28 = 0x0;
        do {
            r4 = MENU_MIDDLE_U16_0000(r27)->unk_0000;
            r3 = r29;
            ((void(*)(void))windowSearchItemID)();
            r4 = 0x0;
            winSpriteSetDisp();
            r27 = r27 + 0x2;
            r28 = r28 + 0x1;
        } while (r28 < (u32)0x12);
        r3 = r29;
        r4 = 0xd9b;
        ((void(*)(void))windowSearchItemID)();
        r0 = 0x4238;
        r4 = 0xd9c;
        MENU_MIDDLE_U32_004C(r3)->unk_004C = r0;
        r3 = r29;
        ((void(*)(void))windowSearchItemID)();
        r0 = 0x0;
        r4 = 0x9d3;
        MENU_MIDDLE_U32_004C(r3)->unk_004C = r0;
        r3 = r29;
        ((void(*)(void))windowSearchItemID)();
        r0 = 0x4238;
        r4 = 0xd9a;
        MENU_MIDDLE_U32_004C(r3)->unk_004C = r0;
        r3 = r29;
        ((void(*)(void))windowSearchItemID)();
        r0 = 0x0;
        r4 = 0x9d4;
        MENU_MIDDLE_U32_004C(r3)->unk_004C = r0;
        r3 = r29;
        ((void(*)(void))windowSearchItemID)();
        r0 = 0x0;
        MENU_MIDDLE_U32_004C(r3)->unk_004C = r0;
    }
    r0 = MENU_MIDDLE_U8_0001(r29)->unk_0001;
    r0 = (s8)r0;
    if ((s32)r0 != (s32)0x2) {
        if ((s32)r0 < (s32)0x2) {
            goto L_8006FBD8;
        }
        goto L_8006FBD8;
    }
    r3 = MENU_MIDDLE_U32_0004(r29)->unk_0004;
    ((void(*)(void))menuGetCursorItemID)();
    if ((s32)r3 == (s32)0x9d2) {
        r0 = 0x1;
        MENU_MIDDLE_U8_0098(r29)->unk_0098 = r0;
    }
    ((void(*)(void))windowGetKeyInfo)();
    r0 = MENU_MIDDLE_U16_0004(r3)->unk_0004;
    r0 = r0 & 0x00000010;
    if ((s32)r0 == (s32)0x0) goto L_8006FBD8;
    ((void(*)(void))fn_80077BD0)();
    r0 = r3 & 0xFF;
    if (r0 != (u32)0x0) {
        r3 = MENU_MIDDLE_U32_0004(r29)->unk_0004;
        r4 = 0x9d2;
        ((void(*)(void))menuGetCursorFromItemID)();
        r3 = (s8)r3;
        r0 = 0x0;
        *(u8*)(sp + 0xD) = r3;
        r4 = (u32)sp + 0x8;
        *(u8*)(sp + 0xC) = r0;
        r0 = *(u16*)(sp + 0xC);
        *(u16*)(sp + 0x8) = r0;
        r3 = MENU_MIDDLE_U32_0004(r29)->unk_0004;
        ((void(*)(void))fn_801044D0)();

    } else {
    r0 = MENU_MIDDLE_U8_0095(r29)->unk_0095;
    r3 = 0x0;
    r0 = (s8)r0;
    if ((s32)r0 == (s32)0x3) {
        r0 = MENU_MIDDLE_U32_0008(r31)->unk_0008;
        if ((s32)r0 == (s32)0x2) {
            r3 = 0x1;
    }
    }
    r0 = r3 & 0xFF;
    MENU_MIDDLE_U8_0098(r29)->unk_0098 = r0;
    }
    L_8006FBD8: ;
    r3 = r29;
    r4 = r30 + 0x4d0;
    r5 = 0x2;
    fn_80070D84();
    return;
}

#endif

#if defined(MENU_MIDDLE_ALL) || defined(MENU_MIDDLE_RESIDUAL_8006B9B8_ONLY) || \
    defined(MENU_MIDDLE_EXACT_8006FBFC_ONLY)
/* 0x8006FBFC | size: 0xFC */
void fn_8006FBFC(void* menu) {
    typedef struct MenuIconEntry {
        u16 itemId;
        u16 padding;
        u32 spriteId;
    } MenuIconEntry;
    extern void* windowSearchItemID(void* menu, s32 itemId);
    extern u8* windowGetKeyInfo(void);
    extern void fn_80070D84(void* menu, void* table, s32 count);
    MenuIconEntry* entry;
    void* widget;
    u32 i;

    widget = windowSearchItemID(menu, 0x9BB);
    MENU_MIDDLE_U32_004C(widget)->unk_004C =
        *(s8*)((u8*)menu + 0x95) < 3 ? 0x3DC0 : 0x3DC1;

    switch (*(s8*)((u8*)menu + 1)) {
    case 0:
        entry = (MenuIconEntry*)lbl_80268234;
        for (i = 0; i < 8; i++) {
            widget = windowSearchItemID(menu, entry[i].itemId);
            MENU_MIDDLE_U32_004C(widget)->unk_004C = entry[i].spriteId;
        }
        break;
    case 2:
        if ((*(u16*)(windowGetKeyInfo() + 4) & 0x400) != 0 &&
            *(s8*)((u8*)menu + 0x95) < 6) {
            MENU_MIDDLE_U8_0098(menu)->unk_0098 = 1;
        }
        break;
    case 3:
        break;
    }

    fn_80070D84(menu, lbl_802681B4, 0x10);
}
#endif

#if defined(MENU_MIDDLE_ALL) || defined(MENU_MIDDLE_EXACT_8006FCF8_ONLY)
/* 0x8006FCF8 | size: 0x2C */
#pragma push
#pragma peephole off
void fn_8006FCF8(u32 r3) {
    extern void fn_80070D84(u32 r3, u32 r4, u32 r5);
    fn_80070D84(r3, (u32)lbl_80268184, 0x6);
}
#pragma pop

/* 0x8006FD24 | size: 0x28 */
#pragma push
#pragma scheduling off
void fn_8006FD24(u32 r3) {
    extern void fn_80070D84(u32 r3, u32 r4, u32 r5);
    fn_80070D84(r3, 0x0, 0x0);
}
#pragma pop

/* 0x8006FD4C | size: 0x28 */
#pragma push
#pragma scheduling off
void fn_8006FD4C(u32 r3) {
    extern void fn_80070D84(u32 r3, u32 r4, u32 r5);
    fn_80070D84(r3, 0x0, 0x0);
}
#pragma pop

/* 0x8006FD74 | size: 0x28 */
#pragma push
#pragma scheduling off
void fn_8006FD74(u32 r3) {
    extern void fn_80070D84(u32 r3, u32 r4, u32 r5);
    fn_80070D84(r3, 0x0, 0x0);
}
#pragma pop

/* 0x8006FD9C | size: 0x28 */
#pragma push
#pragma scheduling off
void fn_8006FD9C(u32 r3) {
    extern void fn_80070D84(u32 r3, u32 r4, u32 r5);
    fn_80070D84(r3, 0x0, 0x0);
}
#pragma pop

/* 0x8006FDC4 | size: 0x28 */
#pragma push
#pragma scheduling off
void fn_8006FDC4(u32 r3) {
    extern void fn_80070D84(u32 r3, u32 r4, u32 r5);
    fn_80070D84(r3, 0x0, 0x0);
}
#pragma pop

/* 0x8006FDEC | size: 0x28 */
#pragma push
#pragma scheduling off
void fn_8006FDEC(u32 r3) {
    extern void fn_80070D84(u32 r3, u32 r4, u32 r5);
    fn_80070D84(r3, 0x0, 0x0);
}
#pragma pop

/* 0x8006FE14 | size: 0x28 */
#pragma push
#pragma scheduling off
void fn_8006FE14(u32 r3) {
    extern void fn_80070D84(u32 r3, u32 r4, u32 r5);
    fn_80070D84(r3, 0x0, 0x0);
}
#pragma pop

/* 0x8006FE3C | size: 0x28 */
#pragma push
#pragma scheduling off
void fn_8006FE3C(u32 r3) {
    extern void fn_80070D84(u32 r3, u32 r4, u32 r5);
    fn_80070D84(r3, 0x0, 0x0);
}
#pragma pop

/* 0x8006FE64 | size: 0x80 */
#pragma peephole off
void fn_8006FE64(void* menu) {
    extern u32 fn_8006B3C8(s32 index);
    extern void fn_80166A28(s32 sndId);
    KeyInfo_8006BB34* keyInfo;
    s32 state;
    s32 flag;
    int stateIndex;
    u32 value;

    state = MENU_MIDDLE_U8_0095(menu)->unk_0095;
    state = (s8)state;
    if (state < 6) {
        stateIndex = state;
        keyInfo = windowGetKeyInfo();
        flag = keyInfo->flags4 & 0x10;
        if (flag != 0) {
            value = fn_8006B3C8(stateIndex);
            if ((u8)value == 0) {
                fn_80166A28(0x26);
                return;
            }
        }
    }
    menuButtonNormal(menu);
}
#pragma peephole reset
#endif

#if defined(MENU_MIDDLE_ALL) || defined(MENU_MIDDLE_RESIDUAL_8006FEE4_ONLY)
/* 0x8006FEE4 | size: 0x390 */
void fn_8006FEE4(void* menu) {
    extern void fn_8006B1F4();
    extern void fn_8006B3C8();
    extern void fn_80070D84();
    extern void winSpriteSetDisp();
    u8 sp[0x30];
    u32 r0 = 0;
    u32 r1 = (u32)sp;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r24 = 0;
    u32 r25 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f0 = 0.0f;
    f32 f1 = 0.0f;
    f32 f2 = 0.0f;

    
    r30 = (u32)menu;
    r0 = MENU_MIDDLE_U8_0095(r30)->unk_0095;
    r3 = (u32)&lbl_80267EA8;
    r31 = (u32)&lbl_80267EA8;
    r0 = (s8)r0;
    if ((s32)r0 < (s32)0x6) {
        *(u32*)&lbl_8047A5FC = r0;
    }
    r3 = *(u32*)&lbl_8047A5FC;
    fn_8006B3C8();
    r28 = r3;
    r3 = r30;
    r4 = 0x957;
    ((void(*)(void))windowSearchItemID)();
    r25 = r3;
    if (r25 != (u32)0x0) {
        r0 = r28 & 0xFF;
        if (r0 != (u32)0x0) {
            r0 = *(u32*)&lbl_8047A5FC;
            r3 = r31 + 0x218;
            r0 = r0 << 2;
            r3 = *(u32*)(r3 + r0);
        } else {

            r3 = 0x26c;
        }
        ((void(*)(void))menuSpriteBiosGetPtr)();
        r0 = r3;
        r3 = r25;
        r4 = r0;
        ((void(*)(void))fn_80071318)();
    }
    r27 = r31 + 0x1c8;
    r25 = 0x0;
    do {
        r4 = MENU_MIDDLE_U16_0000(r27)->unk_0000;
        r3 = r30;
        ((void(*)(void))windowSearchItemID)();
        r29 = r3;
        r3 = r25;
        fn_8006B3C8();
        r0 = r3 & 0xFF;
        if (r0 != (u32)0x0) {
            r0 = MENU_MIDDLE_U32_0004(r27)->unk_0004;
        } else {

            r0 = 0x3daa;
        }
        MENU_MIDDLE_U32_004C(r29)->unk_004C = r0;
        r27 = r27 + 0x8;
        r25 = r25 + 0x1;
    } while ((s32)r25 < (s32)0x6);
    r0 = r25 << 3;
    r27 = r31 + 0x1c8;
    r27 = r27 + r0;
    while (r25 < (u32)0xa) {

        r4 = MENU_MIDDLE_U16_0000(r27)->unk_0000;
        r3 = r30;
        ((void(*)(void))windowSearchItemID)();
        r0 = MENU_MIDDLE_U32_0004(r27)->unk_0004;
        r27 = r27 + 0x8;
        r25 = r25 + 0x1;
        MENU_MIDDLE_U32_004C(r3)->unk_004C = r0;

    }
    r0 = r28 & 0xFF;
    r27 = r31 + 0x230;
    r0 = __cntlzw(r0);
    r25 = 0x0;
    r29 = (u32)r0 >> 5;
    do {
        r4 = MENU_MIDDLE_U16_0000(r27)->unk_0000;
        r3 = r30;
        ((void(*)(void))windowSearchItemID)();
        r4 = r29 & 0xFF;
        winSpriteSetDisp();
        r27 = r27 + 0x2;
        r25 = r25 + 0x1;
    } while (r25 < (u32)0x5);
    r0 = MENU_MIDDLE_U8_0001(r30)->unk_0001;
    r0 = (s8)r0;
    if ((s32)r0 != (s32)0x3) {
        if ((s32)r0 >= (s32)0x3) goto L_8007011C;
        if ((s32)r0 != (s32)0x0) {
            goto L_8007011C;
        }
        r26 = r31 + 0x23c;
        r25 = 0x0;
        r29 = r31 + 0x0;
        do {
            r0 = (u32)r25 >> 31;
            r27 = r26;
            r0 = r0 + r25;
            r24 = 0x0;
            r0 = (s32)r0 >> 1;
            r3 = r0 << 2;
            r28 = r3 + 0xc;
            do {
                r4 = MENU_MIDDLE_U16_0000(r27)->unk_0000;
                r3 = r30;
                ((void(*)(void))windowSearchItemID)();
                r4 = *(u16*)(r29 + r28);
                r3 = r3 + 0xc;
                ((void(*)(void))winSetSequence)();
                r27 = r27 + 0x2;
                r24 = r24 + 0x1;
            } while (r24 < (u32)0x5);
            r26 = r26 + 0xa;
            r25 = r25 + 0x1;
        } while (r25 < (u32)0x6);

    } else {
    r0 = MENU_MIDDLE_U8_0002(r30)->unk_0002;
    r0 = (s8)r0;
    if ((s32)r0 == (s32)0x0) {
        r28 = r31 + 0x23c;
        r25 = 0x0;
        r29 = r31 + 0x0;
        do {
            r0 = (u32)r25 >> 31;
            r26 = r28;
            r0 = r0 + r25;
            r24 = 0x0;
            r0 = (s32)r0 >> 1;
            r3 = r0 << 2;
            r27 = r3 + 0xe;
            do {
                r4 = MENU_MIDDLE_U16_0000(r26)->unk_0000;
                r3 = r30;
                ((void(*)(void))windowSearchItemID)();
                r4 = *(u16*)(r29 + r27);
                r3 = r3 + 0xc;
                ((void(*)(void))winSetSequence)();
                r26 = r26 + 0x2;
                r24 = r24 + 0x1;
            } while (r24 < (u32)0x5);
            r28 = r28 + 0xa;
            r25 = r25 + 0x1;
        } while (r25 < (u32)0x6);
    }
    }
    L_8007011C: ;
    r3 = r30;
    r4 = r31 + 0x278;
    r5 = 0xb;
    fn_80070D84();
    r26 = r31 + 0x2d0;
    r27 = 0x0;
    do {
        r3 = *(u32*)&lbl_8047A5FC;
        if ((s32)r27 == (s32)0x0) {
            r4 = 0x0;
        } else {

            r4 = 0x1;
        }
        fn_8006B1F4();
        r4 = MENU_MIDDLE_U16_0000(r26)->unk_0000;
        r24 = r3;
        r3 = r30;
        ((void(*)(void))windowSearchItemID)();
        r4 = r24;
        winSpriteSetDisp();
        r4 = MENU_MIDDLE_U16_0002(r26)->unk_0002;
        r3 = r30;
        ((void(*)(void))windowSearchItemID)();
        r4 = r24;
        winSpriteSetDisp();
        r4 = MENU_MIDDLE_U16_0004(r26)->unk_0004;
        r3 = r30;
        ((void(*)(void))windowSearchItemID)();
        r0 = r24 & 0xFF;
        if (r0 != (u32)0x0) {
            r0 = -0x1;
        } else {

            r4 = (0x6060 << 16);
            r0 = r4 + 0x60ff;
        }
        MENU_MIDDLE_U32_0064(r3)->unk_0064 = r0;
        r26 = r26 + 0x6;
        r27 = r27 + 0x1;
    } while (r27 < (u32)0x2);
    r27 = 0x0;
    r26 = (u32)&lbl_8047C050;
    do {
        r4 = MENU_MIDDLE_U16_0000(r26)->unk_0000;
        r3 = r30;
        ((void(*)(void))windowSearchItemID)();
        r4 = (u32)r27 >> 31;
        r0 = r27 & 0x1;
        r0 = r0 ^ r4;
        r31 = r3;
        r0 = r0 - r4;
        if ((s32)r0 != (s32)0x0) {
            f1 = MENU_MIDDLE_F32_0070(r31)->unk_0070;
            f0 = *(f32*)&lbl_8047C078;
            f2 = *(f64*)&lbl_8047C080;
            f0 = f1 + f0;
            MENU_MIDDLE_F32_0070(r31)->unk_0070 = f0;
            f1 = MENU_MIDDLE_F32_0070(r31)->unk_0070;
            ((void(*)(void))fmod)();
            f0 = (f32)f1;
            MENU_MIDDLE_F32_0070(r31)->unk_0070 = f0;
        } else {

            f2 = MENU_MIDDLE_F32_0070(r31)->unk_0070;
            f1 = *(f32*)&lbl_8047C078;
            f0 = *(f32*)&lbl_8047C088;
            f1 = f2 - f1;
            MENU_MIDDLE_F32_0070(r31)->unk_0070 = f1;
            f1 = MENU_MIDDLE_F32_0070(r31)->unk_0070;
            if (f0 > f1) {
                f0 = *(f32*)&lbl_8047C08C;
                f0 = f1 + f0;
                MENU_MIDDLE_F32_0070(r31)->unk_0070 = f0;
            }
            f1 = MENU_MIDDLE_F32_0070(r31)->unk_0070;
            f2 = *(f64*)&lbl_8047C080;
            ((void(*)(void))fmod)();
            f0 = (f32)f1;
            MENU_MIDDLE_F32_0070(r31)->unk_0070 = f0;
        }
        r26 = r26 + 0x2;
        r27 = r27 + 0x1;
    } while (r27 < (u32)0x4);
    return;
}
#endif

#if defined(MENU_MIDDLE_ALL) || defined(MENU_MIDDLE_EXACT_80070274_ONLY)
/* 0x80070274 | size: 0x28 */
#pragma push
#pragma scheduling off
void fn_80070274(u32 r3) {
    extern void fn_80070D84(u32 r3, u32 r4, u32 r5);
    fn_80070D84(r3, 0x0, 0x0);
}
#pragma pop

/* 0x8007029C | size: 0x2C */
#pragma push
#pragma peephole off
void fn_8007029C(u32 r3) {
    extern void fn_80070D84(u32 r3, u32 r4, u32 r5);
    fn_80070D84(r3, (u32)lbl_80267FE8, 0x11);
}
#pragma pop

/* 0x800702C8 | size: 0x28 */
#pragma push
#pragma scheduling off
void fn_800702C8(u32 r3) {
    extern void fn_80070D84(u32 r3, u32 r4, u32 r5);
    fn_80070D84(r3, 0x0, 0x0);
}
#pragma pop

/* 0x800702F0 | size: 0x28 */
#pragma push
#pragma scheduling off
void fn_800702F0(u32 r3) {
    extern void fn_80070D84(u32 r3, u32 r4, u32 r5);
    fn_80070D84(r3, 0x0, 0x0);
}
#pragma pop
#endif

#if defined(MENU_MIDDLE_ALL) || defined(MENU_MIDDLE_RESIDUAL_80070318_ONLY)
/* 0x80070318 | size: 0x110 */
void fn_80070318(void* menu) {
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)menu;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    
    r31 = r3;
    r0 = MENU_MIDDLE_U8_0001(r31)->unk_0001;
    r0 = (s8)r0;
    if ((s32)r0 != (s32)0x3) {
        if ((s32)r0 >= (s32)0x3) return;
        if ((s32)r0 != (s32)0x0) {
            return;


        }
        r0 = MENU_MIDDLE_U8_0002(r31)->unk_0002;
        r0 = (s8)r0;
        if ((s32)r0 != (s32)0x0) return;
        r3 = (u32)&lbl_80267F68;
        r28 = 0x0;
        r29 = (u32)&lbl_80267F68;
        r3 = (u32)&lbl_80267EA8;
        r30 = (u32)&lbl_80267EA8;
        do {
            r0 = MENU_MIDDLE_U32_0004(r29)->unk_0004;
            r3 = r31;
            r4 = MENU_MIDDLE_U16_0000(r29)->unk_0000;
            r0 = r0 << 2;
            r5 = *(u16*)(r30 + r0);
            ((void(*)(void))fn_801081F8)();
            r29 = r29 + 0x8;
            r28 = r28 + 0x1;
        } while (r28 < (u32)0x10);
        return;
    }
    r0 = MENU_MIDDLE_U8_0002(r31)->unk_0002;
    r0 = (s8)r0;
    if ((s32)r0 != (s32)0x0) return;
    r3 = (u32)&lbl_80267F68;
    r28 = 0x0;
    r29 = (u32)&lbl_80267F68;
    r3 = (u32)&lbl_80267EA8;
    r30 = (u32)&lbl_80267EA8;
    do {
        r0 = MENU_MIDDLE_U32_0004(r29)->unk_0004;
        r3 = r31;
        r4 = MENU_MIDDLE_U16_0000(r29)->unk_0000;
        r0 = r0 << 2;
        r5 = r30 + r0;
        r5 = MENU_MIDDLE_U16_0002(r5)->unk_0002;
        ((void(*)(void))fn_801081F8)();
        r29 = r29 + 0x8;
        r28 = r28 + 0x1;
    } while (r28 < (u32)0x10);
    r0 = 0x1;
    MENU_MIDDLE_U8_0002(r31)->unk_0002 = r0;

    return;
}


/* 0x80070428 | size: 0x7C */
#pragma peephole off
void fn_80070428(void* arg0, void* menu) {
    extern void msgctrlSetValue();
    u32 value;
    void* context = menu;
    s16 state = MENU_MIDDLE_S16_0006(context)->unk_0006;
    u32 message;

    if (state >= 0xA28 || state < 0xA1D) {
        return;
    }
    message = MENU_MIDDLE_U32_004C(context)->unk_004C;
    if (message != 0) {
        value = GSmsgGetGSchar(message);
        msgctrlSetValue(0x37, value);
        fn_800FB680(0, 0, MENU_MIDDLE_U32_0064(context)->unk_0064, 0xE7);
        MENU_MIDDLE_U32_004C(context)->unk_004C = 0;
    }
}
#pragma peephole reset
#endif

#if defined(MENU_MIDDLE_ALL) || defined(MENU_MIDDLE_EXACT_800704A4_ONLY)
/* 0x800704A4 | size: 0x4 */
void fn_800704A4(void) {
}

/* 0x800704A8 | size: 0x4 */
void fn_800704A8(void) {
}
#endif

#if defined(MENU_MIDDLE_ALL) || defined(MENU_MIDDLE_RESIDUAL_800704AC_ONLY)
/* 0x800704AC | size: 0x218 */
#pragma peephole off
void fn_800704AC(void* menu, void* sprite) {
    extern u8 fn_8006B3C8(s32);
    extern void winSpriteSetDisp(void*, s32);
    u8* menuData = lbl_80267EA8;
    u32 i;
    u8* window;

    switch ((s8)MENU_MIDDLE_U8_0001(menu)->unk_0001) {
    case 0:
    case 1:
    case 2:
        switch (fn_8007162C()) {
        case 0xAA:
            *(u8**)&lbl_8047A5F8 = menuData + 0x28;
            break;
        case 0xAC:
            *(u8**)&lbl_8047A5F8 = menuData + 0x40;
            break;
        case 0xA8:
            *(u8**)&lbl_8047A5F8 = menuData + 0x58;
            break;
        case 0xB3:
            *(u8**)&lbl_8047A5F8 = menuData + 0x64;
            break;
        case 0xAE:
            for (i = 0; i < 6; i++) {
                ((u32*)lbl_802EDE58)[i] = fn_8006B3C8(i) ? ((u32*)(menuData + 0x78))[i] : 0x43FE;
            }
            *(u8**)&lbl_8047A5F8 = lbl_802EDE58;
            break;
        case 0xAF:
            if (windowSearchID(0xBC) != NULL) {
                *(u8**)&lbl_8047A5F8 = menuData + 0x90;
            } else {
                *(u8**)&lbl_8047A5F8 = menuData + 0x9C;
            }
            break;
        case 0xB0:
            *(u8**)&lbl_8047A5F8 = menuData + 0xA8;
            break;
        case 0xB9:
            *(u8**)&lbl_8047A5F8 = menuData + 0xB4;
            break;
        case 0xB6:
            *(u32**)&lbl_8047A5F8 = (u32*)&lbl_8047C048 +
                ((windowGetActiveID() == fn_8007162C()) ? 0 : 1);
            break;
        /* RULE-EXCEPTION(user-approved): isolated case label added so GC/1.3 builds retail's jump table — see docs/RULE_EXCEPTIONS.md */
        case 0xEB:
        case 0xEE:
        default:
            *(u8**)&lbl_8047A5F8 = NULL;
            break;
        }
        break;
    case 3:
    case 4:
    case 5:
        break;
    }

    if (*(u8**)&lbl_8047A5F8 != NULL) {
        winSpriteSetDisp(sprite, 1);
        switch (*(s16*)((u8*)sprite + 6)) {
        case 0x93D:
            window = (u8*)windowSearchID(fn_8007162C());
            if (window == NULL) {
                window = (u8*)windowSearchID(windowGetActiveID());
            }
            if (window != NULL) {
                *(u32*)((u8*)sprite + 0x4C) = (*(u32**)&lbl_8047A5F8)[*(s8*)(window + 0x95)];
            }
            break;
        case 0x93A:
        case 0x93B:
        case 0x93C:
            break;
        }
    } else {
        *(u32*)((u8*)sprite + 0x4C) = 0;
        winSpriteSetDisp(sprite, 0);
    }
}

/* 0x800706C4 | size: 0x3D8 */
void fn_800706C4(void* menu, void* sprite) {
    extern void* fn_8006B420(void);
    extern u8* fn_8006AFC4(u8* p);
    extern void winSpriteSetDisp(void* sprite, u8 visible);
    extern u8* savedataGetStatus(s32 idx, s32 type);
    extern void* windowSearchItemID(void* menu, s32 itemId);
    extern u32 GSmsgGetRect(u32 messageId);
    extern u8 fn_800767B8(void* heroCopy, void* data);
    s16 width;

    switch ((s8)MENU_MIDDLE_U8_0001(menu)->unk_0001) {
    case 0:
    case 1:
    case 2:
        lbl_8047A5F4 = MENU_MIDDLE_U32_004C(sprite)->unk_004C;
        switch ((s32)windowGetParam(menu, 0)) {
        case 0xA8:
            lbl_8047A5F4 = 0x3BE5;
            lbl_8047A5F0 = 1;
            break;
        case 0xAF:
            lbl_8047A5F4 = 0x3C2A;
            lbl_8047A5F0 = 1;
            break;
        case 0xB5:
            lbl_8047A5F4 = 0x3C1C;
            lbl_8047A5F0 = 1;
            break;
        case 0xB0:
            lbl_8047A5F4 = 0x3C52;
            lbl_8047A5F0 = 1;
            break;
        case 0xAD:
            lbl_8047A5F4 = 0x3D87;
            lbl_8047A5F0 = 2;
            break;
        case 0xBF:
            lbl_8047A5F4 = 0x3C18;
            lbl_8047A5F0 = 2;
            break;
        case 0xC2:
            lbl_8047A5F4 = 0x3C1A;
            lbl_8047A5F0 = 2;
            break;
        case 0xB2:
            lbl_8047A5F4 = 0x3C5A;
            lbl_8047A5F0 = 2;
            break;
        case 0xEB:
            lbl_8047A5F4 = 0x4402;
            lbl_8047A5F0 = 2;
            break;
        case 0xB9:
            lbl_8047A5F4 = 0x3D32;
            lbl_8047A5F0 = 1;
            break;
        case 0xCC:
            lbl_8047A5F4 = 0x3BFC;
            lbl_8047A5F0 = 2;
            break;
        case 0xD7:
            lbl_8047A5F4 = 0x44C7;
            lbl_8047A5F0 = 2;
            break;
        case 0xBC:
            break;
        case 0xB1:
            if (fn_8006AFC4(savedataGetStatus(0, 0xE)) != NULL) {
                if (fn_800767B8(fn_8006AFC4(savedataGetStatus(0, 0xE)) + 0xB44, fn_8006B420()) != 0) {
                    lbl_8047A5F4 = 0x3BFC;
                    lbl_8047A5F0 = 2;
                    break;
                }
            }
        default:
            lbl_8047A5F4 = 0;
            lbl_8047A5F0 = 0;
            break;
        }
        break;
    }

    width = (s16)((GSmsgGetRect(lbl_8047A5F4) >> 16) - 0x20);

    switch (MENU_MIDDLE_S16_0006(sprite)->unk_0006) {
    case 0xEF9:
        winSpriteSetDisp(sprite, lbl_8047A5F0 == 2);
        MENU_MIDDLE_U32_004C(sprite)->unk_004C = lbl_8047A5F0 == 2 ? lbl_8047A5F4 : 0;
        break;
    case 0x808:
        winSpriteSetDisp(sprite, lbl_8047A5F0 == 1);
        MENU_MIDDLE_U32_004C(sprite)->unk_004C = lbl_8047A5F0 == 1 ? lbl_8047A5F4 : 0;
        break;
    case 0x809:
        winSpriteSetDisp(sprite, 0);
        break;
    case 0x937:
        winSpriteSetDisp(sprite, lbl_8047A5F0 == 2);
        break;
    case 0x938:
        winSpriteSetDisp(sprite, lbl_8047A5F0 == 2);
        MENU_MIDDLE_S16_0054(sprite)->unk_0054 = width;
        break;
    case 0x939:
        winSpriteSetDisp(sprite, lbl_8047A5F0 == 2);
        MENU_MIDDLE_S16_0050(sprite)->unk_0050 =
            width + MENU_MIDDLE_S16_0050(windowSearchItemID(menu, 0x938))->unk_0050;
        break;
    case 0xEF6:
        winSpriteSetDisp(sprite, lbl_8047A5F0 == 1);
        break;
    case 0xEF7:
        winSpriteSetDisp(sprite, lbl_8047A5F0 == 1);
        MENU_MIDDLE_S16_0054(sprite)->unk_0054 = width;
        break;
    case 0xEF8:
        winSpriteSetDisp(sprite, lbl_8047A5F0 == 1);
        MENU_MIDDLE_S16_0050(sprite)->unk_0050 =
            width + MENU_MIDDLE_S16_0050(windowSearchItemID(menu, 0xEF7))->unk_0050;
        break;
    }
}


/* 0x80070A9C | size: 0x2E8 */
void fn_80070A9C(void* menu, void* sprite) {
    extern u8* savedataGetStatus(s32 idx, s32 type);
    extern void winSpriteSetDisp(void* widget, s32 flag);

    switch ((s8)MENU_MIDDLE_U8_0001(menu)->unk_0001) {
    case 0:
    case 1:
    case 2:
        lbl_8047A5E8 = MENU_MIDDLE_U32_004C(sprite)->unk_004C;
        lbl_8047A5EC = MENU_MIDDLE_U32_004C(sprite)->unk_004C;
        switch ((s32)windowGetParam(menu, 0)) {
        case 0xAA:
        case 0xB6:
        case 0xB8:
        case 0xD1:
            lbl_8047A5E8 = 0;
            break;
        case 0xA8:
            lbl_8047A5E8 = 0x3D3A;
            lbl_8047A5EC = 0;
            break;
        case 0xAC:
            lbl_8047A5E8 = 0x3D3A;
            lbl_8047A5EC = 0x3F3E;
            break;
        case 0xAD:
            lbl_8047A5E8 = 0x3D3A;
            lbl_8047A5EC = 0x3DA6;
            break;
        case 0xB0:
        case 0xB2:
        case 0xEB:
            lbl_8047A5E8 = 0x3D3A;
            lbl_8047A5EC = 0x3D50;
            break;
        case 0xB3:
            lbl_8047A5E8 = 0x3D3A;
            lbl_8047A5EC = 0x3D2D;
            break;
        case 0xCC:
            lbl_8047A5E8 = 0x3D6E;
            lbl_8047A5EC = 0;
            break;
        case 0xB9:
            lbl_8047A5E8 = 0x3D3A;
            lbl_8047A5EC = 0x3D42;
            break;
        case 0xF5:
            lbl_8047A5E8 = 0x4274;
            lbl_8047A5EC = 0;
            break;
        case 0xC0:
        case 0xC1:
            if ((s32)MENU_MIDDLE_U32_0000(savedataGetStatus(0, 0xE))->unk_0000 == 0) {
                lbl_8047A5E8 = 0x4237;
                lbl_8047A5EC = 0;
                break;
            }
            switch (MENU_MIDDLE_U32_0008(savedataGetStatus(0, 0xE))->unk_0008) {
            case 0:
                lbl_8047A5E8 = 0x3D7C;
                lbl_8047A5EC = 0;
                break;
            case 1:
                lbl_8047A5E8 = 0x3D7D;
                lbl_8047A5EC = 0;
                break;
            case 2:
                lbl_8047A5E8 = 0x3D7E;
                lbl_8047A5EC = 0;
                break;
            case 3:
                lbl_8047A5E8 = 0x3D7F;
                lbl_8047A5EC = 0;
                break;
            case 4:
                lbl_8047A5E8 = 0x3D80;
                lbl_8047A5EC = 0;
                break;
            case 5:
                lbl_8047A5E8 = 0x3D81;
                lbl_8047A5EC = 0;
                break;
            }
            break;
        case 0xAE:
        case 0xAF:
        case 0xB1:
        case 0xB5:
        case 0xBF:
        case 0xC2:
        case 0xC4:
        case 0xC6:
        case 0xD7:
            switch (MENU_MIDDLE_U32_0000(savedataGetStatus(0, 0xE))->unk_0000) {
            case 3:
                lbl_8047A5E8 = 0x3D6E;
                lbl_8047A5EC = 0;
                break;
            case 0:
                lbl_8047A5E8 = 0x3D3A;
                lbl_8047A5EC = 0x3DAB;
                break;
            case 1:
                lbl_8047A5E8 = 0x3D3A;
                lbl_8047A5EC = 0x423C;
                break;
            case 2:
                lbl_8047A5E8 = 0x3D3A;
                lbl_8047A5EC = 0x3D2D;
                break;
            }
            break;
        }
        break;
    }

    winSpriteSetDisp(sprite, lbl_8047A5E8 != 0);

    switch (MENU_MIDDLE_S16_0006(sprite)->unk_0006) {
    case 0x89B:
        MENU_MIDDLE_U32_004C(sprite)->unk_004C = lbl_8047A5E8;
        break;
    case 0x93E:
        MENU_MIDDLE_U32_004C(sprite)->unk_004C = lbl_8047A5EC;
        break;
    case 0x80A:
        break;
    }
}


#endif

#if defined(MENU_MIDDLE_ALL) || defined(MENU_MIDDLE_RESIDUAL_80070D84_ONLY)
/* 0x80070D84 | size: 0x318 */
s32 fn_80070D84(MenuMiddleMenu* mm, MenuMiddleEntry* list, u32 count)
{
    extern void fn_801081F8(MenuMiddleMenu* menu, u16 id, u16 sequence);
    extern void winSetSequence(u32* window, u16 sequence);

    MenuMiddleItem* item;
    u32 i;
    /* RULE-EXCEPTION(title-path): aggregate cursors preserve MWCC register
     * lifetimes; see docs/RULE_EXCEPTIONS.md. */
    struct {
        MenuMiddleEntry* openingEntry;
        MenuMiddleItem* openingWindow;
        MenuMiddleEntry* closingEntry;
        MenuMiddleItem* closingWindow;
    } cursor;
    s32 corner;
    s32 y;
    u16 sequence;
    u32 kind;
    s8 done;

    done = mm->done;
    item = mm->items;
    if (done != 0) {
        return 0;
    }

    switch (mm->state) {
    case 0:
        if (list != NULL) {
            cursor.openingEntry = list;
            i = 0;
            while (i < count) {
                kind = cursor.openingEntry->kind;
                fn_801081F8(mm, cursor.openingEntry->id, ((u16*)lbl_80267EA8)[kind * 2]);
                cursor.openingEntry++;
                i++;
            }
        }
        while (item != NULL) {
            if (item->x < 0x12C) {
                y = item->y;
                if (y < 0x64) {
                    corner = 3;
                } else if (y < 0xC8) {
                    corner = 4;
                } else {
                    corner = 5;
                }
            } else {
                y = item->y;
                if (y < 0x64) {
                    corner = 6;
                } else if (y < 0xC8) {
                    corner = 7;
                } else {
                    corner = 8;
                }
            }
            if ((item != NULL && item->window != 0 && item->disabled == 0) == 0) {
                sequence = ((u16*)lbl_80267EA8)[corner * 2];
                winSetSequence(&item->window, sequence);
            }
            item = item->next;
        }
        for (cursor.openingWindow = mm->windows; cursor.openingWindow != NULL;
             cursor.openingWindow = cursor.openingWindow->next) {
            if ((cursor.openingWindow != NULL && cursor.openingWindow->window != 0 &&
                 cursor.openingWindow->disabled == 0) == 0) {
                winSetSequence(&cursor.openingWindow->window, 0x1CA);
            }
        }
        break;

    case 3:
        if (list != NULL) {
            cursor.closingEntry = list;
            i = 0;
            while (i < count) {
                kind = cursor.closingEntry->kind;
                fn_801081F8(mm, cursor.closingEntry->id, ((u16*)lbl_80267EA8)[kind * 2 + 1]);
                cursor.closingEntry++;
                i++;
            }
        }
        while (item != NULL) {
            if (item->x < 0x12C) {
                y = item->y;
                if (y < 0x64) {
                    corner = 3;
                } else if (y < 0xC8) {
                    corner = 4;
                } else {
                    corner = 5;
                }
            } else {
                y = item->y;
                if (y < 0x64) {
                    corner = 6;
                } else if (y < 0xC8) {
                    corner = 7;
                } else {
                    corner = 8;
                }
            }
            if ((item != NULL && item->window != 0 && item->disabled == 0) == 0) {
                sequence = ((u16*)lbl_80267EA8)[corner * 2 + 1];
                winSetSequence(&item->window, sequence);
            }
            item = item->next;
        }
        for (cursor.closingWindow = mm->windows; cursor.closingWindow != NULL;
             cursor.closingWindow = cursor.closingWindow->next) {
            if ((cursor.closingWindow != NULL && cursor.closingWindow->window != 0 &&
                 cursor.closingWindow->disabled == 0) == 0) {
                winSetSequence(&cursor.closingWindow->window, 0x1CE);
            }
        }
        mm->done = 1;
        break;
    }
    return 1;
}
#endif
