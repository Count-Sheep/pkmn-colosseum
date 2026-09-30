/**
 * @file msgctrl_r49_80131A34_suffix.c
 * @brief msgctrl.c carve, 0x80131A34 - 0x80131BA0: the digit wrappers
 *        msgctrlMoney, msgctrlMenuZDigit2/ZDigit, msgctrlMenuHex2/Hex and
 *        msgctrlMenuUDigit2/UDigit.
 *
 * Text only. Built with the TU's -O4,p and the peephole pass off (one
 * unit-wide flag in configure.py, as for msgctrl.c and
 * msgctrl_exact_80132A38.c); with the pass on, each wrapper loses retail's
 * unfolded argument moves. The same bodies are in src/game/msgctrl.c.
 */
#include "dolphin/types.h"
#include "game/effect/effect_util_types.h"

void msgctrlMoney(void)
{
    _msgctrlMakeDigit__FPUslUll(lbl_80427070, 0x10, lbl_8047AE80, 4);
}

void msgctrlMenuZDigit2(void)
{
    _msgctrlMakeDigit__FPUslUll(lbl_80427090, 0x10, lbl_8047AE6C, 2);
}

void msgctrlMenuZDigit(void)
{
    _msgctrlMakeDigit__FPUslUll(lbl_804270B0, 0x10, lbl_8047AE68, 2);
}

void msgctrlMenuHex2(void)
{
    _msgctrlMakeDigit__FPUslUll(lbl_804270D0, 0x10, lbl_8047AE68, 3);
}

void msgctrlMenuHex(void)
{
    _msgctrlMakeDigit__FPUslUll(lbl_804270F0, 0x10, lbl_8047AE68, 3);
}

void msgctrlMenuUDigit2(void)
{
    _msgctrlMakeDigit__FPUslUll(lbl_80427110, 0x10, lbl_8047AE6C, 1);
}

void msgctrlMenuUDigit(void)
{
    _msgctrlMakeDigit__FPUslUll(lbl_80427130, 0x10, lbl_8047AE68, 1);
}
