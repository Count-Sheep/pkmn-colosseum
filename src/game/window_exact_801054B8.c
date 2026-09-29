/**
 * @file window_exact_801054B8.c
 * @brief windowGetPortKeyInfo (0x801054B8 - 0x80105624): merges the key
 *        state of every pad port selected in a port mask into one
 *        MenuKeyInfo buffer and returns it.
 *
 * Function-boundary carve of the window TU, built with its flags (GC/2.5,
 * -O4,p, "-opt nopeephole"; see window_exact_80104318.c). Text only. XD's
 * windowGetPortKeyInfo (TeamOrre/xd-decomp config/GXXE01/symbols.txt
 * @4989794e, 0x80116804, 0x16C; trevor403/xd-asm @b1087f18) has the same
 * size and instructions.
 *
 * The {1, 2, 4, 8} port-mask initializer is the window TU's own .sdata2
 * pool entry (0x8047CDE8); this carve reads it through an extern stand-in,
 * as the window carves do for the TU's 1.0f.
 */
#include "dolphin/types.h"

extern void* memset(void* dst, int val, u32 size);

typedef struct MenuKeyInfo {
    u16 keys;
    u16 previousKeys;
    u16 pressedKeys;
    u16 repeatKeys;
    u16 heldKeys;
    s8 repeatTimer[16];
} MenuKeyInfo;

/* Window system work (WINDOW_SYS_WORK). */
typedef struct MenuSystem {
    u8 pad_00[0x10];
    MenuKeyInfo keyInfo;
    MenuKeyInfo portKeyInfo[4];
    u8 enablePort;
} MenuSystem;

typedef struct PortMasks {
    u8 mask[4];
} PortMasks;

extern MenuSystem lbl_80404ACC;
extern MenuKeyInfo lbl_80404AB0; /* the merged key info handed out */
/* RULE-EXCEPTION(title-path): extern stand-in for the window TU's pool initializer - see docs/RULE_EXCEPTIONS.md */
extern const PortMasks lbl_8047CDE8;

/* 0x801054B8 | 0x16C */
MenuKeyInfo* windowGetPortKeyInfo(u8 port)
{
    PortMasks masks = lbl_8047CDE8;
    s32 i;

    memset(&lbl_80404AB0, 0, sizeof(MenuKeyInfo));
    for (i = 0; i < 4; i++) {
        MenuKeyInfo* info = &lbl_80404ACC.portKeyInfo[i];

        if (port & masks.mask[i]) {
            lbl_80404AB0.keys |= info->keys;
            lbl_80404AB0.previousKeys |= info->previousKeys;
            lbl_80404AB0.pressedKeys |= info->pressedKeys;
            lbl_80404AB0.repeatKeys |= info->repeatKeys;
            lbl_80404AB0.heldKeys |= info->heldKeys;
        }
    }
    return &lbl_80404AB0;
}
