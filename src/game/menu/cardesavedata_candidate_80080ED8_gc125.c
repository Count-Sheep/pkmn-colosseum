/**
 * @file cardesavedata_candidate_80080ED8_gc125.c
 * @brief fn_80080ED8 (0x80080ED8-0x8008102C): Shift-JIS to GS character
 *        conversion.
 *
 * Returns the source length in bytes. With a NULL destination it only
 * measures; otherwise it writes one u16 GS character per Shift-JIS
 * character (two-byte codes through the 0x81-0x9F / 0xE0-0xFC tables,
 * half-width kana 0xA1-0xDF shifted by 0xFEC0) and a terminating zero.
 *
 * XD's counterpart is `SJIStoGSchar__FPUsPc` (GXXE01 0x8010B320, TeamOrre
 * xd-decomp config/GXXE01/symbols.txt at 4989794e; asm in trevor403/xd-asm
 * b1087f18, `code/func_FUN_8010b320.s`), which fixes the (u16*, char*)
 * signature. The unit is data-free (the tables are extern) and uses the
 * default GC/1.3 -O4,p flags with -opt nopeephole: the retail `lhz r0;
 * mr r6,r0` copies and the `clrlwi r0,rN,24` before the lead-byte range
 * tests survive only without the peephole pass (XD's later build folds
 * both). Those clrlwi come from the `(u8)` casts on the `char` lead
 * byte; the inner table test and the kana arithmetic use `c` uncast.
 * Despite the file name, GC/1.2.5 scores lower.
 */
#include "dolphin/types.h"

extern const u16 lbl_80269B68[31][184];
extern const u16 lbl_8026C7F8[29][184];

u32 fn_80080ED8(u16* destination, u8* source)
{
    u8 c;
    u32 length = 0;

    if (source == NULL) {
        return 0;
    }

    if (destination == NULL) {
        while ((c = *source) != 0) {
            if (((u8)c >= 0x81 && (u8)c <= 0x9F) || ((u8)c >= 0xE0 && (u8)c <= 0xFC)) {
                source += 2;
                length += 2;
            } else {
                source++;
                length++;
            }
        }
    } else {
        while ((c = *source) != 0) {
            u16 character = c;
            u32 consumed;

            if (((u8)c >= 0x81 && (u8)c <= 0x9F) || ((u8)c >= 0xE0 && (u8)c <= 0xFC)) {
                if (c >= 0x81 && c <= 0x9F) {
                    character = lbl_80269B68[c - 0x81][(u8)source[1] - 0x40];
                } else {
                    character = lbl_8026C7F8[c - 0xE0][(u8)source[1] - 0x40];
                }
                consumed = 2;
            } else {
                if ((u8)c >= 0xA1 && (u8)c <= 0xDF) {
                    character = (u16)(c + 0xFEC0);
                }
                consumed = 1;
            }
            *destination++ = character;
            source += consumed;
            length += consumed;
        }
        *destination = 0;
    }
    return length;
}
