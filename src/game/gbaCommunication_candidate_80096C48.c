/**
 * @file gbaCommunication_candidate_80096C48.c
 * @brief gbaCommunication carve, 0x80096C48 - 0x80097FFC (16 functions:
 *        the Pokemon-status menu callbacks, fn_80096C48 .. fn_80097FF8).
 *
 * Standalone carve: GBA_COMMUNICATION_DECLS_ONLY takes only the shared
 * declarations from gbaCommunication.c, the status-menu work types are
 * repeated here, and the functions follow in address order. The unit is
 * built with -opt nopeephole (configure.py) instead of the full file's
 * per-function peephole and scheduling pragmas; under it every function
 * but fn_80096FA0 is exact. The bodies are gbaCommunication.c's; keep the
 * two in step.
 */
#define GBA_COMMUNICATION_DECLS_ONLY
#include "src/game/gbaCommunication.c"

typedef struct RibbonGroupDescriptor {
    u16 selector;
    s8 firstRibbon;
    u8 maximum;
} RibbonGroupDescriptor;

typedef struct PokemonRibbonGrid {
    u32 count;
    s8 ribbon[9][4];
} PokemonRibbonGrid;

typedef struct PokemonStatusMenuWork {
    u8 flags;
    u8 state;
    s8 selection;
    s8 previousSelection;
    s32 result;
    u32 entityId;
    void* pokemon;
    void* callback;
    s32 callbackArg;
    u16 hasExtraMove;
    u16 padding;
    PokemonRibbonGrid ribbons;
} PokemonStatusMenuWork;


/* 0x80096C48 | size: 0x10C */
void fn_80096C48(u32 unused, u8* dst) {
    typedef struct {
        f32 x;
        f32 y;
        f32 z;
    } ColorTriple;

    ColorTriple state0;
    ColorTriple state1;
    ColorTriple state2;
    register u8* out;
    register ColorTriple* triple;
    u8* obj;
    s32 state;

    out = dst;
    state0 = *(ColorTriple*)(lbl_8026F5C0 + 0x00);
    state1 = *(ColorTriple*)(lbl_8026F5C0 + 0x0C);
    state2 = *(ColorTriple*)(lbl_8026F5C0 + 0x18);

    obj = windowSearchID(0x53);
    if (obj == NULL) {
        return;
    }

    state = (s8)obj[0x95];
    switch (state) {
    case 0:
        triple = &state0;
        break;
    case 1:
        triple = &state1;
        break;
    case 2:
        triple = &state2;
        break;
    }

    out[0x64] = triple->x;
    out[0x65] = triple->y;
    out[0x66] = triple->z;
}

