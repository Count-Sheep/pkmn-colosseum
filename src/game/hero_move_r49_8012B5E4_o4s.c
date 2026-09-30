/**
 * @file hero_move_r49_8012B5E4_o4s.c
 * @brief heroMoveChkHinderClear (0x8012B5E4 - 0x8012BAD0): is the path from
 *        a party member to the leader clear?
 *
 * Function-boundary carve of the hero_move TU (hero_move.c, GC/1.3 -O4,p),
 * text only, like hero_move_exact_8012C660.c. The body is hero_move.c's
 * (XD heroMoveChkHinderClear, 0x8014F070); keep the two in step. The static
 * inlines are the TU's helpers (getResID, heroMoveCheckMember, getModel,
 * getObjID, getPos, MSL's inline sqrtf and GSvecDistanceXZ).
 *
 * RULE-EXCEPTION(user-approved): extern named stand-ins for the TU's own pool
 * literals - see docs/RULE_EXCEPTIONS.md. The getResID table {100, 101} is
 * read from lbl_8047D030/lbl_8047D034, 0.0f is lbl_8047D038, 8.5f is
 * lbl_8047D03C, the sqrtf constants 0.5 and 3.0 are lbl_8047D048 and
 * lbl_8047D050, 0.0 is lbl_8047D058 and the 1.0f gap is lbl_8047D060 (all
 * defined in sdata2_8047D028.c). The pool is shared by the whole TU, so the
 * carve cannot own it. sqrtf copies 0.5 and 3.0 into locals before its
 * Newton steps: read directly, the stand-ins take f4/f3 and push the
 * argument from retail's f4 to f2, while the TU's literals do not. The clean
 * form needs the whole hero_move TU linked with its pool.
 */
#include "dolphin/types.h"
#include "game/world/gs_field.h"
#include "game/hero_move.h"

extern HeroMoveWork lbl_80426BD0;

/* s32 entries: see HeroMoveResIDTable in hero_move.c. */
typedef struct HeroMoveResIDTable {
    s32 id[2];
} HeroMoveResIDTable;

/* RULE-EXCEPTION(user-approved): pool stand-ins - see docs/RULE_EXCEPTIONS.md */
extern u32 lbl_8047D030;
extern u32 lbl_8047D034;
extern f32 lbl_8047D038;
extern f32 lbl_8047D03C;
extern const f64 lbl_8047D048;
extern const f64 lbl_8047D050;
extern const f64 lbl_8047D058;
extern f32 lbl_8047D060;
/* MSL's __float_nan, addressed absolutely (see hero_move.c). */
extern u8 lbl_80478AC0[];

