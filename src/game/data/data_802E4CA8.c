#include "dolphin/types.h"

#pragma section ".data"

extern void* jumptable_802E4CA8[];

extern u8 menuFightDrawSecretWazaDoc[];

/* Auto-carved .data unit 0x802E4CA8..0x802E4CD8 (1 object): DrawSecretWazaDoc's jump table, after DrawSecretSelect's (owned by menuFight_exact_8000F310.c); 0x802E4CD8..0x802E4D8C belongs to menuFight_exact_8000FFA8.c. Pointer/jump tables as void*[] for R_PPC_ADDR32 relocations. */

void* jumptable_802E4CA8[12] = {
    (void*)((u8*)menuFightDrawSecretWazaDoc + 0xBC),
    (void*)((u8*)menuFightDrawSecretWazaDoc + 0xBC),
    (void*)((u8*)menuFightDrawSecretWazaDoc + 0xBC),
    (void*)((u8*)menuFightDrawSecretWazaDoc + 0xBC),
    (void*)((u8*)menuFightDrawSecretWazaDoc + 0x128),
    (void*)((u8*)menuFightDrawSecretWazaDoc + 0x150),
    (void*)((u8*)menuFightDrawSecretWazaDoc + 0x184),
    (void*)((u8*)menuFightDrawSecretWazaDoc + 0x354),
    (void*)((u8*)menuFightDrawSecretWazaDoc + 0x1EC),
    (void*)((u8*)menuFightDrawSecretWazaDoc + 0x354),
    (void*)((u8*)menuFightDrawSecretWazaDoc + 0x284),
    (void*)((u8*)menuFightDrawSecretWazaDoc + 0x254),
};

