/** Candidate-only owner for 0x801D0C30 - 0x801D1338. */
#include "dolphin/types.h"

extern u8 fn_801D0AA0(u32 index);
extern u16 fn_801D0AFC(s32 mode);

void fn_801D0C30(void)
{
    extern f32 lbl_8047E184;
    extern void _threadSwitch(void);
    extern void fadeSet(f32 duration, s32 mode);
    extern s8 fadeCheck(s32 wait);
    extern u32 fn_8016557C(void);
    extern void fn_80165548(u32 handle);
    extern void fn_801653CC(s32 id, s32 fade, s32 volume);
    s32 running;
    s32 state;
    s32 timer;
    u32 handle;

    running = 1;
    state = 0;
    timer = 0;
    do {
        switch (state) {
        case 0:
            timer = 0;
            fn_801D0AFC(1);
            state = 10;
            break;
        case 10:
            fadeSet(lbl_8047E184, 3);
            state = 12;
            handle = fn_8016557C();
            break;
        case 12:
            if (fadeCheck(0) == 0) {
                state = 6;
            } else {
                _threadSwitch();
            }
            break;
        case 6:
            timer++;
            if (timer >= 10) {
                state = 8;
                fn_801653CC(0x19, 2000, 0xFF);
                timer = 0;
            } else {
                _threadSwitch();
            }
            break;
        case 8:
            timer++;
            if (timer >= 10) {
                state = 9;
            } else {
                _threadSwitch();
            }
            break;
        case 9:
            fn_80165548(handle);
            fadeSet(lbl_8047E184, 2);
            state = 11;
            break;
        case 11:
            if (fadeCheck(0) == 0) {
                state = 1000;
            } else {
                _threadSwitch();
            }
            break;
        case 1000:
            running = 0;
            break;
        }
    } while (running != 0);
}

typedef struct BattleIntroVec {
    f32 x;
    f32 y;
    f32 z;
} BattleIntroVec;

typedef struct BattleIntroModelMap {
    s32 fieldId;
    u32 modelResId;
} BattleIntroModelMap;

/* Retail keeps these four tables at 0x8036E030 (.data), in this order. */
static u32 battleIntroObjectIds[13] = {
    0x03640400, 0x03650400, 0x03640400, 0x036B0400, 0x03360400,
    0x036A0400, 0x03660400, 0x03400400, 0x03670400, 0x03690400,
    0x036C0400, 0x03630400, 0x03680400,
};
static u32 battleIntroPartIds[6] = {9, 10, 11, 12, 13, 14};
static s16 battleIntroAnims[6] = {1, 2, 3, 4, 5, 6};
static BattleIntroModelMap battleIntroModels[11] = {
    {0x20, 0x00651003},
    {0x37, 0x00A31009},
    {0x6E, 0x002B1004},
    {0x89, 0x00511004},
    {0x94, 0x02371003},
    {0x41, 0x00C21005},
    {0x49, 0x01DC1003},
    {0x5E, 0x02E41006},
    {0x5F, 0x02E51005},
    {0x3B, 0x00931003},
    {0x633, 0x10BB1005},
};
extern void* lbl_80467378[6];
extern f32 lbl_8047B3E0;
extern const f32 lbl_8047E188;
extern const f32 lbl_8047E18C;
extern const f32 lbl_8047E190;
extern const f32 lbl_8047E194;
extern const f32 lbl_8047E198;
extern u32 lbl_8047E180;
extern BattleIntroVec lbl_80279320;
extern BattleIntroVec lbl_8027932C;

extern s32 fn_80113F48(void);
extern void* fn_800F92D4(u32 id);
extern void* fn_8018D998(s32 group, s32 id);
extern void* peopleSearchID(void* id);
extern BattleIntroVec* peopleGetPosition(void* person);
extern void* fn_8018FCBC(void* person);
extern void fn_800E0168(BattleIntroVec* dst, BattleIntroVec* a, void* b);
extern void fn_8018805C(s32 group, s32 id, f32 angle, f32 speed);
extern double atan2(double y, double x);
extern void peopleMoveCheck(s32 group, s32 id, s32 wait);
extern void* GSmodelGetPart(void* model, u32 part);
extern void GSpartGetTransform(void* part, BattleIntroVec* pos, void* rot,
                               void* scale);
extern void GSpartFree(void* part);
extern void* floorOpenObject(u32 id);
extern void GSmodelSetPosition(void* model, BattleIntroVec* pos);
extern void GSmodelSetScale(void* model, BattleIntroVec* scale);
extern void GSmodelEnableModulation(void* model, u32* color);
extern void GSmodelSetAnimIndex(void* model, s32 index);
extern void GSmodelSetAnimFrame(void* model, f32 frame);
extern void GSmodelSetAnimRate(void* model, f32 rate);
extern void GSmodelSetAnimType(void* model, s32 type);
extern void GSmodelStartAnimation(void* model);
extern u8 GSmodelIsAnimating(void* model);
extern void GSmodelSetVisibility(void* model, s32 visible);
extern void GSmodelFree(void* model);
extern u32 fn_801662E8(u32 arg0, u32 arg1);
extern void fn_801668DC(u32 handle, u32 volume, u32 arg2);
extern void fn_80165548(u32 handle);
extern void fn_801653CC(s32 id, s32 fade, s32 volume);
extern void fn_80166AB8(s32 id, s32 pan, s32 volume);
extern s32 fn_800D37CC(void);
extern u32 fn_800D3088(void);
extern void _threadSwitch(void);

