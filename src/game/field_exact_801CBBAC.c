/**
 * @file field_exact_801CBBAC.c
 * @brief One-shot SHA-1 of a buffer (0x801CBBAC - 0x801CBCDC).
 *
 * First function of the save-data SHA-1 code, whose translation unit starts
 * here: the script code before it (up to fn_801CBAB8) was built with the
 * peephole pass off, while this function and the rest of the SHA-1 and
 * memory-card code are only exact with it on. Built on that unit's flags
 * (GC/2.5, -O4,p; see configure.py). No data.
 *
 * The context is initialised and the whole buffer fed in the way SHA1Init
 * and SHA1Update do, then fn_801CBF64 (SHA1Final) writes the digest. The
 * full blocks are read through a pointer that advances with the block
 * index.
 */
#include "dolphin/types.h"
#include "game/save/savedata_sha1.h"

extern void* memcpy(void* dst, const void* src, u32 size);

void fn_801CBBAC(u8 digest[20], const u8* input, u32 length)
{
    FieldSha1Context context;
    u32 index;
    u32 part_length;
    u32 i;
    const u8* current_input;

    context.state[0] = 0x67452301;
    context.state[1] = 0xEFCDAB89;
    context.state[2] = 0x98BADCFE;
    context.state[3] = 0x10325476;
    context.state[4] = 0xC3D2E1F0;
    context.count[0] = 0;
    context.count[1] = 0;

    index = (context.count[0] >> 3) & 0x3F;
    if ((context.count[0] += length << 3) < (length << 3)) {
        context.count[1]++;
    }
    context.count[1] += length >> 29;

    if (index + length > 63) {
        part_length = 64 - index;
        memcpy(&context.buffer[index], input, part_length);
        fn_801CC380(context.state, context.buffer);
        current_input = &input[part_length];
        for (i = part_length; i + 63 < length; i += 64) {
            fn_801CC380(context.state, current_input);
            current_input += 64;
        }
        index = 0;
    } else {
        i = 0;
    }
    memcpy(&context.buffer[index], &input[i], length - i);
    fn_801CBF64(digest, &context);
}
