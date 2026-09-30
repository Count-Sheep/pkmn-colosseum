/**
 * @file gs_pcbox_exact_8001EF78.c
 * @brief Title menu 0x15 tick, 0x8001EF78 - 0x8001F1E8.
 *
 * Runs once a frame on the title screen:
 * - the frame step lbl_8047A344 = fn_800D3088() / fn_800D37CC() (an
 *   unsigned count over a signed rate);
 * - the blink phase lbl_8047A338 advances by step / 2 and wraps to 0 at 1
 *   (fn_8001F304 turns it into PRESS START's alpha);
 * - from title state 0x1E on (except state 0xC8) the attract timer
 *   lbl_8047A334 advances by the step. When fn_801666BC(0x46A) returns 0
 *   the timer is cleared, and unless the state is 0x3E8 the title moves to
 *   state 0x28;
 * - the six menu column offsets in lbl_803A1F88 ease toward their targets
 *   at 4 * distance * step per frame, capped at 6. They snap to the target
 *   once the remaining distance is no more than the step or under 1.
 *
 * This unit owns the two int-to-float conversion biases at
 * 0x8047B828-0x8047B838, which MWCC emits into the TU's .sdata2 pool. The
 * other constants of the title TU's pool (0x8047B808-0x8047B8A0) stay in
 * the data objects and are referenced through extern stand-ins. Retail is
 * GC/1.3 with -opt nopeephole, the title TU's flag.
 */
#include "dolphin/types.h"

typedef struct TitleMenuWork {
    u8 pad0[0x10];
    f32 offset[6];     /* 0x10: current column offsets */
    f32 target[6];     /* 0x28: target column offsets */
} TitleMenuWork;

extern TitleMenuWork lbl_803A1F88;
extern s32 lbl_8047A31C;           /* title state */
extern f32 lbl_8047A334;           /* attract timer */
extern f32 lbl_8047A338;           /* blink phase */
extern f32 lbl_8047A344;           /* frame step */

/* RULE-EXCEPTION(title-path): extern named stand-ins for this TU's own pool
 * literals - see docs/RULE_EXCEPTIONS.md */
extern f32 lbl_8047B810;           /* 0.5f */
/* RULE-EXCEPTION(title-path): volatile, so each comparison reloads the zero
 * as retail's pool literal does - see docs/RULE_EXCEPTIONS.md */
extern volatile f32 lbl_8047B814;  /* 0.0f */
extern f32 lbl_8047B818;           /* 1.0f */
extern f32 lbl_8047B81C;           /* 4.0f */
extern f32 lbl_8047B820;           /* 6.0f */
extern f32 lbl_8047B824;           /* -6.0f */

extern s32 fn_800D37CC(void);
extern u32 fn_800D3088(void);
extern s32 fn_801666BC(s32 id);

#define TITLE_ABS(x) ((x) > lbl_8047B814 ? (x) : -(x))

/* Ease `cur` toward `tgt` by the frame step, capped at +-6. */
#define TITLE_EASE(cur, tgt)                                                \
    if ((cur) != (tgt)) {                                                   \
        f32 move;                                                           \
        f32 rest;                                                           \
        f32 absMove;                                                        \
        move = (tgt) - (cur);                                               \
        rest = lbl_8047B81C * move;                                         \
        move = rest * step;                                                 \
        if (move > lbl_8047B820) {                                          \
            /* RULE-EXCEPTION(title-path): no-op cast so the frontend does \
             * not share the compare's load - see docs/RULE_EXCEPTIONS.md */ \
            move = *(f32*)&lbl_8047B820;                                    \
        }                                                                   \
        if (move <= lbl_8047B824) {                                         \
            move = lbl_8047B824;                                            \
        }                                                                   \
        (cur) += move;                                                      \
        rest = (tgt) - (cur);                                               \
        if (move > lbl_8047B814) {                                          \
            absMove = move;                                                 \
        } else {                                                            \
            absMove = -move;                                                \
        }                                                                   \
        if (TITLE_ABS(rest) <= absMove || TITLE_ABS(rest) < lbl_8047B818) { \
            (cur) = (tgt);                                                  \
        }                                                                   \
    }

void fn_8001EF78(void)
{
    f32 rate;
    s32 i;
    f32 step;
    TitleMenuWork* work;

    rate = fn_800D37CC();
    lbl_8047A344 = fn_800D3088() / rate;
    lbl_8047A338 += lbl_8047A344 * lbl_8047B810;
    if (lbl_8047A338 >= lbl_8047B818) {
        lbl_8047A338 = lbl_8047B814;
    }
    if (lbl_8047A31C < 0x1E) {
        return;
    }
    if (lbl_8047A31C == 0xC8) {
        return;
    }
    lbl_8047A334 += lbl_8047A344;
    if (fn_801666BC(0x46A) == 0) {
        lbl_8047A334 = lbl_8047B814;
        if (lbl_8047A31C != 0x3E8) {
            lbl_8047A31C = 0x28;
        }
    }
    step = lbl_8047A344;
    work = &lbl_803A1F88;
    for (i = 0; i < 3; i++) {
        TITLE_EASE(work->offset[i], work->target[i])
        TITLE_EASE(work->offset[i + 3], work->target[i + 3])
    }
}