static inline u32 battleIntroFindModel(s32 fieldId, BattleIntroModelMap* map)
{
    s32 i;

    for (i = 0; i < 11; i++, map++) {
        if (map->fieldId == fieldId) {
            return map->modelResId;
        }
    }
    return 0;
}

static inline u32 battleIntroPlaySe(u32 id, u32 volume)
{
    u32 se = fn_801662E8(0, id);

    if (se != -1) {
        fn_801668DC(se, volume, 0);
    }
    return se;
}

/* RULE-EXCEPTION(user-approved): this inlined pointer iterator reproduces
 * retail's teardown register allocation; the original helper is unproven.
 */
static inline void battleIntroFreeObjects(void** objects)
{
    s32 index;

    for (index = 0; index < 6; index++, objects++) {
        if (*objects != NULL) {
            GSmodelSetVisibility(*objects, 0);
            GSmodelFree(*objects);
        }
    }
}

void fn_801D0DB0(s32 peopleGroup, s32 peopleId)
{
    s32 count;
    void* model;
    u8 hadPerson;
    s32 state;
    s32 running;
    s32 i;
    u32 handle;
    BattleIntroVec pos;
    BattleIntroVec scale;
    BattleIntroVec personPos;
    BattleIntroVec delta;
    u32 color;
    void* part;
    void* person;
    s32 j;
    s16 anim;
    f32 timer;
    f32 frameDelta;

    pos = lbl_80279320;
    scale = lbl_8027932C;
    color = lbl_8047E180;
    running = 1;
    state = 0;
    timer = lbl_8047E188;
    do {
        switch (state) {
        case 0:
            i = 0;
            model = fn_800F92D4(battleIntroFindModel(fn_80113F48(), battleIntroModels));
            count = fn_801D0AFC(0);
            for (j = 0; j < 6; j++) {
                lbl_80467378[j] = NULL;
            }
            person = peopleSearchID(fn_8018D998(peopleGroup, peopleId));
            if (person != NULL) {
                hadPerson = 1;
                part = GSmodelGetPart(model, 0xE);
                GSpartGetTransform(part, &pos, NULL, NULL);
                GSpartFree(part);
                personPos = *peopleGetPosition(person);
                fn_800E0168(&delta, &pos, fn_8018FCBC(person));
                fn_8018805C(peopleGroup, peopleId, atan2(delta.x, delta.z),
                            lbl_8047E18C);
                state = 1;
            } else {
                hadPerson = 0;
                state = 3;
            }
            break;
        case 1:
            peopleMoveCheck(peopleGroup, peopleId, 1);
            state = 3;
            break;
        case 3:
            handle = battleIntroPlaySe(0x406, 0x1068);
            timer = lbl_8047E188;
            part = GSmodelGetPart(model, battleIntroPartIds[i]);
            GSpartGetTransform(part, &pos, NULL, NULL);
            GSpartFree(part);
            lbl_80467378[i] = floorOpenObject(battleIntroObjectIds[fn_801D0AA0((u16)i)]);
            GSmodelSetPosition(lbl_80467378[i], &pos);
            GSmodelSetScale(lbl_80467378[i], &scale);
            GSmodelEnableModulation(lbl_80467378[i], &color);
            fn_80166AB8(0x3C3, 0, 0);
            state = 4;
            break;
        case 4:
            frameDelta = (f32)fn_800D3088() / (f32)fn_800D37CC();
            timer += frameDelta;
            lbl_8047B3E0 = frameDelta;
            if (timer >= lbl_8047E190) {
                i++;
                if (i == count) {
                    state = 5;
                } else {
                    state = 3;
                }
            } else {
                _threadSwitch();
            }
            break;
        case 5:
            anim = battleIntroAnims[count - 1];
            if (model != NULL) {
                GSmodelSetAnimIndex(model, anim);
                GSmodelSetAnimFrame(model, lbl_8047E188);
                GSmodelSetAnimRate(model, lbl_8047E194);
                GSmodelSetAnimType(model, 0);
                GSmodelStartAnimation(model);
            }
            fn_801653CC(0x19, 2000, 0xFF);
            state = 6;
            break;
        case 6:
            while (GSmodelIsAnimating(model)) {
                _threadSwitch();
            }
            timer = lbl_8047E188;
            while (timer < lbl_8047E198) {
                frameDelta = (f32)fn_800D3088() / (f32)fn_800D37CC();
                timer += frameDelta;
                lbl_8047B3E0 = frameDelta;
                _threadSwitch();
            }
            state = 7;
            break;
        case 7:
            battleIntroFreeObjects(lbl_80467378);
            timer = lbl_8047E188;
            while (timer < lbl_8047E198) {
                frameDelta = (f32)fn_800D3088() / (f32)fn_800D37CC();
                timer += frameDelta;
                lbl_8047B3E0 = frameDelta;
                _threadSwitch();
            }
            fn_80165548(handle);
            state = 8;
            break;
        case 8:
            if (hadPerson) {
                peopleSearchID(fn_8018D998(peopleGroup, peopleId));
                fn_8018805C(peopleGroup, peopleId, personPos.y, lbl_8047E18C);
                state = 2;
            } else {
                state = 1000;
            }
            break;
        case 2:
            peopleMoveCheck(peopleGroup, peopleId, 1);
            state = 1000;
            break;
        case 1000:
            running = 0;
            break;
        }
    } while (running);
}
