/**
 * @file hero_move_exact_8012FAD8.c
 * @brief Field model selection for the partner on the next floor.
 *
 * Text-only carve of heroMoveGetKenObjID, 0x8012FAD8-0x8012FCD4.
 * The source tables remain named external data, as in the other hero-move
 * candidate partitions. This object is linked only when the full retail
 * DOL and REL hashes confirm it.
 */
#include "dolphin/types.h"

extern u8 lbl_802729C0[];
extern u8 lbl_80272A10[];
extern u32 fn_801906A0(u32 id);
extern u32 floorGetNextFloorID(void);
extern s32 fn_8006AE18(void);

typedef struct HeroMoveThemeTable {
    u32 words[10];
} HeroMoveThemeTable;

typedef struct HeroMoveFloorTable {
    u32 words[20];
} HeroMoveFloorTable;

u32 heroMoveGetKenObjID(void)
{
    HeroMoveFloorTable floors = *(HeroMoveFloorTable*)lbl_802729C0;
    HeroMoveThemeTable themes = *(HeroMoveThemeTable*)lbl_80272A10;
    u8 flagClear = fn_801906A0(0x8AE) == 0;
    u32 floor;
    s32 area;
    s32 i;

    if (flagClear) {
        return 0x00F70400;
    }
    floor = floorGetNextFloorID();
    for (i = 0; i < 20; i++) {
        if (floor == floors.words[i]) {
            break;
        }
    }
    if (i >= 20) {
        return 0x00F70400;
    }
    area = fn_8006AE18();
    for (i = 0; i < 5; i++) {
        if (area == (s32)themes.words[i * 2]) {
            break;
        }
    }
    return themes.words[i * 2 + 1];
}
