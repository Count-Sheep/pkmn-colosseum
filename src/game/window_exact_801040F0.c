/**
 * @file window_exact_801040F0.c
 * @brief windowDrawSprite (0x801040F0 - 0x80104160).
 *
 * Draws a menu-sprite BIOS entry at its own size through windowDrawSprite2.
 * Window TU flags (GC/2.5, -O4,p, "-opt nopeephole"; see
 * window_exact_80104318.c). Carved so it can link while windowDrawSprite2
 * stays a candidate.
 */
#include "dolphin/types.h"

extern u8* menuSpriteBiosGetPtr(s32 id);
extern void windowDrawSprite2(void* x, void* y, s16 width, s16 height,
                              s32 color, s32 context, s32 spriteId, u32 flags);

/* 0x801040F0 | 0x70 */
void windowDrawSprite(void* x, void* y, void* context, u32 spriteId, u32 flags)
{
    if ((u16)spriteId != 0) {
        u8* sprite = menuSpriteBiosGetPtr((u16)spriteId);

        windowDrawSprite2(x, y, *(s16*)(sprite + 0x0C), *(s16*)(sprite + 0x0E),
                          -1, (s32)context, spriteId, flags);
    }
}