/* Handle confirm/cancel input for the linked-Pokemon status menu. */
void fn_80096D54(u8* menu)
{
    extern void fn_80166A28(s32 id);
    extern void pokemonWazaReplace();

    u16* keys;
    void* pokemon;
    PokemonStatusMenuWork* work;
    register s8* ribbonRow;
    register s32 ribbonColumn;
    register s32 ribbonIndex;
    s8 ribbon;

    keys = windowGetKeyInfo();
    if (keys[2] & 0x10) {
        work = (PokemonStatusMenuWork*)lbl_803FB380;
        switch (work->state) {
        case 0:
        case 1:
        case 6:
        case 8:
            break;
        case 2:
            work->selection = 0;
            work->state = 3;
            break;
        case 3:
            pokemon = work->pokemon;
            if (pokemon != NULL) {
                if ((s32)pokemonGetStatus(pokemon, 0, 0xC2, 0) != 0) {
                    fn_80166A28(0x26);
                } else if (lbl_803FB380[0] & 2) {
                    work->state = 4;
                    lbl_803FB380[3] = lbl_803FB380[2];
                }
            }
            break;
        case 4:
            pokemon = work->pokemon;
            if (pokemon != NULL) {
                pokemonWazaReplace(pokemon, work->previousSelection,
                                   work->selection);
            }
            *(s8*)(lbl_803FB380 + 3) = -1;
            work->state = 3;
            break;
        case 5:
            if ((s32)work->ribbons.count > 0) {
                for (ribbon = 0; ribbon < 36; ribbon++) {
                    ribbonIndex = ribbon % 9;
                    ribbonColumn = ribbon % 4;
                    ribbonRow = (s8*)work + ribbonIndex * 4;
                    ribbonRow += ribbonColumn;
                    if (ribbonRow[0x20] >= 0) {
                        break;
                    }
                }
                work->state = 6;
                *(s8*)(lbl_803FB380 + 0x1A) = ribbon;
            }
            break;
        case 7:
            if (work->flags & 0x10) {
                menu[0x98] = 1;
                work->result = work->selection;
            }
            break;
        }
    } else if (keys[2] & 0x20) {
        switch (lbl_803FB380[1]) {
        case 0:
        case 8:
            break;
        case 1:
        case 2:
        case 5:
        case 7:
            menu[0x98] = 1;
            menu[0x99] = 1;
            lbl_803FB380[1] = 8;
            break;
        case 3:
            lbl_803FB380[1] = 2;
            break;
        case 4:
            lbl_803FB380[1] = 3;
            break;
        case 6:
            *(s8*)(lbl_803FB380 + 0x1A) = -1;
            lbl_803FB380[1] = 5;
            break;
        }
    }
}

/* Handle input for the linked-Pokemon status submenus. */
extern void menuPlaySe(s32 menu, s32 se);
extern void fn_80109C88(void*, u32);
extern u8 pokemonWazaCheckValid(u32 pokemon, s32 waza);

/* Whether move slot `move` of the shown Pokemon can be selected. */
/* RULE-EXCEPTION(user-approved): single-call inline helper (fn_80096FA0) — see docs/RULE_EXCEPTIONS.md */
static inline u16 menuStatusMoveValid(s32 move)
{
    u32 pokemon;
    u16 valid;

    pokemon = *(u32*)(lbl_803FB380 + 0x0C);
    if ((u16)move == 4) {
        valid = *(u16*)(lbl_803FB380 + 0x18);
    } else {
        valid = pokemonGetStatus(pokemon, 0, 0x7F, move);
        if (pokemonWazaCheckValid(pokemon, move) == 0) {
            valid = 0;
        }
    }
    return valid;
}

/* Move the cursor over the move list. */
/* RULE-EXCEPTION(user-approved): single-call inline helper (fn_80096FA0) — see docs/RULE_EXCEPTIONS.md */
static inline void menuStatusMoveInput(u8* menu, u16 input, s8 limit)
{
    s8 moveSelection;

    moveSelection = *(s8*)(lbl_803FB380 + 2);
    if (input & 1) {
        moveSelection--;
    } else if (input & 2) {
        moveSelection++;
    }
    if (moveSelection >= limit) {
        moveSelection = (s8)(limit - 1);
    }
    if (moveSelection < 0) {
        moveSelection = 0;
    }
    if (menuStatusMoveValid(moveSelection) != 0 &&
        moveSelection != *(s8*)(lbl_803FB380 + 2)) {
        menuPlaySe(*(s32*)(menu + 4), 1);
        *(s8*)(lbl_803FB380 + 2) = moveSelection;
    }
}

