/**
 * @file wazaSequenceCamera.c
 * @brief wazaSequenceCamera / battleCamera: camera control during move animation
 * playback (front block is battleCamera code -- kept in this file per
 * split spec, not sub-split further).
 *
 * Split from the former game/battle/battle_waza.c CodeCandidate bucket
 * (0x801D1470-0x801DE698); see config/GC6E01/splits.txt for the exact
 * address range of this translation unit. Shared typedefs and cross-TU
 * forward declarations live in include/game/battle/battle_waza_types.h.
 */

#include "game/battle/battle_waza_types.h"
#include "crt/math_ppc.h"
#include "dolphin/mtx.h"

/* Camera parameters computed per move (lbl_804673A0, 0x34 bytes). */
typedef struct WazaSequenceCameraParams {
    s32 motion;         /* 0x00 */
    f32 size;           /* 0x04 */
    f32 height;         /* 0x08 */
    f32 rotationMin;    /* 0x0C */
    f32 rotationMax;    /* 0x10 */
    f32 rotationBase;   /* 0x14 */
    f32 heightMin;      /* 0x18 */
    f32 heightMax;      /* 0x1C */
    f32 distanceMin;    /* 0x20 */
    f32 distanceMax;    /* 0x24 */
    f32 nearLength;     /* 0x28 */
    f32 middleLength;   /* 0x2C */
    f32 farLength;      /* 0x30 */
} WazaSequenceCameraParams;


/**
 * wazaSequenceCameraGetPattern__Fbi - Waza multi-hit advance.
 * Address: 0x801D2B4C | Size: 0x120
 */
void* wazaSequenceCameraGetPattern__Fbi(u8 shortTable, s32 flags) {
    typedef struct WazaCameraPattern {
        f32 duration;
        u8 data[0x48];
    } WazaCameraPattern;
    extern f32 fn_800E0BE4(u8, s32);
    extern u32 _fadeEffectGetRandom__FUl(u32);
    extern WazaCameraPattern lbl_80371F60[];
    extern WazaCameraPattern lbl_803721C0[];
    WazaCameraPattern* table;
    s32 count;
    s32 i;
    f32 frame = fn_800E0BE4(shortTable, flags);
    f32 end = 0.0f;

    if (shortTable != 0) {
        table = lbl_80371F60;
        count = 8;
        if (flags & 0x20) {
            return &table[4];
        }
        if (flags & 0x40) {
            return &table[5];
        }
        if (flags & 0x80) {
            return &table[6];
        }
    } else {
        table = lbl_803721C0;
        count = 13;
        if (flags & 0x20) {
            return &table[9];
        }
        if (flags & 0x40) {
            return &table[10];
        }
        if (flags & 0x80) {
            return &table[11];
        }
    }

    for (i = 0; i < count; i++, table++) {
        end += table->duration;
        if (frame < end) {
            return table;
        }
    }

    if (shortTable != 0) {
        return &lbl_80371F60[_fadeEffectGetRandom__FUl(8)];
    }
    return &lbl_803721C0[_fadeEffectGetRandom__FUl(13)];
}

/**
 * fn_801D2C6C - Waza get global state from SDA.
 * Address: 0x801D2C6C | Size: 0x8
 */
extern void* lbl_8047B3EC;
extern void* lbl_8047B3F0;
extern u32 lbl_8047B3E8;
extern s32 lbl_8047B410;
extern f32 lbl_80478CDC;

extern u8 lbl_80467CC0[];
extern void fn_801DD158(void* obj);
extern void fn_801DF1D0(void* obj);
extern void fn_801DD3E4(void* obj);
extern void fn_801DD23C(void* obj);
extern void _threadSwitch(void);
extern s32 fn_8017B2CC(s32 id);
extern void fn_800F915C(s32 id);
extern void fn_8017B1CC(s32 id);
void* fn_801D2C6C(void) {
    return lbl_8047B3EC;
}

/* =========================================================================
 * WAZA ANIMATION STATE MACHINES (0x801D2C74 - 0x801D7230)
 *
 * Large state machines that drive multi-step move animations.
 * These contain extensive float math and switch statements.
 * ========================================================================= */

/**
 * fn_801D2C74 - Waza animation pre-check.
 * Address: 0x801D2C74 | Size: 0xB4
 */
void fn_801D2C74(void* owner) {
    extern void GSscene_SetMode(s32 arg);
    extern void cameraStopAnime(void* arg);
    extern void fn_801765F4(s32 arg);
    extern s32  fn_800057A8(void);
    extern u8   lbl_8047B3F4;

    void* obj;

    if (lbl_8047B3F4 != 0) {
        obj = lbl_8047B3F0;
        if (obj == NULL) {
            if (lbl_8047B3EC != NULL) {
                if (obj == NULL) {
                    GSscene_SetMode(8);
                } else {
                    if (*(u32*)((u8*)obj + 0x18) != 0 && *(u32*)((u8*)obj + 0x20) != 0) {
                        cameraStopAnime(obj);
                    }
                    lbl_8047B3F0 = NULL;
                }
                fn_801765F4(0);
                lbl_8047B3EC = NULL;
                if (fn_800057A8() == 2) {
                    GSscene_SetMode(2);
                }
            }
            battleCameraStartWaza(owner, NULL);
        }
    }
}

/**
 * fn_801D2D28 - Waza animation setup from move data.
 * Address: 0x801D2D28 | Size: 0x26C
 */
