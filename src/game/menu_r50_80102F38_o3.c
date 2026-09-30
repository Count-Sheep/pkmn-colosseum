/**
 * @file menu_r50_80102F38_o3.c
 * @brief menu engine, 0x80102F38-0x80103484: menuCursorNormal.
 *
 * Function-boundary carve of the menu TU (see src/game/menu.c), built with
 * the TU's flags (configure.py). Text only: no pooled literal or TU data.
 *
 * The register assignment depends on two things (lane D16, GPR simulator
 * replay):
 * - the item walk goes through a one-line accessor, menuItemNext. Its
 *   result copies are extra precoloured nodes in every search loop, which
 *   raise the degree of the long-lived webs, so up/down/left/right stay in
 *   the spill phase and are coloured in retail's order;
 * - the locals are declared in the order below, which sets their vreg
 *   numbering, and that breaks the cost/degree ties the way retail does.
 */
#include "dolphin/types.h"

typedef struct tagWINDOW_WORK MenuWindow;

typedef struct MenuCursor {
    s8 base;
    s8 offset;
} MenuCursor;

typedef struct MenuData {
    u8 soundGroup;
    u8 actionType;
    u8 pad_02;
    u8 cursorSlot;
    s16 firstItem;
} MenuData;

typedef struct MenuItem {
    u8 selectable : 1;
    u8 last : 1;
    u8 pad_00 : 6;
    u8 pad_01;
    s16 x;
    s16 y;
    u8 pad_06[0x12];
    s16 next;
} MenuItem;

struct tagWINDOW_WORK {
    s8 flags;
    s8 phase;
    u8 phaseFrame;
    u8 pad_03;
    u32 id;
    u8 pad_08[0x11];
    s8 cursorCount;
    u8 cursorMode;
    u8 pad_1B[0x69];
    s16 x;
    s16 y;
    u8 pad_88[0x0C];
    MenuCursor cursor;
    MenuCursor previousCursor;
};

typedef struct MenuKeyInfo {
    u16 keys;
    u16 previousKeys;
    u16 pressedKeys;
    u16 repeatKeys;
} MenuKeyInfo;

const MenuData* menuDataBiosGetPtr(u32 id);
const MenuItem* menuItemBiosGetPtr(u32 itemId);
MenuKeyInfo* windowGetKeyInfo(void);
const MenuItem* windowGetCursorToItem(MenuWindow* window);

static inline const MenuItem* menuItemNext(const MenuItem* item) {
    return menuItemBiosGetPtr(item->next);
}

/* 0x80102F38 | 0x54C */
/*
 * Moves the cursor to the nearest selectable item in the pressed direction:
 * items within a band around the current item's row/column are searched
 * first, and the band is widened until an item is found. Wrapping menus
 * (cursor mode 2) then jump to the farthest item on the opposite side.
 */
