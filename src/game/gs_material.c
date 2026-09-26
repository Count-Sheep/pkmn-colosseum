/**
 * @file gs_material.c
 * @brief GSmaterialSetTexture, 0x800DF028 - 0x800DF11C.
 *
 * XD source unit: game/pxdvs/GSAPI/GSmaterial/GSmaterial.cpp. Standalone
 * source for this split range; the rest of the material TU (research copy
 * used by the candidate wrappers) lives in gs_material_range_800DF028.c.
 */

#include "dolphin/types.h"

extern void* HSD_ImageDescAlloc(void);
extern void* GStextureLockImage(void* texture, u32 wait);
extern u32 GStextureUnlockImage(void* texture);
extern u16 GStextureGetXsize(void* texture);
extern u16 GStextureGetYsize(void* texture);
extern void* GStextureGetGXformat(void* texture, u32 kind);
extern u8 GStextureGetMiplevels(void* texture);
extern void GXDrawDone(void);

extern f32 lbl_8047CAC8;

/* Point the material's first texture object at `image`, allocating the
 * HSD_ImageDesc on first use. */
void GSmaterialSetTexture(u8* obj, void* image) {
    void* desc;
    u32 transparent;
    f32 scale;

    if ((*(u32*)(obj + 0x38) + 0x01020000) == 0xfefe) {
        *(u32*)(obj + 0x38) = *(u32*)(*(u8**)(*(u8**)(obj + 0x8) + 0x8) + 0x58);
        desc = HSD_ImageDescAlloc();
    } else {
        desc = *(void**)(*(u8**)(*(u8**)(obj + 0x8) + 0x8) + 0x58);
    }

    if (desc != 0) {
        *(void**)desc = GStextureLockImage(image, 0);
        *(u16*)((u8*)desc + 0x4) = GStextureGetXsize(image);
        *(u16*)((u8*)desc + 0x6) = GStextureGetYsize(image);
        *(void**)((u8*)desc + 0x8) = GStextureGetGXformat(image, 1);
        transparent = GStextureGetMiplevels(image);
        scale = lbl_8047CAC8;
        *(u32*)((u8*)desc + 0xc) = ((0 - transparent) | transparent) >> 31;
        *(f32*)((u8*)desc + 0x10) = scale;
        *(f32*)((u8*)desc + 0x14) = scale;
        GStextureUnlockImage(image);
        GXDrawDone();
        *(void**)(*(u8**)(*(u8**)(obj + 0x8) + 0x8) + 0x58) = desc;
    }
}
