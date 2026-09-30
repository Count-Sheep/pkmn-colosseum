/**
 * @file gs_pcbox_exact_8001F304.c
 * @brief Title menu item draw callback, 0x8001F304 - 0x8001FD48.
 *
 * winSpriteDraw calls this for every sprite item of the title menus 0x13 and
 * 0x15 (items 0x2C4 and 0xEFA-0xF0D). It picks which items are shown for
 * the current title state and slides the menu columns:
 * - 0xEFA ("PRESS START") is shown while the title state lbl_8047A31C is
 *   0x1E..0x20 and blinks: its alpha is 128 * sin(pi * t * speed) + 200,
 *   clamped to 0..255, with speed 4 in state 0x1F and 2 otherwise;
 * - 0x2C4 is shown for the same states, without the blink;
 * - 0xEFB-0xF0D belong to one of four menu pages (lbl_80478878). An item on
 *   the current page is shown and placed at its menu-data x (the 0x1C-byte
 *   records of lbl_802EF0A8) plus one of the six animated column offsets in
 *   lbl_803A1F88, which fn_8001EF78 eases toward their targets;
 * - any other item is hidden unless lbl_8047A34C is set.
 *
 * Retail builds this with -opt nopeephole (the title TU's flag): the
 * `extsb` before each flag store survives. The float constants are this
 * TU's pool (0x8047B808-0x8047B8A0), referenced through extern stand-ins.
 */
#include "dolphin/types.h"

typedef struct TitleMenuItem {
    u8 pad0[4];
    s8 flags;          /* 0x04: bit 1 = shown */
    u8 pad5;
    s16 id;            /* 0x06: menu item id */
    u8 pad8[0x50 - 0x8];
    s16 x;             /* 0x50 */
    u8 pad52[0x67 - 0x52];
    u8 alpha;          /* 0x67 */
} TitleMenuItem;

typedef struct TitleMenuWork {
    u8 pad0[0x10];
    f32 offset[6];     /* 0x10: current column offsets */
    f32 target[6];     /* 0x28: target column offsets */
} TitleMenuWork;

extern u8 lbl_802EF0A8[];          /* menu item data, 0x1C bytes per item id */
extern TitleMenuWork lbl_803A1F88;
extern s32 lbl_80478878;           /* current title menu page */
extern s32 lbl_8047A31C;           /* title state */
extern f32 lbl_8047A338;           /* blink time */
extern u8 lbl_8047A34C;

/* RULE-EXCEPTION(title-path): extern named stand-ins for this TU's own pool
 * literals - see docs/RULE_EXCEPTIONS.md */
extern f32 lbl_8047B814;           /* 0.0f */
extern f32 lbl_8047B81C;           /* 4.0f */
extern f32 lbl_8047B838;           /* 3.1415927f */
extern f32 lbl_8047B83C;           /* 2.0f */
extern f32 lbl_8047B840;           /* 200.0f */
extern f32 lbl_8047B844;           /* 128.0f */
extern f32 lbl_8047B848;           /* 255.0f */

extern f64 sin(f64);

/* Show the item when the title menu is on `page`, at its menu-data x plus
 * column offset `column`; hide it otherwise. */
#define TITLE_ITEM_ON_PAGE(page, column)                                    \
    if (lbl_80478878 == (page)) {                                           \
        s16 base;                                                           \
        item->flags |= 2;                                                   \
        base = *(s16*)(lbl_802EF0A8 + item->id * 0x1C + 2);                 \
        item->x = base + (s32)lbl_803A1F88.offset[column];                  \
    } else {                                                                \
        item->flags &= ~2;                                                  \
    }                                                                       \
    break;

void fn_8001F304(void* menu, TitleMenuItem* item)
{
    u8 visible;
    f32 speed;
    f32 alpha;

    switch (item->id) {
    case 0xEFA:
        if (lbl_8047A31C < 0x1E || lbl_8047A31C > 0x20) {
            item->flags &= ~2;
            visible = 0;
        } else {
            item->flags |= 2;
            visible = 1;
        }
        if (visible) {
            if (lbl_8047A31C == 0x1F) {
                speed = lbl_8047B81C;
            } else {
                speed = lbl_8047B83C;
            }
            alpha = lbl_8047B844 * (f32)sin(lbl_8047B838 * (lbl_8047A338 * speed)) + lbl_8047B840;
            if (alpha > lbl_8047B848) {
                /* RULE-EXCEPTION(title-path): no-op cast that stops the
                 * frontend sharing this load with the compare, as retail's
                 * pool literal does - see docs/RULE_EXCEPTIONS.md */
                alpha = *(f32*)&lbl_8047B848;
            }
            if (alpha < lbl_8047B814) {
                alpha = lbl_8047B814;
            }
            item->alpha = alpha;
        }
        break;
    case 0x2C4:
        if (lbl_8047A31C < 0x1E || lbl_8047A31C > 0x20) {
            item->flags &= ~2;
        } else {
            item->flags |= 2;
        }
        break;
    case 0xEFB: TITLE_ITEM_ON_PAGE(0, 3)
    case 0xEFC: TITLE_ITEM_ON_PAGE(0, 4)
    case 0xEFD: TITLE_ITEM_ON_PAGE(0, 0)
    case 0xEFE: TITLE_ITEM_ON_PAGE(0, 1)
    case 0xEFF: TITLE_ITEM_ON_PAGE(1, 3)
    case 0xF00: TITLE_ITEM_ON_PAGE(1, 4)
    case 0xF01: TITLE_ITEM_ON_PAGE(1, 5)
    case 0xF02: TITLE_ITEM_ON_PAGE(1, 0)
    case 0xF03: TITLE_ITEM_ON_PAGE(1, 1)
    case 0xF04: TITLE_ITEM_ON_PAGE(1, 2)
    case 0xF05: TITLE_ITEM_ON_PAGE(2, 3)
    case 0xF06: TITLE_ITEM_ON_PAGE(2, 4)
    case 0xF07: TITLE_ITEM_ON_PAGE(2, 0)
    case 0xF08: TITLE_ITEM_ON_PAGE(2, 1)
    case 0xF09: TITLE_ITEM_ON_PAGE(2, 2)
    case 0xF0A: TITLE_ITEM_ON_PAGE(3, 0)
    case 0xF0B: TITLE_ITEM_ON_PAGE(3, 1)
    case 0xF0C: TITLE_ITEM_ON_PAGE(3, 3)
    case 0xF0D: TITLE_ITEM_ON_PAGE(3, 4)
    default:
        if (lbl_8047A34C == 0) {
            item->flags &= ~2;
        }
        break;
    }
}