void fn_801D2D28(void) {
    typedef struct CameraFovKey {
        f32 start;
        f32 end;
        u32 startFrame;
        u32 endFrame;
    } CameraFovKey;
    extern CameraFovKey lbl_804673D4[];
    extern u8 lbl_8047B3F4;
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
    extern BOOL cameraMoveEndCheckSpecial(s32);
    u8* sequence;
    u8* model;
    CameraFovKey* key;
    Vec target;
    Vec origin;
    void* part;
    f32 t;
    u32 frame;
    u32 partId;
    u32 i;

    if (lbl_8047B3F4 == 0 || lbl_8047B3EC == NULL) {
        return;
    }

    if ((u8*)lbl_8047B3F0 != NULL && *(u32*)((u8*)lbl_8047B3F0 + 0x18) != 0 &&
        *(u32*)((u8*)lbl_8047B3F0 + 0x20) != 0) {
        return;
    }

    frame = fn_800D3088();
    sequence = lbl_8047B3EC;
    model = *(u8**)(sequence + 0x24);
    key = (CameraFovKey*)(*(u8**)(sequence + 0x2C) +
                          *(u16*)(sequence + 0x32) * 0xD4);
    if ((u8*)lbl_8047B3F0 != NULL) {
        partId = *(u32*)((u8*)key + 0x4C + ((u8*)lbl_8047B3F0)[0x17] * 4);
    } else {
        partId = *(u32*)((u8*)key + 0x54);
    }
    part = GSmodelGetPart(model, partId);
    if (part != NULL) {
        GSpartGetTransform(part, &target, NULL, NULL);
        if ((u8*)lbl_8047B3F0 != NULL && *(u16*)((u8*)lbl_8047B3F0 + 0x2E) == 3 &&
            (*(u16*)((u8*)lbl_8047B3F0 + 0x2C) == 0x154 ||
             *(u16*)((u8*)lbl_8047B3F0 + 0x2C) == 0x6E)) {
            GSscene_GetCameraPositionVector(&origin);
        } else {
            GSmodelGetPosition(*(u8**)((u8*)lbl_8047B3EC + 0x24), &origin);
        }
        fn_800E0168(&target, &target, &origin);
        cameraMoveTargetOfs(7, &target, 0.2f);
        GSpartFree(part);
    }

    lbl_8047B3E8 += frame;
    key = lbl_804673D4;
    for (i = 0; i < 2; i++, key++) {
        if (lbl_8047B3E8 <= key->startFrame) {
            lbl_80478CDC = key->start;
            break;
        }
        if (lbl_8047B3E8 <= key->endFrame) {
            t = (f32)(lbl_8047B3E8 - key->startFrame) /
                (f32)(key->endFrame - key->startFrame);
            lbl_80478CDC =
                GSlerpGetLinearInterpolationFloat(key->start, key->end, t);
            break;
        }
        lbl_80478CDC = key->end;
    }
    cameraSetFov(lbl_80478CDC);

    if (lbl_8047B3F0 == NULL && !cameraMoveEndCheckSpecial(0)) {
        void* activeSequence = lbl_8047B3F0;
        if (activeSequence == NULL) {
            GSscene_SetMode(8);
        } else {
            if (*(u32*)((u8*)activeSequence + 0x18) != 0 &&
                *(u32*)((u8*)activeSequence + 0x20) != 0) {
                cameraStopAnime(activeSequence);
            }
            lbl_8047B3F0 = NULL;
        }
        fn_801765F4(0);
        lbl_8047B3EC = NULL;
        if (fn_800057A8() == 2) {
            GSscene_SetMode(2);
        }
    }
}

/**
 * fn_801D2F94 - Waza animation teardown.
 * Address: 0x801D2F94 | Size: 0x88
 */
extern void GSscene_SetMode(s32 arg);
extern void cameraStopAnime(void* arg);
extern void fn_801765F4(s32 arg);
extern s32  fn_800057A8(void);
void fn_801D2F94(void) {
    void* obj;
    if (lbl_8047B3EC != NULL) {
        obj = lbl_8047B3F0;
        if (obj == NULL) {
            GSscene_SetMode(8);
        } else {
            if (*(u32*)((u8*)obj + 0x18) != 0 && *(u32*)((u8*)obj + 0x20) != 0) {
                cameraStopAnime(obj);
            }
            lbl_8047B3F0 = NULL;
        }
        fn_801765F4(0);
        lbl_8047B3EC = NULL;
        if (fn_800057A8() == 2) {
            GSscene_SetMode(2);
        }
    }
}

/**
 * fn_801D301C - Waza animation reset: set lbl_8047B3F4=1, lbl_8047B3EC=0, lbl_8047B3F0=0.
 * Address: 0x801D301C | Size: 0x18
 */
extern u8  lbl_8047B3F4;
void fn_801D301C(void) {
    lbl_8047B3F4 = 1;
    lbl_8047B3EC = NULL;
    lbl_8047B3F0 = NULL;
}

/**
 * fn_801D3034 - Waza animation frame step.
 * Address: 0x801D3034 | Size: 0x88
 */
void fn_801D3034(void* state) {
    void* obj;
    if (state == lbl_8047B3EC) {
        obj = lbl_8047B3F0;
        if (obj == NULL) {
            GSscene_SetMode(8);
        } else {
            if (*(u32*)((u8*)obj + 0x18) != 0 && *(u32*)((u8*)obj + 0x20) != 0) {
                cameraStopAnime(obj);
            }
            lbl_8047B3F0 = NULL;
        }
        fn_801765F4(0);
        lbl_8047B3EC = NULL;
        if (fn_800057A8() == 2) {
            GSscene_SetMode(2);
        }
    }
}

/**
 * battleCameraStartWaza - Waza animation state machine dispatcher.
 * Address: 0x801D30BC | Size: 0x3E0
 */
