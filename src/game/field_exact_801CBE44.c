/**
 * @file field_exact_801CBE44.c
 * @brief Digest and scramble save data (0x801CBE44 - 0x801CBF64).
 *
 * Takes the SHA-1 of the whole buffer, then XORs every 20-byte block from
 * `offset` on with a running key: the first key is the complement of that
 * digest, each following key the SHA-1 of the block just scrambled. The
 * digest of the plain data is copied to `hash` for the save header.
 * fn_801CBCDC reverses this when the data is loaded.
 *
 * Built on the save-data SHA-1 unit's flags (GC/2.5 -O4,p; see
 * configure.py). The digest and key buffers are the unit's statics
 * (lbl_80467128, lbl_80467150); this carve reaches them by symbol, as
 * retail does in this function, and leaves them extern.
 */
#include "dolphin/types.h"
#include "game/save/savedata_sha1.h"

extern void* memcpy(void* dst, const void* src, u32 size);

extern u32 lbl_80467128[5];
extern u32 lbl_80467150[5];

void fn_801CBE44(u8* data, u32 size, u8* hash, u32 offset)
{
    u32* block;
    u32* word;
    u32* key;
    const u32* digest;
    s32 i;

    fn_801CBBAC((u8*)lbl_80467128, data, size);
    digest = lbl_80467128;
    key = lbl_80467150;
    for (i = 5; i != 0; i--) {
        *key++ = ~*digest++;
    }

    block = (u32*)(data + offset);
    for (; offset < size; offset += 20) {
        key = lbl_80467150;
        word = block;
        for (i = 5; i != 0; i--) {
            *word++ ^= *key++;
        }
        fn_801CBBAC((u8*)lbl_80467150, (u8*)block, 20);
        block += 5;
    }
    memcpy(hash, lbl_80467128, 20);
}
