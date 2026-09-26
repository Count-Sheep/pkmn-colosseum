/**
 * GSmaterialResetTexture (0x800DEFC8-0x800DF028).
 *
 * First function of the GSmaterial code: GSmaterialSetTexture, right after
 * it at 0x800DF028, keeps the material's original image descriptor at +0x38
 * (0xFEFEFEFE = nothing saved) before binding a GStexture image to the
 * MObj's TObj; this puts the saved descriptor back and frees the one
 * GSmaterialSetTexture allocated. It references no data, so it links as a
 * text-only object; the rest of the GSmaterial range stays in its own
 * candidates.
 */
#include "dolphin/types.h"
#include "hsd/hsd_mobj.h"
#include "hsd/hsd_tobj.h"

#define GSMAT_NO_SAVED_IMAGE ((HSD_ImageDesc*)0xFEFEFEFE)

extern void GXDrawDone(void);

void GSmaterialResetTexture(u8* material)
{
    HSD_TObj* tobj;
    HSD_ImageDesc* bound;

    if (*(HSD_ImageDesc**)(material + 0x38) == GSMAT_NO_SAVED_IMAGE) {
        return;
    }
    /* The GPU may still be reading the bound image. */
    GXDrawDone();
    tobj = (*(HSD_MObj**)(material + 0x8))->tobj;
    bound = tobj->imagedesc;
    tobj->imagedesc = *(HSD_ImageDesc**)(material + 0x38);
    HSD_ImageDescFree(bound);
    *(HSD_ImageDesc**)(material + 0x38) = GSMAT_NO_SAVED_IMAGE;
}
