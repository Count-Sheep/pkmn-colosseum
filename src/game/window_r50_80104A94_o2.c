/**
 * @file window_r50_80104A94_o2.c
 * @brief _windowCreateItemSprite (0x80104A94 - 0x80104CA0), exact standalone
 *        owner.
 *
 * Window TU flags (GC/2.5, -O4,p, "-opt nopeephole"; see
 * window_exact_80104318.c). The types match window_r50_80104CA0_suffix.c.
 *
 * Same body as XD's _windowCreateItemSprite__FP14tagWINDOW_WORK (GXXE01
 * 0x80115B90; TeamOrre/xd-decomp symbols.txt @4989794e, trevor403/xd-asm
 * @b1087f18 code/func_FUN_80115b90.s), apart from the fields Colosseum
 * lacks. The menu tables are read through const pointers (XD mangles
 * _winCalcWindowSize's item as PC13MENU_ITEM_dd, pointer to const). Reading
 * a const s16 field into the s16 item index is a conversion, and that is
 * what leaves retail's `lha r3,4(r3); mr r28,r3` and `lha r0,24(r30);
 * mr r28,r0`: the value is loaded into a temp and copied into the index's
 * home register. With non-const tables both loads go straight into r28.
 * menuItemBiosGetPtr takes an s32 here (retail sign-extends the s16 index
 * at the call, extsh r3,r28), as menu.c declares it (u32).
 */
#include "dolphin/types.h"

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
    u8 unk0_4 : 2;        /* 0x00 bits 4-5 */
    u8 unk0_0 : 4;
    u8 alpha;             /* 0x01 */
    s16 x;                /* 0x02 */
    s16 y;                /* 0x04 */
    s16 width;            /* 0x06 */
    s16 height;           /* 0x08 */
    s16 spriteIndex;      /* 0x0A */
    s16 cursorSprite;     /* 0x0C */
    u8 pad_0E[2];
    void* message;        /* 0x10 */
    u32 unk_14;           /* 0x14 */
    s16 next;             /* 0x18 */
} MenuItem;

typedef struct MenuSprite {
    u16 last : 1;         /* 0x00 bit 15 */
    u16 fitWindow : 1;    /* 0x00 bit 14: take the window's size */
    u16 type : 2;         /* 0x00 bits 12-13 */
    u16 sequence : 12;    /* 0x00 bits 0-11 */
    u8 unk_02[3];
    s8 offsetX;           /* 0x05 */
    s8 offsetY;           /* 0x06 */
    u8 alpha;             /* 0x07 */
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
    u8 sequence[0x48 - 0x0C]; /* 0x0C */
    u32 unk_48;
    u32 unk_4C;
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
    u8 alpha;             /* 0x67 */
    u8 unk_68[0x74 - 0x68];
    u8 unk_74;
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

extern const MenuData* menuDataBiosGetPtr(s32 id);
extern const MenuItem* menuItemBiosGetPtr(s32 index);
extern MenuSprite* menuSpriteBiosGetPtr(s32 index);
extern WinSprite* winSpriteAdd(void* head);
extern void winSpriteRelease(void* head);
extern void winSetSequence(void* out, u32 index);
extern void _winCalcWindowSize__FlPC13MENU_ITEM_dd_PsPs(const MenuItem* item, s16* width, s16* height);

/* 0x80104A94 | 0x20C */
s32 _windowCreateItemSprite__FP14tagWINDOW_WORK(WindowWork* window)
{
    s16 itemId;
    const MenuItem* item;
    WinSprite* work;
    MenuSprite* sprite;
    s16 width;
    s16 height;

    itemId = menuDataBiosGetPtr(window->id)->itemIndex;
    item = menuItemBiosGetPtr(itemId);
    winSpriteRelease(&window->itemSprites);
    _winCalcWindowSize__FlPC13MENU_ITEM_dd_PsPs(item, &width, &height);
    while (TRUE) {
        item = menuItemBiosGetPtr(itemId);
        work = winSpriteAdd(&window->itemSprites);
        if (work == NULL) {
            winSpriteRelease(&window->itemSprites);
            return 1;
        }
        work->unk_06 = itemId;
        work->unk_50 = item->x;
        work->unk_52 = item->y;
        work->unk_54 = item->width;
        work->unk_56 = item->height;
        work->alpha = item->alpha;
        work->unk_74 = item->unk0_4;
        if (item->spriteIndex != 0) {
            sprite = menuSpriteBiosGetPtr(item->spriteIndex);
            if (sprite->type == 1) {
                work->flags |= 1;
                work->unk_58 = sprite->unk_10;
            } else if (sprite->type == 2) {
                work->flags |= 2;
                work->unk_08 = sprite->unk_10;
            }
            work->unk_5C = sprite->unk_08;
            work->unk_5E = sprite->unk_0A;
            work->unk_60 = sprite->unk_0C;
            work->unk_62 = sprite->unk_0E;
            work->alpha = work->alpha * sprite->alpha / 255;
            if (sprite->fitWindow) {
                work->unk_54 = width;
                work->unk_56 = height;
            }
            if (sprite->sequence != 0) {
                winSetSequence(&work->sequence, sprite->sequence);
            }
        }
        work->unk_4C = (u32)item->message;
        if (item->unk_14 != 0) {
            work->flags |= 8;
            work->unk_48 = item->unk_14;
        }
        if (item->last) {
            break;
        }
        itemId = item->next;
    }
    return 0;
}