static inline u8 getResID(u32* group, u32* id, int member)
{
    HeroMoveResIDTable ids;

    ids.id[0] = lbl_8047D030;
    ids.id[1] = lbl_8047D034;

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

static inline s32 getObjID(s32 member)
{
    extern u32 fn_8018D998(u32 group, u32 id);
    extern void* peopleSearchID(u32 id);
    u32 group;
    u32 id;
    void* person;

    if (!getResID(&group, &id, member)) {
        return -1;
    }
    person = peopleSearchID(fn_8018D998(group, id));
    if (person == NULL) {
        return -1;
    }
    return *(s32*)((u8*)person + 0x30);
}

static inline void getPos(HeroMoveVec* pos, s32 member)
{
    extern void GSmodelGetPosition(void* model, void* out);

    GSmodelGetPosition(heroMoveGetModel(member), pos);
}

typedef union HeroMoveFloatShape {
    f32 value;
    u32 bits;
} HeroMoveFloatShape;

static inline f32 heroMoveSqrt(f32 value)
{
    HeroMoveFloatShape shape;
    f64 estimate;
    u32 exponent;
    s32 fpclass;

    if (value > lbl_8047D038) {
        /* RULE-EXCEPTION(user-approved): stand-in copies - see docs/RULE_EXCEPTIONS.md */
        const f64 half = lbl_8047D048;
        const f64 three = lbl_8047D050;

        estimate = __frsqrte(value);
        estimate = half * estimate * (three - value * (estimate * estimate));
        estimate = half * estimate * (three - value * (estimate * estimate));
        estimate = half * estimate * (three - value * (estimate * estimate));
        return (f32)(value * estimate);
    }
    if (value < lbl_8047D058) {
        return *(f32*)lbl_80478AC0;
    }

    shape.value = value;
    exponent = shape.bits & 0x7F800000;
    switch (exponent) {
    case 0x7F800000:
        if ((shape.bits & 0x007FFFFF) != 0) {
            fpclass = 1;
        } else {
            fpclass = 2;
        }
        break;
    case 0:
        if ((shape.bits & 0x007FFFFF) != 0) {
            fpclass = 5;
        } else {
            fpclass = 3;
        }
        break;
    default:
        fpclass = 4;
        break;
    }
    if (fpclass == 1) {
        return *(f32*)lbl_80478AC0;
    }
    return value;
}

static inline f32 GSvecDistanceXZ(HeroMoveVec* a, HeroMoveVec* b)
{
    return heroMoveSqrt((a->x - b->x) * (a->x - b->x) + (a->z - b->z) * (a->z - b->z));
}

u32 heroMoveChkHinderClear(s32 member) {
    extern u32 fn_8018D998(u32 group, u32 id);
    extern u8* peopleSearchID(u32 id);
    extern void* peopleInfoBiosGetPtr(s32 id);
    extern f32 fn_8018F5E4(void* info);
    extern s32 fn_8010F320(void* start, void* end, f32 radius, void* result);
    extern void PSVECSubtract(void* a, void* b, void* out);
    extern void PSVECScale(void* src, void* dst, f32 scale);
    extern void PSVECAdd(void* a, void* b, void* out);
    extern s32 GScolsys2HumanCollision(u32 col, void* from, void* to, u32 flags);

    HeroMoveVec memberPos;
    HeroMoveVec leaderPos;
    HeroMoveVec dir;
    HeroMoveVec ofs;
    s32 leader;
    u32 objID;
    void* info;
    u8* people;
    u32 col;
    u32 group;
    u32 id;
    f32 memberRadius;
    f32 leaderRadius;
    f32 radiusSum;
    f32 dx;
    f32 dz;
    f32 dist;

    if (!heroMoveCheckMember(member)) {
        return 0;
    }
    leader = lbl_80426BD0.leader;
    getPos(&memberPos, member);
    memberPos.y += lbl_8047D03C;
    getPos(&leaderPos, leader);
    leaderPos.y += lbl_8047D03C;

    objID = getObjID(member);
    if (objID == -1) {
        return 0;
    }
    info = peopleInfoBiosGetPtr(objID);
    if (info == NULL) {
        return 0;
    }
    memberRadius = fn_8018F5E4(info);
    if (fn_8010F320(&memberPos, &leaderPos, memberRadius, NULL) != 0) {
        return 0;
    }

    objID = getObjID(leader);
    if (objID == -1) {
        return 0;
    }
    info = peopleInfoBiosGetPtr(objID);
    if (info == NULL) {
        return 0;
    }
    leaderRadius = fn_8018F5E4(info);

    getResID(&group, &id, member);
    people = peopleSearchID(fn_8018D998(group, id));
    if (people == NULL) {
        return 0;
    }
    col = *(u32*)(people + 0x50);

    getPos(&memberPos, member);
    getPos(&leaderPos, leader);
    PSVECSubtract(&leaderPos, &memberPos, &dir);
    radiusSum = memberRadius + leaderRadius;
    dist = GSvecDistanceXZ(&memberPos, &leaderPos);
    if (dist <= lbl_8047D038) {
        return 1;
    }
    if (dist <= radiusSum) {
        return 1;
    }

    PSVECScale(&dir, &ofs, ((dist - radiusSum) - lbl_8047D060) / dist);
    PSVECAdd(&memberPos, &ofs, &leaderPos);
    return GScolsys2HumanCollision(col, &memberPos, &leaderPos, 0) != 6;
}