/* Move the cursor over the 9x4 ribbon grid. */
/* RULE-EXCEPTION(user-approved): single-call inline helper (fn_80096FA0) — see docs/RULE_EXCEPTIONS.md */
static inline void menuStatusRibbonInputBody(u8* menu, u16 input)
{
    s32 previous;
    s32 row;
    s32 column;
    s32 scan;
    s8 selection;
    u8* cell;
    s8 current;
    s32 inner;

    current = *(s8*)(lbl_803FB380 + 0x1A);
    column = current % 9;
    row = current / 9;
    if (input & 1) {
        scan = row;
        while (scan-- > 0) {
            inner = column;
            cell = lbl_803FB380 + scan + column * 4;
            do {
                if ((s8)cell[0x20] >= 0) {
                    row = scan;
                    column = inner;
                    scan = -1;
                    break;
                }
                cell -= 4;
            } while (inner-- > 0);
        }
    } else if (input & 2) {
        scan = row;
        while (++scan < 4) {
            inner = column;
            cell = lbl_803FB380 + scan + column * 4;
            do {
                if ((s8)cell[0x20] >= 0) {
                    row = scan;
                    column = inner;
                    scan = 5;
                    break;
                }
                cell -= 4;
            } while (inner-- > 0);
        }
    } else if (input & 8) {
        previous = column;
        column++;
        if (column >= 9) {
            column = 8;
        }
        if ((s8)(lbl_803FB380 + column * 4)[0x20 + row] < 0) {
            column = previous;
        }
    } else if (input & 4) {
        previous = column;
        column--;
        if (column < 0) {
            column = 0;
        }
        if ((s8)(lbl_803FB380 + column * 4)[0x20 + row] < 0) {
            column = previous;
        }
    }
    selection = column + row * 9;
    if (selection != current) {
        menuPlaySe(*(s32*)(menu + 4), 1);
        *(s8*)(lbl_803FB380 + 0x1A) = selection;
    }
}

/* RULE-EXCEPTION(user-approved): single-call inline helper (fn_80096FA0) — see docs/RULE_EXCEPTIONS.md */
static inline void menuStatusRibbonInput(u8* menu, u16 input)
{
    menuStatusRibbonInputBody(menu, input);
}

/* RULE-EXCEPTION(user-approved): single-call inline helper (fn_80096FA0) — see docs/RULE_EXCEPTIONS.md */
static inline u16 menuStatusGetTrigger(void)
{
    return windowGetKeyInfo()[3];
}

void fn_80096FA0(u8* menu)
{
    typedef u32 (*StatusChangeCallback)(u32, s32, s32);

    s32 input;
    s32 action;
    s8 limit;
    u8 tabSelection;
    u32 result;

    action = 0;
    if (*(u16*)(lbl_803FB380 + 0x18) != 0) {
        limit = 5;
    } else {
        limit = 4;
    }
    input = menuStatusGetTrigger();

    switch (lbl_803FB380[1]) {
        case 1:
        case 2:
        case 5: {
            u16 trigger = input;

            tabSelection = menu[0x95];
            if (trigger & 8) {
                tabSelection++;
            } else if (trigger & 4) {
                tabSelection--;
            }
            if ((s8)tabSelection > 2) {
                tabSelection = 2;
            }
            if ((s8)tabSelection < 0) {
                tabSelection = 0;
            }
            menu[0x95] = tabSelection;
            switch ((s8)tabSelection) {
            case 0:
                lbl_803FB380[1] = 1;
                break;
            case 1:
                lbl_803FB380[1] = 2;
                break;
            case 2:
                lbl_803FB380[1] = 5;
                break;
            }
            if (trigger & 1) {
                action = 1;
            } else if (trigger & 2) {
                action = 2;
            }
            if (action != 0 && *(u32*)(lbl_803FB380 + 0x10) != 0) {
                result = (*(StatusChangeCallback*)(lbl_803FB380 + 0x10))(
                    *(u32*)(lbl_803FB380 + 0x0C), action,
                    *(s32*)(lbl_803FB380 + 0x14));
                if (*(u32*)(lbl_803FB380 + 0x0C) != result) {
                    if (result != 0) {
                        menuPlaySe(*(s32*)(menu + 4), 1);
                        fn_80109C88(lbl_803FB338, result);
                    }
                    *(u32*)(lbl_803FB380 + 0x0C) = result;
                }
            }
            break;
        }

        case 3:
        case 4:
        case 7:
            menuStatusMoveInput(menu, input, limit);
            break;

        case 6:
            menuStatusRibbonInput(menu, input);
            break;

        case 0:
        case 8:
            break;
    }
}

typedef struct MenuStatusPage {
    s8 group;
    s8 page;
} MenuStatusPage;

