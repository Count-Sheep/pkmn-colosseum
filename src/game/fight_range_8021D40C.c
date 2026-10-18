/**
 * @file fight_range_8021D40C.c
 * @brief fn_8021D40C and fn_8021D688, 0x8021D40C - 0x8021D9C0 (battle script
 *        opcodes 0x76 and 0x75).
 *
 * FIGHT_RANGE_8021D40C_ONLY compiles only these two functions from the
 * shared body; the unit's -O4,s flag stands in for the shared file's
 * optimize_for_size pragmas, and its -inline auto default for the
 * dont_inline off region around fn_8021D688.
 */
#define FIGHT_RANGE_8021D40C_ONLY
#include "src/game/fight_range_80211A00.c"
