/**
 * @file win_sequence_exact_80107170.c
 * @brief fn_80107170 / winSeqMoveMenu, 0x80107170 - 0x801074D4.
 *
 * Function-boundary carve of the winSeq TU (see
 * win_sequence_candidate_80107170.c): no jump table, no pooled constant;
 * its data is the two .bss move targets (lbl_80404B68/8C), kept extern.
 * The move blocks are the TU's repeated-expansion helpers
 * (game/win_sequence.h); _winSeqMoveSub is called by its global symbol.
 * Same flags as the TU (GC/1.3 -O4,p, -opt nopeephole, -inline
 * auto,deferred), no pragmas; deferred mode emits in reverse definition
 * order, so the functions are defined from the highest address down.
 */
#include "game/win_sequence.h"

/* 0x801071D0: run one frame of the window's and its sprites' sequences. */
void winSeqMoveMenu(tagWINDOW_WORK* window) {
    tagSPRITE_WORK* sprite;

    winSeqMoveWindow(window);
    for (sprite = window->sprites; sprite != NULL; sprite = sprite->next) {
        winSeqMoveSprite(sprite);
    }
    for (sprite = window->overlaySprites; sprite != NULL; sprite = sprite->next) {
        winSeqMoveSprite(sprite);
    }
}

/* 0x80107170: whether item itemId of window id has a sequence still running. */
s32 fn_80107170(s32 id, s32 itemId) {
    tagSPRITE_WORK* sprite = windowSearchItemID(windowSearchID(id), itemId);

    if (sprite != NULL && sprite->sequence.commands != NULL && sprite->sequence.enabled == 0) {
        return 1;
    }
    return 0;
}
