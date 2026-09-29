/**
 * @file field_camera_r50_80117514_prefix.c
 * @brief floorUpdateFieldCamera, .text 0x80117514 - 0x801176C8.
 *
 * Weights the floor's field-camera points by inverse square distance from
 * pos and returns the blended camera height / distance / rotY. A point
 * within 1.0 (squared) of pos wins outright. Linked carve on the field
 * camera TU's flags (-opt nopeephole, as field_camera.c); the .sdata2
 * constants are owned by game/data/sdata2_8047CFD0.c, declared const here
 * so the loop's constant loads are hoisted as in retail.
 */
#include "dolphin/types.h"

typedef struct FieldCameraPoint {
    f32 pos[3];
    f32 distance;
    f32 height;
    f32 rotY;
} FieldCameraPoint;

extern u32 lbl_8047AD68;
extern FieldCameraPoint* lbl_8047AD6C;
extern const f32 lbl_8047CFD0; /* 0.0f */
extern const f32 lbl_8047CFDC; /* 1.0f */
extern const f32 lbl_8047CFE0; /* 0.001f */
extern const char lbl_80272770[];

extern void GSlogWrite(const char* fmt, ...);
extern void set__5GSvecFfff(void* obj, f32 x, f32 y, f32 z);
extern f32 GSvecSquareDistance(f32* a, f32* b);

u8 floorUpdateFieldCamera(u8* pos, f32* height, f32* distance, f32* rotY)
{
    f32 point[3];
    f32 total;
    f32 sumDistance;
    f32 sumHeight;
    f32 sumRot;
    f32 dist;
    f32 weight;
    u32 i;
    FieldCameraPoint* p;

    if (lbl_8047AD68 == 1) {
        *height = lbl_8047AD6C->height;
        *distance = lbl_8047AD6C->distance;
        *rotY = lbl_8047AD6C->rotY;
        return 1;
    }

    sumRot = sumHeight = sumDistance = total = lbl_8047CFD0;
    for (i = 0; i < lbl_8047AD68; i++) {
        p = &lbl_8047AD6C[i];
        set__5GSvecFfff(point, p->pos[0], p->pos[1], p->pos[2]);
        dist = GSvecSquareDistance(point, (f32*)pos);
        if (dist > lbl_8047CFDC) {
            weight = lbl_8047CFE0 / dist;
            total += weight;
            sumDistance += p->distance * weight;
            sumHeight += p->height * weight;
            sumRot += p->rotY * weight;
        } else {
            total = lbl_8047CFDC;
            sumHeight = p->height;
            sumDistance = p->distance;
            sumRot = p->rotY;
            break;
        }
    }

    if (lbl_8047CFD0 == total) {
        GSlogWrite(lbl_80272770);
        return 0;
    }

    weight = lbl_8047CFDC / total;
    *height = sumHeight * weight;
    *distance = sumDistance * weight;
    *rotY = sumRot * weight;
    return 1;
}
