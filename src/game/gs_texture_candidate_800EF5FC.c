/**
 * @file gs_texture_candidate_800EF5FC.c
 * @brief GStexture pool: creation, loading and pool initialisation.
 *
 * Address range: 0x800EF5FC - 0x800EFD14 (GStextureCreate). The retail
 * GStexture.cpp spans 0x800EF098 - 0x800F0030; the earlier functions,
 * fn_800EFD14/GStextureLoad (gs_texture_exact_800EFD14.c) and GStextureInit
 * (gs_texture_exact_800EFFC0.c) live in their own dtk partitions.
 *
 * The unit stays a candidate. 2026-09-29 (lane D3), raw objdiff 95.38 ->
 * 97.68, retail 460 instructions:
 *  - the log arguments are `width & 0xFFFF` / `height & 0xFFFF`. This is
 *    what makes MWCC load the format string into r3 while width and height
 *    are still live, so they are not coalesced into r3/r4 and retail's
 *    entry copies `mr r9,r3; mr r8,r4` appear;
 *  - the display default loads width before height, as retail does;
 *  - the incremented mip count is an int `levels` read through (u8), giving
 *    retail's `clrlwi r3,rN,24; addi rN,r3,1`;
 *  - the pool scan is written in place with a goto. With the scan in the
 *    textureFindFree inline, the returned pointer outranks the GX helper's
 *    TLUT-format local (tex r31 / TLUT r30, retail has the reverse); in
 *    place, retail's r30/r31 come out;
 *  - the declaration order gives retail's r26/r27/r28 for adjHeight,
 *    adjWidth and pixelCount.
 * Remaining (23 rows): retail keeps the mip parameter and the incremented
 * count in one register (r29). Here the parameter is r28 and `levels` is
 * r29, which moves the pixel-count multiply and w/h loop registers. The
 * in-place scan's `beq found` also differs from retail's `bne; b found`
 * (the inline-return shape). The goto and the `levels` local are research
 * forms, not accepted source.
 */

#include "dolphin/types.h"
#include "game/gs_texture.h"

extern void GSlogWrite(const char* format, ...);
extern u16 fn_800E2C04(u32 size, u32 alignment);   /* GSmemAlloc */
extern void* fn_800E27B0(u16 handle);              /* GSmemGetPtr */
extern void fn_800E209C(u16 handle);               /* GSmemFree */

extern const char lbl_80270F98[]; /* "GStexture: invalid texture format" */
extern const char lbl_80270FBC[]; /* "GStexture: warning -- texture size adjusted from [%d,%d] to [%d,%d]" */

extern u8 lbl_80466BC0[]; /* current display descriptor (width at +4, height at +6) */

extern GStextureHandle* lbl_8047ABF4; /* texture pool */
extern u32 lbl_8047ABF8;              /* texture pool size */

#include "game/gs_texture_init_gx.h"

static inline GStextureHandle* textureFindFree(void)
{
    GStextureHandle* tex = lbl_8047ABF4;
    u32 i;

    for (i = 0; i < lbl_8047ABF8; i++, tex++) {
        if (tex->inUse == 0) {
            return tex;
        }
    }
    return NULL;
}


