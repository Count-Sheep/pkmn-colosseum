/**
 * debug-menu item lookup and its helpers, 0x80133E6C - 0x8013433C, in retail
 * order. Retail expands the helpers (defined after _dbgMenuGetItemNo) into it,
 * so this unit builds with deferred inlining.
 */
#define DBGMENU_TAIL_80133E6C_ONLY
#include "src/game/dbgMenu.c"
