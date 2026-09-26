/**
 * @file ps_r56_801735EC_prefix.c
 * @brief Particle-interpreter getFloat, 0x801735EC - 0x80173624.
 *
 * Standalone source for this split range; the body is the one previously
 * reached through the ps_range_80168C64.c include wrapper.
 * psSetBillboardCamera (0x80173624) opens HAL's generator.c and lives in
 * game/generator.c.
 */
#include "dolphin/types.h"

typedef union PSFloatBytes {
    u8 bytes[4];
    f32 value;
} PSFloatBytes;

extern PSFloatBytes lbl_8047B178;

u8* getFloat(u8* stream, f32* out) {
    lbl_8047B178.bytes[0] = *stream++;
    lbl_8047B178.bytes[1] = *stream++;
    lbl_8047B178.bytes[2] = *stream++;
    lbl_8047B178.bytes[3] = *stream++;
    *out = lbl_8047B178.value;
    return stream;
}
