/**
 * Title floor (floor 900) coroutine, 0x8001FD48 - 0x80020328.
 *
 * Despite the historical split name, this is the first function of the
 * GStitle translation unit that continues at 0x80020328 (game/gs_title.c):
 * it owns the title state word lbl_8047A31C that fn_800203B4 polls, fills the
 * title model/camera globals (lbl_8047A324/33C/340) that fn_800203B4 reads,
 * and finishes by linking to floor 0x3A1. Like the other gs_title units it
 * is built with -opt nopeephole (the retail code keeps unpeepholed
 * `mr r4,r3; mr r30,r4` copy chains and dead loop-entry branches).
 */

#include "dolphin/types.h"
#include "game/menu/menu.h"

typedef struct GSmodel GSmodel;

extern GSmodel* fn_800F92D4(u32 resourceId);
extern void GSmodelSetAnimIndex(GSmodel* model, s32 index);
extern void GSmodelSetAnimFrame(GSmodel* model, f32 frame);
extern void GSmodelSetAnimRate(GSmodel* model, f32 rate);
extern void GSmodelStartAnimation(GSmodel* model);
extern void GSmodelSetAnimType(GSmodel* model, s32 type);
extern u8 GSmodelIsAnimating(GSmodel* model);
extern void GSmodelAllPauseAnimation(void);
extern void GSmodelAllUnpauseAnimation(void);
extern void cameraStartAnimation(void);
extern void cameraStopAnimation(void);
extern void cameraPlayAnime(u32 groupId, u32 animationId, s32 frame, u8 loop);
extern void fn_801662E8(s32 a, s32 b);
extern void fn_80165A20(s32 id, s32 a, s32 volume);
extern void fn_80165548(void);
extern void fn_8016557C(void);
extern s32 fn_8017B1AC(void);
extern void _threadSwitch(void);
extern s32 menuOpen(s32 id, s32 arg);
extern void fadeSet(s32 mode, f32 time);
extern s8 fadeCheck(s32 wait);
extern s32 fn_801D0748(s32 a, s32 b, s32 c);
extern void savedataCreate(s32 a, s32 b);
extern u32 gamedatasaveGetStatus(s32 a, s32 b);
extern void fn_800216E0(u32 value);
extern void floorLink(s32 floorId, s32 arg);

extern s32 lbl_8047A310;      /* title mode */
extern s32 lbl_8047A314;      /* paused while fn_8017B1AC reports 5 */
extern s32 lbl_8047A318;      /* paused while fn_8017B1AC reports 4/11 */
extern s32 lbl_8047A31C;      /* title state */
extern s32 lbl_8047A320;      /* set when title mode 1 starts at state 7 */
extern GSmodel* lbl_8047A324; /* title model */
extern s32 lbl_8047A328;
extern s32 lbl_8047A33C;
extern f32 lbl_8047A340;
extern f32 lbl_8047A344;
extern f32 lbl_8047A348;
extern const f32 lbl_8047B810; /* 0.5f */
extern const f32 lbl_8047B814; /* 0.0f */
extern const f32 lbl_8047B818; /* 1.0f */

/* Expanded four times below with only the animation index changing. */
static inline void titleModelPlay(GSmodel* model, s32 index)
{
    if (model != NULL) {
        GSmodelSetAnimIndex(model, index);
        GSmodelSetAnimFrame(model, lbl_8047B814);
        GSmodelSetAnimRate(model, lbl_8047B810);
        GSmodelStartAnimation(model);
    }
}

/*
 * Guarded wait for a model animation. The early return survives inlining as
 * `bne loop; b done; b loop`; a plain `if (model != NULL)` compiles to a
 * single beq. floor_event carries the same expansion four times
 * (0x801165A4, 0x80116724, 0x80116750, 0x80116CC4).
 */
static inline void modelWaitAnimation(GSmodel* model)
{
    if (model == NULL) {
        return;
    }
    while (GSmodelIsAnimating(model) != 0) {
        _threadSwitch();
    }
}