void battleCameraStartWaza(void* owner, void* sequence) {
    extern void* GSresGetResource(u32 group, u32 resource);
    extern void clear__5GSvecFv(void* vec);
    extern void GSlerpGetLinearInterpolationVector(void* dst, void* src,
                                                   void* target, f32 t);
    extern u8 lbl_804673A0[];
    extern u16 battleGridGetNumPokemonsForTrainer(u32 id);
    extern void cameraSetTargetExt(u32 a, u32 b, u32 c);
    extern void cameraRefreshTargetPos(void);
    extern void cameraMoveStop(void);
    extern void cameraSetOffsetPosition(void* src);
    extern void cameraSetOffsetRotation(void* src);
    extern void cameraSetOffsetScale(void* src);
    extern void cameraPlayOffsetAnime(u32 groupId, u32 animationId,
                                      s32 frame, u8 loop);
    extern void* GSmodelGetBound(void* model);
    extern void GSmodelGetPosition(void* model, Vec* out);
    extern void GSmodelGetRotation(void* model, Vec* out);
    extern void fn_800D1070(s32 mode);
    u8* ownerBytes = owner;
    u8* sequenceBytes = sequence;
    Vec offsetPosition;
    Vec offsetRotation;
    Vec offsetScale;
    Vec center;
    Vec rootPosition;
    Vec ownerPosition;
    void* model;
    u8* bound;
    s32 paramsFlags = 0;
    u8 reverse = FALSE;
    s32 shift = 0;
    f32 scaleValue;

    if (lbl_8047B3F4 == 0) {
        return;
    }

    if (*(s8*)(ownerBytes + 0x76) < 0 &&
        (sequenceBytes == NULL || ((*(s32*)(sequenceBytes + 0x08) & 0x80) != 0x80))) {
        reverse = TRUE;
    }

    lbl_8047B3EC = owner;
    lbl_8047B3F0 = sequence;
    if (sequenceBytes != NULL) {
        battleCameraDisable();
        if (*(u32*)(sequenceBytes + 0x18) != 0 &&
            *(u32*)(sequenceBytes + 0x20) != 0 &&
            GSresGetResource(*(u32*)(sequenceBytes + 0x18),
                             *(u32*)(sequenceBytes + 0x20)) != NULL) {
            model = *(void**)(ownerBytes + 0x24);
            shift = 0;
            if (reverse) {
                shift = 4;
            }

            cameraPlayOffsetAnime(
                *(u32*)(sequenceBytes + 0x18), *(u32*)(sequenceBytes + 0x20),
                0, 0);

            if (*(u32*)(sequenceBytes + 0x08) & 0x4) {
                clear__5GSvecFv(&offsetPosition);
                clear__5GSvecFv(&offsetRotation);
                if (*(s8*)(ownerBytes + 0x76) < 0 &&
                    (*(u32*)(sequenceBytes + 0x08) & 0x02000000) == 0x02000000) {
                    offsetRotation.y = 3.1415927f;
                }
                if ((*(u32*)(sequenceBytes + 0x08) & 0x00800000) == 0x00800000) {
                    battleGridGetNormalisedScale((f32*)&offsetScale);
                    if ((*(u32*)(sequenceBytes + 0x08) & 0x01000000) == 0x01000000) {
                        offsetScale.y = 1.0f;
                    }
                } else {
                    set__5GSvecFfff((f32*)&offsetScale, 1.0f,
                                    1.0f, 1.0f);
                }
            } else {
                GSmodelGetPosition(model, &offsetPosition);
                if (*(u32*)(sequenceBytes + 0x08) & 0x00004000) {
                    bound = GSmodelGetBound(model);
                    GSlerpGetLinearInterpolationVector(
                        &center, bound + 0x10, bound + 0x1C, 0.5f);
                    GSvecAdd(&offsetPosition, &offsetPosition, &center);
                }
                if (*(u16*)(sequenceBytes + 0x2E) == 2 &&
                    (ownerBytes[0x18] & 2) == 2 &&
                    (u8)GSmodelIsRootNullAdded(model) != 0) {
                    GSmodelGetRootPosition(model, (GSvec*)&rootPosition);
                    offsetPosition.y += rootPosition.y;
                }
                GSmodelGetRotation(model, &offsetRotation);
                scaleValue = fn_801DABAC(owner);
                set__5GSvecFfff(
                    (f32*)&offsetScale, scaleValue, scaleValue, scaleValue);
            }

            fn_801765F4(shift);
            cameraMoveStop();
            cameraSetOffsetPosition(&offsetPosition);
            cameraSetOffsetRotation(&offsetRotation);
            cameraSetOffsetScale(&offsetScale);
            fn_800D1070(0);
            cameraUpdate();
            return;
        }

        if (*(u32*)(sequenceBytes + 0x08) & 0x00200000) {
            paramsFlags |= 2;
        } else if (*(u32*)(sequenceBytes + 0x08) & 0x00000200) {
            paramsFlags |= 4;
        } else if (*(u32*)(sequenceBytes + 0x08) & 0x00400000) {
            GSmodelGetPosition(*(void**)(ownerBytes + 0x24), &ownerPosition);
            if (ownerPosition.z < 0.0f) {
                paramsFlags |= 4;
            } else {
                paramsFlags |= 8;
            }
        }

        if (*(u32*)(sequenceBytes + 0x08) & 0x00000400) {
            paramsFlags |= 0x20;
        } else if (*(u32*)(sequenceBytes + 0x08) & 0x00000800) {
            paramsFlags |= 0x40;
        } else if (*(u32*)(sequenceBytes + 0x08) & 0x00001000) {
            paramsFlags |= 0x80;
        }
    } else {
        paramsFlags = 4;
    }

    if (ownerBytes[0x75] == 0) {
        u16 count = battleGridGetNumPokemonsForTrainer((u32)owner);

        if (count == 1) {
            paramsFlags &= ~0x1F;
            paramsFlags |= 0x10;
        } else if (count > 1) {
            paramsFlags &= ~0x1F;
            paramsFlags |= 1;
        }
    }

    GSscene_SetMode(7);
    cameraMoveStop();
    {
        u8* activeOwner = lbl_8047B3EC;
        u8* activeSequence = lbl_8047B3F0;
        u8* cameraParams = *(u8**)(activeOwner + 0x2C) +
                           *(u16*)(activeOwner + 0x32) * 0xD4;
        u32 targetId;

        if (activeSequence != NULL) {
            targetId = ((u32*)(cameraParams + 0x4C))[activeSequence[0x17]];
        } else {
            targetId = *(u32*)(cameraParams + 0x54);
        }
        cameraSetTargetExt(*(u32*)(ownerBytes + 0x00),
                           *(u32*)(ownerBytes + 0x04), targetId);
        cameraRefreshTargetPos();
        cameraSetTargetExt(*(u32*)(ownerBytes + 0x00),
                           *(u32*)(ownerBytes + 0x04), (u32)-1);
    }

    if (ownerBytes[0x75] == 0) {
        shift = 1;
    }

    _wazaSequenceCameraCalculateParams__FP13ModelSequenceiP24wazaSequenceCameraParams(
        owner, paramsFlags, lbl_804673A0);
    _wazaSequenceCameraSelectMotion__FP13ModelSequenceP12WazaSequenceP24wazaSequenceCameraParams(
        owner, sequence, lbl_804673A0);
    _wazaSequenceCameraDoPosition__FP13ModelSequenceP24wazaSequenceCameraParamsfb(
        owner, lbl_804673A0, shift, reverse);
    _wazaSequenceCameraDoFOV__FP13ModelSequenceP24wazaSequenceCameraParamsif(
        owner, lbl_804673A0, paramsFlags, shift);
    cameraUpdate();
}

/**
 * _wazaSequenceCameraDoPosition__FP13ModelSequenceP24wazaSequenceCameraParamsfb - Move animation state machine A.
 * Address: 0x801D349C | Size: 0xAE0
 * Massive state machine (~2.8KB) for a class of move animations.
 * Likely handles physical/contact move animations.
 */
