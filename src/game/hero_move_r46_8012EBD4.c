/**
 * @file hero_move_r46_8012EBD4.c
 * @brief heroMoveMain through initFloor (0x8012EBD4 - 0x8012FAD8): the
 *        per-frame party update, the resource/neck-mode/membership exports
 *        and the party add/dismiss/leader functions of the hero_move TU.
 *
 * Function-boundary carve of the hero_move TU (hero_move.c, GC/1.3 -O4,p),
 * text only, like the suffix carve hero_move_r46_8012FCD4_suffix.c. The
 * bodies are hero_move.c's; keep them in step. The static inlines are the
 * TU's own helpers (getResID, getModel, procStep, the spacing update,
 * getPos/getRot/setPos/setRot, initFootWork).
 *
 * RULE-EXCEPTION(title-path): extern named stand-ins for the TU's own pool
 * literals, with shaping forms that make them load like the literals - see
 * docs/RULE_EXCEPTIONS.md. Stand-ins: the getResID table (lbl_8047D030/
 * D034), 0.0f (D038), the 7.0f step (D064), initFloor's 9.0f (D0AC),
 * -1000000.0f (D0D8, const) and 10.0f (D07C, const, read through a pointer
 * cast), and the 12.0f spacing step (D0D4), all defined in
 * sdata2_8047D028.c/sdata2_8047D098.c. With the stand-ins, initFloor's
 * register pair (getRot/setRot IDs, 99.87% in the whole-TU candidate) comes
 * out as retail. The clean form needs the whole hero_move TU linked with its
 * pool.
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


typedef struct HeroMoveFloorHit {
    f32 height;
    f32 unk04;
    f32 unk08;
} HeroMoveFloorHit;

typedef struct HeroMovePartTable {
    s32 part[2][4];
} HeroMovePartTable;

extern const f32 lbl_8047D064;
extern f32 lbl_8047D0AC;
extern const f32 lbl_8047D0D8;
extern const f32 lbl_8047D07C;
extern u8 lbl_80272A38[];
extern u8 dbgMenuIsOpen(void);
extern u8 menuIsCheck(u32);
extern u8 fn_8018C424(u32, u32, u32);
extern void fn_8000D710(u32);
extern u8 GSscene_GetMode(void);
extern void fn_80116D30(u32 kind, u32 arg);
extern u32 fn_800D3088(void);
extern void getStep__FP8FOOTSTEPP8_GSmodelPiP8FOOTWORK(f32*, void*, s32*, f32*);
extern void fn_8012DE94(u32 playerIndex);
extern f32 moveLeader__F15HEROMOVE_MEMBER();

u32 heroMoveMain(void);
u8 heroMoveGetResID(u32* group, u32* id, s32 member);
u32 heroMoveSetNeckMode(s32 member, HeroMoveNeckMode mode);
u32 heroMoveIsMember(s32 member);
s32 heroMoveDismissMember(s32 member);
s32 fn_8012F1FC(s32 member);
s32 fn_8012F40C(s32 member);
void initFloor__Fv(void);



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

static inline void* heroMoveGetModel(int member)
{
    extern void* GSresGetResource(u32 group, u32 id);
    u32 group;
    u32 id;

    getResID(&group, &id, member);
    return GSresGetResource(group, id);
}

static inline void procStep(s32 member)
{
    HeroMovePartTable parts = *(HeroMovePartTable*)lbl_80272A38;
    f32 footstep[0x10];
    void* model;

    fn_800D3088();
    model = heroMoveGetModel(member);
    if (model != NULL) {
        getStep__FP8FOOTSTEPP8_GSmodelPiP8FOOTWORK(footstep, model, parts.part[member],
                                                  lbl_80426BD0.member[member].footwork.height);
    }
}

u32 heroMoveMain(void)
{
    extern u32 fn_800F7AF0(s32);
    extern u32 fn_800F7BC4(s32);
    extern u32 fn_801906A0(u32);
    extern void fn_800F7434(void*, u32, ...);
    u32 group;
    u32 id;
    u32 event;
    f32 distance;
    s32 i;
    u8 flagClear;

    if (dbgMenuIsOpen()) {
        return 0;
    }
    if (menuIsCheck(0xCA)) {
        return 0;
    }
    if (GSscene_GetMode() == 6) {
        return 0;
    }
    if (lbl_80426BD0.lockFrame > 0) {
        lbl_80426BD0.lockFrame--;
        return 0;
    }
    getResID(&group, &id, lbl_80426BD0.leader);
    if (fn_8018C424(group, id, 0x80000000)) {
        return 0;
    }

    event = lbl_80426BD0.autoEvent[0];
    if (event != 0) {
        lbl_80426BD0.autoEvent[0] = 0;
        fn_800F7434((void*)event, 4, lbl_80426BD0.autoEvent[1],
                    lbl_80426BD0.autoEvent[2], lbl_80426BD0.autoEvent[3],
                    lbl_80426BD0.autoEvent[4]);
        return 0;
    }

    for (i = 0; i < lbl_80426BD0.eventValue[0]; i++) {
        fn_80116D30(3, lbl_80426BD0.eventList[0][i].id);
    }
    for (i = 0; i < lbl_80426BD0.eventValue[2]; i++) {
        fn_80116D30(2, lbl_80426BD0.eventList[2][i].id);
    }
    for (i = 0; i < lbl_80426BD0.eventValue[1]; i++) {
        fn_80116D30(1, lbl_80426BD0.eventList[1][i].id);
    }
    lbl_80426BD0.eventValue[1] = 0;
    lbl_80426BD0.eventValue[2] = 0;
    lbl_80426BD0.eventValue[0] = 0;
    if (lbl_80426BD0.autoEvent[0] != 0) {
        return 0;
    }

    if (fn_800F7BC4(1) & fn_800F7AF0(1) & 0x1C00) {
        fn_8000D710(0);
        return 0;
    }

    for (i = 0; i < 2; i++) {
        if (heroMoveCheckMember(i) && i != lbl_80426BD0.leader) {
            fn_8012DE94(i);
        }
    }
    distance = moveLeader__F15HEROMOVE_MEMBER(lbl_80426BD0.leader);
    procStep(lbl_80426BD0.leader);

    flagClear = fn_801906A0(0x8AE) == 0;
    if (flagClear) {
        lbl_80426BD0.stepAccum += distance;
        while (lbl_80426BD0.stepAccum >= lbl_8047D064) {
            for (i = 0; i < 8; i++) {
                if (lbl_80426BD0.stepCallback[i].func != NULL) {
                    lbl_80426BD0.stepCallback[i].func(lbl_80426BD0.stepCallback[i].arg);
                }
            }
            lbl_80426BD0.stepAccum -= lbl_8047D064;
        }
    }
    return 0;
}

u8 heroMoveGetResID(u32* group, u32* id, s32 member)
{
    return getResID(group, id, member);
}

u32 heroMoveSetNeckMode(s32 member, HeroMoveNeckMode mode)
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

u32 heroMoveIsMember(s32 member)
{
    if (member < 0 || member >= 2) {
        return FALSE;
    }
    return lbl_80426BD0.member[member].flags & 1;
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

s32 heroMoveDismissMember(s32 member)
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

s32 fn_8012F1FC(s32 member)
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

s32 fn_8012F40C(s32 member)
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

static inline void getPos(HeroMoveVec* pos, s32 member)
{
    extern void GSmodelGetPosition(void* model, void* out);

    GSmodelGetPosition(heroMoveGetModel(member), pos);
}

static inline void getRot(HeroMoveVec* rot, s32 member)
{
    extern void GSmodelGetRotation(void* model, void* out);

    GSmodelGetRotation(heroMoveGetModel(member), rot);
}

static inline void setPos(s32 member, HeroMoveVec* pos)
{
    extern void fn_8018C0A8(u32 group, u32 id, void* position);
    u32 group;
    u32 id;

    getResID(&group, &id, member);
    fn_8018C0A8(group, id, pos);
}

static inline void setRot(s32 member, HeroMoveVec* rot)
{
    extern void GSmodelSetRotation(void* model, void* rot);

    GSmodelSetRotation(heroMoveGetModel(member), rot);
}

static inline void initFootWork(FOOTWORK* footwork)
{
    s32 i;

    for (i = 0; i < 4; i++) {
        footwork->height[i] = lbl_8047D038;
    }
}

void initFloor__Fv(void)
{
    extern f64 sin(f64 x);
    extern f64 cos(f64 x);
    extern s32 fn_8010E138(void* position, HeroMoveFloorHit* hits);

    HeroMoveVec rotation;
    HeroMoveVec leaderPos;
    HeroMoveVec position;
    HeroMoveFloorHit hits[8];
    f32 sinY;
    f32 cosY;
    f32 dirX;
    f32 dirZ;
    f32 distance;
    f32 bestAny;
    f32 bestStep;
    BOOL foundStep;
    s32 count;
    s32 j;
    s32 i;

    if (!fn_800FF548()) {
        getPos(&leaderPos, lbl_80426BD0.leader);
        getRot(&rotation, lbl_80426BD0.leader);
        sinY = sin(rotation.y);
        cosY = cos(rotation.y);
        dirX = -sinY;
        dirZ = -cosY;
        position.y = leaderPos.y;
        distance = lbl_8047D0AC;
        for (i = 0; i < 2; i++) {
            if (heroMoveCheckMember(i) && i != lbl_80426BD0.leader) {
                position.x = leaderPos.x + dirX * distance;
                position.y = leaderPos.y;
                position.z = leaderPos.z + dirZ * distance;
                count = fn_8010E138(&position, hits);
                if (count > 0) {
                    if (count >= 2) {
                        /* RULE-EXCEPTION(title-path): const stand-ins, pointer-cast read - see docs/RULE_EXCEPTIONS.md */
                        bestStep = lbl_8047D0D8;
                        bestAny = bestStep;
                        foundStep = FALSE;
                        for (j = 0; j < count; j++) {
                            if (bestAny < hits[j].height) {
                                bestAny = hits[j].height;
                            }
                            if (hits[j].height - position.y >= *(f32*)&lbl_8047D07C) {
                                continue;
                            }
                            if (bestStep < hits[j].height) {
                                bestStep = hits[j].height;
                                foundStep = TRUE;
                            }
                        }
                        if (foundStep) {
                            position.y = bestStep;
                        } else {
                            position.y = bestAny;
                        }
                    } else {
                        position.y = hits[0].height;
                    }
                }
                distance += lbl_8047D0AC;
                setPos(i, &position);
                setRot(i, &rotation);
            }
        }
    }

    for (i = 0; i < 2; i++) {
        heroMoveSetModelVisible(i, heroMoveCheckMember(i));
    }
    for (i = 0; i < 2; i++) {
        initFootWork(&lbl_80426BD0.member[i].footwork);
    }
    for (i = 0; i < 2; i++) {
        if (heroMoveCheckMember(i)) {
            heroMoveSetNeckMode(i, heroMoveGetNeckMode(i));
        }
    }
    lbl_80426BD0.member[0].timer = 300;
    lbl_80426BD0.member[1].timer = 300;
    lbl_80426BD0.stepAccum = lbl_8047D038;
}
