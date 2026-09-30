/**
 * @file camera_exact_80179404.c
 * @brief cameraSetGScamera, cameraResetFloor and fn_80179748,
 *        0x80179404 - 0x80179A18.
 *
 * Carve of the camera TU (whole-TU candidate: camera.c; its header has the
 * TU's layout). On the title path floorInitMap calls cameraSetGScamera with
 * floor 900's map camera:
 * - cameraSetGScamera copies the camera's perspective, interest and eye
 *   into the active scene camera (resource 0/0), switches the scene camera
 *   to mode 3 (GSscene_SetMode(3), inlined in place), and passes the
 *   distance vector's height, depth, yaw (atan2) and the fov to fn_80179748;
 * - fn_80179748 sets the camera's height, distance, yaw and fov for the
 *   current floor. On a floor's first call it clamps them (height >= 0,
 *   distance 10..500, and a too-close distance becomes 30 at height 0) and
 *   records them as the floor's defaults. Later calls reuse the floor's
 *   current values;
 * - cameraResetFloor leaves pad mode 6 and restores the floor's defaults.
 *
 * Text only. The state pointer, the floor table and the camera TU's .sdata2
 * pool (0x8047D720-0x8047D790) stay with the rest of the TU. The camera's
 * setters (cameraSetHeight/Distance/RotY/Fov, linked from
 * camera_exact_801766A8.c) are inlined here as in retail.
 */
#include "game/camera_types.h"
#include "game/gs_render_util.h"
#include "crt/math_ppc.h"

void* GSresGetResource(u32 group, u32 id);
void fn_80179748(f32 height, f32 distance, f32 rotationY, f32 fov);

typedef struct CameraFloorEntry {
    s32 initialized;
    void* floor;
    f32 defaultHeight;
    f32 defaultDistance;
    f32 defaultRotationY;
    f32 defaultFov;
    f32 height;
    f32 distance;
    f32 rotationY;
    f32 fov;
} CameraFloorEntry;

/* RULE-EXCEPTION(title-path): extern named stand-ins for the camera TU's
 * own pool literals - see docs/RULE_EXCEPTIONS.md */
extern const f32 lbl_8047D720;     /* 30.0f */
extern const f32 lbl_8047D728;     /* fov minimum */
extern const f32 lbl_8047D72C;     /* fov maximum */
extern const f32 lbl_8047D740;     /* 0.0f */
extern const f32 lbl_8047D774;     /* 10.0f */
extern const f32 lbl_8047D778;     /* 500.0f */

static inline CameraFloorEntry* cameraFindFloorEntry(void* floor)
{
    CameraFloorEntry* entries = (CameraFloorEntry*) lbl_8047B1A8;
    u32 i;

    for (i = 0; i < *(u32*) lbl_80478FB8; i++) {
        if (floor == entries[i].floor) {
            return &entries[i];
        }
    }
    return 0;
}

/* RULE-EXCEPTION(title-path): inline copies of the linked setters
 * (camera_exact_801766A8.c), which the whole camera TU inlines here - see
 * docs/RULE_EXCEPTIONS.md */
static inline void cameraSetFovInline(f32 fov)
{
    CameraFloorEntry* floorEntry;

    if (fov < lbl_8047D728) {
        fov = lbl_8047D728;
    }
    if (fov > lbl_8047D72C) {
        fov = lbl_8047D72C;
    }

    floorEntry = cameraFindFloorEntry(fn_800FF56C());
    if (floorEntry != NULL) {
        floorEntry->fov = fov;
    }
    ((CameraPadState*) lbl_80478C40)->fov = fov;
}

static inline void cameraSetRotYInline(f32 angle)
{
    CameraFloorEntry* floorEntry;

    floorEntry = cameraFindFloorEntry(fn_800FF56C());
    if (floorEntry != NULL) {
        floorEntry->rotationY = angle;
    }
    ((CameraPadState*) lbl_80478C40)->rotation.y = angle;
}

static inline void cameraSetDistanceInline(f32 distance)
{
    CameraFloorEntry* floorEntry;

    floorEntry = cameraFindFloorEntry(fn_800FF56C());
    if (floorEntry != NULL) {
        floorEntry->distance = distance;
    }
    ((CameraPadState*) lbl_80478C40)->distance = distance;
}

static inline void cameraSetHeightInline(f32 height)
{
    CameraFloorEntry* floorEntry;

    floorEntry = cameraFindFloorEntry(fn_800FF56C());
    if (floorEntry != NULL) {
        floorEntry->height = height;
    }
    ((CameraPadState*) lbl_80478C40)->height = height;
}

void cameraSetGScamera(void* camera)
{
    GSRenderCamera* source;
    GSRenderCamera* target;
    GSRenderVec3 dist;
    f32 perspective;
    f32 aspect;
    f32 near;
    f32 far;
    CameraPadState* state;

    if (camera == 0) {
        return;
    }

    source = (GSRenderCamera*)camera;
    target = (GSRenderCamera*)GSresGetResource(0, 0);

    GScameraGetPerspective(camera, &perspective, &aspect, &near, &far);
    GScameraSetPerspective(target, perspective, aspect, near, far);
    GScameraGetDistanceVector(camera, &dist);

    state = (CameraPadState*)lbl_80478C40;
    if (state->mode != 3) {
        state->mode = 3;
    }

    target->interest = source->interest;
    target->eye = source->eye;
    fn_80179748(dist.y, dist.z, (f32)atan2(dist.x, dist.z), perspective);
}

void cameraResetFloor(void)
{
    CameraFloorEntry* defaults;
    CameraPadState* state;

    state = (CameraPadState*) lbl_80478C40;
    if (state->mode == 6) {
        if (state->mode != 0) {
            state->mode = 0;
        }
    }

    defaults = cameraFindFloorEntry(fn_800FF56C());
    if (defaults == NULL) {
        return;
    }
    cameraSetHeightInline(defaults->defaultHeight);
    cameraSetDistanceInline(defaults->defaultDistance);
    cameraSetRotYInline(defaults->defaultRotationY);
    cameraSetFovInline(defaults->defaultFov);
}

void fn_80179748(f32 height, f32 distance, f32 rotationY, f32 fov)
{
    CameraFloorEntry* floorEntry;

    floorEntry = cameraFindFloorEntry(fn_800FF56C());
    if (floorEntry != 0) {
        if (floorEntry->initialized != 0) {
            height = floorEntry->height;
            distance = floorEntry->distance;
            rotationY = floorEntry->rotationY;
            fov = floorEntry->fov;
        } else {
            if (height < lbl_8047D740) {
                height = lbl_8047D740;
            }
            if (distance < lbl_8047D774) {
                distance = lbl_8047D720;
                /* RULE-EXCEPTION(title-path): no-op cast so the zero is
                 * reloaded as retail's pool literal is - see
                 * docs/RULE_EXCEPTIONS.md */
                height = *(const f32*)&lbl_8047D740;
            } else if (distance > lbl_8047D778) {
                distance = lbl_8047D778;
            }
            floorEntry->initialized = 1;
            floorEntry->defaultHeight = height;
            floorEntry->defaultDistance = distance;
            floorEntry->defaultRotationY = rotationY;
            floorEntry->defaultFov = fov;
        }
    }

    cameraSetHeightInline(height);
    cameraSetDistanceInline(distance);
    cameraSetRotYInline(rotationY);
    cameraSetFovInline(fov);
}