void menuCursorNormal(MenuWindow* window) {
    s8 index;
    s32 cross;
    s32 best;
    s32 up;
    s32 down;
    u16 wrap;
    const MenuData* data;
    s32 left;
    u16 keys;
    s32 xRange;
    const MenuItem* current;
    const MenuItem* item;
    s32 yRange;
    s32 delta;
    s32 right;

    wrap = 0;
    if (window == NULL) {
        return;
    }
    if (window->cursorMode == 2) {
        wrap = 1;
    }

    data = menuDataBiosGetPtr(window->id);
    current = windowGetCursorToItem(window);
    if (current == NULL) {
        window->cursor.offset = 0;
        return;
    }

    xRange = 16;
    yRange = 12;
    keys = windowGetKeyInfo()->repeatKeys;
    up = keys & 1;
    down = keys & 2;
    left = keys & 4;
    right = keys & 8;

    for (;;) {
        index = 0;
        item = menuItemBiosGetPtr(data->firstItem);
        if (up) {
            best = 480;
            for (;;) {
                if (item->selectable) {
                    delta = current->y - item->y;
                    if (delta > 0) {
                        cross = current->x - item->x;
                        if (cross < 0) {
                            cross = -cross;
                        }
                        if (cross < xRange && best > delta) {
                            best = delta;
                            window->cursor.offset = index;
                        }
                    }
                    index++;
                }
                if (item->last) {
                    break;
                }
                item = menuItemNext(item);
            }
        } else if (down) {
            best = 480;
            for (;;) {
                if (item->selectable) {
                    delta = item->y - current->y;
                    if (delta > 0) {
                        cross = current->x - item->x;
                        if (cross < 0) {
                            cross = -cross;
                        }
                        if (cross < xRange && best > delta) {
                            best = delta;
                            window->cursor.offset = index;
                        }
                    }
                    index++;
                }
                if (item->last) {
                    break;
                }
                item = menuItemNext(item);
            }
        } else if (left) {
            best = 640;
            for (;;) {
                if (item->selectable) {
                    delta = current->x - item->x;
                    if (delta > 0) {
                        cross = current->y - item->y;
                        if (cross < 0) {
                            cross = -cross;
                        }
                        if (cross < yRange && best > delta) {
                            best = delta;
                            window->cursor.offset = index;
                        }
                    }
                    index++;
                }
                if (item->last) {
                    break;
                }
                item = menuItemNext(item);
            }
        } else if (right) {
            best = 640;
            for (;;) {
                if (item->selectable) {
                    delta = item->x - current->x;
                    if (delta > 0) {
                        cross = current->y - item->y;
                        if (cross < 0) {
                            cross = -cross;
                        }
                        if (cross < yRange && best > delta) {
                            best = delta;
                            window->cursor.offset = index;
                        }
                    }
                    index++;
                }
                if (item->last) {
                    break;
                }
                item = menuItemNext(item);
            }
        }

        if (window->previousCursor.offset != window->cursor.offset) {
            break;
        }
        xRange += 16;
        yRange += 12;
        if (xRange >= 640) {
            xRange = 640;
        }
        if (yRange >= 480) {
            yRange = 480;
        }
        if (xRange == 640 && yRange == 480) {
            break;
        }
    }

    if (wrap == 0) {
        return;
    }

    xRange = 16;
    yRange = 12;
    for (;;) {
        if (window->previousCursor.offset != window->cursor.offset) {
            return;
        }
        item = menuItemBiosGetPtr(data->firstItem);
        index = 0;
        if (up) {
            best = 0;
            for (;;) {
                if (item->selectable) {
                    delta = item->y - current->y;
                    if (delta > 0) {
                        cross = current->x - item->x;
                        if (cross < 0) {
                            cross = -cross;
                        }
                        if (cross < xRange && best < delta) {
                            best = delta;
                            window->cursor.offset = index;
                        }
                    }
                    index++;
                }
                if (item->last) {
                    break;
                }
                item = menuItemNext(item);
            }
        } else if (down) {
            best = 0;
            for (;;) {
                if (item->selectable) {
                    delta = current->y - item->y;
                    if (delta > 0) {
                        cross = current->x - item->x;
                        if (cross < 0) {
                            cross = -cross;
                        }
                        if (cross < xRange && best < delta) {
                            best = delta;
                            window->cursor.offset = index;
                        }
                    }
                    index++;
                }
                if (item->last) {
                    break;
                }
                item = menuItemNext(item);
            }
        } else if (left) {
            best = 0;
            for (;;) {
                if (item->selectable) {
                    delta = item->x - current->x;
                    if (delta > 0) {
                        cross = current->y - item->y;
                        if (cross < 0) {
                            cross = -cross;
                        }
                        if (cross < yRange && best < delta) {
                            best = delta;
                            window->cursor.offset = index;
                        }
                    }
                    index++;
                }
                if (item->last) {
                    break;
                }
                item = menuItemNext(item);
            }
        } else if (right) {
            best = 0;
            for (;;) {
                if (item->selectable) {
                    delta = current->x - item->x;
                    if (delta > 0) {
                        cross = current->y - item->y;
                        if (cross < 0) {
                            cross = -cross;
                        }
                        if (cross < yRange && best < delta) {
                            best = delta;
                            window->cursor.offset = index;
                        }
                    }
                    index++;
                }
                if (item->last) {
                    break;
                }
                item = menuItemNext(item);
            }
        }

        xRange += 16;
        yRange += 12;
        if (xRange >= 640) {
            xRange = 640;
        }
        if (yRange >= 480) {
            yRange = 480;
        }
        if (xRange == 640 && yRange == 480) {
            break;
        }
    }
}
