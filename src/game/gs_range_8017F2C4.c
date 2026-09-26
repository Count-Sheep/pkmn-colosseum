/**
 * @file gs_range_8017F2C4.c
 * @brief gs-engine code, 0x8017F2C4 - 0x8017F3F8 (1 fn): the FSYS LZSS
 *        decoder.
 *
 * CodeCandidate prefix of the 0x8017F2C4 - 0x80180C78 range (see
 * gs_range_8017F3F8_middle.c for the exact island that follows). Built at
 * `-opt level=0` like the rest of the range.
 *
 * This is Okumura's LZSS Decode (N = 4096, F = 18, THRESHOLD = 2) reading
 * from memory: the stream starts after a 16-byte header, the sliding window
 * is lbl_80452FC8 and lbl_80453FDC holds the header copied by the caller
 * (word 2 is the compressed size).
 *
 * Remaining difference: retail never uses r5 as a scratch register, so the
 * `size` parameter is live somewhere in the original body; the source that
 * references it without emitting code is not recovered yet.
 */
#include "dolphin/types.h"

extern u8 lbl_80452FC8[0x1000];
extern u32 lbl_80453FDC[];

void fn_8017F2C4(u8* destination, u8* source, u32 size)
{
    u32 inPos;
    u8* in;
    u32 r;
    u32 c;
    s32 j;
    s32 k;
    u32 flags;
    u32 outPos;
    u32 inEnd;
    u8* out;
    s32 i;
    s32 count;

    out = destination;
    in = source;
    inPos = 0;
    outPos = 0;
    r = 0xFEE;
    flags = 0;
    count = 0;
    in += 0x10;
    inEnd = lbl_80453FDC[2] - 0x10;

    for (;;) {
        if (((flags >>= 1) & 0x100) == 0) {
            c = in[inPos++];
            if (inPos > inEnd) {
                return;
            }
            flags = c | 0xFF00;
        }
        if (flags & 1) {
            c = in[inPos++];
            if (inPos > inEnd) {
                return;
            }
            out[outPos++] = c;
            lbl_80452FC8[r++] = c;
            r &= 0xFFF;
        } else {
            i = in[inPos++];
            if (inPos > inEnd) {
                return;
            }
            j = in[inPos++];
            if (inPos > inEnd) {
                return;
            }
            i |= (j & 0xF0) << 4;
            j = (j & 0xF) + 2;
            for (k = 0; k <= j; k++) {
                c = lbl_80452FC8[(i + k) & 0xFFF];
                out[outPos++] = c;
                lbl_80452FC8[r++] = c;
                r &= 0xFFF;
            }
        }
        count++;
    }
}
