/**
 * @file window_r50_80104CA0_suffix.c
 * @brief windowCreateCursorSprite, windowOpen and _winCalcWindowSize
 *        (0x80104CA0 - 0x80105410), exact standalone owner.
 *
 * Not linked (coordinator, 2026-09-28): lbl_8047CDEC is not a shared global
 * but the window TU's own compiler-pool 1.0f. Only windowOpen and
 * windowDrawSprite2 (another chunk) read it, and it is misfiled in
 * gs_model_sdata2_8047CD98.c. Reading it through an extern stand-in is the
 * rejected "synthetic compiler data" form, and the pool can't move into this
 * unit while windowDrawSprite2 shares it. The unit stays CodeCandidate until
 * the window TU's pool pairs honestly (e.g. a unit that also holds
 * windowDrawSprite2 and owns the literal).
 *
 * Window TU flags (GC/2.5, -O4,p, "-opt nopeephole"; see
 * window_exact_80104318.c). Data stays extern (lbl_80404ACC, the 1.0f
 * constant lbl_8047CDEC and the _winGetNewWork error string lbl_80271E94).
 *
 * Pokemon XD references: TeamOrre/xd-decomp config/GXXE01/symbols.txt
 * @4989794e, bodies in trevor403/xd-asm @b1087f18.
 *
 * windowOpen (XD 0x80115FAC, code/func_FUN_80115fac.s) calls two XD statics
 * that Colosseum expands in place:
 * - winGetNewWork: XD _winGetNewWork__Fv (0x80116430, 0xA0,
 *   code/func_FUN_80116430.s). It scans the work array with a CTR loop
 *   (lhz count; mtctr; ... bdnz), memsets the first free slot and marks it
 *   (state 7, the two 1.0f fields, -1), or calls GSlog with the "Error!!
 *   _winGetNewWork()" message and returns NULL. The calls (memset, then
 *   GSlog on failure) are the same, in the same order, and the retail
 *   expansion has the same shape (0x80104FDC - 0x80105054).
 * - winInsertWork: XD _winInsertWork__FP14tagWINDOW_WORKSc (0x801163A0,
 *   0x90, no calls, code/func_FUN_801163a0.s). It inserts the window into
 *   the open list in priority order; the retail expansion
 *   (0x80105118 - 0x80105198) is the same code.
 * Both are named after the XD functions (leading underscore dropped):
 * sister-title clause, docs/CAMPAIGN_OPERATIONS.md.
 *
 * Colosseum's own same-TU functions that retail inlines here, kept as
 * compile-only copies (as in musyx seq_exact_80146E88.c):
 * windowGetActiveID (0x801046B8) and windowSearchID (0x80104704), expanded
 * four times in windowOpen, and windowGetCursorToItem (0x80104318), expanded
 * in windowCreateCursorSprite. XD's windowCreateCursorSprite calls
 * windowGetCursorToItem (0x80115608) at the same point. Each copy compiled
 * alone gives the linked function's bytes (window_exact_801046B8.c,
 * window_exact_80104704.c, window_exact_80104318.c).
 *
 * cursorBiosGetPos returns a 2-byte struct in r3: retail stores its value
 * with `srwi r0,r3,16; sth`, which is the struct copy. The menu tables are
 * read through const pointers, as the XD mangling of _winCalcWindowSize
 * (PC13MENU_ITEM_dd, pointer to const MENU_ITEM_dd) shows.
 * _winCalcWindowSize keeps that XD-derived symbol name, but Colosseum's
 * takes (item, width, height), and menuItemBiosGetPtr here takes only the
 * item index.
 */
#include "dolphin/types.h"
#include "crt/stdarg.h"

typedef struct WindowWork WindowWork;

typedef struct WindowCursor {
    s8 x;
    s8 y;
} WindowCursor;

