#include "dolphin/types.h"

#pragma section ".data"

extern u8 lbl_802E4B98[];
extern void* jumptable_802E4BB8[];
extern void* jumptable_802E4C20[];

extern u8 menuFightDrawSecretPokemonStatus[];
extern u8 menuFightDrawSecretPokemon[];

/* Auto-carved .data unit 0x802E4B98..0x802E4C80 (3 objects). Non-relocated data as byte-exact u8[]; pointer/jump tables as void*[] for R_PPC_ADDR32 relocations. */

u8 lbl_802E4B98[32] = {
    0x00, 0x30, 0x00, 0x31, 0x00, 0x32, 0x00, 0x33, 0x00, 0x34, 0x00, 0x35,
    0x00, 0x36, 0x00, 0x37, 0x00, 0x38, 0x00, 0x39, 0x00, 0x41, 0x00, 0x42,
    0x00, 0x43, 0x00, 0x44, 0x00, 0x45, 0x00, 0x46,
};

void* jumptable_802E4BB8[26] = {
    (void*)((u8*)menuFightDrawSecretPokemonStatus + 0x100),
    (void*)((u8*)menuFightDrawSecretPokemonStatus + 0x100),
    (void*)((u8*)menuFightDrawSecretPokemonStatus + 0x100),
    (void*)((u8*)menuFightDrawSecretPokemonStatus + 0x100),
    (void*)((u8*)menuFightDrawSecretPokemonStatus + 0x100),
    (void*)((u8*)menuFightDrawSecretPokemonStatus + 0x100),
    (void*)((u8*)menuFightDrawSecretPokemonStatus + 0x18C),
    (void*)((u8*)menuFightDrawSecretPokemonStatus + 0x1FC),
    (void*)((u8*)menuFightDrawSecretPokemonStatus + 0x1B4),
    (void*)((u8*)menuFightDrawSecretPokemonStatus + 0x260),
    (void*)((u8*)menuFightDrawSecretPokemonStatus + 0x260),
    (void*)((u8*)menuFightDrawSecretPokemonStatus + 0x260),
    (void*)((u8*)menuFightDrawSecretPokemonStatus + 0x260),
    (void*)((u8*)menuFightDrawSecretPokemonStatus + 0x2F8),
    (void*)((u8*)menuFightDrawSecretPokemonStatus + 0x2F8),
    (void*)((u8*)menuFightDrawSecretPokemonStatus + 0x2F8),
    (void*)((u8*)menuFightDrawSecretPokemonStatus + 0x2F8),
    (void*)((u8*)menuFightDrawSecretPokemonStatus + 0x40C),
    (void*)((u8*)menuFightDrawSecretPokemonStatus + 0x40C),
    (void*)((u8*)menuFightDrawSecretPokemonStatus + 0x40C),
    (void*)((u8*)menuFightDrawSecretPokemonStatus + 0x40C),
    (void*)((u8*)menuFightDrawSecretPokemonStatus + 0x4B8),
    (void*)((u8*)menuFightDrawSecretPokemonStatus + 0x504),
    (void*)((u8*)menuFightDrawSecretPokemonStatus + 0x5F0),
    (void*)((u8*)menuFightDrawSecretPokemonStatus + 0x654),
    (void*)((u8*)menuFightDrawSecretPokemonStatus + 0x72C),
};

void* jumptable_802E4C20[24] = {
    (void*)((u8*)menuFightDrawSecretPokemon + 0x80),
    (void*)((u8*)menuFightDrawSecretPokemon + 0x78),
    (void*)((u8*)menuFightDrawSecretPokemon + 0x70),
    (void*)((u8*)menuFightDrawSecretPokemon + 0x68),
    (void*)((u8*)menuFightDrawSecretPokemon + 0x60),
    (void*)((u8*)menuFightDrawSecretPokemon + 0x58),
    (void*)((u8*)menuFightDrawSecretPokemon + 0x80),
    (void*)((u8*)menuFightDrawSecretPokemon + 0x78),
    (void*)((u8*)menuFightDrawSecretPokemon + 0x70),
    (void*)((u8*)menuFightDrawSecretPokemon + 0x68),
    (void*)((u8*)menuFightDrawSecretPokemon + 0x60),
    (void*)((u8*)menuFightDrawSecretPokemon + 0x58),
    (void*)((u8*)menuFightDrawSecretPokemon + 0x80),
    (void*)((u8*)menuFightDrawSecretPokemon + 0x78),
    (void*)((u8*)menuFightDrawSecretPokemon + 0x70),
    (void*)((u8*)menuFightDrawSecretPokemon + 0x68),
    (void*)((u8*)menuFightDrawSecretPokemon + 0x60),
    (void*)((u8*)menuFightDrawSecretPokemon + 0x58),
    (void*)((u8*)menuFightDrawSecretPokemon + 0x80),
    (void*)((u8*)menuFightDrawSecretPokemon + 0x78),
    (void*)((u8*)menuFightDrawSecretPokemon + 0x70),
    (void*)((u8*)menuFightDrawSecretPokemon + 0x68),
    (void*)((u8*)menuFightDrawSecretPokemon + 0x60),
    (void*)((u8*)menuFightDrawSecretPokemon + 0x58),
};
