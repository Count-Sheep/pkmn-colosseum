/**
 * @file gs_texture_candidate_800EF5FC.c
 * @brief GStexture pool: creation, loading and pool initialisation.
 *
 * Address range: 0x800EF5FC - 0x800F0030 (GStextureCreate, fn_800EFD14,
 * GStextureLoad, GStextureInit). The retail GStexture.cpp spans
 * 0x800EF098 - 0x800F0030; the earlier functions live in their own
 * dtk partitions.
 */

#include "dolphin/types.h"
#include "game/gs_texture.h"

extern void GSlogWrite(const char* format, ...);
extern u16 fn_800E2C04(u32 size, u32 alignment);   /* GSmemAlloc */
extern u16 _toolentryAlloc__FUl(u32 size);         /* GSmemAllocRaw */
extern void* fn_800E27B0(u16 handle);              /* GSmemGetPtr */
extern void fn_800E209C(u16 handle);               /* GSmemFree */
extern void fn_800BB050(void* tlutObj, void* data, u32 format, u32 entries); /* GXInitTlutObj */
extern void fn_800BA9E4(void* texObj, void* data, u16 width, u16 height, u32 format,
                        u32 wrapS, u32 wrapT, u32 mipmap);                   /* GXInitTexObj */

extern const char lbl_80270F98[]; /* "GStexture: invalid texture format" */
extern const char lbl_80270FBC[]; /* "GStexture: warning -- texture size adjusted from [%d,%d] to [%d,%d]" */

extern u8 lbl_80466BC0[]; /* current display descriptor (width at +4, height at +6) */

extern u16 lbl_8047ABF0;              /* texture pool GSmem handle */
extern GStextureHandle* lbl_8047ABF4; /* texture pool */
extern u32 lbl_8047ABF8;              /* texture pool size */

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

/*
 * Shared tail of GStextureCreate and GStextureLoad: the target expands the
 * same GX format / TLUT setup sequence in both functions.
 */
static inline void textureInitGXObjects(GStextureHandle* tex)
{
    s32 gxFormat;
    u32 tlutEntries;
    u32 tlutFormat;

    tlutEntries = 0;
    switch (tex->format) {
    case 0x00:
        gxFormat = 0x08;
        break;
    case 0x01:
        gxFormat = 0x09;
        break;
    case 0x30:
        gxFormat = 0x0A;
        break;
    case 0x40:
        gxFormat = 0x00;
        break;
    case 0x41:
        gxFormat = 0x02;
        break;
    case 0x42:
        gxFormat = 0x01;
        break;
    case 0x43:
        gxFormat = 0x03;
        break;
    case 0x44:
        gxFormat = 0x04;
        break;
    case 0x45:
        gxFormat = 0x06;
        break;
    case 0x90:
        gxFormat = 0x05;
        break;
    case 0xB0:
        gxFormat = 0x0E;
        break;
    case 0xA0:
        gxFormat = 0x01;
        break;
    default:
        gxFormat = -1;
        break;
    }

    if (tex->tlutData != NULL) {
        switch (tex->format) {
        case 0x00:
            tlutEntries = 0x10;
            break;
        case 0x01:
            tlutEntries = 0x100;
            break;
        case 0x30:
            tlutEntries = 0x400;
            break;
        }

        /* Retail leaves the GX TLUT format unset for an unknown TLUT format. */
        switch (tex->tlutFormat) {
        case 1:
            tlutFormat = 0;
            break;
        case 2:
            tlutFormat = 1;
            break;
        case 3:
            tlutFormat = 2;
            break;
        }

        fn_800BB050(tex->gxTlutObj, tex->tlutData, tlutFormat, tlutEntries);
    }

    fn_800BA9E4(tex->gxTexObj, tex->mipData[0], tex->width, tex->height, gxFormat, 0, 0,
                tex->mipLevels > 1);
    tex->dirty = 1;
}

GStextureHandle* GStextureCreate(s32 width, s32 height, s32 format, s32 tlutFormat,
                                 u8 mipLevels)
{
    u16 adjWidth;
    u16 adjHeight;
    s32 align;
    u8 maxLevels;
    u16 w;
    u16 h;
    u32 pixelCount;
    u32 mipSize;
    s32 tlutEntries;
    s32 level;
    GStextureHandle* tex;

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
        if (tlutFormat == 0 || tlutFormat < 0 || tlutFormat >= 4) {
            return NULL;
        }
        tex->totalSize += (tlutEntries * 16) / 8;
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

void fn_800EFD14(GStextureHandle* tex, u16 handle)
{
    if (tex == NULL || tex->inUse != 0) {
        return;
    }
    tex->inUse = 1;
    tex->memHandle = handle;
}

GStextureHandle* GStextureLoad(GStextureHandle* tex)
{
    s32 i;

    for (i = 0; i < tex->mipLevels; i++) {
        tex->mipData[i] = (u8*)tex + (u32)tex->mipData[i];
    }
    for (; i < 8; i++) {
        tex->mipData[i] = NULL;
    }
    if (tex->tlutData != NULL) {
        tex->tlutData = (u8*)tex + (u32)tex->tlutData;
    }

    textureInitGXObjects(tex);
    tex->memHandle = 0;
    return tex;
}

void GStextureInit(u32 count)
{
    u32 i;

    lbl_8047ABF8 = count;
    lbl_8047ABF0 = _toolentryAlloc__FUl(count << 7);
    if (lbl_8047ABF0 == 0) {
        return;
    }

    lbl_8047ABF4 = fn_800E27B0(lbl_8047ABF0);
    for (i = 0; i < lbl_8047ABF8; i++) {
        ((u8*)lbl_8047ABF4)[i * 0x80 + 6] = 0;
    }
}
