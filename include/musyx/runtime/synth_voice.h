#ifndef MUSYX_RUNTIME_SYNTH_VOICE_H
#define MUSYX_RUNTIME_SYNTH_VOICE_H

/*
 * SYNTH_VOICE and the types it embeds, as laid out in Colosseum's MusyX
 * (size 0x404, pack(4) so the u64 fields do not 8-byte-align). Same layout
 * as the reference MusyX synth.h; synthmacros.c and synth.c keep their own
 * copies of this definition.
 */
#include "dolphin/types.h"

#pragma pack(4)
typedef struct MSTEP { u32 para[2]; } MSTEP;

typedef struct SYNTH_QUEUE {
    struct SYNTH_QUEUE* next; // 0x0
    struct SYNTH_QUEUE* prev; // 0x4
    u8 voice;                 // 0x8
    u8 jobTabIndex;           // 0x9
} SYNTH_QUEUE; // size 0xC

typedef struct SYNTH_LFO {
    u32 time;
    u32 period;
    s16 value;
    s16 lastValue;
} SYNTH_LFO; // size 0xC

typedef struct ADSR_VARS {
    u8 mode;
    u8 state;
    u16 pad_2;
    u32 cnt;
    s32 currentVolume;
    s32 currentIndex;
    s32 currentDelta;
    union {
        struct {
            u32 aTime;  // 0x0
            u32 dTime;  // 0x4
            u16 sLevel; // 0x8
            u16 pad_A;
            u32 rTime;  // 0xC
            u16 cutOff; // 0x10
            u8 aMode;   // 0x12
            u8 pad_13;
        } dls;
        struct {
            u32 aTime;
            u32 dTime;
            u16 sLevel;
            u32 rTime;
        } linear;
        u8 raw[20];
    } data;
} ADSR_VARS; // size 0x28

/* ADSR_INFO: upstream musyx/adsr.h layout (size 0x14), used only as the
 * decoded-curve scratch struct inside the mcmdSetADSR* family below. */
typedef struct ADSR_INFO {
    union {
        struct {
            s32 atime;  // 0x0
            s32 dtime;  // 0x4
            u16 slevel; // 0x8
            u16 rtime;  // 0xA
            s32 ascale; // 0xC
            s32 dscale; // 0x10
        } dls;
        struct {
            u16 atime;  // 0x0
            u16 dtime;  // 0x2
            u16 slevel; // 0x4
            u16 rtime;  // 0x6
        } linear;
    } data;
} ADSR_INFO; // size 0x14

typedef struct CTRL_SOURCE { u8 midiCtrl; u8 combine; u16 pad; s32 scale; } CTRL_SOURCE; // 0x8
typedef struct CTRL_DEST { CTRL_SOURCE source[4]; u16 oldValue; u8 numSource; u8 pad; } CTRL_DEST; // 0x24

/* 4 entries * 8 bytes = 0x20, matching callStackIndex's "& 3" masking and
 * mcmdGosub/mcmdReturn's addr+curAddr pair per entry (synth.c left this
 * opaque since it never touches callStack fields directly). */
typedef struct CALL_STACK_ENTRY { MSTEP* addr; MSTEP* curAddr; } CALL_STACK_ENTRY;

