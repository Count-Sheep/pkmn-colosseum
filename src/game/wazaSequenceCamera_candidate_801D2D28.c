/* Waza camera target and field-of-view update, 0x801D2D28-0x801D2F94.
 * Owns the 0.2f and unsigned conversion literals at 0x8047E1E0-0x8047E1F0.
 * GC/1.3 with the camera TU's normal flags; no compiler-control pragmas.
 */
#include "game/battle/waza_camera_stop.h"
#include "dolphin/mtx.h"

typedef struct CameraFovKey {
    f32 start;
    f32 end;
    u32 startFrame;
    u32 endFrame;
} CameraFovKey;

typedef struct WazaCameraMove {
    u8 pad00[0x4C];
    u32 targetParts[3];
    u8 pad58[0x7C];
} WazaCameraMove;

typedef struct WazaCameraSequence {
    u8 pad00[0x24];
    void* model;
    u8 pad28[4];
    WazaCameraMove* moves;
    u16 pad30;
    u16 moveIndex;
} WazaCameraSequence;

extern CameraFovKey lbl_804673D4[];
extern u8 lbl_8047B3F4;
extern u32 lbl_8047B3E8;
extern f32 lbl_80478CDC;
extern u32 fn_800D3088(void);
extern void* GSmodelGetPart(void*, u32);
extern void GSpartGetTransform(void*, Vec*, void*, void*);
extern void GSpartFree(void*);
extern void GSmodelGetPosition(void*, Vec*);
extern void GSscene_GetCameraPositionVector(Vec*);
extern void fn_800E0168(Vec*, Vec*, Vec*);
extern void cameraMoveTargetOfs(s32, Vec*, f32);
extern f32 GSlerpGetLinearInterpolationFloat(f32, f32, f32);
extern void cameraSetFov(f32);
extern u32 cameraMoveEndCheckSpecial(u8);

/* RULE-EXCEPTION(title-path): named shared pool literal, also referenced by
 * _wazaSequenceCameraDoFOV; see docs/RULE_EXCEPTIONS.md. */
#pragma section ".sdata2"
__declspec(section ".sdata2") f32 lbl_8047E1E0 = 0.2f;

void fn_801D2D28(void)
{
    CameraFovKey* key = lbl_804673D4;
    WazaCameraMove* move;
    void* model;
    Vec target;
    Vec origin;
    void* part;
    f32 t;
    u32 frame;
    u32 partId;
    u32 i;

    if (lbl_8047B3F4 != 0 && lbl_8047B3EC != NULL &&
        (lbl_8047B3F0 == NULL || *(u32*)((u8*)lbl_8047B3F0 + 0x18) == 0 ||
         *(u32*)((u8*)lbl_8047B3F0 + 0x20) == 0)) {
        frame = fn_800D3088();
        move = &((WazaCameraSequence*)lbl_8047B3EC)->moves[
            ((WazaCameraSequence*)lbl_8047B3EC)->moveIndex];
        model = ((WazaCameraSequence*)lbl_8047B3EC)->model;
        if (lbl_8047B3F0 != NULL) {
            partId = move->targetParts[((u8*)lbl_8047B3F0)[0x17]];
        } else {
            partId = move->targetParts[2];
        }
        part = GSmodelGetPart(model, partId);
        if (part != NULL) {
            GSpartGetTransform(part, &target, NULL, NULL);
            if (lbl_8047B3F0 != NULL && *(u16*)((u8*)lbl_8047B3F0 + 0x2E) == 3 &&
                (*(u16*)((u8*)lbl_8047B3F0 + 0x2C) == 0x154 ||
                 *(u16*)((u8*)lbl_8047B3F0 + 0x2C) == 0x6E)) {
                GSscene_GetCameraPositionVector(&origin);
            } else {
                GSmodelGetPosition(*(void**)((u8*)lbl_8047B3EC + 0x24), &origin);
            }
            fn_800E0168(&target, &target, &origin);
            cameraMoveTargetOfs(7, &target, lbl_8047E1E0);
            GSpartFree(part);
        }

        lbl_8047B3E8 += frame;
        for (i = 0; i < 2; i++, key++) {
            if (lbl_8047B3E8 <= key->startFrame) {
                lbl_80478CDC = key->start;
                break;
            }
            if (lbl_8047B3E8 <= key->endFrame) {
                t = (f32)(lbl_8047B3E8 - key->startFrame) /
                    (f32)(key->endFrame - key->startFrame);
                lbl_80478CDC = GSlerpGetLinearInterpolationFloat(key->start, key->end, t);
                break;
            }
            lbl_80478CDC = key->end;
        }
        cameraSetFov(lbl_80478CDC);

        if (lbl_8047B3F0 == NULL && (u8)cameraMoveEndCheckSpecial(0) == 0) {
            /* RULE-EXCEPTION(title-path): redundant status temporary preserves
             * retail's li/cmpwi/bne; see docs/RULE_EXCEPTIONS.md. */
            s32 moving = 0;
            if (moving == 0) {
                wazaCameraStop();
            }
        }
    }
}
