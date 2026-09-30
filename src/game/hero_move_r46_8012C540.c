/**
 * @file hero_move_r46_8012C540.c
 * @brief heroMoveCheckEvent (0x8012C540 - 0x8012C660): GScolsys2CheckGetEventID
 *        for the point in front of the leader.
 *
 * Function-boundary carve of the hero_move TU (hero_move.c, GC/1.3 -O4,p),
 * text only, like hero_move_exact_8012C660.c. The body is hero_move.c's;
 * keep the two in step. Like retail, the leader's ID (out-of-range leader)
 * and the result (no person) are left uninitialised on those paths.
 *
 * RULE-EXCEPTION(user-approved): extern named stand-ins for the TU's own pool
 * literals - see docs/RULE_EXCEPTIONS.md. The getResID table {100, 101} is
 * read from lbl_8047D030/lbl_8047D034, 0.0f is lbl_8047D038, and the eye
 * height and reach are lbl_8047D078/lbl_8047D07C (all defined in
 * sdata2_8047D028.c). The pool is shared by the whole TU, so the carve
 * cannot own it. The clean form needs the whole hero_move TU linked with its
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

/* RULE-EXCEPTION(user-approved): pool stand-ins - see docs/RULE_EXCEPTIONS.md */
extern u32 lbl_8047D030;
extern u32 lbl_8047D034;
extern f32 lbl_8047D038;
extern f32 lbl_8047D078;
extern f32 lbl_8047D07C;

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

s32 heroMoveCheckEvent(void* event)
{
    typedef struct HeroMoveEventVec {
        f32 x;
        f32 y;
        f32 z;
    } HeroMoveEventVec;
    extern u32 fn_8018D998(u32 group, u32 handle);
    extern void* peopleSearchID(u32 id);
    extern HeroMoveEventVec* fn_8018FCBC(void* person);
    extern HeroMoveEventVec* peopleGetPosition(void* person);
    extern f64 sin(f64 angle);
    extern f64 cos(f64 angle);
    extern void PSVECAdd(HeroMoveEventVec* dst, const HeroMoveEventVec* lhs,
                         const HeroMoveEventVec* rhs);
    extern s32 GScolsys2CheckGetEventID(const HeroMoveEventVec* position,
                                        const HeroMoveEventVec* offset,
                                        void* event);
    u32 group;
    u32 id;
    HeroMoveEventVec* rotation;
    HeroMoveEventVec* position;
    HeroMoveEventVec origin;
    HeroMoveEventVec offset;
    void* person;
    s32 result;

    getResID(&group, &id, lbl_80426BD0.leader);
    person = peopleSearchID(fn_8018D998(group, id));
    if (person != NULL) {
        position = fn_8018FCBC(person);
        rotation = peopleGetPosition(person);
        origin = *position;
        origin.y += lbl_8047D078;
        offset.x = lbl_8047D07C * (f32)sin(rotation->y);
        offset.y = lbl_8047D038;
        offset.z = lbl_8047D07C * (f32)cos(rotation->y);
        PSVECAdd(&origin, &offset, &offset);
        result = GScolsys2CheckGetEventID(&origin, &offset, event);
    }

    return result;
}
