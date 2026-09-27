/**
 * @file GScolsys2Util_exact_8010FA54.c
 * @brief GScolsy2UtilGetCpPlanePoint, 0x8010FA54 - 0x8010FAF4.
 *
 * The last function of the GScolsys2Util TU (see
 * GScolsys2Util_exact_8010F6A0.c), carved on its own: it uses no data.
 * Project default flags (GC/1.3), no pragmas.
 */
#include "dolphin/types.h"
#include "game/gs_colsys.h"

/* 0x8010FA54 | 0xA0 */
void GScolsy2UtilGetCpPlanePoint(Vec3f* out, Vec3f* normal, Vec3f* verts, Vec3f* point) {
    f32 scale;
    extern void PSVECScale(void*, void*, f32);
    extern void PSVECAdd(void*, void*, void*);

    scale = (normal->x * (verts->x - point->x)
           + normal->y * (verts->y - point->y)
           + normal->z * (verts->z - point->z))
          / (normal->x * normal->x
           + normal->y * normal->y
           + normal->z * normal->z);
    PSVECScale(normal, out, scale);
    PSVECAdd(out, point, out);
}
