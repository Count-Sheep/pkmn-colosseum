/**
 * camera.c range 0x80176C78 - 0x80176F68: cameraPlayOffsetAnime and
 * cameraPlayAnime, as a standalone source covering exactly the split.
 *
 * Both functions are text-exact. The unit stays CodeCandidate because the
 * (f32)frame conversion needs the signed int-to-float bias, which retail
 * keeps once for the whole camera TU at lbl_8047D738, inside the camera
 * TU's .sdata2 pool 0x8047D720-0x8047D790 (today the data unit
 * game/data/sdata2_8047D720.c). cameraUpdate, _cameraPadRotateUpdate and
 * _cameraPadMoveUpdate use the same constant, and the pool also holds
 * cameraPlayAnime's 0.5f animation rate at 0x8047D730, so only a unit
 * compiling the whole camera TU (.text 0x801765F4-0x80179F4C; the -O0-style
 * fn_80179F4C/fn_80179FA4 after it belong to another unit) can own it.
 * That needs
 * cameraUpdate (99.8%: stack-slot order, a branch shape, jump-table address
 * hoisting), _cameraPadRotateUpdate (99.1%), _cameraPadMoveUpdate (99.8%)
 * and cameraInit (98.5%) exact first.
 */

#include "game/camera_types.h"
#include "game/data/sdata2_8047D690.h"
#include "game/gs_render_util.h"

void GScameraSetAnimIndex(void* camera, s32 index);
void GScameraSetAnimFrame(void* camera, f32 frame);
void GScameraSetAnimRate(void* camera, f32 rate);
void GScameraStartAnimation(void* camera);
void fn_800D1858(void* camera, s32 loop);
void clear__5GSvecFv(void* vector);
extern const f32 lbl_8047D730;

/*
 * Same body as GSscene_SetMode (0x80177A44). Both players expand it inline:
 * the already-in-mode path returns the requested mode as a constant instead
 * of the value it just read.
 */
static inline u32 cameraSetMode(u32 mode)
{
    CameraPadState* state = (CameraPadState*)lbl_80478C40;
    u32 previous;

    if (state->mode == (u8)mode) {
        return mode;
    }
    previous = state->mode;
    state->mode = (u8)mode;
    return previous;
}

void cameraPlayOffsetAnime(u32 groupId, u32 animationId, s32 frame, u8 loop)
{
    CameraPadState* state;
    void* animation;
    u8 previousMode;
    CameraPadState* current;
    void* camera;

    state = lbl_80478C40;
    if (state->animationGroup != 0 || state->animationId != 0) {
        animation = GSresGetResource(state->animationGroup, state->animationId);
        if (animation == NULL) {
            animation = fn_800F92D4(state->animationId);
        }
        ((CameraPadState*)lbl_80478C40)->animationGroup = 0;
        ((CameraPadState*)lbl_80478C40)->animationId = 0;
        if (animation != NULL) {
            GScameraStopAnimation(animation);
        }
    }

    previousMode = cameraSetMode(8);
    ((CameraPadState*)lbl_80478C40)->animationGroup = groupId;
    ((CameraPadState*)lbl_80478C40)->animationId = animationId;
    ((CameraPadState*)lbl_80478C40)->flags[1] = previousMode;

    current = lbl_80478C40;
    animation = GSresGetResource(current->animationGroup, current->animationId);
    if (animation == NULL) {
        animation = fn_800F92D4(current->animationId);
    }
    if (animation == NULL) {
        return;
    }
    GScameraSetAnimIndex(animation, 0);
    if (loop != 0) {
        fn_800D1858(animation, 1);
    } else {
        fn_800D1858(animation, 0);
    }
    GScameraSetAnimRate(animation, lbl_8047D730);
    GScameraSetAnimFrame(animation, (f32)frame);
    GScameraStartAnimation(animation);
    clear__5GSvecFv(&((CameraPadState*)lbl_80478C40)->offsetPosition);
    clear__5GSvecFv(&((CameraPadState*)lbl_80478C40)->offsetRotation);
    set__5GSvecFfff(&((CameraPadState*)lbl_80478C40)->offsetScale,
                    lbl_8047D724, lbl_8047D724, lbl_8047D724);
    camera = GSresGetResource(0, 0);
    fn_800D258C(camera);
}

void cameraPlayAnime(u32 groupId, u32 animationId, s32 frame, u8 loop)
{
    CameraPadState* state;
    void* animation;
    u8 previousMode;
    CameraPadState* current;

    state = lbl_80478C40;
    if (state->animationGroup != 0 || state->animationId != 0) {
        animation = GSresGetResource(state->animationGroup, state->animationId);
        if (animation == NULL) {
            animation = fn_800F92D4(state->animationId);
        }
        ((CameraPadState*)lbl_80478C40)->animationGroup = 0;
        ((CameraPadState*)lbl_80478C40)->animationId = 0;
        if (animation != NULL) {
            GScameraStopAnimation(animation);
        }
    }

    previousMode = cameraSetMode(4);
    ((CameraPadState*)lbl_80478C40)->animationGroup = groupId;
    ((CameraPadState*)lbl_80478C40)->animationId = animationId;
    ((CameraPadState*)lbl_80478C40)->flags[1] = previousMode;

    current = lbl_80478C40;
    animation = GSresGetResource(current->animationGroup, current->animationId);
    if (animation == NULL) {
        animation = fn_800F92D4(current->animationId);
    }
    if (animation == NULL) {
        return;
    }
    GScameraSetAnimIndex(animation, 0);
    if (loop != 0) {
        fn_800D1858(animation, 1);
    } else {
        fn_800D1858(animation, 0);
    }
    GScameraSetAnimRate(animation, lbl_8047D730);
    GScameraSetAnimFrame(animation, (f32)frame);
    GScameraStartAnimation(animation);
    fn_800D258C(animation);
}
