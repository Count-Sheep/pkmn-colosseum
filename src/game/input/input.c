/**
 * @file input.c
 * @brief GS script VM context pool setup and the game pad manager.
 *
 * Address range: 0x800F7758 - 0x800F915C, .sdata2 0x8047CCC8 - 0x8047CD00.
 *
 * fn_800F7758 allocates the script VM context pool (GSVMPool at lbl_80401BF8,
 * contexts of 0x16C bytes). The rest of the range is the pad manager: four
 * InputPad slots in lbl_80401C10, each bound to a PAD channel, read through
 * small accessors that look the slot up by id, the per-frame update
 * (fn_800F7F64), the one-time init (fn_800F8138), the PAD sampling callback
 * (fn_800F8268) with its rumble driver (fn_800F8428), and the stick filter
 * and output stage the update calls (fn_800F8654, fn_800F8A54).
 *
 * Unit boundary: the functions up to fn_800F8138 use two .sdata2 literals,
 * the int-to-float bias (fn_800F7C8C/fn_800F7D38) and 0.0f (fn_800F8138).
 * Retail has exactly that pair at 0x8047CCC8/0x8047CCD0, and the only other
 * code that references them is 0x800F8268-0x800F915C, whose own literals
 * follow at 0x8047CCD4-0x8047CCF8 in first-use order (0.25f, 0.5f, 1.0f and
 * the unsigned bias from fn_800F8654, then sqrtf's 0.5, 3.0 and 0.0 from
 * fn_800F8A54). Compiler literals cannot be shared between objects, so the
 * two ranges are one translation unit. The three registry-release functions
 * that used to follow (0x800F915C-0x800F9318) belong to GSres (gs_res.c).
 *
 * Dolphin PAD calls that have no symbol name yet, identified by their order
 * in the SDK's Pad.c and by their arguments here:
 *   fn_800AAD34 PADReset(mask)            fn_800AAE34 PADRecalibrate(mask)
 *   fn_800AAF38 PADInit()                 fn_800AB4FC PADControlAllMotors(cmds)
 */

#include "game/input/input.h"
#include "dolphin/os/OSInterrupt.h"
#include "dolphin/si/SI.h"

extern void* memset(void* dst, int val, u32 size);
extern void* memcpy(void* dst, const void* src, u32 n);
extern void GSlogWritef(const char* fmt, ...);
extern u16 _toolentryAlloc__FUl(u32 size);   /* GSmemAllocRaw */
extern void* fn_800E27B0(u16 handle);         /* GSmemGetPtr */
extern u32 fn_800D3094(void);
extern void fn_800AAD34(u32 mask);            /* PADReset */
extern BOOL fn_800AAE34(u32 mask);            /* PADRecalibrate */
extern BOOL fn_800AAF38(void);                /* PADInit */
extern void fn_800AB4FC(const u32* commands); /* PADControlAllMotors */
extern void PADSetAnalogMode(u32 mode);
extern u32 fn_800AB150(PADStatus* status);    /* PADRead */
extern u32 fn_800D0F44(s32 chan);             /* SI type/status probe */
extern f64 __frsqrte(f64 value);
extern const f32 lbl_80478AC0[];              /* NaN */

void fn_800F8268(void);
void fn_800F8428(void);
void fn_800F8654(InputPad* pad, s8 x, s8 y, s8* lastX, s8* lastY,
                 f32* stepX, f32* stepY, f32* posX, f32* posY);
void fn_800F8A54(InputPad* pad);

extern GSVMPool lbl_80401BF8;
extern GSVMPool* lbl_80478B00;
extern const char lbl_802712E4[];

extern InputManager lbl_80401C10;
/* Sampling-callback frame counter. */
extern u32 lbl_8047AC48;
/* Channels waiting for PADReset. The PAD sampling callback (fn_800F8268)
 * sets and clears these bits, so fn_800F7F64 re-reads it on every use. */
extern volatile u32 lbl_8047AC4C;
/* fn_800D3094 value at the last PADReset. */
extern u32 lbl_8047AC50;

static inline InputPad* InputFindPad(s32 id) {
    InputPad* pad = lbl_80401C10.pads;
    s32 i;

    for (i = 0; i < INPUT_PAD_COUNT; i++, pad++) {
        if (pad->id == id) {
            return pad;
        }
    }
    return NULL;
}

