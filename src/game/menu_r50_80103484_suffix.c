/**
 * @file menu_r50_80103484_suffix.c
 * @brief menu engine, 0x80103484-0x80103E68: menuPlaySe, the GC/AGB pad
 *        readers, the per-frame key-repeat update, menuGetKeyInfo, the
 *        enable-port accessors and menuInit.
 *
 * Standalone source for this split range, in retail address order, built
 * like the rest of the menu TU (see configure.py). Still a CodeCandidate:
 * - _menuGetGcKeyInfo's code matches, but its pad-ID initializer and float
 *   literals are compiler-pooled data that retail keeps in .rodata
 *   0x80271E00 and in the .sdata2 pool 0x8047CDC0-0x8047CDE0, which it
 *   shares with the 0x80102014 range (50.0f at 0x8047CDC4 is only used
 *   there). This range can only link once the whole menu TU is one unit that
 *   owns that data.
 * - Under the TU's -inline auto, GC/1.3.2 inlines _menuGetAgbKeyInfo into
 *   _menuUpdateKeyInfo; retail calls it. With -inline noauto this source
 *   matches _menuUpdateKeyInfo exactly, but the 0x80102014 range needs auto
 *   inlining.
 */
#include "dolphin/types.h"

typedef struct MenuKeyInfo {
    u16 keys;
    u16 previousKeys;
    u16 pressedKeys;
    u16 repeatKeys;
    u16 heldKeys;
    s8 repeatTimer[16];
} MenuKeyInfo;

/* Open window (tagWINDOW_WORK); only the input-port byte is used here. */
typedef struct MenuWindow {
    u8 pad_00[0x0B];
    u8 keyPort;
} MenuWindow;

/* Window system work (WINDOW_SYS_WORK). */
typedef struct MenuSystem {
    u8 pad_00[0x10];
    MenuKeyInfo keyInfo;        /* keys of the active window's port(s) */
    MenuKeyInfo portKeyInfo[4]; /* per-port state, updated every frame */
    u8 enablePort;
} MenuSystem;

typedef struct MenuData {
    u8 soundGroup;
} MenuData;

extern MenuSystem lbl_80404ACC;
extern u32 lbl_8047AD00;   /* menu texture-resource group, created once */
extern u8 lbl_8047AD04[4]; /* input type per port: 0 none, 1 GC pad, 2 AGB, 3 AGB lost */
extern u8 lbl_8047AD08[4]; /* the same, one frame earlier */
extern u8 lbl_803156E0[];
extern u8 lbl_803254E0[];
extern u8 lbl_803357E0[];

extern void* memset(void* dst, int val, u32 size);
extern const MenuData* menuDataBiosGetPtr(u32 id);
extern void* menuSeBiosGetPtr(s32 group);
extern u16 fn_8005D798(void*, s32);
extern int fn_80166A28(u16);
extern u8 fn_8008ABA0(s32);
extern s32 fn_8008AB8C(s32);
extern u8 fn_800F7EF8(s32 pad_id);
extern u32 fn_800F7A08(s32 pad_id, s32 axis);
extern u32 fn_800F7A7C(s32 pad_id, s32 axis);
extern u32 fn_800F7BC4(s32 pad_id);
extern f64 atan2(f64 y, f64 x);
extern u32 fn_800D3088(void);
extern MenuWindow* windowSearchID(u32 id);
extern u32 windowGetActiveID(void);
extern MenuKeyInfo* windowGetPortKeyInfo(u8 ports);
extern void windowInit(u16 count);
extern void cursorBiosInit(void);
extern void menuOffScreenInit(void);
extern void fn_8010C224(u32 count);
extern u32 fn_800D7894(void);
extern void fn_800D7868(u32, u32, u32, u32, u32, u32, u32, u32);
extern void* GStextureLoad(void* data);
extern void GSresRegisterResource(void* texture, u32, u32 resourceId, u32);

/* 0x80103484 | 0x58 */
void menuPlaySe(u32 id, s32 event) {
    void* sound = menuSeBiosGetPtr(menuDataBiosGetPtr(id)->soundGroup & 7);

    if (sound != NULL) {
        u32 se = fn_8005D798(sound, event);
        if (se != 0) {
            fn_80166A28(se);
        }
    }
}