void _wazaSequenceCameraDoPosition__FP13ModelSequenceP24wazaSequenceCameraParamsfb(
    void* modelSequence, void* cameraParams, s32 shift, u8 reverse)
{
    typedef struct WazaSequenceCameraPattern {
        u32 duration_index;
        u8 pad_04[0xD4 - 4];
    } WazaSequenceCameraPattern;
    typedef struct WazaSequenceCameraParamsLocal {
        s32 mode;
        u8 pad_04[8];
        f32 rotation_min;
        f32 rotation_max;
        f32 rotation_base;
        f32 height_min;
        f32 height_max;
        f32 distance_min;
        f32 distance_max;
        f32 out_near;
        f32 out_mid;
        f32 out_far;
    } WazaSequenceCameraParamsLocal;
    extern f32 fn_800E0BE4(void);
    extern s32 fn_800D37CC(void);
    extern void cameraSetDistance(f32);
    extern void cameraSetHeight(f32);
    extern void cameraSetRotY(f32);
    extern void cameraUpdate(void);
    extern void cameraMovePosition(s32, Vec*, f32);
    extern void cameraMoveRotationXYZ(f32, f32, f32, f32);
    extern void GSscene_GetCameraDirectionVector(Vec*);
    extern void GSscene_SetCameraDirectionVector(Vec*);
    u8* sequence = modelSequence;
    WazaSequenceCameraParamsLocal* params = cameraParams;
    WazaSequenceCameraPattern* pattern;
    s32 duration;

    pattern = (WazaSequenceCameraPattern*)
        (*(u8**)(sequence + 0x2C) + *(u16*)(sequence + 0x32) * 0xD4);

    switch (params->mode) {
    case 0:
        _wazaSequenceCameraDoDollyPosition__FP21TemplateExpFileHeaderP24wazaSequenceCameraParamsfb(
            pattern, params, shift, reverse);
        break;

    case 1: {
        Vec direction;
        f32 offset;
        f32 height;
        f32 distance;
        f32 square;
        u8* durationPtr = (u8*)pattern + (*(u32*)pattern << 2);

        duration = *(s32*)(durationPtr + 4);
        if (duration == 0) {
            duration = *(s32*)(durationPtr + 8);
        }
        duration <<= shift;

        offset = 10.0f * fn_800E0BE4() + 25.0f;
        height = 9.0f * fn_800E0BE4() + 1.0f;
        distance = 30.0f * fn_800E0BE4() + 20.0f;

        cameraSetDistance(distance);
        cameraSetHeight(height);
        cameraSetRotY(0.0f);
        cameraUpdate();

        GSscene_GetCameraDirectionVector(&direction);
        if (reverse) {
            direction.x += 10.0f;
        } else {
            direction.x -= 10.0f;
        }
        GSscene_SetCameraDirectionVector(&direction);
        if (reverse) {
            direction.x += offset;
        } else {
            direction.x -= offset;
        }
        cameraMovePosition(7, &direction, (f32)duration / (f32)fn_800D37CC());

        square = offset * offset + height * height;
        params->out_near = sqrtf(square);
        params->out_far = sqrtf(distance * distance + square);
        params->out_mid = 0.5f * (params->out_near + params->out_far);
        break;
    }

    case 2: {
        f32 distance;
        f32 height;
        f32 rotA;
        f32 rotB;
        f32 length;
        u8* durationPtr = (u8*)pattern + (*(u32*)pattern << 2);

        duration = *(s32*)(durationPtr + 4);
        if (duration == 0) {
            duration = *(s32*)(durationPtr + 8);
        }
        duration <<= shift;

        distance = params->distance_min +
            (params->distance_max - params->distance_min) * fn_800E0BE4();
        height = params->height_min +
            (params->height_max - params->height_min) * fn_800E0BE4();
        if (reverse) {
            rotA = (params->rotation_base - params->rotation_min) -
                   (params->rotation_max - params->rotation_min) * fn_800E0BE4();
            rotB = (params->rotation_base - params->rotation_min) -
                   (params->rotation_max - params->rotation_min) * fn_800E0BE4();
        } else {
            rotA = (params->rotation_max - params->rotation_min) * fn_800E0BE4() +
                   (params->rotation_base + params->rotation_min);
            rotB = (params->rotation_max - params->rotation_min) * fn_800E0BE4() +
                   (params->rotation_base + params->rotation_min);
        }
        if (rotA > rotB) {
            f32 swap = rotA;
            rotA = rotB;
            rotB = swap;
        }

        cameraSetDistance(distance);
        cameraSetHeight(height);
        cameraSetRotY(rotA);
        cameraUpdate();
        cameraMoveRotationXYZ(0.0f, rotB, 0.0f, (f32)duration / (f32)fn_800D37CC());

        length = sqrtf(distance * distance + height * height);
        params->out_far = length;
        params->out_mid = length;
        params->out_near = length;
        break;
    }

    case 3: {
        Vec direction;
        f32 distance;
        f32 height;
        f32 rotation;
        f32 length;
        u8* durationPtr = (u8*)pattern + (*(u32*)pattern << 2);

        duration = *(s32*)(durationPtr + 4);
        if (duration == 0) {
            duration = *(s32*)(durationPtr + 8);
        }
        duration <<= shift;

        distance = params->distance_min +
            (params->distance_max - params->distance_min) * fn_800E0BE4();
        height = params->height_min +
            (params->height_max - params->height_min) * fn_800E0BE4();
        if (reverse) {
            rotation = (params->rotation_base - params->rotation_min) -
                       (params->rotation_max - params->rotation_min) * fn_800E0BE4();
        } else {
            rotation = (params->rotation_max - params->rotation_min) * fn_800E0BE4() +
                       (params->rotation_base + params->rotation_min);
        }

        cameraSetDistance(distance);
        cameraSetHeight(height);
        cameraSetRotY(rotation);
        cameraUpdate();
        GSscene_GetCameraDirectionVector(&direction);
        cameraMovePosition(7, &direction, (f32)duration / (f32)fn_800D37CC());

        length = sqrtf(distance * distance + height * height);
        params->out_far = length;
        params->out_mid = length;
        params->out_near = length;
        break;
    }

    case 4: {
        Vec direction;
        f32 rotation;
        f32 length;
        u8* durationPtr = (u8*)pattern + (*(u32*)pattern << 2);

        duration = *(s32*)(durationPtr + 4);
        if (duration == 0) {
            duration = *(s32*)(durationPtr + 8);
        }
        duration <<= shift;

        if (reverse) {
            rotation = params->rotation_base - 0.47123894f;
        } else {
            rotation = 0.47123894f + params->rotation_base;
        }

        cameraSetDistance(110.0f);
        cameraSetHeight(25.0f);
        cameraSetRotY(rotation);
        cameraUpdate();
        GSscene_GetCameraDirectionVector(&direction);
        cameraMovePosition(7, &direction, (f32)duration / (f32)fn_800D37CC());

        length = sqrtf(110.0f * 110.0f + 25.0f * 25.0f);
        params->out_far = length;
        params->out_mid = length;
        params->out_near = length;
        break;
    }

    case 5: {
        Vec direction;
        f32 scale;
        f32 distance;
        f32 height;
        f32 rotation;
        f32 length;
        s32 mode = *(s32*)(sequence + 0x10);
        u8* durationPtr = (u8*)pattern + (*(u32*)pattern << 2);

        duration = *(s32*)(durationPtr + 4);
        if (duration == 0) {
            duration = *(s32*)(durationPtr + 8);
        }
        duration <<= shift;

        switch (mode) {
        case -2:
        case -1:
            scale = 0.875f;
            break;
        case 1:
            scale = 1.4f;
            break;
        case 2:
            scale = 1.8f;
            break;
        case 3:
            scale = 3.0f;
            break;
        default:
            scale = 1.0f;
            break;
        }

        distance = 50.0f * scale;
        height = params->height_min +
            (params->height_max - params->height_min) * fn_800E0BE4();
        if (reverse) {
            rotation = params->rotation_base - 0.7853982f;
        } else {
            rotation = 0.7853982f + params->rotation_base;
        }

        cameraSetDistance(distance);
        cameraSetHeight(height);
        cameraSetRotY(rotation);
        cameraUpdate();
        GSscene_GetCameraDirectionVector(&direction);
        cameraMovePosition(7, &direction, (f32)duration / (f32)fn_800D37CC());

        length = sqrtf(distance * distance + height * height);
        params->out_far = length;
        params->out_mid = length;
        params->out_near = length;
        break;
    }
    }
}