GStextureHandle* GStextureCreate(s32 width, s32 height, s32 format, s32 tlutFormat,
                                 u8 mipLevels)
{
    s32 align;
    u16 w;
    GStextureHandle* tex;
    s32 levels;
    u32 i;
    u32 mipSize;
    u32 pixelCount;
    u16 adjWidth;
    u8 maxLevels;
    u16 adjHeight;
    u16 h;
    s32 tlutEntries;
    s32 level;

    if ((u16)width == 0 && (u16)height == 0) {
        width = *(u16*)(lbl_80466BC0 + 4);
        height = *(u16*)(lbl_80466BC0 + 6);
    }

    if ((u16)width > 0x400 || (u16)height > 0x400 || (u16)width < 4 || (u16)height < 4) {
        return NULL;
    }

    switch (format) {
    case 0x00:
    case 0x40:
    case 0x41:
    case 0xB0:
        align = 8;
        break;
    case 0x01:
    case 0x42:
    case 0x43:
    case 0xA0:
        align = 4;
        break;
    case 0x30:
    case 0x44:
    case 0x45:
    case 0x90:
        align = 4;
        break;
    default:
        GSlogWrite(lbl_80270F98);
        return NULL;
    }

    adjWidth = (width + align - 1) & ~(align - 1);
    adjHeight = (height + align - 1) & ~(align - 1);
    if (adjWidth != (u16)width || adjHeight != (u16)height) {
        GSlogWrite(lbl_80270FBC, width & 0xFFFF, height & 0xFFFF, adjWidth, adjHeight);
    }

    w = adjWidth;
    h = adjHeight;
    maxLevels = 0;
    while (w > 4 && h > 4 && maxLevels < 7) {
        w >>= 1;
        h >>= 1;
        maxLevels++;
    }
    if (mipLevels > maxLevels) {
        mipLevels = maxLevels;
    }

    tex = lbl_8047ABF4;
    for (i = 0; i < lbl_8047ABF8; i++, tex++) {
        if (tex->inUse == 0) {
            goto found;
        }
    }
    tex = NULL;
found:
    if (tex == NULL) {
        return NULL;
    }

    tlutEntries = 0;
    switch (format) {
    case 0x00:
        tlutEntries = 0x10;
    case 0x40:
    case 0xB0:
        tex->bitsPerPixel = 4;
        break;
    case 0x01:
        tlutEntries = 0x100;
    case 0x41:
    case 0x42:
    case 0xA0:
        tex->bitsPerPixel = 8;
        break;
    case 0x30:
        tlutEntries = 0x400;
    case 0x43:
    case 0x44:
    case 0x90:
        tex->bitsPerPixel = 16;
        break;
    case 0x45:
        tex->bitsPerPixel = 32;
        break;
    default:
        return NULL;
    }

    pixelCount = adjWidth * adjHeight;
    tex->totalSize = 0;
    levels = mipLevels + 1;
    mipSize = tex->bitsPerPixel * pixelCount / 8;
    for (level = 0; level < (u8)levels; level++) {
        tex->totalSize += (mipSize + 0x1F) & ~0x1F;
        mipSize >>= 1;
    }

    if (tlutEntries != 0) {
        if (tlutFormat != 0 && tlutFormat >= 0 && tlutFormat < 4) {
            tex->totalSize += (tlutEntries * 16) >> 3;
        } else {
            return NULL;
        }
    }

    tex->memHandle = fn_800E2C04(tex->totalSize, 0x20);
    if (tex->memHandle == 0) {
        return NULL;
    }
    tex->mipData[0] = fn_800E27B0(tex->memHandle);
    if (tex->mipData[0] == NULL) {
        fn_800E209C(tex->memHandle);
        return NULL;
    }

    tex->inUse = 1;
    tex->width = adjWidth;
    tex->height = adjHeight;
    tex->mipLevels = levels;
    tex->format = format;
    tex->tlutFormat = tlutFormat;
    tex->wrapS = 0;
    tex->wrapT = 0;
    tex->minFilter = 2;
    tex->magFilter = 2;
    if ((u8)levels > 1) {
        tex->lodClamp = 2;
    } else {
        tex->lodClamp = 0;
    }
    tex->refCount = 0;
    tex->unk52 = 0;

    mipSize = tex->bitsPerPixel * pixelCount / 8;
    for (level = 1; level < 8; level++) {
        if (level < tex->mipLevels) {
            tex->mipData[level] = (u8*)tex->mipData[level - 1] + mipSize;
            mipSize >>= 1;
        } else {
            tex->mipData[level] = NULL;
        }
    }

    switch (tlutFormat) {
    case 1:
    case 2:
    case 3:
        tex->tlutData = (u8*)tex->mipData[tex->mipLevels - 1] + mipSize;
        break;
    default:
        tex->tlutData = NULL;
        break;
    }

    textureInitGXObjects(tex);
    return tex;
}

/* fn_800EFD14 and GStextureLoad (0x800EFD14 - 0x800EFFC0) are linked from
 * gs_texture_exact_800EFD14.c. */