struct WindowWork {
    s8 state;             /* 0x00 */
    u8 step;              /* 0x01 */
    s8 reopen;            /* 0x02 */
    u8 pad_03;
    s32 id;               /* 0x04 */
    u8 pad_08;
    s8 priority;          /* 0x09 */
    u8 closeFlag;         /* 0x0A */
    u8 port;              /* 0x0B */
    WindowWork* parent;   /* 0x0C */
    WindowWork* next;     /* 0x10 */
    WindowWork* prev;     /* 0x14 */
    u8 unk_18;            /* 0x18 */
    u8 itemCount;         /* 0x19 */
    u8 unk_1A;            /* 0x1A */
    u8 openFlags;         /* 0x1B */
    void* itemSprites;    /* 0x1C */
    void* cursorSprites;  /* 0x20 */
    u8 pad_24[0x60 - 0x24];
    u32 args[8];          /* 0x60 */
    u8 pad_80[4];
    s16 unk_84;           /* 0x84 */
    s16 unk_86;           /* 0x86 */
    s32 unk_88;           /* 0x88 */
    f32 unk_8C;           /* 0x8C */
    f32 unk_90;           /* 0x90 */
    WindowCursor cursor;  /* 0x94 */
    u8 pad_96[2];
    u8 unk_98;            /* 0x98 */
    u8 unk_99;            /* 0x99 */
    u8 pad_9A[0xB4 - 0x9A];
};

typedef struct WindowSystemWork {
    u16 count;            /* 0x00 */
    u8 pad_02[2];
    s32 activeID;         /* 0x04 */
    WindowWork* works;    /* 0x08 */
    WindowWork* windows;  /* 0x0C */
} WindowSystemWork;

typedef struct MenuItem {
    u8 selectable : 1;    /* 0x00 bit 7 */
    u8 last : 1;          /* 0x00 bit 6 */
    u8 unk0_0 : 6;
    u8 unk_01;
    s16 x;                /* 0x02 */
    s16 y;                /* 0x04 */
    s16 width;            /* 0x06 */
    s16 height;           /* 0x08 */
    s16 spriteIndex;      /* 0x0A */
    s16 cursorSprite;     /* 0x0C */
    u8 pad_0E[2];
    void* message;        /* 0x10 */
    u8 pad_14[4];
    s16 next;             /* 0x18 */
} MenuItem;

typedef struct MenuSprite {
    u16 last : 1;         /* 0x00 bit 15 */
    u16 unk0_14 : 1;
    u16 type : 2;         /* 0x00 bits 12-13 */
    u16 sequence : 12;    /* 0x00 bits 0-11 */
    u8 unk_02[3];
    s8 offsetX;           /* 0x05 */
    s8 offsetY;           /* 0x06 */
    u8 unk_07;
    s16 unk_08;
    s16 unk_0A;
    s16 unk_0C;
    s16 unk_0E;
    u32 unk_10;
    s16 next;             /* 0x14 */
} MenuSprite;

typedef struct WinSprite {
    u8 unk_00[5];
    u8 flags;             /* 0x05 */
    s16 unk_06;
    u32 unk_08;
    u8 sequence[0x50 - 0x0C]; /* 0x0C */
    s16 unk_50;
    s16 unk_52;
    s16 unk_54;
    s16 unk_56;
    u32 unk_58;
    s16 unk_5C;
    s16 unk_5E;
    s16 unk_60;
    s16 unk_62;
    u8 unk_64[3];
    u8 unk_67;
} WinSprite;

typedef struct MenuData {
    u8 unk0_5 : 3;
    u8 topmost : 1;       /* 0x00 bit 4: always opens at priority 100 */
    u8 unk0_0 : 4;
    u8 unk1_5 : 3;        /* 0x01 bits 5-7 */
    u8 unk1_2 : 3;        /* 0x01 bits 2-4: 0 = no active focus */
    u8 unk1_0 : 2;
    u8 unk_02;
    u8 cursorId;          /* 0x03 */
    s16 itemIndex;        /* 0x04 */
    s16 unk_06;
    s16 unk_08;
    u8 pad_0A[0x14 - 0x0A];
    void (*callback)(WindowWork* window); /* 0x14 */
} MenuData;

extern WindowSystemWork lbl_80404ACC;
extern const f32 lbl_8047CDEC;
extern char lbl_80271E94[];