/**
 * _wazaSequenceCameraDoDollyPosition__FP21TemplateExpFileHeaderP24wazaSequenceCameraParamsfb - Move animation state machine B.
 * Address: 0x801D3F7C | Size: 0x548
 * State machine for beam/projectile move animations.
 */
void _wazaSequenceCameraDoDollyPosition__FP21TemplateExpFileHeaderP24wazaSequenceCameraParamsfb(
    void* header, void* params, s32 shift, u8 reverse)
{
    typedef struct WazaCameraParams {
        u8 pad_00[0x0C];
        f32 rotationBase;
        f32 rotationRange;
        f32 rotationOffset;
        f32 heightMin;
        f32 heightMax;
        f32 distanceMin;
        f32 distanceMax;
        f32 nearDistance;
        f32 middleDistance;
        f32 farDistance;
        f32 nearLength;
        f32 middleLength;
        f32 farLength;
    } WazaCameraParams;
    extern f32 fn_800E0BE4();
    extern u32 _fadeEffectGetRandom__FUl(u32);
    extern void cameraSetDistance(f32);
    extern void cameraSetHeight(f32);
    extern void cameraSetRotY(f32);
    extern void cameraUpdate(void);
    extern void GSscene_GetCameraDirectionVector(Vec*);
    extern void GSscene_GetCameraPositionVector(Vec*);
    extern void fn_800E0168(Vec*, Vec*, Vec*);
    extern void fn_800E013C(Vec*, Vec*, f32);
    extern void GSvecAdd(Vec*, Vec*, Vec*);
    extern s32 fn_800D37CC(void);
    extern void cameraMovePosition(s32, Vec*, f32);
    u8* file = header;
    WazaCameraParams* camera = params;
    Vec direction;
    Vec position;
    s32 randomDelay;
    s32 duration;
    s32 delay;
    f32 threshold0 = 0.5f;
    f32 threshold1 = 0.75f;
    f32 distance0;
    f32 distance1;
    f32 nearDistance;
    f32 farDistance;
    f32 height;
    f32 rotation;
    f32 chance;
    u8 alternate;

    duration = *(s32*)(file + (*(s32*)file << 2) + 4);
    if (duration == 0) {
        duration = *(s32*)(file + (*(s32*)file << 2) + 8);
    }
    duration <<= shift;

    if (fn_800E0BE4() <= 0.7f) {
        threshold0 = 0.3f;
        threshold1 = 0.65f;
        alternate = TRUE;
    } else {
        alternate = FALSE;
    }

    chance = fn_800E0BE4();
    if (chance < threshold0) {
        randomDelay = 0;
    } else if (chance < threshold1) {
        s32 fullDuration;

        randomDelay = 0;
        fullDuration = duration;
        duration = _fadeEffectGetRandom__FUl(duration);
        if (duration < (fullDuration >> 1)) {
            duration = fullDuration >> 1;
        }
    } else {
        randomDelay = _fadeEffectGetRandom__FUl(duration);
        if (randomDelay > (duration >> 1)) {
            randomDelay = duration >> 1;
        }
    }

    distance0 = camera->distanceMin +
        (camera->distanceMax - camera->distanceMin) * fn_800E0BE4();
    nearDistance = distance0;
    distance1 = camera->distanceMin +
        (camera->distanceMax - camera->distanceMin) * fn_800E0BE4();
    farDistance = distance1;
    /* RULE-EXCEPTION(user-approved): the height is staged in distance1 (reused scalar) as retail's f0 temporary - see docs/RULE_EXCEPTIONS.md */
    if (alternate) {
        if (distance0 < distance1) {
            farDistance = distance0;
            nearDistance = distance1;
        }
        distance1 = camera->heightMin +
            (camera->heightMax - camera->heightMin) * fn_800E0BE4();
        height = distance1;
    } else {
        if (distance0 > distance1) {
            farDistance = distance0;
            nearDistance = distance1;
        }
        distance1 = camera->heightMin +
            (camera->heightMax - camera->heightMin) * fn_800E0BE4();
        height = distance1;
    }

    if (reverse) {
        rotation = (camera->rotationOffset - camera->rotationBase) -
                   (camera->rotationRange - camera->rotationBase) * fn_800E0BE4();
    } else {
        rotation = (camera->rotationRange - camera->rotationBase) *
                       fn_800E0BE4() +
                   (camera->rotationOffset + camera->rotationBase);
    }

    cameraSetDistance(nearDistance);
    cameraSetHeight(height);
    cameraSetRotY(rotation);
    cameraUpdate();
    GSscene_GetCameraDirectionVector(&direction);
    GSscene_GetCameraPositionVector(&position);
    fn_800E0168(&direction, &direction, &position);
    direction.y = 0.0f;
    fn_800E013C(&direction, &direction, farDistance / nearDistance);
    direction.y = height;
    GSvecAdd(&direction, &direction, &position);
    delay = duration - randomDelay;
    cameraMovePosition(7, &direction, (f32)delay / (f32)fn_800D37CC());

    camera->nearDistance =
        sqrtf(nearDistance * nearDistance + height * height);
    distance0 = 0.5f * (nearDistance + farDistance);
    camera->middleDistance =
        sqrtf(distance0 * distance0 + height * height);
    camera->farDistance =
        sqrtf(farDistance * farDistance + height * height);
}

