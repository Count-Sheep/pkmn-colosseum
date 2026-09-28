/* fightSideSetStatus and fightSideGetStatus, 0x801F75F8 - 0x801F7798, with
 * their switch tables (.data 0x80375628 - 0x80375670). Linked together:
 * fightSideGetStatus's table (0x8037564C) is not 8-aligned, so it can only
 * follow fightSideSetStatus's table (0x80375628) in the same object. */
#define FIGHT_SIDE_801F75F8_801F76B8
#define FIGHT_SIDE_801F76B8_801F7798
#include "src/game/fight_side.c"
