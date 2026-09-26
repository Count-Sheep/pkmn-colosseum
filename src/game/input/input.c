/**
 * @file input.c
 * @brief GS script VM context pool setup and the game pad manager.
 *
 * Address range: 0x800F7758 - 0x800F8268.
 *
 * fn_800F7758 allocates the script VM context pool (GSVMPool at lbl_80401BF8,
 * contexts of 0x16C bytes). The rest of the range is the pad manager: four
 * InputPad slots in lbl_80401C10, each bound to a PAD channel, read through
 * small accessors that look the slot up by id, plus the per-frame update
 * (fn_800F7F64) and the one-time init (fn_800F8138). The PAD sampling
 * callback and the stick filter the update calls live in
 * gs_thread_hi_range_800F8268.c (fn_800F8268, fn_800F8654, fn_800F8A54).
 *
 * Unit boundary: this object's only .sdata2 literals are the int-to-float
 * bias 0x4330000080000000 (fn_800F7C8C/fn_800F7D38) and 0.0f (fn_800F8138),
 * emitted in that order. Retail has exactly that pair at 0x8047CCC8 and
 * 0x8047CCD0, and gs_thread_hi_range_800F8268.c references the same two
 * addresses before adding its own literals at 0x8047CCD4+. The literal pool
 * is shared, so this range and 0x800F8268-0x800F9318 were one translation
 * unit, and neither can be linked alone.
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
extern void fn_800F8268(void);
extern void fn_800F8654(InputPad* pad, s8 x, s8 y, s8* lastX, s8* lastY,
                        f32* stepX, f32* stepY, f32* posX, f32* posY);
extern void fn_800F8A54(InputPad* pad);

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
    pad->unk08 = value;
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
        pad->unk08 = 0;
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
