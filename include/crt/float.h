/**
 * @file float.h
 * @brief MSL <float.h> limits as Colosseum's code reads them: FLT_MIN and
 *        FLT_EPSILON are MSL's __float_min / __float_epsilon words (.sdata
 *        0x80478AC8 / 0x80478ACC), addressed absolutely as incomplete
 *        arrays (lis/lfs, not an sda21 load). As in MSL they are plain
 *        (non-const) integer words read through a float cast, so stores
 *        through pointers may alias them; retail's scheduling around the
 *        billboard matrix stores (displayfunc.c) depends on that.
 */
#ifndef CRT_FLOAT_H
#define CRT_FLOAT_H

#include "dolphin/types.h"

/* __float_min */
extern s32 lbl_80478AC8[];
/* __float_epsilon */
extern s32 lbl_80478ACC[];

#define FLT_MIN (*(f32*) lbl_80478AC8)
#define FLT_EPSILON (*(f32*) lbl_80478ACC)

#endif /* CRT_FLOAT_H */
