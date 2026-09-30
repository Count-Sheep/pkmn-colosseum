/**
 * @file hero_move_r46_8012FCD4_suffix.c
 * @brief heroMoveInit, heroMoveSyncWithHero and fn_8013024C
 *        (0x8012FCD4 - 0x80130660): the tail of the hero_move TU.
 *
 * Function-boundary carve of the hero_move TU (hero_move.c, GC/1.3 -O4,p),
 * text only, linked on its own so fn_8013024C (heroMoveAllInit in XD) links
 * before the rest of the TU. The bodies are hero_move.c's; keep the two in
 * step. The party helpers they expand (heroMoveAddStepCallback,
 * heroMoveSetNeckMode, heroMoveDismissMember, fn_8012F1FC, fn_8012F40C) are
 * global functions of the TU and are static inline copies here, so the
 * carve expands them as the TU does but emits no second definition.
 * cbPoison and cbTsureFriend are only referenced by address.
 *
 * RULE-EXCEPTION(title-path): extern named stand-ins for the TU's own pool
 * literals, and static inline copies of the TU's global party helpers - see
 * docs/RULE_EXCEPTIONS.md. The getResID table {100, 101} is read from
 * lbl_8047D030/lbl_8047D034, the 0.0f is lbl_8047D038 and the spacing step
 * 12.0f is lbl_8047D0D4 (all defined in sdata2_8047D028.c); the other
 * hero_move target objects reference those names, so the carve cannot own
 * the pool. Two shaping forms make the stand-ins behave like the TU's
 * literals: the table is stored through a pointer to its second entry
 * (keeps retail's stack stores where the member is constant, and names each
 * word's symbol, which a block copy of one extern cannot), and the step's
 * second read goes through a pointer cast (the TU's two 12.0f literals are
 * separate loads; two reads of one extern are merged into one value and the
 * spacing moves from f2 to f1). The clean form needs the whole hero_move TU
 * linked with its pool.
 */
#include "dolphin/types.h"
#include "game/world/gs_field.h"
#include "game/hero_move.h"

extern HeroMoveWork lbl_80426BD0;

/* s32 entries: see HeroMoveResIDTable in hero_move.c. */
typedef struct HeroMoveResIDTable {
    s32 id[2];
} HeroMoveResIDTable;

/* Local copies of the .rodata floor list and area/model pairs. */
typedef struct HeroMoveThemeTable {
    u32 words[10];
} HeroMoveThemeTable;

typedef struct HeroMoveFloorTable {
    u32 words[20];
} HeroMoveFloorTable;

extern u32 lbl_8047D030;
extern u32 lbl_8047D034;
extern f32 lbl_8047D038;
extern f32 lbl_8047D0D4;
extern u8 lbl_802729C0[];
extern u8 lbl_80272A10[];

extern u8 fn_800FF548(void);
extern u32 fn_801906A0(u32 flag);
extern u32 heroGetStatus(u8* a, u32 b, u32 c);
extern void fn_8018C1E8(u32 group, u32 id, u8 visible);
extern void fn_80188AF4(u32 group, u32 id);
extern void fn_80188F78(u32 group, u32 id);
extern u32 floorGetNextFloorID(void);
extern s32 fn_8006AE18(void);
extern void peopleOpen(u32 group, u32 id, u32 theme);
extern void GSmodelEnableAnimBlend(void* model);
extern void fn_8018CB5C(u32 group, u32 id);
extern void fn_80189328(u32 group, u32 id, u32 flag);
extern void fn_8018BF24(u32 group, u32 id, void* rotation);
extern void initFloor__Fv(void);
extern void cbPoison__Fl15FootStepCounterl(s32 arg);
extern void cbTsureFriend__Fl15FootStepCounterl(s32 arg);

s32 heroMoveInit(void* position, void* rotation);
void heroMoveSyncWithHero(void);
void fn_8013024C(void);

static inline u8 getResID(u32* group, u32* id, int member)
{
    HeroMoveResIDTable ids;
    /* RULE-EXCEPTION(title-path): shaping pointer for the stand-in table - see docs/RULE_EXCEPTIONS.md */
    s32* p = &ids.id[1];

    p[-1] = lbl_8047D030;
    p[0] = lbl_8047D034;

    if (member < 0 || member >= 2) {
        return FALSE;
    }
    *group = 0;
    *id = ids.id[member];
    return TRUE;
}

