/**
 * @file GScolsys2Util_exact_8010C77C.c
 * @brief GScolsy2UtilGetSidePlanePoint, 0x8010C77C - 0x8010C7BC.
 *
 * A single-function carve. The function sits between the surface-type
 * table code (gs_colsys.c) and the GScolsys2 core TU, and nothing in the
 * binary shows which of the two TUs it closes. It uses no data and is
 * exact on the project default flags (GC/1.3), the flags both
 * neighbouring TUs build with. The local "optimization_level 0" and
 * fp_contract pragmas that used to wrap it in gs_colsys.c are not
 * needed: retail is plain level-4 code.
 *
 * Returns the signed distance of p2 from the plane through p1 with the
 * given normal (a dot product of the normal with p2 - p1).
 */
#include "dolphin/types.h"
#include "game/gs_colsys.h"

/* 0x8010C77C | 0x40 */
f32 GScolsy2UtilGetSidePlanePoint(Vec3f* normal, Vec3f* p1, Vec3f* p2)
{
    return normal->x * (p2->x - p1->x) + normal->y * (p2->y - p1->y) +
           normal->z * (p2->z - p1->z);
}
