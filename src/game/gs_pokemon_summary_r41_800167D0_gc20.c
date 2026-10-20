/* RULE-EXCEPTION(user-approved): constant_import (temporary) — remove when gs_pokemon_summary.c links as one unit — see docs/RULE_EXCEPTIONS.md */
/*
 * Exact gs_pokemon_summary source island 0x800167D0 - 0x80016ABC
 * (fn_800167D0). Its float literals (pi, 0.5, 2, 0, 1, pi/2) and the
 * signed int-to-float bias are the TU's shared .sdata2 literals
 * (src/game/data/sdata2_8047B740.c); the temporary constant_import build
 * step redirects the island's private copies to them.
 */
#define GS_POKEMON_SUMMARY_800167D0_ONLY
#include "src/game/gs_pokemon_summary.c"
