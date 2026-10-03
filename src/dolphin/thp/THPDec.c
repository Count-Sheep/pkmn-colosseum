/**
 * @file THPDec.c
 * @brief The whole THP decoder translation unit, 0x801E5548 - 0x801ECFE0.
 *
 * Built from the shared SDK source with the decoder's own state: the
 * zig-zag/scale tables (.rodata 0x80279AE8), the IDCT workspace, locked-cache
 * work pointers and MCU buffers (.bss 0x8046D500-0x8046D630), the Huffman
 * tables and IDCT globals (.sbss 0x8047B4A0-0x8047B5B8) and its float pool
 * (.sdata2 0x8047E4B0-0x8047E4D0). Replaces the five text islands that used
 * to reference that state by name.
 */
#define THP_DECODER_ONLY
#include "THP_range_801E1B54.c"
