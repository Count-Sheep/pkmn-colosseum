/**
 * @file win_sequence_exact_80108518.c
 * @brief winSetSequence, 0x80108518 - 0x80108580.
 *
 * Function-boundary carve of the winSeq TU (see
 * win_sequence_candidate_80107170.c): the out-of-line copy of
 * winSetSequence, emitted last in retail. No jump table, no data. Same
 * flags as the TU, no pragmas.
 */
#include "game/win_sequence.h"

/* Start sequence id on state (0 clears it). */
void winSetSequence(WinSeqState* state, u32 id) {
    if ((u16)id == 0) {
        state->commands = NULL;
        state->commandIndex = 0;
    } else {
        memset(state, 0, sizeof(WinSeqState));
        state->commands = menuSeqBiosGetPtr((u16)id);
    }
}
