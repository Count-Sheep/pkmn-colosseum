/**
 * @file hero_move_exact_8012C660.c
 * @brief updateAnimation__Ff15HEROMOVE_MEMBER (0x8012C660 - 0x8012CA84):
 *        picks and blends a party member's walk/run animations from its
 *        movement amount (back-step, blend to idle, walk, run blend).
 *
 * Function-boundary carve of the hero_move TU, GC/1.3 -O4,p, text only.
 * getResID/getObjID are the TU's static inlines (hero_move.c).
 *
 * RULE-EXCEPTION(title-path): extern named stand-ins for the TU's own pool
 * literals (0x8047D030 - 0x8047D090, defined in sdata2_8047D028.c), with the
 * 0.0f stand-in volatile so its loads are not shared -- see
 * docs/RULE_EXCEPTIONS.md. The pool is shared by the whole TU, so the clean
 * form (real literals) needs the whole hero_move TU linked with its pool.
 */
#include "dolphin/types.h"

typedef struct HeroMoveResIDTable {
    u32 id[2];
} HeroMoveResIDTable;

extern const u32 lbl_8047D030;
extern const u32 lbl_8047D034;
extern volatile f32 lbl_8047D038;
extern f32 lbl_8047D040;
extern f32 lbl_8047D080;
extern f32 lbl_8047D084;
extern f32 lbl_8047D088;
extern f32 lbl_8047D08C;
extern f32 lbl_8047D090;

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

void updateAnimation__Ff15HEROMOVE_MEMBER(void* model, s32 member, f32 amount)
{
    extern void* peopleInfoBiosGetPtr(s32);
    extern void fn_8018F4C8(void*, u8, s32*, u8*);
    extern void GSmodelGetAnimIndex(void*, s32*, s32*);
    extern f32 GSmodelGetAnimFrame(void*);
    extern void GSmodelGetFrameCount(void*, f32*, f32*);
    extern void GSmodelSetAnimIndex(void*, s32);
    extern void GSmodelSetAnimFrame(void*, f32);
    extern void GSmodelSetAnimRate(void*, f32);
    extern void GSmodelSetAnimBlend(void*, s32, s32);
    extern void GSmodelSetBlendFactor(void*, f32);
    s32 current_anim;
    s32 blend_anim;
    s32 anim1;
    s32 anim2;
    s32 anim3;
    s32 anim4;
    f32 frame_count_a;
    f32 frame_count_b;
    u8 loop;
    f32 frame;
    void* info;

    info = peopleInfoBiosGetPtr(getObjID(member));
    fn_8018F4C8(info, 1, &anim1, &loop);
    fn_8018F4C8(info, 2, &anim2, &loop);
    fn_8018F4C8(info, 3, &anim3, &loop);
    fn_8018F4C8(info, 4, &anim4, &loop);

    if (amount > lbl_8047D084) {
        amount = lbl_8047D084;
    }
    GSmodelGetAnimIndex(model, &current_anim, &blend_anim);

    if (amount < lbl_8047D088) {
        if (current_anim != anim4 || blend_anim != -1) {
            frame = lbl_8047D038;
            if (blend_anim != -1) {
                GSmodelGetFrameCount(model, &frame_count_a, &frame_count_b);
                frame = (frame_count_a / frame_count_b) *
                        GSmodelGetAnimFrame(model);
            }
            GSmodelSetAnimIndex(model, anim4);
            GSmodelSetAnimFrame(model, frame);
        }
        GSmodelSetAnimRate(model, lbl_8047D080);
    } else if (amount < lbl_8047D038) {
        if (current_anim != anim4 || blend_anim != anim1) {
            frame = lbl_8047D038;
            if (blend_anim == -1) {
                frame = GSmodelGetAnimFrame(model);
            }
            GSmodelSetAnimBlend(model, anim4, anim1);
            GSmodelGetFrameCount(model, &frame_count_a, &frame_count_b);
            GSmodelSetAnimFrame(model,
                                frame * (frame_count_b / frame_count_a));
        }
        amount = (amount - lbl_8047D088) / lbl_8047D08C;
        GSmodelSetBlendFactor(model, amount);
        GSmodelSetAnimRate(model, lbl_8047D040);
    } else if (amount < lbl_8047D08C) {
        if (current_anim != anim2 || blend_anim != anim1) {
            frame = lbl_8047D038;
            if (blend_anim == -1) {
                frame = GSmodelGetAnimFrame(model);
            }
            GSmodelSetAnimBlend(model, anim2, anim1);
            GSmodelGetFrameCount(model, &frame_count_a, &frame_count_b);
            GSmodelSetAnimFrame(model,
                                frame * (frame_count_b / frame_count_a));
        }
        GSmodelSetBlendFactor(model, lbl_8047D080 - lbl_8047D090 * amount);
        GSmodelSetAnimRate(model, lbl_8047D040);
    } else if (amount < lbl_8047D080) {
        if (current_anim != anim2 || blend_anim != -1) {
            frame = lbl_8047D038;
            if (blend_anim != -1) {
                GSmodelGetFrameCount(model, &frame_count_a, &frame_count_b);
                frame = (frame_count_a / frame_count_b) *
                        GSmodelGetAnimFrame(model);
            }
            GSmodelSetAnimIndex(model, anim2);
            GSmodelSetAnimFrame(model, frame);
        }
        GSmodelSetAnimRate(model, lbl_8047D040);
    } else {
        if (current_anim != anim2 || blend_anim != anim3) {
            frame = lbl_8047D038;
            if (blend_anim == -1) {
                frame = GSmodelGetAnimFrame(model);
            }
            GSmodelSetAnimBlend(model, anim2, anim3);
            GSmodelGetFrameCount(model, &frame_count_a, &frame_count_b);
            GSmodelSetAnimFrame(model,
                                frame * (frame_count_b / frame_count_a));
        }
        amount = amount - lbl_8047D080;
        GSmodelSetBlendFactor(model, amount);
        GSmodelSetAnimRate(model, lbl_8047D040);
    }
}
