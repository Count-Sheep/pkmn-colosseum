/**
 * GXTev.c tail: GXSetAlphaCompare, GXSetZTexture and GXSetTevOrder,
 * 0x800BC618 - 0x800BC8C8.
 */
#define SDK_800BC618_SUFFIX_ACTIVE
#define SDK_800BC618_ONLY
#include "src/dolphin/sdk_range_800BB30C.c"

extern u32 lbl_803135E0[];

#define TEV_SET_REG(field, pos, size, value) \
    (field) = ((field) & ~(((1 << (size)) - 1) << (31 - (pos) - (size) + 1))) | \
              ((int)(value) << (31 - (pos) - (size) + 1))

/* GXSetTevOrder */
void fn_800BC6F0(int stage, int coord, int map, int color)
{
    u32* reg;
    u32 tempMap;
    u32 tempCoord;

    reg = &gx->tref[stage / 2];
    gx->texmapId[stage] = map;

    tempMap = map & ~0x100;
    tempMap = (tempMap >= 8) ? 0 : tempMap;

    if (coord >= 8) {
        tempCoord = 0;
        gx->tevTcEnab = gx->tevTcEnab & ~(1 << stage);
    } else {
        tempCoord = coord;
        gx->tevTcEnab = gx->tevTcEnab | (1 << stage);
    }

    if (stage & 1) {
        TEV_SET_REG(*reg, 17, 3, tempMap);
        TEV_SET_REG(*reg, 14, 3, tempCoord);
        TEV_SET_REG(*reg, 10, 3, (color == 0xFF ? 7 : lbl_803135E0[color]));
        TEV_SET_REG(*reg, 13, 1, ((map != 0xFF) && !(map & 0x100)));
    } else {
        TEV_SET_REG(*reg, 29, 3, tempMap);
        TEV_SET_REG(*reg, 26, 3, tempCoord);
        TEV_SET_REG(*reg, 22, 3, (color == 0xFF ? 7 : lbl_803135E0[color]));
        TEV_SET_REG(*reg, 25, 1, ((map != 0xFF) && !(map & 0x100)));
    }

    GX_BP_REG(*reg);
    gx->field_002 = 0;
    gx->dirtyState |= 1;
}
