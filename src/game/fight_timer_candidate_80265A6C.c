/**
 * @file fight_timer_candidate_80265A6C.c
 * @brief Timer-command initialization candidate, 0x80265A6C - 0x80265B3C.
 */

/* Link triage (2026-09-28): not carvable. fightTimerCommandInit reads
 * fight_timer's pool entries 0x8047E6D8/0x8047E6E8/0x8047E6F0, which the
 * linked fight_timer chunks and fightTimerAllInit also read; a carve would
 * need extern stand-ins for its literals. */
#define FIGHT_TIMER_SPLIT_ACTIVE
#define FIGHT_TIMER_COMMAND_INIT_ACTIVE
#include "src/game/fight_timer.c"
