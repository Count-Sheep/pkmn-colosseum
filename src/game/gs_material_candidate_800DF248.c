/**
 * GSmaterialDisableExtension, GSmaterialEnableExtension and
 * GSmaterialGetFlags (0x800DF248 - 0x800DF470).
 *
 * XD source unit: game/pxdvs/GSAPI/GSmaterial/GSmaterial.cpp. Standalone
 * source for this split range; text only.
 */
#include "dolphin/types.h"

extern void _matGSmatEnableEnvMapExt(u8*);
extern void HSD_ImageDescFree(u32);
extern void HSD_TObjRemove(void*);
extern void HSD_MObjCompileTev(void*);
extern u32 HSD_MObjGetFlags(void*);

void GSmaterialDisableExtension(u8* obj, u32 flags) {
    u8* material = obj;
    u8* image;
    u8* it;
    u8* list;
    u32 desc;
    u32 bits;
    u8* mobj;

    bits = flags & *(u16*)(material + 0x2);
    mobj = *(u8**)(material + 0x8);
    if (bits & 1) {
        material[0xf] = 0x7f;
        material[0xe] = 0x7f;
        material[0xd] = 0x7f;
        material[0xc] = 0x7f;
    }
    if (bits & 2) {
        *(u32*)(material + 0x10) = 0;
        *(u32*)(material + 0x14) = 1;
        *(u32*)(material + 0x18) = 2;
        *(u32*)(material + 0x1c) = 3;
    }
    if (bits & 4) {
        list = *(u8**)(material + 0x8);
        image = *(u8**)(material + 0x20);
        it = *(u8**)(list + 0x8);
        desc = *(u32*)(material + 0x24);
        if (it != 0 && image != 0) {
            if (it == image) {
                *(u32*)(list + 0x8) = *(u32*)(image + 0x8);
            } else {
                while (it != 0) {
                    if (*(u8**)(it + 0x8) == image) {
                        *(u32*)(it + 0x8) = *(u32*)(image + 0x8);
                    }
                    it = *(u8**)(it + 0x8);
                }
            }
            if (desc != 0) {
                HSD_ImageDescFree(desc);
            }
            if (image != 0) {
                HSD_TObjRemove(image);
            }
        }
        *(u32*)(material + 0x28) = 0;
    }

    *(u16*)(material + 0x2) &= ~bits;
    if (*(u16*)(material + 0x2) == 0) {
        *(u32*)(mobj + 0x20) = 0;
    }
    HSD_MObjCompileTev(mobj);
}

void GSmaterialEnableExtension(u8* obj, u32 flags) {
    u32 new_bits;
    u8* ptr;
    u16 cur;
    cur = *(u16*)(obj + 0x2);
    ptr = *(u8**)(obj + 0x8);
    new_bits = flags & ~cur;
    if (!new_bits) return;
    *(u8**)(ptr + 0x20) = obj;
    if (new_bits & (1 << 2)) _matGSmatEnableEnvMapExt(obj);
    *(u16*)(obj + 0x2) = *(u16*)(obj + 0x2) | new_bits;
    HSD_MObjCompileTev(ptr);
}

u32 GSmaterialGetFlags(void* obj) {
    u32 v = HSD_MObjGetFlags(*(void**)((u8*)obj + 8));
    u32 flags = 0;
    if (v & 0x1) flags |= 0x1;
    if (v & 0x2) flags |= 0x2;
    if (v & 0x4) flags |= 0x4;
    if (v & 0x8) flags |= 0x8;
    if (v & 0x40000000) flags |= 0x10;
    if (v & 0x2000) flags |= 0x20;
    if (v & 0x4000) flags |= 0x40;
    return flags;
}