extern u8 menuIsCheck(s32 menuId);
extern void menuOpen(s32 menuId, s32 arg1);
extern void menuClose(s32 menuId);

/* Open the status submenu for the current page and close the others. */
static inline void menuStatusSyncPage(u8* menu)
{
    MenuStatusPage page;
    u32 pokemon;
    u32 index;
    s32 menuId;

    page = *(MenuStatusPage*)(menu + 0x94);
    menuId = 0;
    pokemon = *(u32*)(lbl_803FB380 + 0x0C);
    if (pokemon == 0) {
        return;
    }

    switch (page.page) {
    case 0:
        if ((s32)pokemonGetStatus(pokemon, 0, 0xC2, 0) != 0) {
            menuId = 0x55;
        } else {
            menuId = 0x54;
        }
        break;
    case 1:
        if (*(u16*)(lbl_803FB380 + 0x18) != 0) {
            menuId = 0x56;
        } else {
            menuId = 0x57;
        }
        break;
    case 2:
        menuId = 0x58;
        break;
    }

    for (index = 0; index < 5; index++) {
        if (menuId == lbl_802EEFC4[index]) {
            if (menuIsCheck(menuId) == 0) {
                menuOpen(menuId, 0);
            }
        } else if (menuIsCheck(lbl_802EEFC4[index]) != 0) {
            menuClose(lbl_802EEFC4[index]);
        }
    }
}

/* Select and synchronize the Pokemon-status submenu for the current page. */
s32 fn_800973EC(u8* menu)
{
    switch ((s8)menu[1]) {
    case 0:
        *(s8*)(menu + 0x97) = -1;
        lbl_803FB380[1] = 0;
        lbl_803FB380[2] = menu[0x95];
        *(s8*)(lbl_803FB380 + 3) = -1;
        *(s8*)(lbl_803FB380 + 0x1A) = -1;
        if (lbl_803FB380[0] & 4) {
            menu[0x95] = 0;
            lbl_803FB380[1] = 1;
        } else {
            menu[0x95] = 1;
            lbl_803FB380[1] = 7;
        }
        menuStatusSyncPage(menu);
        break;
    case 1:
        break;
    case 2:
        menuStatusSyncPage(menu);
        break;
    }

    return 0;
}
/* 0x80097BBC | size: 0x114 */
s32 fn_80097BBC(u8 chan) {
    extern void* savedataGetStatus();
    extern void* heroBiosGetPokemonPtr();
    extern int pokemonCheckValid();
    extern int fn_8010B560();
    void* entity;
    void* mgr;

    entity = NULL;
    if (chan < 6) {
        mgr = savedataGetStatus(0, 2);
        if (mgr != 0) {
            entity = heroBiosGetPokemonPtr(mgr, chan);
            if ((u8)pokemonCheckValid() == 0) {
                entity = NULL;
            }
        }
    }
    if (entity == 0) {
        return -1;
    }
    while ((u8)fn_8010B560() != 0) {
        _threadSwitch();
    }
    memset(lbl_803FB380, 0, 0x44);
    *(u8*)(lbl_803FB380 + 0x0) = 0x11;
    *(u32*)(lbl_803FB380 + 0x8) = 0;
    *(u32*)(lbl_803FB380 + 0xC) = (u32)entity;
    *(u16*)(lbl_803FB380 + 0x18) = 0;
    *(u32*)(lbl_803FB380 + 0x10) = 0;
    *(u32*)(lbl_803FB380 + 0x14) = 0;
    *(u32*)(lbl_803FB380 + 0x4) = -1;
    fn_800FF730(0x39d);
    if (lbl_803FB380[0] & 8) {
        floorSetFadeScript(0, 0);
    }
    _threadSwitch();
    return *(s32*)(lbl_803FB380 + 0x4);
}