/**
 * _wazaSequenceCameraDoFOV__FP13ModelSequenceP24wazaSequenceCameraParamsif - Move animation state machine C.
 * Address: 0x801D44C4 | Size: 0x514
 * State machine for status/field effect move animations.
 */
void _wazaSequenceCameraDoFOV__FP13ModelSequenceP24wazaSequenceCameraParamsif(
    void* modelSequence, void* cameraParams, s32 flags, s32 shift)
{
    typedef struct CameraFovKey {
        f32 start;
        f32 end;
        u32 startFrame;
        u32 endFrame;
    } CameraFovKey;
    typedef struct WazaSequenceCameraParamsFov {
        s32 mode;
        f32 scaleMin;
        f32 scaleMax;
        u8 pad_0C[0x1C];
        f32 range0;
        f32 range1;
        f32 range2;
    } WazaSequenceCameraParamsFov;
    typedef struct WazaSequenceCameraFovTiming {
        s32 count;
        u8 pad_04[8];
        s32 frame0;
        s32 frame1;
        s32 frame2;
    } WazaSequenceCameraFovTiming;
    typedef struct WazaSequenceCameraFovPattern {
        s32 mode;
        s32 durationMode;
        f32 thresholds[4];
        u32 initialFlags;
        u32 flags;
        s32 timingMode;
    } WazaSequenceCameraFovPattern;
    extern f32 atan2(f32, f32);
    extern f32 fn_800E0BE4(void);
    extern void battleCameraDisable(void);
    extern void cameraSetFov(f32);
    extern CameraFovKey lbl_804673D4[];
    extern f32 lbl_80478CDC;
    extern u32 lbl_8047B3E8;
    u8* sequence = modelSequence;
    WazaSequenceCameraParamsFov* params = cameraParams;
    WazaSequenceCameraFovTiming* timing;
    WazaSequenceCameraFovPattern* pattern;
    CameraFovKey* key;
    u8* timingCursor;
    s32 prevFrame;
    s32 nextFrame;
    s32 count;
    s32 i;
    u32 choice;
    u8 hasFirst;
    u8 hasSecond;
    f32 currentFov;
    f32 lowFov;
    f32 highFov;
    f32 span;
    f32 mixStart;
    f32 mixEnd;

    timing = (WazaSequenceCameraFovTiming*)
        (*(u8**)(sequence + 0x2C) + *(u16*)(sequence + 0x32) * 0xD4);
    count = timing->count;
    span = 0.75f * params->scaleMin;
    if (params->scaleMax > span) {
        span = params->scaleMax;
    }

    lowFov = 57.29578f *
        (2.0f * atan2(0.5f * span, params->range0));
    highFov = 57.29578f *
        (2.0f * atan2(2.0f * span, params->range0));

    if (lowFov < 15.0f) {
        lowFov = 15.0f;
    }
    if (highFov < 15.0f) {
        highFov = 15.0f;
    }
    if (lowFov > 85.0f) {
        lowFov = 85.0f;
    }
    if (highFov > 85.0f) {
        highFov = 85.0f;
    }

    if (params->mode == 0 || (params->mode >= 4 && params->mode < 6)) {
        if (flags & 0x20) {
            choice = 1;
        } else if (flags & 0x80) {
            choice = 4;
        } else {
            choice = 2;
        }

        mixStart = 0.0f;
        mixEnd = 1.0f;
        hasFirst = FALSE;
        hasSecond = FALSE;
        if (choice & 1) {
            mixStart = 0.0f;
            mixEnd = 0.2f;
            hasFirst = TRUE;
        }
        if (choice & 2) {
            if (!hasFirst) {
                mixStart = 0.35f;
            }
            mixEnd = 0.6f;
            hasSecond = TRUE;
        }
        if (choice & 4) {
            if (!hasSecond) {
                mixStart = 0.75f;
            }
            mixEnd = 1.0f;
        }

        currentFov = lowFov + (highFov - lowFov) *
            (mixStart + (mixEnd - mixStart) * fn_800E0BE4());
        lbl_804673D4[0].start = currentFov;
        lbl_804673D4[0].end = currentFov;
        lbl_804673D4[1].start = currentFov;
        lbl_804673D4[1].end = currentFov;
        lbl_80478CDC = currentFov;
        lbl_804673D4[0].startFrame = timing->frame0 << shift;
        lbl_804673D4[0].endFrame = timing->frame0 << shift;
        lbl_804673D4[1].startFrame = timing->frame0 << shift;
        lbl_804673D4[1].endFrame = timing->frame0 << shift;
        lbl_8047B3E8 = timing->frame0 << shift;
        cameraSetFov(currentFov);
        return;
    }

    pattern = (WazaSequenceCameraFovPattern*)((u8*)wazaSequenceCameraGetPattern__Fbi(
        (*(u16*)(sequence + 0x32) != 8 && *(u16*)(sequence + 0x32) != 9), flags) + 4);

    mixStart = 0.0f;
    mixEnd = 1.0f;
    hasFirst = FALSE;
    hasSecond = FALSE;
    if (pattern->initialFlags & 1) {
        mixStart = 0.0f;
        mixEnd = 0.2f;
        hasFirst = TRUE;
    }
    if (pattern->initialFlags & 2) {
        if (!hasFirst) {
            mixStart = 0.35f;
        }
        mixEnd = 0.6f;
        hasSecond = TRUE;
    }
    if (pattern->initialFlags & 4) {
        if (!hasSecond) {
            mixStart = 0.75f;
        }
        mixEnd = 1.0f;
    }

    currentFov = lowFov + (highFov - lowFov) *
        (mixStart + (mixEnd - mixStart) * fn_800E0BE4());
    lbl_804673D4[0].start = currentFov;
    lbl_804673D4[0].end = currentFov;
    lbl_804673D4[1].start = currentFov;
    lbl_804673D4[1].end = currentFov;
    lbl_80478CDC = currentFov;
    lbl_804673D4[0].startFrame = timing->frame0 << shift;
    lbl_804673D4[0].endFrame = timing->frame0 << shift;
    lbl_804673D4[1].startFrame = timing->frame0 << shift;
    lbl_804673D4[1].endFrame = timing->frame0 << shift;
    lbl_8047B3E8 = timing->frame0 << shift;
    cameraSetFov(currentFov);

    if (count <= 2) {
        return;
    }

    prevFrame = timing->frame0;
    key = lbl_804673D4;
    timingCursor = (u8*)timing;
    for (i = 0; i < 2; i++, key++, pattern++, timingCursor += 4, currentFov = key->end) {
        nextFrame = *(s32*)(timingCursor + 0x10);
        if (prevFrame == nextFrame) {
            key->startFrame = prevFrame << shift;
            key->endFrame = prevFrame << shift;
            key->start = currentFov;
            key->end = currentFov;
            continue;
        }

        if (pattern->mode >= 3 && pattern->mode < 5) {
            s32 duration =
                _wazaSequenceCameraSelectDuration__FUcPff(
                    pattern->durationMode, pattern->thresholds,
                    nextFrame - prevFrame);
            f32 radius = *(f32*)((u8*)params + 0x2C);

            span = 0.75f * params->scaleMin;
            if (params->scaleMax > span) {
                span = params->scaleMax;
            }

            lowFov = 57.29578f *
                (2.0f * atan2(0.5f * span, radius));
            highFov = 57.29578f *
                (2.0f * atan2(2.0f * span, radius));

            if (lowFov < 15.0f) {
                lowFov = 15.0f;
            }
            if (highFov < 15.0f) {
                highFov = 15.0f;
            }
            if (lowFov > 85.0f) {
                lowFov = 85.0f;
            }
            if (highFov > 85.0f) {
                highFov = 85.0f;
            }

            hasFirst = FALSE;
            hasSecond = FALSE;
            if (pattern->flags & 1) {
                mixStart = 0.0f;
                mixEnd = 0.2f;
                hasFirst = TRUE;
            }
            if (pattern->flags & 2) {
                if (!hasFirst) {
                    mixStart = 0.35f;
                }
                mixEnd = 0.6f;
                hasSecond = TRUE;
            }
            if (pattern->flags & 4) {
                if (!hasSecond) {
                    mixStart = 0.75f;
                }
                mixEnd = 1.0f;
            }

            key->end = lowFov + (highFov - lowFov) *
                (mixStart + (mixEnd - mixStart) * fn_800E0BE4());
            if (pattern->timingMode == 2) {
                key->endFrame = nextFrame << shift;
                key->startFrame = (nextFrame - duration) << shift;
            } else {
                key->startFrame = prevFrame << shift;
                key->endFrame = (prevFrame + duration) << shift;
            }
            key->start = currentFov;
        } else {
            key->startFrame = prevFrame << shift;
            key->endFrame = nextFrame << shift;
            key->start = currentFov;
            key->end = currentFov;
        }

        params = (WazaSequenceCameraParamsFov*)((u8*)params + 4);
        prevFrame = nextFrame;
    }
}