/* Allocate `count` script VM contexts and reset the pool. */
s32 fn_800F7758(u16 count) {
    GSVMContext* ctx;
    s32 i;

    memset(&lbl_80401BF8, 0, sizeof(GSVMPool));
    lbl_80478B00 = &lbl_80401BF8;
    lbl_80478B00->handle = _toolentryAlloc__FUl(count * sizeof(GSVMContext));
    if (lbl_80478B00->handle == 0) {
        GSlogWritef(lbl_802712E4);
        return -1;
    }

    lbl_80478B00->contexts = fn_800E27B0(lbl_80478B00->handle);
    ctx = lbl_80478B00->contexts;
    for (i = 0; i < count; i++) {
        ctx->script = NULL;
        ctx->state = 0;
        ctx++;
    }
    lbl_80478B00->count = count;
    return 0;
}

void fn_800F78A4(s32 id, u8 motor, u8 strength, u32 frames, u8 decay) {
    InputPad* pad = InputFindPad(id);

    if (pad == NULL) {
        return;
    }
    if (motor != 0) {
        return;
    }
    pad->rumbleMode = 1;
    pad->rumbleStrength = strength * 15;
    pad->rumbleFrames = frames;
    pad->rumbleDecay = decay;
}

s8 fn_800F7920(s32 id, s32 filtered) {
    InputPad* pad = InputFindPad(id);

    if (pad == NULL) {
        return 0;
    }
    if (filtered == 1) {
        return pad->outSubstickY;
    }
    return pad->current.substickY;
}

s8 fn_800F7994(s32 id, s32 filtered) {
    InputPad* pad = InputFindPad(id);

    if (pad == NULL) {
        return 0;
    }
    if (filtered == 1) {
        return pad->outSubstickX;
    }
    return pad->current.substickX;
}

s8 fn_800F7A08(s32 id, s32 filtered) {
    InputPad* pad = InputFindPad(id);

    if (pad == NULL) {
        return 0;
    }
    if (filtered == 1) {
        return pad->outStickY;
    }
    return pad->current.stickY;
}

s8 fn_800F7A7C(s32 id, s32 filtered) {
    InputPad* pad = InputFindPad(id);

    if (pad == NULL) {
        return 0;
    }
    if (filtered == 1) {
        return pad->outStickX;
    }
    return pad->current.stickX;
}

/* Buttons whose state changed since the previous update. */
u32 fn_800F7AF0(s32 id) {
    InputPad* pad = InputFindPad(id);

    if (pad == NULL) {
        return 0;
    }
    return pad->current.button ^ pad->prevButton;
}

u32 fn_800F7B5C(s32 id) {
    InputPad* pad = InputFindPad(id);

    if (pad == NULL) {
        return 0;
    }
    return ~pad->current.button;
}

u16 fn_800F7BC4(s32 id) {
    InputPad* pad = InputFindPad(id);

    if (pad == NULL) {
        return 0;
    }
    return pad->current.button;
}

u32 fn_800F7C28(s32 id) {
    InputPad* pad = InputFindPad(id);

    if (pad == NULL) {
        return 2;
    }
    return pad->type;
}

/* Snap the filtered substick to a position. */
void fn_800F7C8C(s32 id, s8 x, s8 y) {
    InputPad* pad = InputFindPad(id);

    if (pad == NULL) {
        return;
    }
    pad->lastSubstickX = x;
    pad->lastSubstickY = y;
    pad->substickX = x;
    pad->substickY = y;
}

/* Snap the filtered main stick to a position. */
void fn_800F7D38(s32 id, s8 x, s8 y) {
    InputPad* pad = InputFindPad(id);

    if (pad == NULL) {
        return;
    }
    pad->lastStickX = x;
    pad->lastStickY = y;
    pad->stickX = x;
    pad->stickY = y;
}

void fn_800F7DE4(s32 id, u32 mode) {
    InputPad* pad = InputFindPad(id);

    if (pad == NULL) {
        return;
    }
    pad->smoothMode = mode;
}

void fn_800F7E40(s32 id, u8 frames) {
    InputPad* pad = InputFindPad(id);

    if (pad == NULL) {
        return;
    }
    pad->smoothFrames = frames;
}

void fn_800F7E9C(s32 id, u32 value) {
    InputPad* pad = InputFindPad(id);

    if (pad == NULL) {
        return;
    }
    pad->outputMode = value;
}

