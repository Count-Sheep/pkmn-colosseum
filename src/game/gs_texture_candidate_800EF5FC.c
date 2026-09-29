/**
 * @file gs_texture_candidate_800EF5FC.c
 * @brief GStexture pool: creation, loading and pool initialisation.
 *
 * Address range: 0x800EF5FC - 0x800EFD14 (GStextureCreate). The retail
 * GStexture.cpp spans 0x800EF098 - 0x800F0030; the earlier functions,
 * fn_800EFD14/GStextureLoad (gs_texture_exact_800EFD14.c) and GStextureInit
 * (gs_texture_exact_800EFFC0.c) live in their own dtk partitions.
 *
 * The unit stays a candidate
 * because GStextureCreate (95.4%) still differs in register allocation
 * only: retail colours the TLUT-format local of textureInitGXObjects (r31)
 * before the free-slot pointer (r30), where every textureFindFree helper
 * form colours the pointer first; retail also copies width/height into
 * r9/r8 at entry and keeps the incremented mip count in the parameter's
 * saved register (clrlwi r3,r29,24; addi r29,r3,1).
 * A 2026-09-28 follow-up tested explicit effective width/height locals
 * (95.08%, worse), 178 semantics-preserving rewrites, and 150 declaration
 * permutations (both unchanged at 95.38106% raw objdiff). The tail remains
 * an r30/r31 conflict between the free slot and GX TLUT format. Neither an
 * exact function nor a linked object has been established.
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
    u16 adjHeight;
    s32 align;
    u16 w;
    u16 h;
    u32 pixelCount;
    u32 mipSize;
    s32 tlutEntries;
    s32 level;
    GStextureHandle* tex;
    u8 maxLevels;
    u16 adjWidth;

    if ((u16)width == 0 && (u16)height == 0) {
        height = *(u16*)(lbl_80466BC0 + 6);
        width = *(u16*)(lbl_80466BC0 + 4);
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
        GSlogWrite(lbl_80270FBC, (u16)width, (u16)height, adjWidth, adjHeight);
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

    tex = textureFindFree();
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
    mipLevels++;
    tex->totalSize = 0;
    mipSize = tex->bitsPerPixel * pixelCount / 8;
    for (level = 0; level < mipLevels; level++) {
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
    tex->mipLevels = mipLevels;
    tex->format = format;
    tex->tlutFormat = tlutFormat;
    tex->wrapS = 0;
    tex->wrapT = 0;
    tex->minFilter = 2;
    tex->magFilter = 2;
    if (mipLevels > 1) {
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
