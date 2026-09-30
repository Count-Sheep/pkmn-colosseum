/**
 * @file camera_exact_80179A18.c
 * @brief cameraSetFloorDefault and cameraInit, 0x80179A18 - 0x80179DFC.
 *
 * Carve of the camera TU (whole-TU candidate: camera.c), next to
 * camera_exact_80179404.c. cameraInit runs at boot: it clears the camera
 * state (lbl_80452EC8, .bss owned by bss_80452EC8.c), registers the scene
 * camera as resource 0/0, targets 0/100, allocates the per-floor camera
 * table and registers the camera's save-state handlers.
 *
 * The unit owns .rodata 0x80273D98-0x80273DC8, the image of cameraInit's
 * four local initialisers (view, up, eye and the save-state handler table);
 * rodata_80273A00.c and rodata_80273DC8.c hold the rest of that block.
 * GSscene_SetMode(0), cameraSetTarget(0, 100) and
 * GSscene_SetCameraViewVector are inlined in place as retail expands them.
 * Built with GC/1.3.2, the camera TU's compiler: GC/1.3 lays out and copies
 * the initialisers differently (80%).
 */
#include "game/camera_types.h"
#include "game/gs_render_util.h"

typedef struct CameraStateHandlers {
    void (*restore)(void* data);
    void (*make)(void* data);
    u32 (*getSize)(void);
} CameraStateHandlers;

extern CameraPadState lbl_80452EC8;
u32 _cameraGetStateSize(void);
void _cameraMakeStateData(void* data);
void _cameraRestoreStateData(void* data);
extern u8* lbl_80478FBC;
extern const f32 lbl_8047D724;     /* 1.0f */

extern void clear__5GSvecFv(void* vector);
extern void _cameraLoadCameraMatrix__FP9_GScamera12GSgfxLayerID(void);
extern u32 _toolentryAlloc__FUl(u32 size);
extern void* fn_800E27B0(u16 handle);
extern void* fn_800D29A0(void);
extern void GSresRegisterResource(void* resource, u32 group, u32 id, u32 flags);
extern void fn_800FF4D4(void* data, u8 typeId);

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

void cameraSetFloorDefault(f32 height, f32 distance, f32 rotationY)
{
    CameraFloorEntry* floorEntry;

    floorEntry = cameraFindFloorEntry(fn_800FF56C());
    if (floorEntry == NULL) {
        return;
    }

    floorEntry->defaultHeight = height;
    floorEntry->defaultDistance = distance;
    floorEntry->defaultRotationY = rotationY;
    if (floorEntry->initialized != 1) {
        return;
    }

    floorEntry->initialized = 2;
    cameraSetHeightInline(height);
    cameraSetDistanceInline(distance);
    cameraSetRotYInline(rotationY);
}

void cameraInit(void)
{
    GSSceneVec3 view = { 0.0f, 14.0f, 0.0f };
    GSSceneVec3 up = { 0.0f, 1.0f, 0.0f };
    GSSceneVec3 eye = { 0.0f, 0.0f, 100.0f };
    CameraStateHandlers handlers = {
        _cameraRestoreStateData, _cameraMakeStateData, _cameraGetStateSize
    };
    void* camera;
    u32 i;
    u16 handle;

    memset(&lbl_80452EC8, 0, sizeof(CameraPadState));
    lbl_80478C40 = &lbl_80452EC8;
    camera = fn_800D29A0();
    GSresRegisterResource(camera, 0, 0, 0);

    if (((CameraPadState*)lbl_80478C40)->mode != 0) {
        ((CameraPadState*)lbl_80478C40)->mode = 0;
    }
    ((CameraPadState*)lbl_80478C40)->targetGroup = 0;
    ((CameraPadState*)lbl_80478C40)->targetId = 100;
    ((CameraPadState*)lbl_80478C40)->targetSubId = -1;
    GSvecCopy(&((CameraPadState*)lbl_80478C40)->view, &view);

    handle = _toolentryAlloc__FUl(*(u32*)lbl_80478FB8 *
                                  sizeof(CameraFloorEntry));
    lbl_8047B1AC = handle;
    lbl_8047B1A8 = fn_800E27B0(handle);
    memset(lbl_8047B1A8, 0, *(u32*)lbl_80478FB8 * sizeof(CameraFloorEntry));
    for (i = 0; i < *(u32*)lbl_80478FB8; i++) {
        ((CameraFloorEntry*)lbl_8047B1A8)[i].initialized = 0;
        ((CameraFloorEntry*)lbl_8047B1A8)[i].floor =
            *(void**)(lbl_80478FBC + 0x0C + i * 0x4C);
    }

    clear__5GSvecFv(&((CameraPadState*)lbl_80478C40)->offsetPosition);
    clear__5GSvecFv(&((CameraPadState*)lbl_80478C40)->offsetRotation);
    set__5GSvecFfff(&((CameraPadState*)lbl_80478C40)->offsetScale,
                    lbl_8047D724, lbl_8047D724, lbl_8047D724);
    ((CameraPadState*)lbl_80478C40)->fov = lbl_8047D720;
    fn_800FF4D4(&handlers, 1);
    fn_800FF4D4(&handlers, 2);
    GScameraLookAt((GSRenderCamera*)camera,
                   (const GSRenderVec3*)&up,
                   (const GSRenderVec3*)&view);
    GScameraSetPosition(camera, &eye);
    fn_800D258C(camera);
    _cameraLoadCameraMatrix__FP9_GScamera12GSgfxLayerID();
}
