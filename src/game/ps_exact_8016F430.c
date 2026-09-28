/**
 * @file ps_exact_8016F430.c
 * @brief HAL psinterpret.c: psInterpretParticles, 0x8016F430 - 0x8016F500,
 *        with its .rodata (0x802739A0) and .sdata2 (0x8047D628) strings.
 *
 * Function-boundary carve of psinterpret.c (see src/game/psinterpret.c for
 * the TU extent), built from that file with PSINTERPRET_EXACT_8016F430 so
 * the body is the TU's and its HSD_ASSERT keeps __FILE__ "psinterpret.c".
 * Its data is the TU's only .rodata, __FILE__ "psinterpret.c"
 * (0x802739A0-0x802739AE, 8-aligned; the next pool, generator.c's, starts
 * at 0x802739B0), and the assert expression "lastPP", the first entry of
 * the TU's .sdata2 (0x8047D628, 8-aligned; the float pool resumes at
 * 0x8047D630). No other function references either string. No jump table;
 * psInterpretParticle0 is called out of line (it is too large for the
 * auto-inliner). Built with the particle library flags (GC/1.3.2 -O4,p
 * -inline auto,deferred -use_lmw_stmw on -sdata 8 -sdata2 8 -str
 * reuse,readonly), no local pragmas.
 */
#define PSINTERPRET_EXACT_8016F430
#include "src/game/psinterpret.c"