/* 0x8009769C | size: 0x350 */
u32 fn_8009769C(u8 flags, u32 arg1, s32 pokemon, u16 arg3, u32 arg4, u32 arg5) {
    extern void menuModelInit(void* model, s32 width, s32 height);
    extern void fn_80109C88(void* model, u32 pokemon);
    extern void fadeCheck(s32 wait);
    extern void fadeSet(s32 mode, f32 value);
    extern s32 menuOpenCustom(s32 menu, ...);
    extern u32 pokemonGetStatus(u32 pokemon, u32 index, u32 status, s32 slot);
    extern u8 pokemonWazaCheckValid(u32 pokemon, s32 slot);
    extern s32 wazaGetStatus(u32 data, u16 index, u32 status, u32 arg3);
    extern void winMsgOpen(s32 slot, s32 message, s32 arg2, s32 arg3);
    extern void winMsgClose(s32 slot);
    extern s32 menuIsCheck(s32 menu);
    extern void menuCloseCustom(s32 menu, s32 mode, s32 wait);
    extern void fn_800FF660(void);
    extern void fn_8010A420(void* model);

    u32 cursor;
    s32 result;
    u16 move;
    u32 selectedPokemon;
    u8 currentFlags;
    u8* state;

    menuModelInit(lbl_803FB338, 0xC8, 0xB4);
    fn_80109C88(lbl_803FB338, pokemon);

    if (lbl_803FB380[0] & 8) {
        fadeCheck(1);
        if (lbl_803FB380[0] & 0x80) {
            fadeSet(2, lbl_8047C234);
        } else {
            fadeSet(2, lbl_8047C238);
        }
    }

    cursor = 0;
    state = lbl_803FB380;
    for (;;) {
        result = menuOpenCustom(0x53, 0, &cursor, 0, 1, 0);
        if (result == -1) {
            *(s32*)(state + 4) = result;
            break;
        }

        pokemon = *(s8*)(state + 2);
        *(u32*)(state + 4) = *(s8*)(state + 2);
        if (!(state[0] & 0x40)) {
            break;
        }

        selectedPokemon = *(u32*)(state + 0xC);
        if ((u16)pokemon == 4) {
            move = *(u16*)(state + 0x18);
        } else {
            move = (u16)pokemonGetStatus(selectedPokemon, 0, 0x7F, pokemon);
            if ((u8)pokemonWazaCheckValid(selectedPokemon, pokemon) == 0) {
                move = 0;
            }
        }

        if (wazaGetStatus(0, move, 0x19, 0) == 0) {
            break;
        }
        winMsgOpen(2, 0x2BE9, 1, 0);
        winMsgClose(1);
    }

    if (lbl_803FB380[0] & 8) {
        fadeCheck(1);
        if (lbl_803FB380[0] & 0x80) {
            fadeSet(3, lbl_8047C234);
        } else {
            fadeSet(3, lbl_8047C238);
        }
        fadeCheck(1);
    }

    currentFlags = lbl_803FB380[0];
    if ((currentFlags & 1) && !(currentFlags & 8)) {
        fadeCheck(1);
        fadeSet(3, lbl_8047C238);
        fadeCheck(1);
    }

    if ((u8)menuIsCheck(0x54) != 0) {
        menuCloseCustom(0x54, 0, 0);
    }
    if ((u8)menuIsCheck(0x55) != 0) {
        menuCloseCustom(0x55, 0, 0);
    }
    if ((u8)menuIsCheck(0x57) != 0) {
        menuCloseCustom(0x57, 0, 0);
    }
    if ((u8)menuIsCheck(0x56) != 0) {
        menuCloseCustom(0x56, 0, 0);
    }
    if ((u8)menuIsCheck(0x58) != 0) {
        menuCloseCustom(0x58, 0, 0);
    }
    menuCloseCustom(0x53, 0, 1);

    if (lbl_803FB380[0] & 1) {
        fn_800FF660();
        if (lbl_803FB380[0] & 8) {
            floorSetFadeScript(0, 0);
        }
    }

    fn_8010A420(lbl_803FB338);
    _threadSwitch();
    return *(u32*)(state + 4);
}