u8 fn_800F7EF8(s32 id) {
    InputPad* pad = InputFindPad(id);

    if (pad == NULL) {
        return FALSE;
    }
    return pad->status == 0;
}

/* Per-frame update: latch the sampled status, service pending PAD resets,
 * and run the stick filter. Returns the pad's connection status. */
u32 fn_800F7F64(s32 id) {
    InputPad* pad = InputFindPad(id);
    BOOL level;
    u32 now;

    if (pad == NULL) {
        return 2;
    }

    pad->prevButton = pad->current.button;
    level = OSDisableInterrupts();
    now = fn_800D3094();
    if (lbl_8047AC4C != 0 && now != lbl_8047AC50) {
        fn_800AAD34(lbl_8047AC4C);
        lbl_8047AC50 = now;
    }
    memcpy(&pad->current, &pad->latched, sizeof(PADStatus));
    OSRestoreInterrupts(level);

    fn_800F8654(pad, pad->current.stickX, pad->current.stickY,
                &pad->lastStickX, &pad->lastStickY,
                &pad->stickStepX, &pad->stickStepY, &pad->stickX, &pad->stickY);
    fn_800F8654(pad, pad->current.substickX, pad->current.substickY,
                &pad->lastSubstickX, &pad->lastSubstickY,
                &pad->substickStepX, &pad->substickStepY, &pad->substickX, &pad->substickY);
    fn_800F8A54(pad);
    return pad->status;
}

/* Bind a free slot to PAD channel `id` - 1. */
s32 fn_800F80B0(s32 id) {
    InputPad* pad = InputFindPad(0);

    if (pad == NULL) {
        return 4;
    }
    switch (id) {
    case 1:
    case 2:
    case 3:
    case 4:
        pad->id = id;
        break;
    default:
        return 1;
    }
    return 0;
}

void fn_800F8138(void) {
    InputManager* mgr = &lbl_80401C10;
    InputPad* pad = mgr->pads;
    u32* motor = mgr->motorCommand;
    s32* timer = mgr->rumbleTimer;
    s32 i;

    lbl_8047AC48 = 0;
    lbl_8047AC4C = PAD_CHAN0_BIT | PAD_CHAN1_BIT | PAD_CHAN2_BIT | PAD_CHAN3_BIT;
    lbl_8047AC50 = 0;

    for (i = 0; i < INPUT_PAD_COUNT; i++, pad++) {
        pad->id = 0;
        pad->outputMode = 0;
        pad->status = 3;
        pad->smoothFrames = 0;
        pad->smoothMode = 0;
        pad->prevButton = 0;
        memset(&pad->latched, 0, sizeof(PADStatus));
        memset(&pad->current, 0, sizeof(PADStatus));
        pad->lastStickX = 0;
        pad->lastStickY = 0;
        pad->lastSubstickX = 0;
        pad->lastSubstickY = 0;
        pad->stickX = 0.0f;
        pad->stickY = 0.0f;
        pad->substickX = 0.0f;
        pad->substickY = 0.0f;
        pad->rumbleMode = 3;
        pad->rumbleStrength = 0;
        pad->rumbleFrames = 0;
        pad->rumbleDecay = 0;
        *motor++ = PAD_MOTOR_STOP_HARD;
        *timer++ = 0;
    }

    PADSetAnalogMode(0);
    fn_800AAF38();
    fn_800AB4FC(mgr->motorCommand);
    fn_800AAE34(lbl_8047AC4C);
    SISetSamplingRate(11);
    PADSetSamplingCallback(fn_800F8268);
}

/* PAD sampling callback: read all four channels, latch each bound slot's
 * status, track connection changes, then update the rumble motors. */
