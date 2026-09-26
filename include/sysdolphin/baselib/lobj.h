/**
 * @file lobj.h
 * @brief HAL sysdolphin lobj.h: LObj flags and the inline accessors.
 *
 * The file carries HAL's name because its inlines assert through
 * __FILE__, which MWCC expands to the including file's basename: retail
 * pools the literal "lobj.h" with lobj.c's .sdata2 strings (0x8047DBD8) for
 * HSD_LObjGetPriority's assertion. include/hsd/hsd_lobj.h holds the
 * structures and prototypes.
 */
#ifndef SYSDOLPHIN_BASELIB_LOBJ_H
#define SYSDOLPHIN_BASELIB_LOBJ_H

#include "dolphin/types.h"
#include "hsd/hsd_debug.h"
#include "hsd/hsd_lobj.h"

#define LOBJ_AMBIENT (0 << 0)
#define LOBJ_INFINITE (1 << 0)
#define LOBJ_POINT (2 << 0)
#define LOBJ_SPOT (3 << 0)
#define LOBJ_TYPE_MASK 3

#define LOBJ_DIFFUSE (1 << 2)
#define LOBJ_SPECULAR (1 << 3)
#define LOBJ_ALPHA (1 << 4)
#define LOBJ_HIDDEN (1 << 5)
#define LOBJ_RAW_PARAM (1 << 6)
#define LOBJ_DIFF_DIRTY (1 << 7)
#define LOBJ_SPEC_DIRTY (1 << 8)

#define LOBJ_LIGHT_ATTN 1

static inline u8 HSD_LObjGetPriority(HSD_LObj* lobj)
{
    HSD_ASSERT(355, lobj);
    return lobj->priority;
}

#endif /* SYSDOLPHIN_BASELIB_LOBJ_H */