/* 0x801034DC | 0x138 */
u8 _menuGetAgbKeyInfo__FlPUs(s32 port, u16* keys) {
    s32 device;
    u16 buttons;
    u16 result;

    device = port + 1;
    result = 0;
    if (fn_8008ABA0(device) == 0) {
        return 0;
    }

    buttons = fn_8008AB8C(device);
    if (buttons & 0x40) result |= 0x1;
    if (buttons & 0x80) result |= 0x2;
    if (buttons & 0x20) result |= 0x4;
    if (buttons & 0x10) result |= 0x8;
    if (buttons & 0x1) result |= 0x10;
    if (buttons & 0x2) result |= 0x20;
    if (buttons & 0x4) result |= 0x100;
    if (buttons & 0x200) result |= 0x200;
    if (buttons & 0x100) result |= 0x400;
    if (buttons & 0x8) result |= 0x800;

    *keys = result;
    return 1;
}

/* 0x80103614 | 0x2E4 */
u8 _menuGetGcKeyInfo__FlPUs(s32 port, u16* keys) {
    s32 padIds[4] = { 1, 2, 3, 4 };
    u16 result = 0;
    s32 padId;
    u32 stickX;
    u32 stickY;
    u32 buttons;
    f32 angle;

    padId = padIds[port];
    if (fn_800F7EF8(padId) == 0) {
        return 0;
    }

    stickX = fn_800F7A08(padId, 0);
    stickY = fn_800F7A7C(padId, 0);
    if (((s8)stickY < 0 ? -(s8)stickY : (s8)stickY) > 0x20 ||
        ((s8)stickX < 0 ? -(s8)stickX : (s8)stickX) > 0x20) {
        angle = atan2((s8)stickY, (s8)stickX);
        if ((angle > 0.0f ? angle : -angle) < 0.9599311f) {
            result |= 2;
        } else if ((angle > 0.0f ? angle : -angle) > 2.1816616f) {
            result |= 1;
        }
        if (0.61086524f < (angle > 0.0f ? angle : -angle) &&
            (angle > 0.0f ? angle : -angle) < 2.5307274f) {
            if (angle < 0.0f) {
                result |= 4;
            } else {
                result |= 8;
            }
        }
    }

    buttons = fn_800F7BC4(padId);
    if (buttons & 0x8) result |= 0x1;
    if (buttons & 0x4) result |= 0x2;
    if (buttons & 0x1) result |= 0x4;
    if (buttons & 0x2) result |= 0x8;
    if (buttons & 0x100) result |= 0x10;
    if (buttons & 0x200) result |= 0x20;
    if (buttons & 0x400) result |= 0x40;
    if (buttons & 0x800) result |= 0x80;
    if (buttons & 0x10) result |= 0x100;
    if (buttons & 0x40) result |= 0x200;
    if (buttons & 0x20) result |= 0x400;
    if (buttons & 0x1000) result |= 0x800;

    *keys = result;
    return 1;
}

