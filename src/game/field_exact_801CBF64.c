/**
 * @file field_exact_801CBF64.c
 * @brief SHA1Final of the save-data SHA-1 (0x801CBF64 - 0x801CC380).
 *
 * Steve Reid's SHA1Final: append the 0x80 pad byte, zero bytes up to 56 mod
 * 64 and the 64-bit bit count, write the big-endian digest, wipe the
 * context and (SHA1HANDSOFF) run one more transform over the wiped buffer.
 *
 * SHA1Update is the static inline helper Final expands three times here
 * (and fn_801CBBAC once): each expansion carries the same count update,
 * the "(j + len) > 63" test, the first-block memcpy + transform and the
 * block loop.
 *
 * Built on the SHA-1/memory-card unit's flags: GC/2.5 -O4,p (GC/1.3 and
 * 2.0 schedule the first loop differently, 94.6%) with string literals
 * read-only, which is what puts "\200" and "\0" in .sdata2 (0x8047E160,
 * 0x8047E164), where this unit emits them.
 */
#include "dolphin/types.h"
#include "game/save/savedata_sha1.h"

extern void* memcpy(void* dst, const void* src, u32 size);
extern void* memset(void* dst, s32 value, u32 size);

static inline void SHA1Update(FieldSha1Context* context, const u8* data, u32 len)
{
    u32 i;
    u32 j;

    j = (context->count[0] >> 3) & 63;
    if ((context->count[0] += len << 3) < (len << 3)) {
        context->count[1]++;
    }
    context->count[1] += (len >> 29);
    if ((j + len) > 63) {
        memcpy(&context->buffer[j], data, (i = 64 - j));
        fn_801CC380(context->state, context->buffer);
        for (; i + 63 < len; i += 64) {
            fn_801CC380(context->state, &data[i]);
        }
        j = 0;
    } else {
        i = 0;
    }
    memcpy(&context->buffer[j], &data[i], len - i);
}

void fn_801CBF64(u8 digest[20], FieldSha1Context* context)
{
    u32 i;
    u8 finalcount[8];

    for (i = 0; i < 8; i++) {
        finalcount[i] = (u8)((context->count[(i >= 4 ? 0 : 1)] >> ((3 - (i & 3)) * 8)) & 255);
    }
    SHA1Update(context, (const u8*)"\200", 1);
    while ((context->count[0] & 504) != 448) {
        SHA1Update(context, (const u8*)"\0", 1);
    }
    SHA1Update(context, finalcount, 8);
    for (i = 0; i < 20; i++) {
        digest[i] = (u8)((context->state[i >> 2] >> ((3 - (i & 3)) * 8)) & 255);
    }
    memset(context->buffer, 0, 64);
    memset(context->state, 0, 20);
    memset(context->count, 0, 8);
    memset(finalcount, 0, 8);
    fn_801CC380(context->state, context->buffer);
}