/* 0x800979EC | size: 0x4C */
void menuPokemonStatus(void) {
    *(u32*)(lbl_803FB380 + 4) = fn_8009769C(
        lbl_803FB380[0],
        *(u32*)(lbl_803FB380 + 8),
        *(u32*)(lbl_803FB380 + 0xC),
        *(u16*)(lbl_803FB380 + 0x18),
        *(u32*)(lbl_803FB380 + 0x10),
        *(u32*)(lbl_803FB380 + 0x14));
}

/* 0x80097FCC | size: 0x4 */
void fn_80097FCC(void) {
}

/* 0x80097FD0 | size: 0x28 */
void fn_80097FD0(void) {
    extern int fn_80113F48();
    GSresGetResource(fn_80113F48(), 0x12670000);
}

/* 0x80097FF8 | size: 0x4 */
void fn_80097FF8(void) {
}

/* 0x80097A38 | size: 0xCC */
s32 fn_80097A38(u32 arg0, u16 arg1) {
    extern int fn_8010B560();

    while ((u8)fn_8010B560() != 0) {
        _threadSwitch();
    }
    memset(lbl_803FB380, 0, 0x44);
    *(u8*)(lbl_803FB380 + 0x0) = 0x59;
    *(u32*)(lbl_803FB380 + 0x8) = 0;
    *(u32*)(lbl_803FB380 + 0xC) = arg0;
    *(u16*)(lbl_803FB380 + 0x18) = arg1;
    *(u32*)(lbl_803FB380 + 0x10) = 0;
    *(u32*)(lbl_803FB380 + 0x14) = 0;
    *(s32*)(lbl_803FB380 + 0x4) = -1;
    fn_800FF730(0x39d);
    if (lbl_803FB380[0] & 8) {
        floorSetFadeScript(0, 0);
    }
    _threadSwitch();
    return *(s32*)(lbl_803FB380 + 0x4);
}

/* 0x80097B04 | size: 0xB8 */
s32 fn_80097B04(u32 arg0, u16 arg1) {
    extern int fn_8010B560();

    while ((u8)fn_8010B560() != 0) {
        _threadSwitch();
    }
    memset(lbl_803FB380, 0, 0x44);
    *(u8*)(lbl_803FB380 + 0x0) = 0x58;
    *(u32*)(lbl_803FB380 + 0x8) = 0;
    *(u32*)(lbl_803FB380 + 0xC) = arg0;
    *(u16*)(lbl_803FB380 + 0x18) = arg1;
    *(u32*)(lbl_803FB380 + 0x10) = 0;
    *(u32*)(lbl_803FB380 + 0x14) = 0;
    *(s32*)(lbl_803FB380 + 0x4) = -1;
    fn_8009769C(lbl_803FB380[0], *(u32*)(lbl_803FB380 + 0x8), *(u32*)(lbl_803FB380 + 0xC),
                arg1, *(u32*)(lbl_803FB380 + 0x10), *(u32*)(lbl_803FB380 + 0x14));
    return *(s32*)(lbl_803FB380 + 0x4);
}

/* 0x80097CD0 | size: 0xC4 */
s32 fn_80097CD0(u32 arg0, u32 arg1, u32 arg2) {
    extern int fn_8010B560();

    while ((u8)fn_8010B560() != 0) {
        _threadSwitch();
    }
    memset(lbl_803FB380, 0, 0x44);
    *(u8*)(lbl_803FB380 + 0x0) = 0xc;
    *(u32*)(lbl_803FB380 + 0x8) = 0;
    *(u32*)(lbl_803FB380 + 0xC) = arg0;
    *(u16*)(lbl_803FB380 + 0x18) = 0;
    *(u32*)(lbl_803FB380 + 0x10) = arg1;
    *(u32*)(lbl_803FB380 + 0x14) = arg2;
    *(s32*)(lbl_803FB380 + 0x4) = -1;
    fn_8009769C(lbl_803FB380[0], *(u32*)(lbl_803FB380 + 0x8), *(u32*)(lbl_803FB380 + 0xC),
                *(u16*)(lbl_803FB380 + 0x18), *(u32*)(lbl_803FB380 + 0x10), *(u32*)(lbl_803FB380 + 0x14));
    return *(s32*)(lbl_803FB380 + 0x4);
}

