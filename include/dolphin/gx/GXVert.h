/**
 * @file GXVert.h
 * @brief Dolphin SDK GXVert.h: the write-gather pipe and the inline vertex
 *        command writers (GXPosition3f32, GXColor4u8, ...).
 *
 * Follows the SDK header as the Melee decompilation carries it
 * (doldecomp/melee, libs/dolphin/include/dolphin/gx/GXVert.h), with the
 * SDK's const parameters: pobj.c's GXColor3u8/GXColor4u8 expansions
 * re-truncate their later byte arguments (0x801AC140, 0x801AC180), which
 * MWCC does only for const u8 parameters.
 */
#ifndef DOLPHIN_GX_GXVERT_H
#define DOLPHIN_GX_GXVERT_H

#include "dolphin/types.h"

#define GXFIFO_ADDR 0xCC008000

typedef union {
    u8 u8;
    u16 u16;
    u32 u32;
    u64 u64;
    s8 s8;
    s16 s16;
    s32 s32;
    s64 s64;
    f32 f32;
    f64 f64;
} PPCWGPipe;

volatile PPCWGPipe GXWGFifo : GXFIFO_ADDR;

#define GXVERT_FUNC_1PARAM(name, T)                                           \
    static inline void name##1##T(const T x)                                  \
    {                                                                         \
        GXWGFifo.T = x;                                                       \
    }

#define GXVERT_FUNC_2PARAM(name, T)                                           \
    static inline void name##2##T(const T x, const T y)                       \
    {                                                                         \
        GXWGFifo.T = x;                                                       \
        GXWGFifo.T = y;                                                       \
    }

#define GXVERT_FUNC_3PARAM(name, T)                                           \
    static inline void name##3##T(const T x, const T y, const T z)            \
    {                                                                         \
        GXWGFifo.T = x;                                                       \
        GXWGFifo.T = y;                                                       \
        GXWGFifo.T = z;                                                       \
    }

#define GXVERT_FUNC_4PARAM(name, T)                                           \
    static inline void name##4##T(const T x, const T y, const T z, const T w) \
    {                                                                         \
        GXWGFifo.T = x;                                                       \
        GXWGFifo.T = y;                                                       \
        GXWGFifo.T = z;                                                       \
        GXWGFifo.T = w;                                                       \
    }

#define GXVERT_FUNC_INDEX8(name)                                              \
    static inline void name##1x8(const u8 x)                                  \
    {                                                                         \
        GXWGFifo.u8 = x;                                                      \
    }

#define GXVERT_FUNC_INDEX16(name)                                             \
    static inline void name##1x16(const u16 x)                                \
    {                                                                         \
        GXWGFifo.u16 = x;                                                     \
    }

GXVERT_FUNC_3PARAM(GXPosition, f32)
GXVERT_FUNC_INDEX16(GXPosition)
GXVERT_FUNC_INDEX8(GXPosition)

GXVERT_FUNC_3PARAM(GXNormal, f32)
GXVERT_FUNC_INDEX16(GXNormal)
GXVERT_FUNC_INDEX8(GXNormal)

GXVERT_FUNC_4PARAM(GXColor, u8)
GXVERT_FUNC_1PARAM(GXColor, u32)
GXVERT_FUNC_3PARAM(GXColor, u8)
GXVERT_FUNC_1PARAM(GXColor, u16)
GXVERT_FUNC_INDEX16(GXColor)
GXVERT_FUNC_INDEX8(GXColor)

GXVERT_FUNC_2PARAM(GXTexCoord, u8)
GXVERT_FUNC_1PARAM(GXTexCoord, u8)
GXVERT_FUNC_INDEX16(GXTexCoord)
GXVERT_FUNC_INDEX8(GXTexCoord)

GXVERT_FUNC_1PARAM(GXMatrixIndex, u8)

#undef GXVERT_FUNC_1PARAM
#undef GXVERT_FUNC_2PARAM
#undef GXVERT_FUNC_3PARAM
#undef GXVERT_FUNC_4PARAM
#undef GXVERT_FUNC_INDEX8
#undef GXVERT_FUNC_INDEX16

static inline void GXEnd(void) {}

#endif /* DOLPHIN_GX_GXVERT_H */
