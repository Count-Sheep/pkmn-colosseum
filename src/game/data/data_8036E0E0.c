#include "dolphin/types.h"

#pragma section ".data"

/* Tail of the former auto-carved .data unit 0x8036DE70..0x8036E14C; the memory-card TU (src/game/memcard.c) owns its switch tables 0x8036DE70..0x8036E030, the battle-intro TU (src/game/battle/battle_candidate_801D0C30.c) owns its object/part/anim/model tables 0x8036E030..0x8036E0E0, and the mailMain TU (src/game/mailMain.c) owns chkMailSend's jump table 0x8036E130..0x8036E14C. */

u32 lbl_8036E0E0[20] = {
    0x0000000D, 0x000036C3,
    0x0000000E, 0x000036C4,
    0x0000000F, 0x000036C5,
    0x00000011, 0x000036C6,
    0x00000010, 0x000036C7,
    0x00000012, 0x000036C8,
    0x00000013, 0x000036C9,
    0x00000014, 0x000036CA,
    0x00000015, 0x000036CB,
    0x00000016, 0x000036CC,
};