/* 0x80097D94 | size: 0xC4 */
s32 fn_80097D94(u32 arg0, u32 arg1, u32 arg2) {
    extern int fn_8010B560();

    while ((u8)fn_8010B560() != 0) {
        _threadSwitch();
    }
    memset(lbl_803FB380, 0, 0x44);
    *(u8*)(lbl_803FB380 + 0x0) = 0xe;
    *(u32*)(lbl_803FB380 + 0x8) = 0;
    *(u32*)(lbl_803FB380 + 0xC) = arg0;
    *(u16*)(lbl_803FB380 + 0x18) = 0;
    *(u32*)(lbl_803FB380 + 0x10) = arg1;
    *(u32*)(lbl_803FB380 + 0x14) = arg2;
    *(s32*)(lbl_803FB380 + 0x4) = -1;
    fn_8009769C(lbl_803FB380[0], *(u32*)(lbl_803FB380 + 0x8), *(u32*)(lbl_803FB380 + 0xC),
                *(u16*)(lbl_803FB380 + 0x18), *(u32*)(lbl_803FB380 + 0x10), *(u32*)(lbl_803FB380 + 0x14));
    return *(s32*)(lbl_803FB380 + 0x4);
}

/* 0x80097E58 | size: 0xB0 */
s32 fn_80097E58(u32 arg0, u32 arg1, u32 arg2, u32 arg3) {
    extern int fn_8010B560();

    while ((u8)fn_8010B560() != 0) {
        _threadSwitch();
    }
    memset(lbl_803FB380, 0, 0x44);
    *(u8*)(lbl_803FB380 + 0x0) = 0xac;
    *(u32*)(lbl_803FB380 + 0x8) = arg0;
    *(u32*)(lbl_803FB380 + 0xC) = arg1;
    *(u16*)(lbl_803FB380 + 0x18) = 0;
    *(u32*)(lbl_803FB380 + 0x10) = arg2;
    *(u32*)(lbl_803FB380 + 0x14) = arg3;
    *(s32*)(lbl_803FB380 + 0x4) = -1;
    fn_8009769C(lbl_803FB380[0], *(u32*)(lbl_803FB380 + 0x8), *(u32*)(lbl_803FB380 + 0xC),
                *(u16*)(lbl_803FB380 + 0x18), *(u32*)(lbl_803FB380 + 0x10), *(u32*)(lbl_803FB380 + 0x14));
    return *(s32*)(lbl_803FB380 + 0x4);
}

/* 0x80097F08 | size: 0xC4 */
s32 fn_80097F08(u32 arg0, u32 arg1, u32 arg2) {
    extern int fn_8010B560();

    while ((u8)fn_8010B560() != 0) {
        _threadSwitch();
    }
    memset(lbl_803FB380, 0, 0x44);
    *(u8*)(lbl_803FB380 + 0x0) = 0x8e;
    *(u32*)(lbl_803FB380 + 0x8) = 0;
    *(u32*)(lbl_803FB380 + 0xC) = arg0;
    *(u16*)(lbl_803FB380 + 0x18) = 0;
    *(u32*)(lbl_803FB380 + 0x10) = arg1;
    *(u32*)(lbl_803FB380 + 0x14) = arg2;
    *(s32*)(lbl_803FB380 + 0x4) = -1;
    fn_8009769C(lbl_803FB380[0], *(u32*)(lbl_803FB380 + 0x8), *(u32*)(lbl_803FB380 + 0xC),
                *(u16*)(lbl_803FB380 + 0x18), *(u32*)(lbl_803FB380 + 0x10), *(u32*)(lbl_803FB380 + 0x14));
    return *(s32*)(lbl_803FB380 + 0x4);
}