void fn_8001FD48(void)
{
    s32 running;
    GSmodel* model;
    s32 status;

    running = 1;
    lbl_8047A31C = 0;
    lbl_8047A348 = lbl_8047B814;
    model = fn_800F92D4(0x0B541000);
    lbl_8047A340 = lbl_8047B814;
    lbl_8047A324 = model;
    lbl_8047A33C = 0;
    lbl_8047A328 = 0;
    fn_801662E8(0, 0x406);
    fn_80165A20(0x46A, 0, 0xFF);

    if (lbl_8047A310 == 1) {
        lbl_8047A31C = 7;
        lbl_8047A310 = 0;
        lbl_8047A320 = 1;
    } else {
        lbl_8047A320 = 0;
    }

    menuOpen(0x15, 0);

    do {
        status = fn_8017B1AC();
        if (status == 11 || status == 4) {
            if (lbl_8047A318 == 0) {
                GSmodelAllPauseAnimation();
                cameraStopAnimation();
                fn_8016557C();
                lbl_8047A318 = 1;
            }
            _threadSwitch();
            continue;
        }
        if (lbl_8047A318 == 1) {
            cameraStartAnimation();
            fn_801662E8(0, 0x406);
            fn_80165548();
            GSmodelAllUnpauseAnimation();
            lbl_8047A318 = 0;
        }

        if (status == 5) {
            if (lbl_8047A314 == 0) {
                GSmodelAllPauseAnimation();
                cameraStopAnimation();
                fn_8016557C();
                lbl_8047A314 = 1;
            }
            _threadSwitch();
            continue;
        }
        if (lbl_8047A314 == 1) {
            cameraStartAnimation();
            fn_801662E8(0, 0x406);
            fn_80165548();
            GSmodelAllUnpauseAnimation();
            lbl_8047A314 = 0;
        }

        switch (lbl_8047A31C) {
        case 0:
            titleModelPlay(model, 0);
            GSmodelSetAnimType(model, 0);
            cameraPlayAnime(0x12, 0x0B551800, 0, 0);
            fadeSet(4, lbl_8047B810);
            if (lbl_8047A31C == 0) {
                lbl_8047A31C = 1;
            }
            break;
        case 1:
            if (GSmodelIsAnimating(model) == 0) {
                lbl_8047A31C = 2;
                break;
            }
            _threadSwitch();
            break;
        case 2:
            titleModelPlay(model, 1);
            GSmodelSetAnimType(model, 0);
            lbl_8047A31C = 3;
            break;
        case 3:
            modelWaitAnimation(model);
            lbl_8047A31C = 8;
            break;
        case 4:
            fadeSet(5, lbl_8047B810);
            lbl_8047A31C = 6;
            break;
        case 5:
            if (fadeCheck(0) == 0) {
                lbl_8047A31C = 6;
                break;
            }
            _threadSwitch();
            break;
        case 6:
            while (lbl_8047A348 < lbl_8047B810) {
                lbl_8047A348 += lbl_8047A344;
                _threadSwitch();
            }
            lbl_8047A31C = 7;
            break;
        case 7:
            cameraPlayAnime(0x12, 0x0B561800, 0, 0);
            titleModelPlay(model, 2);
            GSmodelSetAnimType(model, 1);
            fadeSet(4, lbl_8047B810);
            fadeCheck(1);
            lbl_8047A31C = 0x1E;
            break;
        case 8:
            cameraPlayAnime(0x12, 0x0B561800, 0, 0);
            titleModelPlay(model, 2);
            GSmodelSetAnimType(model, 1);
            lbl_8047A31C = 0x1E;
            break;
        case 0x28:
            lbl_8047A31C = 0x3E8;
            break;
        case 0x1E:
            lbl_8047A31C = 0x20;
            break;
        case 0x20:
            if (menuOpenCustom(MENU_ID(0x13), windowGetActiveID(), NULL, 0,
                               MENU_CURSOR_CHECK(1), 0) < 0) {
                break;
            }
            lbl_8047A31C = 0x1F;
            lbl_8047A348 = lbl_8047B814;
            break;
        case 0x1F:
            while (lbl_8047A348 < lbl_8047B818) {
                lbl_8047A348 += lbl_8047A344;
                _threadSwitch();
            }
            lbl_8047A31C = 0xC8;
            break;
        case 0xC8:
            status = fn_801D0748(1, 2, 0);
            if (status == -1) {
                lbl_8047A31C = 0x20;
                break;
            }
            if (status != 3) {
                savedataCreate(0, 0);
            }
            if (gamedatasaveGetStatus(0, 4) != 0) {
                fn_800216E0(1);
            } else {
                fn_800216E0(0);
            }
            lbl_8047A31C = 0x29;
            break;
        case 0x29:
            floorLink(0x3A1, 0);
            lbl_8047A31C = 0x3E8;
            break;
        case 0xC9:
            break;
        case 0x3E8:
            running = 0;
            break;
        }
    } while (running != 0);
}
