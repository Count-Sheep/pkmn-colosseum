#include "dolphin/types.h"

#pragma section ".data"

extern void* jumptable_802E4CA8[];
extern void* jumptable_802E4CD8[];
extern void* jumptable_802E4D2C[];

extern u8 menuFightDrawSecretWazaDoc[];
extern u8 menuFightDrawSecretWazaSelect[];
extern u8 menuFightDrawBall[];

/* Auto-carved .data unit 0x802E4CA8..0x802E4D8C (3 objects): the menuFight jump tables after DrawSecretSelect's (owned by menuFight_exact_8000F310.c). Pointer/jump tables as void*[] for R_PPC_ADDR32 relocations. */

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

void* jumptable_802E4CD8[21] = {
    (void*)((u8*)menuFightDrawSecretWazaSelect + 0x80),
    (void*)((u8*)menuFightDrawSecretWazaSelect + 0x88),
    (void*)((u8*)menuFightDrawSecretWazaSelect + 0x90),
    (void*)((u8*)menuFightDrawSecretWazaSelect + 0x78),
    (void*)((u8*)menuFightDrawSecretWazaSelect + 0x94),
    (void*)((u8*)menuFightDrawSecretWazaSelect + 0x80),
    (void*)((u8*)menuFightDrawSecretWazaSelect + 0x88),
    (void*)((u8*)menuFightDrawSecretWazaSelect + 0x90),
    (void*)((u8*)menuFightDrawSecretWazaSelect + 0x78),
    (void*)((u8*)menuFightDrawSecretWazaSelect + 0x80),
    (void*)((u8*)menuFightDrawSecretWazaSelect + 0x88),
    (void*)((u8*)menuFightDrawSecretWazaSelect + 0x90),
    (void*)((u8*)menuFightDrawSecretWazaSelect + 0x78),
    (void*)((u8*)menuFightDrawSecretWazaSelect + 0x80),
    (void*)((u8*)menuFightDrawSecretWazaSelect + 0x88),
    (void*)((u8*)menuFightDrawSecretWazaSelect + 0x90),
    (void*)((u8*)menuFightDrawSecretWazaSelect + 0x78),
    (void*)((u8*)menuFightDrawSecretWazaSelect + 0x80),
    (void*)((u8*)menuFightDrawSecretWazaSelect + 0x88),
    (void*)((u8*)menuFightDrawSecretWazaSelect + 0x90),
    (void*)((u8*)menuFightDrawSecretWazaSelect + 0x78),
};

void* jumptable_802E4D2C[24] = {
    (void*)((u8*)menuFightDrawBall + 0x58),
    (void*)((u8*)menuFightDrawBall + 0x60),
    (void*)((u8*)menuFightDrawBall + 0x68),
    (void*)((u8*)menuFightDrawBall + 0x70),
    (void*)((u8*)menuFightDrawBall + 0x78),
    (void*)((u8*)menuFightDrawBall + 0x80),
    (void*)((u8*)menuFightDrawBall + 0x58),
    (void*)((u8*)menuFightDrawBall + 0x60),
    (void*)((u8*)menuFightDrawBall + 0x68),
    (void*)((u8*)menuFightDrawBall + 0x70),
    (void*)((u8*)menuFightDrawBall + 0x78),
    (void*)((u8*)menuFightDrawBall + 0x80),
    (void*)((u8*)menuFightDrawBall + 0x58),
    (void*)((u8*)menuFightDrawBall + 0x60),
    (void*)((u8*)menuFightDrawBall + 0x68),
    (void*)((u8*)menuFightDrawBall + 0x70),
    (void*)((u8*)menuFightDrawBall + 0x78),
    (void*)((u8*)menuFightDrawBall + 0x80),
    (void*)((u8*)menuFightDrawBall + 0x58),
    (void*)((u8*)menuFightDrawBall + 0x60),
    (void*)((u8*)menuFightDrawBall + 0x68),
    (void*)((u8*)menuFightDrawBall + 0x70),
    (void*)((u8*)menuFightDrawBall + 0x78),
    (void*)((u8*)menuFightDrawBall + 0x80),
};

