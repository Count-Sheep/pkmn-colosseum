/**
 * @file cardesavedata_r51_80083D30.c
 * @brief fn_80083D30 / fn_80083ECC (0x80083D30 - 0x80084034): encode the
 *        party's move descriptions into Card-e text records.
 *
 * Function-boundary carve of the Card-e save-data TU (see cardesavedata.c),
 * text only, on the TU's unit-wide -opt nopeephole. fn_80083D30 carries
 * fn_80083ECC's body inline. MWCC only auto-inlines a function defined
 * earlier, so the unit is built with -inline auto,deferred, under which it
 * is generated in reverse definition order (like pslist.c); GC/1.3 only
 * auto-inlines fn_80083ECC with the line copy as its own inline.
 *
 * Open: the two line buffers are laid out in opposite orders in the two
 * functions here, but in the same order in retail, and the second line's
 * pointer and character trade scratch registers.
 */
#include "dolphin/types.h"

extern void* memset(void* dst, int val, u32 size);
extern void* heroBiosGetPokemonPtr(void* hero, u16 index);
extern u16 pokemonBiosGetPokemonWazaDataId(void* pokemon, u16 index);
extern void* wazaDataBiosGetPtr(u16 id);
extern u32 wazaDataBiosGetDoc(void* waza);
extern const u16* GSmsgGetGSchar(u32 msg);
extern u32 gamedataGetStatus(s32, s32);
extern u32 fn_800F9AEC(u8* destination, const u16* text, u32 font);

/* Copy one 0x0000/0xFFFF-terminated line (at most 0x50 characters). */
static inline const u16* CardECopyLine(u16* buffer, const u16* text)
{
    s32 count;

    count = 0;
    while (*text != 0 && *text != 0xFFFF) {
        if (count < 0x50) {
            buffer[count++] = *text;
        }
        text++;
    }
    buffer[count] = 0;
    return text;
}

/* Encode one move's description (two lines split at 0xFFFF). */
s32 fn_80083ECC(u8* destination, u16 id)
{
    u16 first[0x52];
    u16 second[0x52];
    const u16* text;

    text = wazaDataBiosGetPtr(id);
    memset(destination, 0, 0x50);
    if (text != NULL) {
        text = GSmsgGetGSchar(wazaDataBiosGetDoc((void*)text));
        text = CardECopyLine(first, text);
        destination += fn_800F9AEC(destination, first, gamedataGetStatus(0, 5));
        if (*text == 0xFFFF) {
            *destination++ = 0xFE;
            CardECopyLine(second, (const u16*)((const u8*)text + 3));
            destination += fn_800F9AEC(destination, second, gamedataGetStatus(0, 5));
        }
        *destination = 0xFF;
    }
    return 0;
}

/* Encode the four move descriptions of each of the six party members. */
s32 fn_80083D30(void* hero, u8* destination)
{
    s32 partyIndex;
    s32 moveIndex;
    void* pokemon;

    for (partyIndex = 0; partyIndex < 6; partyIndex++) {
        pokemon = heroBiosGetPokemonPtr(hero, partyIndex);
        for (moveIndex = 0; moveIndex < 4; moveIndex++) {
            fn_80083ECC(destination,
                        pokemonBiosGetPokemonWazaDataId(pokemon, moveIndex));
            destination += 0x50;
        }
    }
    return 0;
}
