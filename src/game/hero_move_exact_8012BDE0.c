/**
 * @file hero_move_exact_8012BDE0.c
 * @brief heroMove TU: heroMoveAddStepCallback, .text 0x8012BDE0-0x8012BEB4.
 *
 * Text-only unit carved from the hero_move range at function boundaries.
 * The hero-move state (.bss lbl_80426BD0) stays extern. Built with the
 * hero_move TU's GC/1.3 -O4,p flags; no pragma is active. The search loop
 * is fully unrolled by the compiler, which is also how fn_8013024C inlines
 * this function twice (cbPoison and cbTsureFriend registration).
 */
#include "game/hero_move.h"

extern HeroMoveWork lbl_80426BD0;

/*
 * Registers a per-step callback in the first free slot and returns its
 * index, or -1 when all eight slots are in use. heroMoveMain calls
 * func(arg) for every registered slot.
 */
s32 heroMoveAddStepCallback(void (*func)(s32 arg), s32 arg)
{
    s32 i;

    for (i = 0; i < 8; i++) {
        if (lbl_80426BD0.stepCallback[i].func == NULL) {
            break;
        }
    }
    if (i >= 8) {
        return -1;
    }
    lbl_80426BD0.stepCallback[i].func = func;
    lbl_80426BD0.stepCallback[i].arg = arg;
    return i;
}
