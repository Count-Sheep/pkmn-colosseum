/**
 * @file wazaSequenceCamera_exact_801D49D8.c
 * @brief _wazaSequenceCameraSelectDuration, 0x801D49D8 - 0x801D4DA0.
 *
 * Function-boundary carve of the waza camera TU (see wazaSequenceCamera.c):
 * clamp a camera cut's duration to the 12/20/45-frame steps allowed by the
 * mode's bit set, choosing between them with fn_800E0BE4's random value
 * against the move's thresholds. No jump table (the mode tests are a
 * compare chain), no pooled constant (the float compares use the caller's
 * thresholds), no data. GC/1.3 -O4,p like the TU, no pragmas; the body is
 * the TU's.
 */
#include "dolphin/types.h"

extern f32 fn_800E0BE4();

s32 _wazaSequenceCameraSelectDuration__FUcPff(
    s32 mode, f32* thresholds, s32 duration)
{
    f32 random;

    if (mode == 1) {
        return duration < 12 ? duration : 12;
    }
    if (mode == 2) {
        return duration < 20 ? duration : 20;
    }
    if (mode == 4) {
        return duration < 45 ? duration : 45;
    }
    if (mode == 8) {
        return duration;
    }
    if (mode == 3) {
        random = fn_800E0BE4();
        return random < thresholds[0]
                   ? (duration < 12 ? duration : 12)
                   : (duration < 20 ? duration : 20);
    }
    if (mode == 5) {
        random = fn_800E0BE4();
        return random < thresholds[0]
                   ? (duration < 12 ? duration : 12)
                   : (duration < 45 ? duration : 45);
    }
    if (mode == 9) {
        random = fn_800E0BE4();
        return random < thresholds[0]
                   ? (duration < 12 ? duration : 12)
                   : duration;
    }
    if (mode == 6) {
        random = fn_800E0BE4();
        return random < thresholds[1]
                   ? (duration < 20 ? duration : 20)
                   : (duration < 45 ? duration : 45);
    }
    if (mode == 10) {
        random = fn_800E0BE4();
        return random < thresholds[1]
                   ? (duration < 20 ? duration : 20)
                   : duration;
    }
    if (mode == 12) {
        random = fn_800E0BE4();
        return random < thresholds[2]
                   ? (duration < 45 ? duration : 45)
                   : duration;
    }
    if (mode == 7) {
        random = fn_800E0BE4();
        if (random < thresholds[0]) {
            return duration < 12 ? duration : 12;
        }
        if (random < thresholds[1]) {
            return duration < 20 ? duration : 20;
        }
        return duration < 45 ? duration : 45;
    }
    if (mode == 11) {
        random = fn_800E0BE4();
        if (random < thresholds[0]) {
            return duration < 12 ? duration : 12;
        }
        if (random < thresholds[1]) {
            return duration < 20 ? duration : 20;
        }
        return duration;
    }
    if (mode == 13) {
        random = fn_800E0BE4();
        if (random < thresholds[0]) {
            return duration < 12 ? duration : 12;
        }
        if (random < thresholds[2]) {
            return duration < 45 ? duration : 45;
        }
        return duration;
    }
    if (mode == 14) {
        random = fn_800E0BE4();
        if (random < thresholds[1]) {
            return duration < 12 ? duration : 12;
        }
        if (random < thresholds[2]) {
            return duration < 20 ? duration : 20;
        }
        return duration;
    }
    if (mode == 15) {
        random = fn_800E0BE4();
        if (random < thresholds[0]) {
            return duration < 12 ? duration : 12;
        }
        if (random < thresholds[1]) {
            return duration < 12 ? duration : 12;
        }
        if (random < thresholds[2]) {
            return duration < 20 ? duration : 20;
        }
        return duration;
    }
    return duration;
}
