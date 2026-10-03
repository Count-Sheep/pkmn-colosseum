/**
 * @file THPDec_range_801E5A28.c
 * @brief THP quantization-table reader, 0x801E5A28 - 0x801E5DE4.
 *
 * Text island over the shared decoder state (owned by the data splits) that
 * owns the zig-zag and AAN scale-factor tables it reads (.rodata 0x80279AE8).
 */
#define THP_DECODER_ONLY
#define THP_DECODER_QUANT_ONLY
#define THP_DECODER_EXTERNAL_DATA
#define THP_DECODER_OWN_TABLES
#include "THP_range_801E1B54.c"