static inline u8 heroMoveCheckMember(s32 member)
{
    if (member < 0 || member >= 2) {
        return FALSE;
    }
    if (!(lbl_80426BD0.member[member].flags & 1)) {
        return FALSE;
    }
    return TRUE;
}

static inline s32 heroMoveAddStepCallback(void (*func)(s32 arg), s32 arg)
{
    s32 i;

    for (i = 0; i < 8; i++) {
        if (lbl_80426BD0.stepCallback[i].func == NULL) {
            break;
        }
    }
    if (i >= 8) {
        return -1;
    }
    lbl_80426BD0.stepCallback[i].func = func;
    lbl_80426BD0.stepCallback[i].arg = arg;
    return i;
}

static inline u32 heroMoveSetNeckMode(s32 member, HeroMoveNeckMode mode)
{
    u32 group;
    u32 id;

    if (mode < 0 || mode >= 2) {
        return FALSE;
    }
    if (!heroMoveCheckMember(member)) {
        return FALSE;
    }
    getResID(&group, &id, member);
    switch (lbl_80426BD0.member[member].neckMode) {
    case 1:
        fn_80188AF4(group, id);
        break;
    }
    switch (mode) {
    case 1:
        fn_80188F78(group, id);
        break;
    }
    lbl_80426BD0.member[member].neckMode = mode;
    return TRUE;
}

static inline void heroMoveUpdateSpacing(void)
{
    f32 spacing;
    s32 i;

    lbl_80426BD0.member[lbl_80426BD0.leader].spacing = lbl_8047D038;
    spacing = lbl_8047D0D4;
    for (i = 0; i < 2; i++) {
        if ((lbl_80426BD0.member[i].flags & 1) && lbl_80426BD0.leader != i) {
            lbl_80426BD0.member[i].spacing = spacing;
            /* RULE-EXCEPTION(title-path): pointer-cast read of the stand-in - see docs/RULE_EXCEPTIONS.md */
            spacing += *(f32*)&lbl_8047D0D4;
        }
    }
}

static inline s32 heroMoveDismissMember(s32 member)
{
    if (member < 0 || member >= 2) {
        return FALSE;
    }
    if (member == lbl_80426BD0.leader) {
        return FALSE;
    }
    lbl_80426BD0.member[member].flags &= ~1;
    heroMoveUpdateSpacing();
    return TRUE;
}

static inline void heroMoveSetModelVisible(s32 member, u8 visible)
{
    u32 group;
    u32 id;

    getResID(&group, &id, member);
    fn_8018C1E8(group, id, visible);
}

static inline s32 fn_8012F1FC(s32 member)
{
    u32 group;
    u32 id;
    HeroMoveNeckMode mode;

    if (member < 0 || member >= 2) {
        return FALSE;
    }
    if (heroMoveCheckMember(member)) {
        return TRUE;
    }
    lbl_80426BD0.member[member].flags |= 1;
    /* RULE-EXCEPTION(title-path): constant-only local for MWCC propagation - see docs/RULE_EXCEPTIONS.md */
    mode = HERO_MOVE_NECK_ON;
    heroMoveSetNeckMode(member, mode);
    heroMoveUpdateSpacing();
    heroMoveSetModelVisible(member, TRUE);
    return TRUE;
}

static inline HeroMoveNeckMode heroMoveGetNeckMode(s32 member)
{
    if (heroMoveCheckMember(member)) {
        return lbl_80426BD0.member[member].neckMode;
    }
    return HERO_MOVE_NECK_NONE;
}

static inline s32 fn_8012F40C(s32 member)
{
    if (!heroMoveCheckMember(member)) {
        return FALSE;
    }
    lbl_80426BD0.leader = member;
    if (heroMoveGetNeckMode(member) == 1) {
        heroMoveSetNeckMode(member, 0);
    }
    heroMoveUpdateSpacing();
    lbl_80426BD0.historyHead = 0;
    lbl_80426BD0.historyCount = 0;
    return TRUE;
}