/* 0x801038F8 | 0x2B0 */
/* Retail ignores the work argument and updates the global directly. */
void _menuUpdateKeyInfo__FP15WINDOW_SYS_WORK(MenuSystem* work) {
    MenuWindow* window;
    MenuKeyInfo* keyInfo;
    s32 i;
    s32 port;
    u16 keys;
    u16 pressed;
    u16 repeat;
    u16 held;
    u16 bit;
    u8 type;

    for (port = 0; port < 4; port++) {
        keyInfo = &lbl_80404ACC.portKeyInfo[port];
        lbl_8047AD08[port] = lbl_8047AD04[port];
        keys = 0;
        if (_menuGetGcKeyInfo__FlPUs(port, &keys) == 0) {
            if (_menuGetAgbKeyInfo__FlPUs(port, &keys) == 0) {
                keys = 0;
                type = 0;
            } else {
                type = 2;
            }
        } else {
            type = 1;
        }

        /* The case lists are the values the retail compare trees test. */
        switch (type) {
        case 0:
            switch (lbl_8047AD08[port]) {
            case 0:
            case 1:
            default:
                lbl_8047AD04[port] = 0;
                break;
            case 2:
            case 3:
                /* An AGB was unplugged: report it once as key 0x8000. */
                lbl_8047AD04[port] = 3;
                keys = 0x8000;
                break;
            }
            break;
        case 1:
            lbl_8047AD04[port] = 1;
            break;
        case 2:
            switch (lbl_8047AD08[port]) {
            case 0:
            case 2:
            case 3:
            default:
                lbl_8047AD04[port] = 2;
                break;
            case 1:
                lbl_8047AD04[port] = 2;
                break;
            }
            break;
        default:
            lbl_8047AD04[port] = type;
            break;
        }

        keyInfo->previousKeys = keyInfo->keys;
        pressed = (keyInfo->previousKeys ^ 0xFFFF) & keys;
        repeat = 0;
        held = 0;
        for (i = 0; i < 16; i++) {
            bit = 1 << i;
            if (pressed & bit) {
                keyInfo->repeatTimer[i] = 15;
                repeat |= bit;
            } else if (keys & bit) {
                keyInfo->repeatTimer[i] -= fn_800D3088();
                if (keyInfo->repeatTimer[i] <= 0) {
                    keyInfo->repeatTimer[i] = 5;
                    repeat |= bit;
                    held |= bit;
                } else {
                    held |= bit & keyInfo->heldKeys;
                }
            }
        }
        if (pressed & 0xF) {
            keyInfo->repeatTimer[0] = 15;
            keyInfo->repeatTimer[1] = 15;
            keyInfo->repeatTimer[2] = 15;
            keyInfo->repeatTimer[3] = 15;
        }
        keyInfo->keys = keys;
        keyInfo->pressedKeys = pressed;
        keyInfo->repeatKeys = repeat;
        keyInfo->heldKeys = held;
    }

    window = windowSearchID(windowGetActiveID());
    if (window == NULL) {
        type = 1;
    } else {
        type = window->keyPort;
    }
    lbl_80404ACC.keyInfo = *windowGetPortKeyInfo(type);
}

/* 0x80103BA8 | 0x108 */
void menuGetKeyInfo(MenuKeyInfo* out, s32 port) {
    MenuKeyInfo info;
    u8 mask;

    switch (port) {
    case 1: mask = 1; break;
    case 2: mask = 2; break;
    case 3: mask = 4; break;
    case 4: mask = 8; break;
    default: mask = 0; break;
    }
    if (mask != 0) {
        info = *windowGetPortKeyInfo(mask);
    } else {
        memset(&info, 0, sizeof(info));
    }
    *out = info;
}

/* 0x80103CB0 | 0x10 */
u8 menuGetEnablePort(void) {
    return lbl_80404ACC.enablePort;
}

/* 0x80103CC0 | 0x18 */
u8 menuSetEnablePort(u8 enable) {
    u8 old = lbl_80404ACC.enablePort;
    lbl_80404ACC.enablePort = enable;
    return old;
}

/* 0x80103CD8 | 0x190 */
void menuInit(u16 windowCount) {
    void* texture;
    s32 port;

    windowInit(windowCount);
    cursorBiosInit();
    menuOffScreenInit();
    fn_8010C224(0x18);
    lbl_80404ACC.enablePort = 1;
    for (port = 0; port < 4; port++) {
        lbl_8047AD04[port] = 0;
        lbl_8047AD08[port] = 0;
    }

    if (lbl_8047AD00 == 0) {
        lbl_8047AD00 = fn_800D7894();
        fn_800D7868(lbl_8047AD00, 1, 0, 0, 3, 0, 0, 0);
        fn_800D7868(lbl_8047AD00, 4, 0, 6, 10, 0, 0, 0);
        fn_800D7868(lbl_8047AD00, 6, 0, 8, 4, 0, 0, 0);
        fn_800D7868(lbl_8047AD00, 7, 0, 8, 4, 0, 0, 0);
        fn_800D7868(lbl_8047AD00, 8, 0, 8, 4, 0, 0, 0);
    }

    texture = GStextureLoad(lbl_803156E0);
    GSresRegisterResource(texture, 0, 0x31A1200, 0);
    texture = GStextureLoad(lbl_803254E0);
    GSresRegisterResource(texture, 0, 0x6221200, 0);
    texture = GStextureLoad(lbl_803357E0);
    GSresRegisterResource(texture, 0, 0x6F71200, 0);
}
