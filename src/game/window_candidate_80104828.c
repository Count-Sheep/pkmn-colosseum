/**
 * @file window_candidate_80104828.c
 * @brief windowClose (0x80104828 - 0x80104A94), exact standalone owner.
 *
 * Window TU flags (GC/2.5, -O4,p, "-opt nopeephole"; see
 * window_exact_80104318.c). Data stays extern.
 *
 * Pokemon XD's windowClose (GXXE01 0x80115998, TeamOrre/xd-decomp
 * config/GXXE01/symbols.txt @4989794e, body in trevor403/xd-asm @b1087f18
 * code/func_FUN_80115998.s) calls, in this order: _windowCheckParent,
 * _windowSetCloseFlag, _windowSetCloseFlag, windowGetActiveID,
 * windowSearchID (its result copied into the window parameter's register),
 * _windowCheckParent. Colosseum's windowClose is that function with every
 * call expanded in place:
 *
 * - windowCheckParent: XD static
 *   _windowCheckParent__FP14tagWINDOW_WORKP14tagWINDOW_WORK (0x801162B8,
 *   0x38, no calls; xd-asm code/func_FUN_801162b8.s). XD's standalone body
 *   keeps a dead `b` to the loop test after its early `return 0`; so does
 *   each Colosseum expansion (0x80104878, 0x801049FC). Of the loop spellings
 *   tried, only the `for (; window != NULL; window = window->parent)` walk
 *   testing window->parent keeps that block (the argument copy lands in
 *   the guard's block and the emptied loop-entry block survives); the same
 *   source compiled standalone gives XD's 14 instructions exactly.
 * - windowSetCloseFlag: XD static _windowSetCloseFlag__FP14tagWINDOW_WORKb
 *   (0x80115B3C, 0x54, one call: menuDataBiosGetPtr(window->id)).
 *   Colosseum tests menu-data bit 3 (extrwi 1,28) where XD tests bit 0.
 * - windowGetActiveID / windowSearchID: Colosseum's own same-TU functions
 *   (0x801046B8 / 0x80104704, exact in window_exact_801046B8.c and
 *   window_exact_80104704.c; XD 0x801158A4 / 0x801158F0), which the retail
 *   compiler auto-inlined here. These are compile-only copies that preserve
 *   that same-TU inlining (as in musyx seq_exact_80146E88.c), nested as XD
 *   calls them: windowSearchID(windowGetActiveID()).
 *
 * The search result is stored back into the `window` parameter, as XD does
 * (its windowClose copies windowSearchID's r3 into the parameter's home
 * r30). That is what leaves retail's `mr r29,r5`: the inlined
 * windowSearchID's result is routed through r5 into the parameter's home
 * r29. The earlier candidate copied the parameter into a local
 * (`u8* window = ptr;`); that local's second range coalesced with the
 * result in r6 and the copy disappeared.
 *
 * The XD helpers are named after the XD functions (leading underscore
 * dropped): sister-title clause, docs/CAMPAIGN_OPERATIONS.md.
 */
#include "dolphin/types.h"

typedef struct WindowWork WindowWork;

struct WindowWork {
    u8 pad_00[4];
    s32 id;
    u8 pad_08[2];
    u8 closeFlag;
    u8 pad_0B;
    WindowWork* parent;
    WindowWork* next;
    u8 pad_14[4];
    u8 unk_18;
};

typedef struct WindowSystemWork {
    u32 field_00;
    u32 activeID;
    u8 pad_08[4];
    WindowWork* windows;
} WindowSystemWork;

extern WindowSystemWork lbl_80404ACC;

extern u8* menuDataBiosGetPtr(s32 id);

static inline u32 windowGetActiveID(void)
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

static inline u8 windowCheckParent(WindowWork* window, WindowWork* parent)
{
    if (window == parent) {
        return 0;
    }
    for (; window != NULL; window = window->parent) {
        if (window->parent == parent) {
            return 1;
        }
    }
    return 0;
}

static inline void windowSetCloseFlag(WindowWork* window, u8 checkData)
{
    if (checkData) {
        u8* data = menuDataBiosGetPtr(window->id);

        if ((u8)((data[0] >> 3) & 1) != 0) {
            return;
        }
    }
    window->closeFlag = 1;
}

/* 0x80104828 | 0x26C */
s32 windowClose(WindowWork* window, u32 flags)
{
    WindowWork* current;
    u32 keepParent;
    u8 checkData;
    u8 found;

    keepParent = flags & 2;
    checkData = flags & 4;
    found = 0;
    if (keepParent == 0) {
        for (current = lbl_80404ACC.windows; current != NULL; current = current->next) {
            if (windowCheckParent(current, window)) {
                windowSetCloseFlag(current, checkData);
            }
        }
    }
    if ((flags & 1) == 0 && window != NULL) {
        windowSetCloseFlag(window, checkData);
        if (keepParent != 0) {
            for (current = lbl_80404ACC.windows; current != NULL; current = current->next) {
                if (current->parent == window) {
                    current->parent = window->parent;
                }
            }
        }
    }

    for (window = windowSearchID(windowGetActiveID()); window != NULL; window = window->parent) {
        if (window->closeFlag != 0 || window->unk_18 != 0) {
            for (current = lbl_80404ACC.windows; current != NULL; current = current->next) {
                if (current->closeFlag == 0 && current->unk_18 == 0 &&
                    windowCheckParent(current, window->parent)) {
                    found = 1;
                    lbl_80404ACC.activeID = current->id;
                }
            }
            if (found) {
                break;
            }
        } else {
            lbl_80404ACC.activeID = window->id;
            break;
        }
    }
    if (window == NULL) {
        lbl_80404ACC.activeID = 0;
    }
    return 0;
}
