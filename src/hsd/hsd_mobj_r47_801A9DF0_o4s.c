/* Score instrumentation only; not evidence of a retail TU boundary. The
 * whole mtx.c TU is compiled; objdiff pairs the functions in this range.
 *
 * HSD_MtxInverseConcat (fn_801A9DF0) is exact here but cannot be carved on
 * its own: its det == 0.0f and 1.0f / det load mtx.c's pool literals
 * 1.0f (0x8047DC58) and 0.0f (0x8047DC5C), in that order, inside the
 * pool data unit hsd_sdata2_8047DC48.c (after "mtx.c", "mtx", "vec").
 * A standalone copy emits its own [0.0f, 1.0f] pool; reading the two
 * through extern const f32 compiles to the same code except that
 * det == 0.0f's fcmpu swaps its operands (f8,f0 for retail's f0,f8). */
#include "src/hsd/mtx.c"