typedef struct SYNTH_VOICE {
    SYNTH_QUEUE lowPrecisionJob;             // 0x0
    SYNTH_QUEUE zeroOffsetJob;               // 0xC
    SYNTH_QUEUE eventJob;                    // 0x18
    u64 lastLowCallTime;                     // 0x24
    u64 lastZeroCallTime;                    // 0x2C
    MSTEP* addr;                             // 0x34
    MSTEP* curAddr;                          // 0x38
    struct SYNTH_VOICE* nextMacActive;       // 0x3C
    struct SYNTH_VOICE* prevMacActive;       // 0x40
    struct SYNTH_VOICE* nextTimeQueueMacro;  // 0x44
    struct SYNTH_VOICE* prevTimeQueueMacro;  // 0x48
    s32 macState;                            // 0x4C (signed: MAC_STATE enum -- confirmed by
                                              // cmpwi/cmplwi choice in target disassembly)
    MSTEP* trapEventAddr[3];                 // 0x50
    MSTEP* trapEventCurAddr[3];              // 0x5C
    u8 trapEventAny;                         // 0x68
    u8 pad_69[3];
    CALL_STACK_ENTRY callStack[4];            // 0x6C
    u8 callStackEntryNum;                    // 0x8C
    u8 callStackIndex;                       // 0x8D
    u8 pad_8E[2];
    u64 macStartTime;                        // 0x90
    u64 wait;                                // 0x98
    u64 waitTime;                            // 0xA0
    u8 timeUsedByInput;                      // 0xA8
    u8 pad_A9;
    u16 loop;                                // 0xAA
    s32 local_vars[16];                      // 0xAC
    u32 child;                               // 0xEC
    u32 parent;                              // 0xF0
    u32 id;                                  // 0xF4
    void* vidList;                           // 0xF8
    void* vidMasterList;                     // 0xFC
    u16 allocId;                             // 0x100
    u16 macroId;                             // 0x102
    u8 keyGroup;                             // 0x104
    u8 pad_105[3];
    u32 lastVID;                             // 0x108
    u8 prio;                                 // 0x10C
    u8 pad_10D;
    u16 ageSpeed;                            // 0x10E
    u32 age;                                 // 0x110
    u64 cFlags;                              // 0x114
    u8 block;                                // 0x11C
    u8 fxFlag;                               // 0x11D
    u8 vGroup;                               // 0x11E
    u8 studio;                               // 0x11F
    u8 track;                                // 0x120
    u8 midi;                                 // 0x121
    u8 midiSet;                              // 0x122
    u8 section;                              // 0x123
    u32 sInfo;                               // 0x124
    u32 playFrq;                             // 0x128
    u16 curNote;                             // 0x12C
    s8 curDetune;                            // 0x12E
    u8 orgNote;                              // 0x12F
    u8 lastNote;                             // 0x130
    u8 portType;                             // 0x131
    u16 portLastCtrlState;                   // 0x132
    u32 portDuration;                        // 0x134
    u32 portCurPitch;                        // 0x138
    u32 portTime;                            // 0x13C
    u8 vibKeyRange;                          // 0x140
    u8 vibCentRange;                         // 0x141
    u8 pad_142[2];
    u32 vibPeriod;                           // 0x144
    u32 vibCurTime;                          // 0x148
    s32 vibCurOffset;                        // 0x14C
    s16 vibModAddScale;                      // 0x150
    u16 pad_152;
    u32 volume;                              // 0x154
    u32 orgVolume;                           // 0x158
    f32 lastVolFaderScale;                   // 0x15C
    u32 lastPan;                             // 0x160
    u32 lastSPan;                            // 0x164
    f32 treCurScale;                         // 0x168
    u16 treScale;                            // 0x16C
    u16 treModAddScale;                      // 0x16E
    u32 panning[2];                          // 0x170
    s32 panDelta[2];                         // 0x178
    u32 panTarget[2];                        // 0x180
    u32 panTime[2];                          // 0x188
    u8 revVolScale;                          // 0x190
    u8 revVolOffset;                         // 0x191
    u8 volTable;                             // 0x192
    u8 itdMode;                              // 0x193
    s32 envDelta;                            // 0x194
    u32 envTarget;                           // 0x198
    u32 envCurrent;                          // 0x19C
    u32 sweepOff[2];                         // 0x1A0
    s32 sweepAdd[2];                         // 0x1A8
    s32 sweepCnt[2];                         // 0x1B0
    u8 sweepNum[2];                          // 0x1B8
    u8 pad_1BA[2];
    SYNTH_LFO lfo[2];                        // 0x1BC
    u8 lfoUsedByInput[2];                    // 0x1D4
    u8 pbLowerKeyRange;                      // 0x1D6
    u8 pbUpperKeyRange;                      // 0x1D7
    u16 pbLast;                              // 0x1D8
    u8 pad_1DA[2];
    ADSR_VARS pitchADSR;                     // 0x1DC
    s16 pitchADSRRange;                      // 0x204
    u16 curPitch;                            // 0x206
    u8 setup_vol;                            // 0x208
    u8 setup_pan;                            // 0x209
    u8 setup_midi;                           // 0x20A
    u8 setup_midiSet;                        // 0x20B
    u8 setup_section;                        // 0x20C
    u8 setup_track;                          // 0x20D
    u8 setup_vGroup;                         // 0x20E
    u8 setup_studio;                         // 0x20F
    u8 setup_itdMode;                        // 0x210
    u8 pad_211[3];
    u32 midiDirtyFlags;                      // 0x214
    CTRL_DEST inpVolume;                     // 0x218
    CTRL_DEST inpPanning;                    // 0x23C
    CTRL_DEST inpSurroundPanning;            // 0x260
    CTRL_DEST inpPitchBend;                  // 0x284
    CTRL_DEST inpDoppler;                    // 0x2A8
    CTRL_DEST inpModulation;                 // 0x2CC
    CTRL_DEST inpPedal;                      // 0x2F0
    CTRL_DEST inpPortamento;                 // 0x314
    CTRL_DEST inpPreAuxA;                    // 0x338
    CTRL_DEST inpReverb;                     // 0x35C
    CTRL_DEST inpPreAuxB;                    // 0x380
    CTRL_DEST inpPostAuxB;                   // 0x3A4
    CTRL_DEST inpTremolo;                    // 0x3C8
    u8 mesgNum;                              // 0x3EC
    u8 mesgRead;                             // 0x3ED
    u8 mesgWrite;                            // 0x3EE
    u8 pad_3EF;
    s32 mesgQueue[4];                        // 0x3F0
    u16 curOutputVolume;                     // 0x400
    u8 pad_402[2];
} SYNTH_VOICE; // size 0x404
#pragma pack()

#endif /* MUSYX_RUNTIME_SYNTH_VOICE_H */
