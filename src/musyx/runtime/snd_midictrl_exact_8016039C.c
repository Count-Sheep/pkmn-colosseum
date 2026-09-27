/**
 * @file snd_midictrl_exact_8016039C.c
 * @brief First function of MusyX's snd_midictrl.c, 0x8016039C - 0x801603C0.
 *
 * snd_math.c ends at 0x8016039C; the reference snd_midictrl.c opens with
 * inpSetGlobalMIDIDirtyFlag (its static reset helpers are inlined into
 * later functions). No data or literal pool.
 */
#include "dolphin/types.h"

extern u32 lbl_80449390[8][16]; /* inpGlobalMIDIDirtyFlags */

void fn_8016039C(u8 chan, u8 midiSet, s32 flag) /* inpSetGlobalMIDIDirtyFlag */
{
    lbl_80449390[midiSet][chan] |= flag;
}
