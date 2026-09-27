/**
 * @file msgctrl_exact_80132A38.c
 * @brief msgctrl.c tail: the message-variable setter and reset
 *        (0x80132A38 - 0x80132C6C).
 *
 * msgctrlSetValue stores the value one of the message control codes reads
 * (for example 0x4D/0x57 set the strings msgctrlString/msgctrlString2 print,
 * 0x55/0x56 the menu message IDs); other ids are ignored. msgctrlInitValue
 * clears the menu and string variables. The switch compiles to
 * jumptable_80363630, which this unit owns in .data; the case values are
 * read from that retail table (id - 0x0D indexes it), and the cases are in
 * the order their stores are laid out in retail.
 *
 * msgctrl.c was built at -O4,p with the peephole pass off: with that one
 * unit-wide flag (configure.py) and no local pragmas these two functions and
 * most of the TU's other functions are exact; retail keeps the redundant
 * clrlwi before each halfword store that the peephole pass removes.
 */
#include "dolphin/types.h"
#include "game/effect/effect_util_types.h"

void msgctrlSetValue(u32 id, u32 value)
{
    switch (id) {
    case 0x2D: lbl_8047AE50 = (u16)value; return;
    case 0x2E: lbl_8047AE52 = (u16)value; return;
    case 0x2F: lbl_8047AE54 = value; return;
    case 0x30: lbl_8047AE58 = value; return;
    case 0x31: lbl_8047AE5C = value; return;
    case 0x32: lbl_8047AE60 = value; return;
    case 0x33: lbl_8047AE64 = value; return;
    case 0x34: lbl_8047AE68 = value; return;
    case 0x35: lbl_8047AE6C = value; return;
    case 0x36: lbl_8047AE70 = value; return;
    case 0x37: lbl_8047AE74 = value; return;
    case 0x51: lbl_8047AE78 = value; return;
    case 0x39: lbl_8047AE7C = (u16)value; return;
    case 0x4D: lbl_8047AE88 = value; return;
    case 0x57: lbl_8047AE8C = value; return;
    case 0x4E: lbl_8047AE90 = (u16)value; return;
    case 0x4B: lbl_8047AE80 = value; return;
    case 0x4C: lbl_8047AE84 = value; return;
    case 0x50: lbl_8047AE94 = value; return;
    case 0x55: lbl_8047AE98 = value; return;
    case 0x56: lbl_8047AE9C = value; return;
    case 0x58: lbl_8047AEA0 = (u16)value; return;
    case 0x59: lbl_8047AEA2 = (u16)value; return;
    case 0x5D: lbl_8047AEA4 = (u16)value; return;
    case 0x0D: lbl_8047ADC8 = value; return;
    case 0x0E: lbl_8047ADCC = value; return;
    case 0x41: lbl_8047ADD0 = value; return;
    case 0x0F: lbl_8047ADD4 = value; return;
    case 0x10: lbl_8047ADD8 = value; return;
    case 0x11: lbl_8047ADDC = value; return;
    case 0x12: lbl_8047ADE0 = value; return;
    case 0x13: lbl_8047ADE4 = value; return;
    case 0x14: lbl_8047ADE8 = value; return;
    case 0x15: lbl_8047ADEC = value; return;
    case 0x16: lbl_8047ADF0 = value; return;
    case 0x17: lbl_8047ADF4 = value; return;
    case 0x18: lbl_8047ADF8 = value; return;
    case 0x19: lbl_8047ADFC = value; return;
    case 0x1A: lbl_8047AE00 = value; return;
    case 0x1B: lbl_8047AE04 = value; return;
    case 0x1C: lbl_8047AE08 = value; return;
    case 0x1D: lbl_8047AE0C = value; return;
    case 0x1E: lbl_8047AE10 = value; return;
    case 0x1F: lbl_8047AE14 = value; return;
    case 0x20: lbl_8047AE18 = value; return;
    case 0x21: lbl_8047AE1C = value; return;
    case 0x22: lbl_8047AE20 = value; return;
    case 0x23: lbl_8047AE24 = value; return;
    case 0x24: lbl_8047AE28 = value; return;
    case 0x25: lbl_8047AE2C = value; return;
    case 0x26: lbl_8047AE30 = value; return;
    case 0x27: lbl_8047AE34 = value; return;
    case 0x28: lbl_8047AE38 = value; return;
    case 0x29: lbl_8047AE3C = value; return;
    case 0x2A: lbl_8047AE40 = value; return;
    case 0x42: lbl_8047AE44 = value; return;
    case 0x43: lbl_8047AE48 = value; return;
    case 0x44: lbl_8047AE4C = value; return;
    }
}

void msgctrlInitValue(void)
{
    lbl_8047AE70 = 0;
    lbl_8047AE74 = 0;
    lbl_8047AE78 = 0;
    lbl_8047AE60 = 0;
    lbl_8047AE64 = 0;
    lbl_8047AE88 = 0;
    lbl_8047AE8C = 0;
}
