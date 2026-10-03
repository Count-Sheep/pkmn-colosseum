/* Name-entry backdrop sprite drawing, 0x80028830-0x80028948.
 * RULE-EXCEPTION(user-approved): retained register-qualified locals;
 * see docs/RULE_EXCEPTIONS.md.
 */
#include "dolphin/types.h"

typedef struct WorldMapOverlay {
    s32 active;
    f32 x;
    f32 y;
    f32 scale;
    f32 unused10;
    u32 color;
    u8 alpha;
    u8 pad19[3];
    f32 timer;
    f32 lifetime;
} WorldMapOverlay;

extern WorldMapOverlay lbl_803A20DC[];
extern void* menuSpriteBiosGetPtr(s32);
extern void windowDrawSprite2(s32, s32, s32, s32, u32, void*, s32, s32);
extern f32 lbl_8047B940;

s32 menuNameEntryBackDrawBall(void* r3) {
    void* r27;
    WorldMapOverlay* r31;
    register s32 r30;
    register s32 r29;
    register s32 r28;
    f32 scale;
    f32 sx;
    f32 sy;
    s32 x0;
    s32 y0;
    s32 x1;
    s32 y1;

    r27 = r3;
    r29 = *(s16*)((u8*)menuSpriteBiosGetPtr(0x98) + 0xc);
    r28 = *(s16*)((u8*)menuSpriteBiosGetPtr(0x98) + 0xe);
    r30 = 0;
    while (r30 < 0x1e) {
        r31 = &lbl_803A20DC[r30];
        if (r31->active != 0) {
            scale = r31->scale;
            sx = (f32)r29 * scale;
            sy = (f32)r28 * scale;
            x1 = (s32)(lbl_8047B940 + sx);
            x0 = (s32)(lbl_8047B940 + (r31->x - sx * lbl_8047B940));
            y1 = (s32)(lbl_8047B940 + sy);
            y0 = (s32)(lbl_8047B940 + (r31->y - sy * lbl_8047B940));
            windowDrawSprite2(x0, y0, x1, y1, r31->color | r31->alpha, r27, 0x98, 0);
        }
        r30++;
    }
    return 0;
}
