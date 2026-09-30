#ifndef GAME_MSGCTRL_OBJ_H
#define GAME_MSGCTRL_OBJ_H

/**
 * @file msgctrl_obj.h
 * @brief The message object as the msgctrl control-code callbacks see it.
 *
 * GSmsgDispatchControl passes this object to each msgctrl callback. The
 * callbacks read their operands from `stream` and advance it past them.
 * Kept apart from effect_util_types.h so that a linked msgctrl carve can
 * include it without that header's placeholder prototypes.
 */

#include "dolphin/types.h"

typedef struct EffectUtilCommandObj {
    /* 0x00 */ u8 field_00;
    /* 0x01 */ u8 activeFlag;
    /* 0x02 */ u8 field_02;
    /* 0x03 */ u8 field_03;
    /* 0x04 */ f32 field_04;
    /* 0x08 */ f32 field_08;
    /* 0x0C */ f32 field_0C;
    /* 0x10 */ f32 field_10;
    /* 0x14 */ u8 pad_14[0x0C];
    /* 0x20 */ u16 commandValue;
    /* 0x22 */ u8 pad_22;
    /* 0x23 */ u8 field_23;
    /* 0x24 */ u32 colorRgba;
    /* 0x28 */ u8 pad_28[4];
    /* 0x2C */ u8* savedStream;
    /* 0x30 */ u8* stream;
    /* 0x34 */ u8 pad_34[0x0D];
    /* 0x41 */ u8 field_41;
    /* 0x42 */ s8 field_42; /* line spacing (msgctrlLineSpace) */
    /* 0x43 */ s8 field_43; /* baseline bias (msgctrlBaseLineBias) */
    /* 0x44 */ u8 flags;
    /* 0x45 */ u8 pendingFlag;
    /* 0x46 */ u8 doneFlag;
    /* 0x47 */ u8 pad_47;
    /* 0x48 */ s16 waitCounter;
    /* 0x4A */ u8 alignMode;
    /* 0x4B */ u8 field_4B;
    /* 0x4C */ u8 pad_4C[0x18];
    /* 0x64 */ f32 field_64;
} EffectUtilCommandObj;

#endif /* GAME_MSGCTRL_OBJ_H */