/* Initialize the two field hero models and select the area's model theme. */
s32 heroMoveInit(void* position, void* rotation)
{
    extern void fn_8018D998(u32 group, u32 object);
    extern void fn_8018C8F4(u32 group, u32 object, u32 flags);
    extern void fn_8018C0A8(u32 group, u32 object, void* position);
    extern void* GSresGetResource(u32 group, u32 handle);
    extern void updateAnimation__Ff15HEROMOVE_MEMBER(void* model, s32 member, f32 amount);

    HeroMoveFloorTable floors;
    HeroMoveThemeTable themes;
    void* models[2];
    u32 handles[2];
    s32 i;
    u32* floorCursor;
    u32 floor;
    u32 theme;
    u32 handle;
    s32 area;
    u8 unavailable;

    if (fn_800FF548() == 0) {
        floors = *(HeroMoveFloorTable*)lbl_802729C0;
        themes = *(HeroMoveThemeTable*)lbl_80272A10;

        unavailable = fn_801906A0(0x8AE) == 0;
        if (unavailable != 0) {
            theme = 0x00F70400;
        } else {
            floor = floorGetNextFloorID();
            floorCursor = floors.words;
            i = 0;
            while (i < 20) {
                if (floor == *floorCursor) {
                    break;
                }
                floorCursor++;
                i++;
            }

            if (i >= 20) {
                theme = 0x00F70400;
            } else {
                area = fn_8006AE18();
                for (i = 0; i < 5; i++) {
                    if (area == (s32)themes.words[i * 2]) {
                        break;
                    }
                }
                theme = themes.words[i * 2 + 1];
            }
        }

        peopleOpen(0, 100, theme);
        peopleOpen(0, 101, 0x00F30400);
    } else {
        fn_8018D998(0, 100);
        fn_8018D998(0, 101);
    }

    for (i = 0; i < 2; i++) {
        handles[0] = lbl_8047D030;
        handles[1] = lbl_8047D034;
        if (i >= 0 && i < 2) {
            handle = handles[i];
        }
        models[i] = GSresGetResource(0, handle);
        GSmodelEnableAnimBlend(models[i]);
    }

    fn_8018CB5C(0, 100);
    fn_8018CB5C(0, 101);

    if (fn_800FF548() == 0) {
        fn_8018C8F4(0, 100, 0x40000F00);
        fn_8018C8F4(0, 101, 0x701);
    }

    fn_80189328(0, 101, 1);

    if (fn_800FF548() == 0) {
        fn_8018C0A8(0, 100, position);
        fn_8018BF24(0, 100, rotation);
    }

    for (i = 0; i < 2; i++) {
        updateAnimation__Ff15HEROMOVE_MEMBER(models[i], i, lbl_8047D038);
    }

    initFloor__Fv();
    lbl_80426BD0.historyHead = 0;
    lbl_80426BD0.historyCount = 0;
    lbl_80426BD0.eventValue[1] = 0;
    lbl_80426BD0.eventValue[2] = 0;
    lbl_80426BD0.eventValue[0] = 0;
    lbl_80426BD0.lockFrame = 0;
    return 0;
}

/* 0x80130054 | 0x1F8: the partner follows the hero unless flag 0x8AE is set. */
void heroMoveSyncWithHero(void)
{
    u8 follow = FALSE;
    u8 flagClear = fn_801906A0(0x8AE) == 0;

    if (flagClear && (s32)heroGetStatus(0, 0x18, 0) != 0) {
        follow = TRUE;
    }
    if (follow) {
        fn_8012F1FC(1);
    } else {
        heroMoveDismissMember(1);
    }
}

/* 0x8013024C | 0x414: reset the party to the hero alone and register the
 * poison and friendship step callbacks (XD heroMoveAllInit). */
void fn_8013024C(void)
{
    s32 i;

    lbl_80426BD0.member[0].flags = 0;
    lbl_80426BD0.member[1].flags = 0;
    lbl_80426BD0.leader = 0;
    lbl_80426BD0.lockFrame = 0;
    fn_8012F1FC(0);
    fn_8012F40C(0);
    lbl_80426BD0.autoEvent[0] = 0;
    lbl_80426BD0.stepAccum = lbl_8047D038;
    for (i = 0; i < 8; i++) {
        lbl_80426BD0.stepCallback[i].func = NULL;
    }
    lbl_80426BD0.poisonSteps = 0;
    heroMoveAddStepCallback(cbPoison__Fl15FootStepCounterl, 0);
    lbl_80426BD0.friendSteps = 0;
    heroMoveAddStepCallback(cbTsureFriend__Fl15FootStepCounterl, 0);
}
