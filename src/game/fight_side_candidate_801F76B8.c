/* Link triage (2026-09-28): fightSideGetStatus is exact and its switch table
 * is jumptable_8037564C (.data 0x8037564C-0x80375670), right after
 * fightSideSetStatus's table at 0x80375628. MWCC aligns an object's .data to
 * 8, so a carve of this function alone would place its table at 0x80375650.
 * It can link together with 0x801F75F8 (fightSideSetStatus, .data from
 * 0x80375628) once fightSideSetStatus (89.4%) is exact. */
#define FIGHT_SIDE_801F76B8_801F7798
#include "src/game/fight_side.c"
