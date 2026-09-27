/**
 * @file hw_dolphin_exact_80163F98.c
 * @brief MusyX hw_dolphin.c dspResumeCallback and salInitAi,
 *        0x80163F98 - 0x801640C4.
 *
 * Follows the reference MusyX runtime's hw_dolphin.c. The AI/DSP state
 * words are the reference's `static volatile u32` globals (.sbss, owned
 * elsewhere, kept extern here). callUserCallback is the reference's static
 * inline helper: retail expands the same sequence in salCallback
 * (0x80163F3C) and dspResumeCallback (0x80163FC0).
 */
#include "dolphin/types.h"

typedef void (*SND_SOME_CALLBACK)(void);

typedef struct SND_PLAYBACKINFO {
    u32 frq;
    u8 stereo;
    u8 bits;
    s8 deviceName[256];
    s8 versionText[256];
} SND_PLAYBACKINFO;

typedef struct SynthInfo {
    u32 mixFrq;
    u32 numSamples;
    SND_PLAYBACKINFO pbInfo;
    u8 voiceNum;
    u8 maxMusic;
    u8 maxSFX;
    u8 studioNum;
} SynthInfo;

extern SynthInfo lbl_80434C50;               /* synthInfo */
extern u8 lbl_8047B0A0;                      /* salAIBufferIndex */
extern void* lbl_8047B09C;                   /* salAIBufferBase */
extern volatile u32 lbl_8047B098;            /* salDspIsDone */
extern volatile u32 lbl_8047B094;            /* salLogicIsWaiting */
extern volatile u32 lbl_8047B090;            /* salLogicActive */
extern SND_SOME_CALLBACK lbl_8047B0A4;       /* userCallback */

extern void* fn_801643D8(u32 size);          /* salMalloc */

#define OSCachedToPhysical(caddr) ((u32)(caddr) - 0x80000000)
extern void* memset(void* dst, int value, u32 size);
extern void DCFlushRange(void* addr, u32 size);
extern void AIRegisterDMACallback(void (*callback)(void));
extern void AIInitDMA(u32 addr, u32 length);
extern void salCallback(void);
extern s32 OSEnableInterrupts(void);
extern s32 OSDisableInterrupts(void);

static inline void callUserCallback(void)
{
    if (lbl_8047B090) {
        return;
    }
    lbl_8047B090 = 1;
    OSEnableInterrupts();
    lbl_8047B0A4();
    OSDisableInterrupts();
    lbl_8047B090 = 0;
}

void dspResumeCallback(void)
{
    lbl_8047B098 = TRUE;
    if (lbl_8047B094) {
        lbl_8047B094 = FALSE;
        callUserCallback();
    }
}

u32 salInitAi(SND_SOME_CALLBACK callback, u32 unk, u32* outFreq)
{
    if ((lbl_8047B09C = fn_801643D8(0x280 * 4)) != NULL) {
        memset(lbl_8047B09C, 0, 0x280 * 4);
        DCFlushRange(lbl_8047B09C, 0x280 * 4);
        lbl_8047B0A0 = TRUE;
        lbl_8047B094 = FALSE;
        lbl_8047B098 = TRUE;
        lbl_8047B090 = FALSE;
        lbl_8047B0A4 = callback;
        AIRegisterDMACallback(salCallback);
        AIInitDMA(OSCachedToPhysical(lbl_8047B09C) + (lbl_8047B0A0 * 0x280), 0x280);
        lbl_80434C50.numSamples = 0x20;
        *outFreq = 32000;
        return TRUE;
    }

    return FALSE;
}
