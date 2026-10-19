/**
 * @file menuFight_exact_8000F768.c
 * @brief menuFightDrawSecretWazaSelect .. menuFightDrawBall,
 *        .text 0x8000F768 - 0x800100C0, .data 0x802E4CD8 - 0x802E4D8C.
 *
 * Function-boundary carve of the menuFight TU. DrawSecretWazaSelect's
 * switch table (0x802E4CD8, 8-aligned) is followed directly by DrawBall's
 * (0x802E4D2C, only 4-aligned), so the two tables have to come from one
 * object: this unit compiles DrawSecretWazaSelect from menuFight.c (under
 * MENU_FIGHT_ONLY_8000F768), then the secret-move top window functions and
 * DrawBall in address order. GC/1.3 -O4,p with the TU's unit-wide
 * -opt nopeephole, no pragmas.
 */
#define MENU_FIGHT_ONLY_8000F768
#include "src/game/menuFight.c"
#include "src/game/menuFight_exact_8000F964.c"
#include "src/game/menuFight_exact_8000FFA8.c"
