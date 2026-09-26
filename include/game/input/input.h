/**
 * @file input.h
 * @brief Game pad manager and GS script VM context pool (src/game/input/input.c).
 *
 * Layouts are recovered from the functions in 0x800F75FC-0x800F915C,
 * including the PAD sampling callback, rumble driver and stick filter
 * (fn_800F8268, fn_800F8428, fn_800F8654, fn_800F8A54). status, smoothMode
 * and rumbleMode are signed: those functions compare them with cmpwi.
 */

#ifndef GAME_INPUT_INPUT_H
#define GAME_INPUT_INPUT_H

#include "dolphin/types.h"
#include "dolphin/pad/Pad.h"

#define INPUT_PAD_COUNT 4

/* One logical controller slot. Slots are bound to a PAD channel by
 * fn_800F80B0; the sampling callback fills `latched`, and fn_800F7F64 copies
 * it to `current` once per game frame and runs the stick filter. */
typedef struct InputPad {
    /* 0x00 */ s32 id;             /* PAD channel + 1 (1..4); 0 = free slot */
    /* 0x04 */ u32 type;           /* 0 = standard controller, 2 = other SI type */
    /* 0x08 */ u32 outputMode;     /* fn_800F8A54: 0 = filtered sticks, 2 = dead zone + circle */
    /* 0x0C */ s32 status;         /* 0 = connected, 3 = no controller, 4 = error */
    /* 0x10 */ u8 smoothFrames;    /* stick filter length */
    /* 0x11 */ u8 smoothCount;
    /* 0x12 */ u8 pad12[2];
    /* 0x14 */ s32 smoothMode;     /* stick filter mode (0/1) */
    /* 0x18 */ PADStatus latched;  /* written by the sampling callback */
    /* 0x24 */ PADStatus current;  /* this frame's status */
    /* 0x30 */ u32 prevButton;     /* previous frame's current.button */
    /* 0x34 */ s8 lastStickX;
    /* 0x35 */ s8 lastStickY;
    /* 0x36 */ s8 lastSubstickX;
    /* 0x37 */ s8 lastSubstickY;
    /* 0x38 */ f32 stickStepX;
    /* 0x3C */ f32 stickStepY;
    /* 0x40 */ f32 substickStepX;
    /* 0x44 */ f32 substickStepY;
    /* 0x48 */ f32 stickX;         /* filtered stick position */
    /* 0x4C */ f32 stickY;
    /* 0x50 */ f32 substickX;
    /* 0x54 */ f32 substickY;
    /* 0x58 */ s8 outStickX;       /* filtered stick, quantised */
    /* 0x59 */ s8 outStickY;
    /* 0x5A */ s8 outSubstickX;
    /* 0x5B */ s8 outSubstickY;
    /* 0x5C */ s32 rumbleMode;     /* 1 = pulse, 2 = stop, 3 = hard stop */
    /* 0x60 */ u32 rumbleStrength;
    /* 0x64 */ u32 rumbleFrames;
    /* 0x68 */ u8 rumbleDecay;
    /* 0x69 */ u8 pad69[3];
} InputPad; /* size 0x6C */

/* lbl_80401C10 */
typedef struct InputManager {
    /* 0x000 */ InputPad pads[INPUT_PAD_COUNT];
    /* 0x1B0 */ s32 rumbleTimer[INPUT_PAD_COUNT];
    /* 0x1C0 */ u32 motorCommand[INPUT_PAD_COUNT]; /* PADControlAllMotors commands */
} InputManager; /* size 0x1D0 */

/* One loaded GS script, linked into GSVMPool.scripts by fn_800F76E4. */
typedef struct GSVMScript {
    /* 0x00 */ u16 id;
    /* 0x02 */ u8 unk02[4];
    /* 0x06 */ u16 relocCount;
    /* 0x08 */ u8 unk08[2];
    /* 0x0A */ u8 relocated;
    /* 0x0B */ u8 unk0B;
    /* 0x0C */ u32 relocOffset;    /* offset of the u32 relocation table */
    /* 0x10 */ s32 globalsOffset;
    /* 0x14 */ struct GSVMScript* next;
} GSVMScript;

/* One script VM execution context (see GSVMCtx in gs_thread.c for the
 * interpreter's view of the registers and operand stack). */
typedef struct GSVMContext {
    /* 0x000 */ GSVMScript* script;
    /* 0x004 */ u8 state;          /* 0 = free */
    /* 0x005 */ u8 unk005;
    /* 0x006 */ u16 key;
    /* 0x008 */ u32 entryId;       /* high half = script id */
    /* 0x00C */ u8 unk00C[0x16C - 0xC];
} GSVMContext; /* size 0x16C */

/* lbl_80401BF8, reached through the lbl_80478B00 pointer. */
typedef struct GSVMPool {
    /* 0x00 */ u16 count;
    /* 0x02 */ u16 handle;         /* GSmem handle of the context array */
    /* 0x04 */ u16 lastKey;
    /* 0x06 */ u16 unk06;
    /* 0x08 */ GSVMScript* scripts;
    /* 0x0C */ GSVMContext* contexts;
    /* 0x10 */ void* nativeTable;  /* set by fn_800F75FC */
} GSVMPool; /* size 0x14 */

#endif /* GAME_INPUT_INPUT_H */