/**
 * _wazaSequenceCameraSelectDuration__FUcPff - Move animation state machine D.
 * Address: 0x801D49D8 | Size: 0x3C8
 * State machine for spread/multi-target move animations.
 */
s32 _wazaSequenceCameraSelectDuration__FUcPff(
    s32 mode, f32* thresholds, s32 duration)
{
    extern f32 fn_800E0BE4();
    f32 random;

    if (mode == 1) {
        return duration < 12 ? duration : 12;
    }
    if (mode == 2) {
        return duration < 20 ? duration : 20;
    }
    if (mode == 4) {
        return duration < 45 ? duration : 45;
    }
    if (mode == 8) {
        return duration;
    }
    if (mode == 3) {
        random = fn_800E0BE4();
        return random < thresholds[0]
                   ? (duration < 12 ? duration : 12)
                   : (duration < 20 ? duration : 20);
    }
    if (mode == 5) {
        random = fn_800E0BE4();
        return random < thresholds[0]
                   ? (duration < 12 ? duration : 12)
                   : (duration < 45 ? duration : 45);
    }
    if (mode == 9) {
        random = fn_800E0BE4();
        return random < thresholds[0]
                   ? (duration < 12 ? duration : 12)
                   : duration;
    }
    if (mode == 6) {
        random = fn_800E0BE4();
        return random < thresholds[1]
                   ? (duration < 20 ? duration : 20)
                   : (duration < 45 ? duration : 45);
    }
    if (mode == 10) {
        random = fn_800E0BE4();
        return random < thresholds[1]
                   ? (duration < 20 ? duration : 20)
                   : duration;
    }
    if (mode == 12) {
        random = fn_800E0BE4();
        return random < thresholds[2]
                   ? (duration < 45 ? duration : 45)
                   : duration;
    }
    if (mode == 7) {
        random = fn_800E0BE4();
        if (random < thresholds[0]) {
            return duration < 12 ? duration : 12;
        }
        if (random < thresholds[1]) {
            return duration < 20 ? duration : 20;
        }
        return duration < 45 ? duration : 45;
    }
    if (mode == 11) {
        random = fn_800E0BE4();
        if (random < thresholds[0]) {
            return duration < 12 ? duration : 12;
        }
        if (random < thresholds[1]) {
            return duration < 20 ? duration : 20;
        }
        return duration;
    }
    if (mode == 13) {
        random = fn_800E0BE4();
        if (random < thresholds[0]) {
            return duration < 12 ? duration : 12;
        }
        if (random < thresholds[2]) {
            return duration < 45 ? duration : 45;
        }
        return duration;
    }
    if (mode == 14) {
        random = fn_800E0BE4();
        if (random < thresholds[1]) {
            return duration < 12 ? duration : 12;
        }
        if (random < thresholds[2]) {
            return duration < 20 ? duration : 20;
        }
        return duration;
    }
    if (mode == 15) {
        random = fn_800E0BE4();
        if (random < thresholds[0]) {
            return duration < 12 ? duration : 12;
        }
        if (random < thresholds[1]) {
            return duration < 12 ? duration : 12;
        }
        if (random < thresholds[2]) {
            return duration < 20 ? duration : 20;
        }
        return duration;
    }
    return duration;
}

/**
 * _wazaSequenceCameraSelectMotion__FP13ModelSequenceP12WazaSequenceP24wazaSequenceCameraParams - Move animation helper: particle burst.
 * Address: 0x801D4DA0 | Size: 0x218
 */