extern void* memset(void* dst, int val, u32 size);
extern void GSlogWrite(const char* fmt, ...);
extern u32 GSmsgGetRect(void* message);
extern const MenuData* menuDataBiosGetPtr(s32 id);
extern const MenuItem* menuItemBiosGetPtr(s32 index);
extern MenuSprite* menuSpriteBiosGetPtr(s32 index);
extern void menuPlaySe(s32 id, s32 se);
extern u8 menuGetEnablePort(void);
extern WindowCursor cursorBiosGetPos(u16 index);
extern s32 _windowCreateItemSprite__FP14tagWINDOW_WORK(WindowWork* window);
extern WinSprite* winSpriteAdd(void* head);
extern void winSpriteRelease(void* head);
extern void winSetSequence(void* out, u32 index);

static inline const MenuItem* windowGetCursorToItem(WindowWork* window)
{
    const MenuItem* item = menuItemBiosGetPtr(menuDataBiosGetPtr(window->id)->itemIndex);
    s32 index = 0;

    while (TRUE) {
        if (item->selectable) {
            if (window->cursor.y == index) {
                return item;
            }
            index++;
        }
        if (item->last) {
            break;
        }
        item = menuItemBiosGetPtr(item->next);
    }
    return NULL;
}

/* 0x80104CA0 | 0x1E0 */
void windowCreateCursorSprite(WindowWork* window)
{
    const MenuData* data;
    const MenuItem* item;
    MenuSprite* sprite;
    WinSprite* work;

    data = menuDataBiosGetPtr(window->id);
    item = windowGetCursorToItem(window);
    if (item == NULL) {
        return;
    }
    winSpriteRelease(&window->cursorSprites);
    if (item->cursorSprite == 0) {
        return;
    }
    sprite = menuSpriteBiosGetPtr(item->cursorSprite);
    while (TRUE) {
        work = winSpriteAdd(&window->cursorSprites);
        if (work == NULL) {
            winSpriteRelease(&window->cursorSprites);
            return;
        }
        if (sprite->type == 1) {
            work->flags |= 1;
            work->unk_58 = sprite->unk_10;
        } else if (sprite->type == 2) {
            work->flags |= 2;
            work->unk_08 = sprite->unk_10;
        }
        work->unk_06 = data->itemIndex;
        work->unk_50 = item->x + sprite->offsetX;
        work->unk_52 = item->y + sprite->offsetY;
        work->unk_54 = sprite->unk_0C;
        work->unk_56 = sprite->unk_0E;
        work->unk_5C = sprite->unk_08;
        work->unk_5E = sprite->unk_0A;
        work->unk_60 = sprite->unk_0C;
        work->unk_62 = sprite->unk_0E;
        work->unk_67 = sprite->unk_07;
        if (sprite->sequence != 0) {
            winSetSequence(&work->sequence, sprite->sequence);
        }
        if (sprite->last) {
            break;
        }
        sprite = menuSpriteBiosGetPtr(sprite->next);
    }
}

static inline s32 windowGetActiveID(void)
{
    return lbl_80404ACC.activeID;
}

static inline WindowWork* windowSearchID(s32 id)
{
    WindowWork* window;

    if (id <= 0) {
        return NULL;
    }

    window = lbl_80404ACC.windows;
    while (window != NULL) {
        if (window->id == id) {
            return window;
        }
        window = window->next;
    }
    return NULL;
}

static inline WindowWork* winGetNewWork(void)
{
    WindowWork* work = lbl_80404ACC.works;
    s32 i;

    for (i = 0; i < lbl_80404ACC.count; i++) {
        if (work->state == 0) {
            memset(work, 0, sizeof(WindowWork));
            work->state = 7;
            work->unk_8C = lbl_8047CDEC;
            work->unk_90 = lbl_8047CDEC;
            work->unk_88 = -1;
            return work;
        }
        work++;
    }
    GSlogWrite(lbl_80271E94);
    return NULL;
}

static inline s32 winInsertWork(WindowWork* window, s8 priority)
{
    WindowWork* work = lbl_80404ACC.windows;

    if (work == NULL) {
        lbl_80404ACC.windows = window;
    } else {
        while (TRUE) {
            if (work->priority > priority) {
                window->prev = work->prev;
                window->next = work;
                if (work->prev == NULL) {
                    lbl_80404ACC.windows = window;
                } else {
                    work->prev->next = window;
                }
                work->prev = window;
                break;
            }
            if (work->next == NULL) {
                work->next = window;
                window->prev = work;
                window->next = NULL;
                break;
            }
            work = work->next;
        }
    }
    return 0;
}

