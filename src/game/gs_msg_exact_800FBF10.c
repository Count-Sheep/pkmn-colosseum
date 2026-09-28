/**
 * @file gs_msg_exact_800FBF10.c
 * @brief GSmsgDaemon, 0x800FBF10 - 0x800FBF74.
 *
 * Function-boundary carve of the GSmsg TU (see gs_msg.c): flush the glyph
 * atlas being drawn and flip to the other one. No jump table, no pooled
 * constant; its only data is the .sdata message-system pointer
 * (lbl_80478B08), kept extern. GC/1.3 -O4,p with the TU's unit-wide
 * -opt nopeephole, no pragmas.
 */
#include "dolphin/types.h"
#include "game/gs_texture.h"

/* The message-system record (layout as in gs_msg.c; only the fields used
 * here are named). */
struct MessageSystem {
    u16 taskCount;                   /* 0x00 */
    u16 taskHandle;                  /* 0x02 */
    u16 fontCount;                   /* 0x04 */
    u16 fontHandle;                  /* 0x06 */
    void* groups;                    /* 0x08 */
    GStextureHandle* textures[2];    /* 0x0C: double-buffered glyph atlas */
    void* image;                     /* 0x14: locked atlas image */
    s16 atlasX;                      /* 0x18: glyph atlas cursor */
    s16 atlasY;                      /* 0x1A */
    u8 unk1C;                        /* 0x1C */
    s8 textureIndex;                 /* 0x1D */
    u8 reserved_1E[2];               /* 0x1E */
    u8* tasks;                       /* 0x20 */
    void* fonts;                     /* 0x24 */
    void* controls;                  /* 0x28 */
};

extern struct MessageSystem* lbl_80478B08;

void GSmsgDaemon(void) {
    GStextureUnlockImage(lbl_80478B08->textures[lbl_80478B08->textureIndex]);
    lbl_80478B08->atlasX = 2;
    lbl_80478B08->atlasY = 1;
    lbl_80478B08->textureIndex ^= 1;
}