void fn_800F8268(void) {
    PADStatus status[PAD_MAX_CONTROLLERS];
    PADStatus* s;
    InputPad* pad;
    s32 chan;
    u32 type;

    fn_800AB150(status);
    s = status;
    for (chan = 0; chan < PAD_MAX_CONTROLLERS; chan++, s++) {
        pad = InputFindPad(chan + 1);
        if (pad == NULL) {
            continue;
        }
        switch (s->err) {
        case PAD_ERR_NONE:
            if (pad->status == 3) {
                switch (fn_800D0F44(chan)) {
                case 0x09000000: /* SI_GC_CONTROLLER */
                    pad->type = 0;
                    break;
                default:
                    pad->type = 2;
                    break;
                }
                pad->status = 0;
            }
            s->stickY = -s->stickY;
            s->substickY = -s->substickY;
            memcpy(&pad->latched, s, sizeof(PADStatus));
            lbl_8047AC4C &= ~(PAD_CHAN0_BIT >> chan);
            break;
        case PAD_ERR_NO_CONTROLLER:
            type = fn_800D0F44(chan);
            if (type == 8) { /* SI_ERROR_NO_RESPONSE */
                pad->status = 3;
            } else if (type == 0x40) {
                pad->status = 4;
            }
            memset(&pad->latched, 0, sizeof(PADStatus));
            lbl_8047AC4C |= PAD_CHAN0_BIT >> chan;
            break;
        case PAD_ERR_NOT_READY:
            break;
        }
    }
    fn_800F8428();
    lbl_8047AC48++;
}

/* Drive the rumble motors from each connected slot's rumble request. */
void fn_800F8428(void) {
    u8 changed = FALSE;
    s32 chan;
    u32* motor = lbl_80401C10.motorCommand;
    s32* timer = lbl_80401C10.rumbleTimer;
    InputPad* pad;

    for (chan = 0; chan < PAD_MAX_CONTROLLERS; chan++, motor++, timer++) {
        pad = InputFindPad(chan + 1);
        if (pad == NULL || pad->status != 0) {
            continue;
        }
        switch (pad->rumbleMode) {
        case 1:
            if (pad->rumbleStrength > 3600 && *motor != PAD_MOTOR_RUMBLE) {
                changed = TRUE;
                *motor = PAD_MOTOR_RUMBLE;
            } else if (pad->rumbleStrength < 100 && *motor != PAD_MOTOR_STOP) {
                changed = TRUE;
                *motor = PAD_MOTOR_STOP;
            } else {
                *timer += pad->rumbleStrength;
                if (*timer > 3600) {
                    *timer = 0;
                    if (*motor != PAD_MOTOR_RUMBLE) {
                        changed = TRUE;
                        *motor = PAD_MOTOR_RUMBLE;
                    }
                } else if (*motor != PAD_MOTOR_STOP) {
                    changed = TRUE;
                    *motor = PAD_MOTOR_STOP;
                }
            }
            break;
        case 2:
            if (*motor != PAD_MOTOR_STOP) {
                changed = TRUE;
                *motor = PAD_MOTOR_STOP;
            }
            break;
        case 3:
            if (*motor != PAD_MOTOR_STOP_HARD) {
                changed = TRUE;
                *motor = PAD_MOTOR_STOP_HARD;
            }
            break;
        }
        if (pad->rumbleFrames != 0) {
            if (--pad->rumbleFrames == 0) {
                pad->rumbleMode = 2;
            }
        }
        if (pad->rumbleDecay != 0) {
            if (pad->rumbleStrength < pad->rumbleDecay) {
                pad->rumbleMode = 2;
                pad->rumbleDecay = 0;
            } else {
                pad->rumbleStrength -= pad->rumbleDecay;
            }
        }
    }
    if (changed) {
        fn_800AB4FC(lbl_80401C10.motorCommand);
    }
}

/* Stick smoothing filter for one stick: when the raw position moves by more
 * than 2, recompute the per-frame step towards it, then advance the filtered
 * position without overshooting. */
