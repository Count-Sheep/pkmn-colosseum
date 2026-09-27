/**
 * @file win_msg_exact_80105FB0.c
 * @brief winMsgCloseError (0x80105FB0 - 0x80105FF8).
 *
 * Closes the error message window (menu 0x10C) if it is open and clears
 * the error-window flag lbl_8047AD10 that winMsgOpenError sets.
 *
 * Flags: the winMsg TU (0x801058CC - 0x80106F98) is built with the
 * project's -O4,p but with the peephole pass off. Retail keeps
 * "clrlwi r0,r3,24; cmplwi r0,0" instead of the record form, leaves
 * "bne; b" pairs uninverted (winMsgOpenError) and keeps "mr r6,r3;
 * mr r3,r6" copies; with "-opt nopeephole" and no local pragmas this
 * function is exact (83% without it), and winMsgClose, winMsgCheck,
 * winMsgCheckField, winMsgCloseField, winMsgCloseFight,
 * winMsgCloseCheckFight and winMsgCloseLevelUpStatus differ only in their
 * jump-table relocation names. Carved out so it can link while the rest
 * of the TU stays a candidate.
 */
#include "dolphin/types.h"

extern u8 lbl_8047AD10; /* error window open */

extern u8 menuIsCheck(s32 menuId);
extern s32 menuCloseCustom(s32 menuId, s32 mode, s32 wait);

void winMsgCloseError(void)
{
    if (menuIsCheck(0x10C)) {
        menuCloseCustom(0x10C, 2, 0);
    }
    lbl_8047AD10 = 0;
}