/* 0x80104E80 | 0x474 */
WindowWork* windowOpen(s32* cursor, s32 id, s32 parentId, u32 flags, s32 argc, va_list args)
{
    WindowWork* parent;
    const MenuData* data;
    WindowWork* window;
    s8 priority;
    const MenuItem* item;
    s32 i;

    parent = windowSearchID(parentId);
    data = menuDataBiosGetPtr(id);
    window = windowSearchID(id);
    if (window == NULL) {
        if (data->topmost) {
            priority = 100;
        } else if ((flags & 2) != 0) {
            priority = 100;
        } else {
            if (parentId == 0) {
                parentId = windowGetActiveID();
            }
            parent = windowSearchID(parentId);
            if (parent != NULL) {
                priority = parent->priority + 1;
            } else {
                priority = 0;
            }
            if (priority > 100) {
                priority = 100;
            }
        }
        window = winGetNewWork();
        if (window == NULL) {
            return NULL;
        }
        window->step = 0;
        window->id = id;
        window->priority = priority;
        window->unk_1A = data->unk1_5;
        window->parent = windowSearchID(parentId);
        window->unk_84 = data->unk_06;
        window->unk_86 = data->unk_08;
        item = menuItemBiosGetPtr(data->itemIndex);
        while (TRUE) {
            if (item->selectable) {
                window->itemCount++;
            }
            if (item->last) {
                break;
            }
            item = menuItemBiosGetPtr(item->next);
        }
        _windowCreateItemSprite__FP14tagWINDOW_WORK(window);
        winInsertWork(window, priority);
        menuPlaySe(id, 4);
    } else {
        window->unk_98 = 0;
        window->unk_99 = 0;
        window->step = 0;
        window->reopen = 1;
    }

    if (cursor == NULL) {
        if (data->cursorId != 0) {
            window->cursor = cursorBiosGetPos(data->cursorId);
        }
    } else {
        window->cursor.x = 0;
        window->cursor.y = *cursor;
    }

    windowCreateCursorSprite(window);
    window->port = menuGetEnablePort();
    window->openFlags = flags;
    if (argc > 8) {
        argc = 8;
    }
    for (i = 0; i < argc; i++) {
        window->args[i] = va_arg(args, u32);
    }

    if (data->unk1_2 == 0) {
        window->unk_98 = 1;
    } else if ((flags & 4) == 0) {
        lbl_80404ACC.activeID = id;
    }
    if (lbl_80404ACC.activeID != id) {
        window->unk_18 = 1;
    }
    if (data->callback != NULL) {
        data->callback(window);
    }
    if (window->reopen != 0) {
        window->closeFlag = 0;
        window->step = 2;
        window->reopen = 0;
    }
    return window;
}

/* 0x801052F4 | 0x11C */
void _winCalcWindowSize__FlPC13MENU_ITEM_dd_PsPs(const MenuItem* item, s16* width, s16* height)
{
    s32 minX = 640;
    s32 minY = 480;
    s32 maxX = 0;
    s32 maxY = 0;
    u32 rect;
    s16 textWidth;
    s16 textHeight;

    *width = 0;
    *height = 0;
    while (TRUE) {
        menuSpriteBiosGetPtr(item->spriteIndex);
        if (item->message != NULL) {
            rect = GSmsgGetRect(item->message);
        } else {
            rect = 0;
        }
        textWidth = rect >> 16;
        textHeight = (u16)rect;
        if (minX > item->x) {
            minX = item->x;
        }
        if (maxX < item->x + item->width) {
            maxX = item->x + item->width;
        }
        if (maxX < item->x + textWidth) {
            maxX = item->x + textWidth;
        }
        if (minY > item->y) {
            minY = item->y;
        }
        if (maxY < item->y + item->height) {
            maxY = item->y + item->height;
        }
        if (maxY < item->y + textHeight) {
            maxY = item->y + textHeight;
        }
        if (item->last) {
            break;
        }
        item = menuItemBiosGetPtr(item->next);
    }
    *width = maxX - minX;
    *height = maxY - minY;
}