void _wazaSequenceCameraSelectMotion__FP13ModelSequenceP12WazaSequenceP24wazaSequenceCameraParams(
    void* modelSequence, void* wazaSequence, void* cameraParams)
{
    extern f32 fn_800E0BE4(void);
    extern s32 lbl_80478CD8;
    u32 flags;
    u8 option0;
    u8 option1;
    u8 option2;
    u8 option3;
    s32 count = 0;
    s32* motion;
    f32 interval;

    motion = cameraParams;
    if (*(u16*)((u8*)modelSequence + 0x70) == 0x13A) {
        *motion = 4;
        return;
    }

    if (wazaSequence != NULL) {
        flags = *(u32*)((u8*)wazaSequence + 8);
        if ((flags & 1) != 0) {
            *motion = 5;
            return;
        }
        option0 = (u8)((flags >> 3) & 1);
        if (option0 != 0) {
            count++;
        }
        option1 = (u8)((flags >> 4) & 1);
        if (option1 != 0) {
            count++;
        }
        option2 = (u8)((flags >> 5) & 1);
        if (option2 != 0) {
            count++;
        }
        option3 = (u8)((flags >> 6) & 1);
        if (option3 != 0) {
            count++;
        }
    }

    if (count == 0) {
        count = 4;
        option0 = 1;
        option1 = 1;
        option2 = 1;
        option3 = 1;
    } else if (count == 1) {
        if (option0) {
            *motion = 3;
        }
        if (option1) {
            *motion = 0;
        }
        if (option2) {
            *motion = 1;
        }
        if (option3) {
            *motion = 2;
        }
        lbl_80478CD8 = *motion;
        return;
    }

    interval = 1.0f / (f32)count;
    for (;;) {
        f32 random;
        f32 limit;

        random = fn_800E0BE4();
        limit = interval;
        if (option0) {
            if (limit > random) {
                *motion = 3;
                if (lbl_80478CD8 != *motion) {
                    break;
                }
            }
            limit += interval;
        }
        if (option1) {
            if (limit > random) {
                *motion = 0;
                if (lbl_80478CD8 != *motion) {
                    break;
                }
            }
            limit += interval;
        }
        if (option2) {
            if (limit > random) {
                *motion = 1;
                if (lbl_80478CD8 != *motion) {
                    break;
                }
            }
            limit += interval;
        }
        if (option3 && limit > random) {
            *motion = 2;
            if (lbl_80478CD8 != *motion) {
                break;
            }
        }
    }
    lbl_80478CD8 = *motion;
}

/**
 * _wazaSequenceCameraCalculateParams__FP13ModelSequenceiP24wazaSequenceCameraParams - Move animation helper: model projectile.
 * Address: 0x801D4FB8 | Size: 0x370
 */
void _wazaSequenceCameraCalculateParams__FP13ModelSequenceiP24wazaSequenceCameraParams(
    void* modelSequence, s32 flags, void* cameraParams)
{
    typedef struct WazaCameraBound {
        u8 pad_00[0x14];
        f32 minY;
        u8 pad_18[0x08];
        f32 maxY;
        u8 pad_24[0x04];
        Vec size;
    } WazaCameraBound;
    extern void* GSmodelGetBound(void*);
    extern void GSmodelGetRotation(void*, Vec*);
    extern f32 fn_800E008C(Vec*);
    u8* sequence = modelSequence;
    WazaSequenceCameraParams* params = cameraParams;
    void* model = *(void**)(sequence + 0x24);
    WazaCameraBound* bound = GSmodelGetBound(model);
    Vec* extent;
    Vec rotation;
    f32 distanceScale;
    f32 sizeScale;
    f32 lower;
    f32 upper;
    s32 mode;

    GSmodelGetRotation(model, &rotation);
    params->rotationBase = rotation.y;
    mode = *(s32*)(sequence + 0x10);
    switch (mode) {
    case -2:
        sizeScale = 1.1f;
        distanceScale = 0.875f;
        break;
    case -1:
        sizeScale = 1.1f;
        distanceScale = 0.875f;
        break;
    case 1:
        sizeScale = 1.25f;
        distanceScale = 1.4f;
        break;
    case 2:
        sizeScale = 1.3f;
        distanceScale = 1.8f;
        break;
    case 3:
        sizeScale = 1.5f;
        distanceScale = 3.0f;
        break;
    default:
        sizeScale = 1.2f;
        distanceScale = 1.0f;
        break;
    }

    if (flags & 1) {
        params->rotationMin = 0.0f;
        params->rotationMax = 0.5237035f;
    } else if (flags & 2) {
        params->rotationMin = 0.31415927f;
        params->rotationMax = 0.62831855f;
    } else if (flags & 4) {
        if (mode > 0) {
            params->rotationMin = 0.31415927f;
            params->rotationMax = 0.62831855f;
        } else {
            params->rotationMin = 0.62831855f;
            params->rotationMax = 0.9424779f;
        }
    } else if (flags & 8) {
        if (mode > 0) {
            params->rotationMin = 0.62831855f;
            params->rotationMax = 0.9424779f;
        } else {
            params->rotationMin = 0.9424779f;
            params->rotationMax = 1.2566371f;
        }
    } else if (flags & 0x10) {
        params->rotationMin = 1.2566371f;
        params->rotationMax = 1.5707964f;
    } else if (mode > 0) {
        params->rotationMin = 0.31415927f;
        params->rotationMax = 0.9424779f;
    } else {
        params->rotationMin = 0.31415927f;
        params->rotationMax = 1.0995574f;
    }

    if (flags & 0x20) {
        params->distanceMin = 25.0f;
        params->distanceMax = 35.0f;
        lower = 6.0f;
        upper = 8.0f;
    } else if (flags & 0x40) {
        params->distanceMin = 35.0f;
        params->distanceMax = 50.0f;
        lower = 6.0f;
        upper = 11.0f;
    } else if (flags & 0x80) {
        params->distanceMin = 50.0f;
        params->distanceMax = 60.0f;
        lower = 6.0f;
        upper = 20.0f;
    } else {
        params->distanceMin = 20.0f;
        params->distanceMax = 60.0f;
        lower = 6.0f;
        upper = 20.0f;
    }

    extent = &bound->size;
    params->distanceMin *= distanceScale;
    params->distanceMax *= distanceScale;
    params->size = 0.57735026f * fn_800E008C(extent);
    params->height = extent->y;
    params->heightMin = bound->minY;
    params->heightMax = bound->maxY;

    if (params->heightMin < lower) {
        params->heightMin = lower;
        if (params->heightMax < lower) {
            params->heightMax = 1.5f * lower;
        }
    }
    if (params->heightMax > upper) {
        params->heightMax = upper;
        if (params->heightMin > upper) {
            params->heightMin = 0.8f * upper;
        }
    }
    params->size *= sizeScale;
    params->height *= sizeScale;
    if (params->distanceMin < 20.0f) {
        params->distanceMin = 20.0f;
    }
    if (params->distanceMax < 40.0f) {
        params->distanceMax = 40.0f;
    }
    if (params->distanceMin > 48.0f) {
        params->distanceMin = 48.0f;
    }
    if (params->distanceMax > 60.0f) {
        params->distanceMax = 60.0f;
    }
}
