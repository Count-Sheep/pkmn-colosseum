/**
 * @file fight_timer_candidate_80265C84.c
 * @brief All-timer initialization candidate, 0x80265C84 - 0x80265D54.
 */

/* Link triage (2026-09-28): not carvable. fightTimerAllInit reads
 * fight_timer's pool entries 0x8047E6D8/0x8047E6E8/0x8047E6F0, which the
 * linked fight_timer chunks and fightTimerCommandInit also read; a carve
 * would need extern stand-ins for its literals. */
#define FIGHT_TIMER_SPLIT_ACTIVE
#define FIGHT_TIMER_ALL_INIT_ACTIVE
#include "src/game/fight_timer.c"
