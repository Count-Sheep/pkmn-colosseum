/** Exact menuCB Pokemon reset callback, 0x800574A8 - 0x800574E0. */
#include "dolphin/types.h"

extern u8 lbl_803A9768[];
extern void pokemonInit(void* pokemon);

/* RULE-EXCEPTION(user-approved): inherited scheduling-off mode; see docs/RULE_EXCEPTIONS.md. */
void fn_800574A8(void)
{
    pokemonInit(lbl_803A9768 + *(u32*)(lbl_803A9768 + 0x278) * 0x138 + 8);
}