void fn_800F8654(InputPad* pad, s8 x, s8 y, s8* lastX, s8* lastY,
                 f32* stepX, f32* stepY, f32* posX, f32* posY) {
    if (*lastX < x - 2 || *lastX > x + 2 || *lastY < y - 2 || *lastY > y + 2) {
        if (pad->smoothMode == 0) {
            *stepX = (x - *posX) / pad->smoothFrames;
            *stepY = (y - *posY) / pad->smoothFrames;
        } else if (pad->smoothMode == 1) {
            /* Retail multiplies frames * rate with the rate as the second
             * operand. MWCC moves a literal (or a const/static const) to the
             * first operand of the fmuls, so the rate reached this multiply
             * through a variable. */
            f32 rate = 0.25f;
            *stepX = (x - *posX) / (pad->smoothFrames * rate);
            *stepY = (y - *posY) / (pad->smoothFrames * rate);
        }
        pad->smoothCount = 0;
    }
    *lastX = x;
    *lastY = y;
    *posX += *stepX;
    *posY += *stepY;

    if (pad->smoothMode == 1) {
        pad->smoothCount++;
        if (pad->smoothCount < pad->smoothFrames) {
            *stepX *= 0.5f;
            *stepY *= 0.5f;
            if ((*stepX > 0.0f ? *stepX : -*stepX) < 1.0f) {
                *stepX = *stepX > 0.0f ? 1 : -1;
            }
            /* Retail tests *stepX here too (lfs 0(r8)), not *stepY. */
            if ((*stepY > 0.0f ? *stepY : -*stepY) < 1.0f) {
                *stepY = *stepX > 0.0f ? 1 : -1;
            }
        }
    }

    if (*stepX < 0.0f) {
        if (*posX < *lastX) {
            *posX = *lastX;
        }
    } else {
        if (*posX > *lastX) {
            *posX = *lastX;
        }
    }
    if (*stepY < 0.0f) {
        if (*posY < *lastY) {
            *posY = *lastY;
        }
    } else {
        if (*posY > *lastY) {
            *posY = *lastY;
        }
    }
}

/* MSL <math.h> fpclassify/sqrtf, inlined four times into fn_800F8A54 (the
 * same frsqrte + three Newton steps + NaN fallbacks each time). */
static inline s32 InputFpClassify(f32 value) {
    switch (*(s32*)&value & 0x7F800000) {
    case 0x7F800000:
        if (*(s32*)&value & 0x007FFFFF) {
            return 1;
        }
        return 2;
    case 0:
        if (*(s32*)&value & 0x007FFFFF) {
            return 5;
        }
        return 3;
    }
    return 4;
}

static inline f32 InputSqrtf(f32 value) {
    if (value > 0.0f) {
        f64 guess = __frsqrte(value);
        guess = 0.5 * guess * (3.0 - value * (guess * guess));
        guess = 0.5 * guess * (3.0 - value * (guess * guess));
        guess = 0.5 * guess * (3.0 - value * (guess * guess));
        value = (f32)(value * guess);
    } else if ((f64)value < 0.0) {
        value = lbl_80478AC0[0];
    } else if (InputFpClassify(value) == 1) {
        value = lbl_80478AC0[0];
    }
    return value;
}

/* Apply the +-10 dead zone to one stick and limit it to a circle of
 * `radius`. fn_800F8A54 expands this four times: both axes are sign-extended
 * up front (argument evaluation), then the same dead zone, squared-length
 * test, inlined sqrtf and rescale follow with radius 56 or 44. */
static inline void InputClampStick(s8 inX, s8 inY, s8* outX, s8* outY, s32 radius) {
    s32 x = inX;
    s32 y = inY;
    s32 sq;
    f32 len;

    if (x > -10 && x < 10) {
        x = 0;
    } else if (x > 0) {
        x -= 10;
    } else {
        x += 10;
    }
    if (y > -10 && y < 10) {
        y = 0;
    } else if (y > 0) {
        y -= 10;
    } else {
        y += 10;
    }
    sq = x * x + y * y;
    if (sq > radius * radius) {
        len = InputSqrtf(sq);
        x = x * radius / (s32)len;
        y = y * radius / (s32)len;
    }
    *outX = x;
    *outY = y;
}

/* Produce the pad's output stick values. Mode 0 passes the filtered sticks
 * through; mode 2 applies the dead zone and circle limit to the raw sticks
 * in place and to the filtered sticks into the outputs (radius 56 for the
 * main stick, 44 for the C stick). */
void fn_800F8A54(InputPad* pad) {
    switch (pad->outputMode) {
    case 0:
        pad->outStickX = pad->stickX;
        pad->outStickY = pad->stickY;
        pad->outSubstickX = pad->substickX;
        pad->outSubstickY = pad->substickY;
        break;
    case 2:
        InputClampStick(pad->current.stickX, pad->current.stickY,
                        &pad->current.stickX, &pad->current.stickY, 56);
        InputClampStick(pad->current.substickX, pad->current.substickY,
                        &pad->current.substickX, &pad->current.substickY, 44);
        InputClampStick(pad->stickX, pad->stickY,
                        &pad->outStickX, &pad->outStickY, 56);
        InputClampStick(pad->substickX, pad->substickY,
                        &pad->outSubstickX, &pad->outSubstickY, 44);
        break;
    }
}
