/**
 * @file THPPlayer.c
 * @brief Dolphin SDK THP player sample: player control, 0x801E25C8 - 0x801E4AC4.
 *
 * Deferred inlining emits functions and .bss objects in reverse source order
 * (THPPlayerInit is last in the text, the audio stream code first) and still
 * expands inline helpers defined after their callers. The SDK's AX mixing
 * path is replaced in this build by a pair of MusyX streams (sndStream*), fed
 * from the decoded audio buffers.
 *
 * Candidate status: every function is exact except the stream resync routine
 * fn_801E2CA8 (98.7%). Its instructions all match; three pairs of its own
 * values take swapped saved registers: requestOffset and the marker-loop
 * base (retail r28/r27), decodedOffset and the 64-bit copy of `sample` used
 * by the modulo calls (r26/r24), and `sample - requestOffset` and
 * `sample / 2` in the wrap-around path (r23/r21). Declaration order (1,500
 * of the 5,040 orders tried; only two outcomes), statement order,
 * block-scoped and u64 temporaries, and GC/1.3.2 to GC/2.7 do not move them.
 *
 * Also tried, without effect: every top-level/else/path scoping of the six
 * locals (729 layouts); per-path copies of size, marker, remaining and dst;
 * named locals for `sample / 2` and `sample - requestOffset`; operand and
 * comparison flips; and the unit flags (-O3/-O4 p/s, -opt nolifetimes, noloop,
 * nopropagation, nodead, space and others). None of these makes the unit exact.
 * Pattern: in every pair, retail colours the value it computes itself before
 * the loop-hoisted invariant (struct base, 64-bit `sample`, `sample / 2`),
 * and ours colours the invariant first. -opt noloop is the only flag that
 * moves these registers (to 95.6%).
 *
 * Lead: retail also schedules `requestOffset * 2` (the memcpy source offset)
 * before the byte count in both memcpy pairs. Only computing the byte count
 * inside or after the first memcpy call reproduces that order (98.70%, not
 * kept). The next step is to replay the colouring with the public MWCC
 * reconstruction's rule (github.com/JackPriceBurns/mwcc,
 * src/backend/Coloring.c) and lane FL2's scripts (session scratchpad,
 * colouring/), the tool that closed gs_floor.
 *
 * The unit builds with -inline noauto,deferred: under auto, fn_801E34F0 is
 * inlined into THPPlayerPrepare (fn_801E40F8), which retail did not do.
 */
#include "dolphin/thp/THPPlayer.h"
#include "dolphin/gx/GX.h"

extern void* memset(void* dst, int value, u32 size);
extern void* memcpy(void* dst, const void* src, u32 size);
extern int strcmp(const char* lhs, const char* rhs);
extern void DCInvalidateRange(void* addr, u32 nBytes);
extern void DCFlushRange(void* addr, u32 nBytes);
extern BOOL THPInit(void);
extern u32 VIGetTvFormat(void);

typedef void (*VIRetraceCallback)(u32 retraceCount);
extern VIRetraceCallback fn_800A8850(VIRetraceCallback callback); /* VISetPostRetraceCallback */
extern u32 fn_800AA2F0(void);                                     /* VIGetNextField */

/* MusyX streaming (src/musyx/runtime/stream.c) */
typedef u32 (*SND_STREAM_UPDATE_CALLBACK)(void* buffer1, u32 len1, void* buffer2, u32 len2,
                                          u32 user);
extern u32 fn_8014EE40(u8 prio, void* buffer, u32 samples, u32 frq, u8 vol, u8 pan, u8 span,
                       u8 auxa, u8 auxb, u8 studio, u32 flags,
                       SND_STREAM_UPDATE_CALLBACK updateFunction, u32 user,
                       void* adpcmInfo); /* sndStreamAllocEx */
extern void fn_8014E9B4(u32 stid, u32 offset, u32 num, u32 offset2, u32 num2); /* sndStreamADPCMParameter / update */
extern u8 sndStreamActivate(u32 stid);
extern void sndStreamDeactivate(u32 stid);
extern void sndStreamFree(u32 stid);

#define THP_STREAM_FRAMES 5

/* .bss (emitted in reverse order of these definitions) */
THPPlayer lbl_8046AC60;                                   /* ActivePlayer */
static s16 lbl_8046A4E0[0x3C0] __attribute__((aligned(32))); /* WorkBuffer */
static OSMessageQueue lbl_8046A4B4;                       /* PrepareReadyQueue */
static OSMessageQueue lbl_8046A494;                       /* UsedTextureSetQueue */
static OSMessage lbl_8046A488[3]; /* UsedTextureSetMessage */
static struct {
    u64 markers[THP_STREAM_FRAMES]; /* end position of each queued THP audio frame */
    s32 readMarker;
    s32 writeMarker;
    u64 dmaPosition;       /* samples handed to MusyX */
    u64 requestedPosition; /* samples MusyX has asked for */
    u64 decodedPosition;   /* samples pulled from the decoded audio buffers */
} lbl_8046A440; /* stream ring state */

/* .sbss (also emitted in reverse order) */
static OSMessage lbl_8047B478[2];      /* PrepareReadyMessage */
s16* lbl_8047B474;                     /* right stream buffer */
s16* lbl_8047B470;                     /* left stream buffer */
static VIRetraceCallback lbl_8047B46C; /* OldVIPostCallback */
BOOL lbl_8047B468;                     /* Initialized */

/* .sdata */
u32 lbl_80478D00 = 0xFFFFFFFF; /* left stream id */
u32 lbl_80478D04 = 0xFFFFFFFF; /* right stream id */

static void fn_801E3A50(u32 retraceCount);
static inline OSMessage PopUsedTextureSet(void);
static inline void PushUsedTextureSet(OSMessage msg);
static inline BOOL ProperTimingForStart(void);
static inline BOOL ProperTimingForGettingNextFrame(void);
static inline void FillStreamBuffer(s16* left, s16* right, u32 sample);
BOOL fn_801E34F0(void);
void fn_801E2CA8(void);
u32 fn_801E2B74(s16* left, s16* right, u32 sample, s32* status);
u32 fn_801E260C(void* buffer1, u32 len1, void* buffer2, u32 len2, u32 user);

BOOL fn_801E4A6C(void)
{
    memset(&lbl_8046AC60, 0, sizeof(THPPlayer));
    fn_8009F1D0(&lbl_8046A494, lbl_8046A488, 3);
    lbl_8047B468 = TRUE;
    return TRUE;
}

BOOL fn_801E4778(const char* fileName, BOOL onMemory)
{
    s32 offset;
    s32 i;

    if (THPInit() == FALSE) {
        return FALSE;
    }

    if (lbl_8047B468 == FALSE) {
        return FALSE;
    }

    if (lbl_8046AC60.open) {
        return FALSE;
    }

    memset(&lbl_8046AC60.videoInfo, 0, sizeof(THPVideoInfo));
    memset(&lbl_8046AC60.audioInfo, 0, sizeof(THPAudioInfo));

    if (DVDOpen(fileName, &lbl_8046AC60.fileInfo) == FALSE) {
        return FALSE;
    }

    if (DVDRead(&lbl_8046AC60.fileInfo, lbl_8046A4E0, 64, 0, 2) < 0) {
        DVDClose(&lbl_8046AC60.fileInfo);
        return FALSE;
    }

    memcpy(&lbl_8046AC60.header, lbl_8046A4E0, sizeof(THPHeader));

    if (strcmp(lbl_8046AC60.header.magic, "THP") != 0) {
        DVDClose(&lbl_8046AC60.fileInfo);
        return FALSE;
    }

    if (lbl_8046AC60.header.version != 0x11000) {
        DVDClose(&lbl_8046AC60.fileInfo);
        return FALSE;
    }

    offset = lbl_8046AC60.header.compInfoDataOffsets;

    if (DVDRead(&lbl_8046AC60.fileInfo, lbl_8046A4E0, 32, offset, 2) < 0) {
        DVDClose(&lbl_8046AC60.fileInfo);
        return FALSE;
    }

    memcpy(&lbl_8046AC60.compInfo, lbl_8046A4E0, sizeof(THPFrameCompInfo));
    offset += sizeof(THPFrameCompInfo);
    lbl_8046AC60.audioExist = 0;

    for (i = 0; i < lbl_8046AC60.compInfo.numComponents; i++) {
        switch (lbl_8046AC60.compInfo.frameComp[i]) {
        case 0:
            if (DVDRead(&lbl_8046AC60.fileInfo, lbl_8046A4E0, 32, offset, 2) < 0) {
                DVDClose(&lbl_8046AC60.fileInfo);
                return FALSE;
            }
            memcpy(&lbl_8046AC60.videoInfo, lbl_8046A4E0, sizeof(THPVideoInfo));
            offset += sizeof(THPVideoInfo);
            break;
        case 1:
            if (DVDRead(&lbl_8046AC60.fileInfo, lbl_8046A4E0, 32, offset, 2) < 0) {
                DVDClose(&lbl_8046AC60.fileInfo);
                return FALSE;
            }
            memcpy(&lbl_8046AC60.audioInfo, lbl_8046A4E0, sizeof(THPAudioInfo));
            offset += sizeof(THPAudioInfo);
            lbl_8046AC60.audioExist = 1;
            break;
        default:
            return FALSE;
        }
    }

    lbl_8046AC60.internalState = 0;
    lbl_8046AC60.state = 0;
    lbl_8046AC60.playFlag = 0;
    lbl_8046AC60.onMemory = onMemory;
    lbl_8046AC60.open = TRUE;
    return TRUE;
}

BOOL fn_801E4724(void)
{
    if (lbl_8046AC60.open && lbl_8046AC60.state == 0) {
        lbl_8046AC60.open = FALSE;
        DVDClose(&lbl_8046AC60.fileInfo);
        return TRUE;
    }
    return FALSE;
}

u32 fn_801E4650(void)
{
    u32 size;

    if (lbl_8046AC60.open) {
        if (lbl_8046AC60.onMemory) {
            size = OSRoundUp32B(lbl_8046AC60.header.movieDataSize);
        } else {
            size = OSRoundUp32B(lbl_8046AC60.header.bufSize) * 10;
        }

        size += OSRoundUp32B(lbl_8046AC60.videoInfo.xSize * lbl_8046AC60.videoInfo.ySize) * 3;
        size += OSRoundUp32B(lbl_8046AC60.videoInfo.xSize * lbl_8046AC60.videoInfo.ySize / 4) * 3;
        size += OSRoundUp32B(lbl_8046AC60.videoInfo.xSize * lbl_8046AC60.videoInfo.ySize / 4) * 3;

        if (lbl_8046AC60.audioExist) {
            size += OSRoundUp32B(lbl_8046AC60.header.audioMaxSamples * 4) * 3;
            size += lbl_8046AC60.audioInfo.sndChannels *
                    OSRoundUp32B(lbl_8046AC60.audioInfo.sndFrequency * 40 / 500);
        }
        return size + 0x1000;
    }
    return 0;
}

BOOL fn_801E449C(u8* buffer)
{
    u32 i;
    u8* ptr;
    u32 ysize;
    u32 uvsize;
    u32 streamSize;

    if (lbl_8046AC60.open && lbl_8046AC60.state == 0) {
        ptr = buffer;
        if (lbl_8046AC60.onMemory) {
            lbl_8046AC60.movieData = buffer;
            ptr += lbl_8046AC60.header.movieDataSize;
        } else {
            for (i = 0; i < 10; i++) {
                lbl_8046AC60.readBuffer[i].ptr = ptr;
                ptr += OSRoundUp32B(lbl_8046AC60.header.bufSize);
            }
        }

        ysize = OSRoundUp32B(lbl_8046AC60.videoInfo.xSize * lbl_8046AC60.videoInfo.ySize);
        uvsize = OSRoundUp32B(lbl_8046AC60.videoInfo.xSize * lbl_8046AC60.videoInfo.ySize / 4);

        for (i = 0; i < 3; i++) {
            lbl_8046AC60.textureSet[i].ytexture = ptr;
            DCInvalidateRange(ptr, ysize);
            ptr += ysize;

            lbl_8046AC60.textureSet[i].utexture = ptr;
            DCInvalidateRange(ptr, uvsize);
            ptr += uvsize;

            lbl_8046AC60.textureSet[i].vtexture = ptr;
            DCInvalidateRange(ptr, uvsize);
            ptr += uvsize;
        }

        if (lbl_8046AC60.audioExist) {
            for (i = 0; i < 3; i++) {
                lbl_8046AC60.audioBuffer[i].buffer = (s16*)ptr;
                lbl_8046AC60.audioBuffer[i].curPtr = (s16*)ptr;
                lbl_8046AC60.audioBuffer[i].validSample = 0;
                ptr += OSRoundUp32B(lbl_8046AC60.header.audioMaxSamples * 4);
            }

            lbl_8047B470 = (s16*)ptr;
            streamSize = OSRoundUp32B(lbl_8046AC60.audioInfo.sndFrequency * 40 / 500);
            ptr += streamSize;
            if (lbl_8046AC60.audioInfo.sndChannels == 2) {
                lbl_8047B474 = (s16*)ptr;
                ptr += streamSize;
            }
        }

        lbl_8046AC60.thpWork = ptr;
        return TRUE;
    }
    return FALSE;
}

static inline void InitAllMessageQueue(void)
{
    s32 i;

    if (lbl_8046AC60.onMemory == FALSE) {
        for (i = 0; i < 10; i++) {
            fn_801E1BB8(&lbl_8046AC60.readBuffer[i]);
        }
    }

    for (i = 0; i < 3; i++) {
        fn_801E4F34(&lbl_8046AC60.textureSet[i]);
    }

    if (lbl_8046AC60.audioExist) {
        for (i = 0; i < 3; i++) {
            fn_801E4B08(&lbl_8046AC60.audioBuffer[i]);
        }
    }

    fn_8009F1D0(&lbl_8046A4B4, lbl_8047B478, 2);
}

static inline BOOL WaitUntilPrepare(void)
{
    OSMessage msg0;
    OSMessage msg1;

    if (lbl_8046AC60.audioExist) {
        fn_8009F2F8(&lbl_8046A4B4, &msg0, OS_MESSAGE_BLOCK);
        fn_8009F2F8(&lbl_8046A4B4, &msg1, OS_MESSAGE_BLOCK);
        if ((BOOL)msg0 && (BOOL)msg1) {
            return TRUE;
        } else {
            return FALSE;
        }
    } else {
        fn_8009F2F8(&lbl_8046A4B4, &msg0, OS_MESSAGE_BLOCK);
        if ((BOOL)msg0) {
            return TRUE;
        } else {
            return FALSE;
        }
    }
}

void fn_801E446C(BOOL msg)
{
    fn_8009F230(&lbl_8046A4B4, (OSMessage)msg, OS_MESSAGE_BLOCK);
}

BOOL fn_801E40F8(s32 frame, u8 flag, s32 audioTrack)
{
    u8* threadData;

    if (lbl_8046AC60.open && lbl_8046AC60.state == 0) {
        if (frame > 0) {
            if (lbl_8046AC60.header.offsetDataOffsets == 0) {
                return FALSE;
            }

            if (lbl_8046AC60.header.numFrames > frame) {
                if (DVDRead(&lbl_8046AC60.fileInfo, lbl_8046A4E0, 0x20,
                            lbl_8046AC60.header.offsetDataOffsets + (frame - 1) * 4, 2) < 0) {
                    return FALSE;
                }

                lbl_8046AC60.initOffset = lbl_8046AC60.header.movieDataOffsets + lbl_8046A4E0[0];
                lbl_8046AC60.initReadFrame = frame;
                lbl_8046AC60.initReadSize = lbl_8046A4E0[1] - lbl_8046A4E0[0];
            } else {
                return FALSE;
            }
        } else {
            lbl_8046AC60.initOffset = lbl_8046AC60.header.movieDataOffsets;
            lbl_8046AC60.initReadSize = lbl_8046AC60.header.firstFrameSize;
            lbl_8046AC60.initReadFrame = frame;
        }

        if (lbl_8046AC60.audioExist) {
            if (audioTrack < 0 || audioTrack >= lbl_8046AC60.audioInfo.sndNumTracks) {
                return FALSE;
            }
            lbl_8046AC60.curAudioTrack = audioTrack;
        }

        lbl_8046AC60.playFlag = flag & 1;
        lbl_8046AC60.videoDecodeCount = 0;

        if (lbl_8046AC60.onMemory) {
            if (DVDRead(&lbl_8046AC60.fileInfo, lbl_8046AC60.movieData,
                        lbl_8046AC60.header.movieDataSize,
                        lbl_8046AC60.header.movieDataOffsets, 2) < 0) {
                return FALSE;
            }

            threadData = lbl_8046AC60.movieData + lbl_8046AC60.initOffset -
                         lbl_8046AC60.header.movieDataOffsets;
            fn_801E5470(20, threadData);
            if (lbl_8046AC60.audioExist) {
                fn_801E4E1C(12, threadData);
            }
        } else {
            fn_801E5470(20, NULL);
            if (lbl_8046AC60.audioExist) {
                fn_801E4E1C(12, NULL);
            }
            fn_801E1D7C(8);
        }

        InitAllMessageQueue();

        fn_801E543C();
        if (lbl_8046AC60.audioExist) {
            fn_801E4DE8();
        }
        if (lbl_8046AC60.onMemory == FALSE) {
            fn_801E1D48();
        }

        if (!WaitUntilPrepare()) {
            return FALSE;
        }

        lbl_8046AC60.state = 1;
        lbl_8046AC60.internalState = 0;
        lbl_8046AC60.dispTextureSet = NULL;
        lbl_8046AC60.playAudioBuffer = NULL;
        lbl_8046AC60.curVideoNumber = -1;
        lbl_8046AC60.curAudioNumber = 0;

        if (lbl_8046AC60.audioExist) {
            fn_801E34F0();
        }

        lbl_8047B46C = fn_800A8850(fn_801E3A50);
        return TRUE;
    }
    return FALSE;
}

BOOL fn_801E4058(void)
{
    if (lbl_8046AC60.open && (lbl_8046AC60.state == 1 || lbl_8046AC60.state == 4)) {
        if (lbl_8046AC60.state == 4 && lbl_8046AC60.audioExist) {
            fn_801E2CA8();
        }
        lbl_8046AC60.state = 2;
        lbl_8046AC60.prevCount = 0;
        lbl_8046AC60.curCount = 0;
        lbl_8046AC60.retraceCount = -1;
        return TRUE;
    }
    return FALSE;
}

void fn_801E3F54(void)
{
    if (lbl_8046AC60.open && lbl_8046AC60.state != 0) {
        lbl_8046AC60.internalState = 0;
        lbl_8046AC60.state = 0;
        fn_800A8850(lbl_8047B46C);
        if (lbl_8046AC60.onMemory == FALSE) {
            DVDCancel(&lbl_8046AC60.fileInfo.cb);
            fn_801E1D0C();
        }

        fn_801E5400();
        if (lbl_8046AC60.audioExist) {
            sndStreamFree(lbl_80478D00);
            lbl_80478D00 = 0xFFFFFFFF;
            if (lbl_80478D04 != 0xFFFFFFFF) {
                sndStreamFree(lbl_80478D04);
                lbl_80478D04 = 0xFFFFFFFF;
            }
            fn_801E4DAC();
        }

        while (PopUsedTextureSet() != NULL) {
        }

        lbl_8046AC60.dvdError = 0;
        lbl_8046AC60.videoError = 0;
    }
}

/* Starts the MusyX streams once the first frame is due. */
static inline BOOL ActivateStreams(void)
{
    if (lbl_80478D00 != 0xFFFFFFFF && sndStreamActivate(lbl_80478D00)) {
        if (lbl_80478D04 != 0xFFFFFFFF) {
            if (sndStreamActivate(lbl_80478D04)) {
                return TRUE;
            }
            sndStreamDeactivate(lbl_80478D00);
        } else {
            return TRUE;
        }
    }
    return FALSE;
}

static void fn_801E3A50(u32 retraceCount)
{
    THPTextureSet* decodedTexture;

    if (lbl_8047B46C != NULL) {
        lbl_8047B46C(retraceCount);
    }

    decodedTexture = (THPTextureSet*)-1;
    if (lbl_8046AC60.open && lbl_8046AC60.state == 2) {
        if (lbl_8046AC60.dvdError || lbl_8046AC60.videoError) {
            lbl_8046AC60.internalState = 5;
            lbl_8046AC60.state = 5;
            return;
        }

        ++lbl_8046AC60.retraceCount;

        if (lbl_8046AC60.retraceCount == 0) {
            if (ProperTimingForStart()) {
                if (lbl_8046AC60.audioExist) {
                    if (lbl_8046AC60.curVideoNumber - lbl_8046AC60.curAudioNumber <= 1) {
                        decodedTexture = (THPTextureSet*)fn_801E4EF0(0);
                        lbl_8046AC60.videoDecodeCount--;
                        lbl_8046AC60.curVideoNumber++;
                    } else {
                        if (!ActivateStreams()) {
                            lbl_8046AC60.internalState = 5;
                            lbl_8046AC60.state = 5;
                            return;
                        }
                        lbl_8046AC60.internalState = 2;
                    }
                } else {
                    decodedTexture = (THPTextureSet*)fn_801E4EF0(0);
                }
            } else {
                lbl_8046AC60.retraceCount = -1;
            }
        } else {
            if (lbl_8046AC60.audioExist && lbl_8046AC60.retraceCount == 1 &&
                lbl_8046AC60.internalState != 2) {
                if (!ActivateStreams()) {
                    lbl_8046AC60.internalState = 5;
                    lbl_8046AC60.state = 5;
                    return;
                }
                lbl_8046AC60.internalState = 2;
            }

            if (ProperTimingForGettingNextFrame()) {
                if (lbl_8046AC60.audioExist) {
                    if (lbl_8046AC60.curVideoNumber - lbl_8046AC60.curAudioNumber <= 1) {
                        decodedTexture = (THPTextureSet*)fn_801E4EF0(0);
                        lbl_8046AC60.videoDecodeCount--;
                        lbl_8046AC60.curVideoNumber++;
                    }
                } else {
                    decodedTexture = (THPTextureSet*)fn_801E4EF0(0);
                }
            }
        }

        if (decodedTexture != NULL && decodedTexture != (THPTextureSet*)-1) {
            if (lbl_8046AC60.dispTextureSet != NULL) {
                PushUsedTextureSet(lbl_8046AC60.dispTextureSet);
            }
            lbl_8046AC60.dispTextureSet = decodedTexture;
        }

        if ((lbl_8046AC60.playFlag & 1) == 0) {
            if (lbl_8046AC60.audioExist) {
                s32 audioFrame = lbl_8046AC60.curAudioNumber + lbl_8046AC60.initReadFrame;

                if (audioFrame == lbl_8046AC60.header.numFrames &&
                    lbl_8046AC60.playAudioBuffer == NULL) {
                    lbl_8046AC60.internalState = 3;
                    lbl_8046AC60.state = 3;
                }
            } else {
                u32 curFrame;

                if (lbl_8046AC60.dispTextureSet != NULL) {
                    curFrame = lbl_8046AC60.dispTextureSet->frameNumber +
                               lbl_8046AC60.initReadFrame;
                } else {
                    curFrame = lbl_8046AC60.initReadFrame - 1;
                }

                if (curFrame == lbl_8046AC60.header.numFrames - 1 && decodedTexture == NULL) {
                    lbl_8046AC60.internalState = 3;
                    lbl_8046AC60.state = 3;
                }
            }
        }
    }
}

static inline BOOL ProperTimingForStart(void)
{
    if (lbl_8046AC60.videoInfo.videoType & 1) {
        if (fn_800AA2F0() == 0) {
            return TRUE;
        }
    } else if (lbl_8046AC60.videoInfo.videoType & 2) {
        if (fn_800AA2F0() == 1) {
            return TRUE;
        }
    } else {
        return TRUE;
    }
    return FALSE;
}

static inline BOOL ProperTimingForGettingNextFrame(void)
{
    s32 frameRate;

    if (lbl_8046AC60.videoInfo.videoType & 1) {
        if (fn_800AA2F0() == 0) {
            return TRUE;
        }
    } else if (lbl_8046AC60.videoInfo.videoType & 2) {
        if (fn_800AA2F0() == 1) {
            return TRUE;
        }
    } else {
        frameRate = lbl_8046AC60.header.frameRate * 100.0f;
        if (VIGetTvFormat() == 1) {
            lbl_8046AC60.curCount = lbl_8046AC60.retraceCount * frameRate / 5000;
        } else {
            lbl_8046AC60.curCount = lbl_8046AC60.retraceCount * frameRate / 5994;
        }

        if (lbl_8046AC60.prevCount != lbl_8046AC60.curCount) {
            lbl_8046AC60.prevCount = lbl_8046AC60.curCount;
            return TRUE;
        }
    }
    return FALSE;
}

s32 fn_801E3978(GXRenderModeObj* rmode, u32 x, u32 y, u32 polygonW, u32 polygonH)
{
    if (lbl_8046AC60.open && lbl_8046AC60.state != 0 && lbl_8046AC60.dispTextureSet != NULL) {
        fn_801E1FF8(rmode);
        fn_801E1E1C(lbl_8046AC60.dispTextureSet->ytexture, lbl_8046AC60.dispTextureSet->utexture,
                    lbl_8046AC60.dispTextureSet->vtexture, x, y, lbl_8046AC60.videoInfo.xSize,
                    lbl_8046AC60.videoInfo.ySize, polygonW, polygonH);
        fn_801E24B0();
        return (lbl_8046AC60.dispTextureSet->frameNumber + lbl_8046AC60.initReadFrame) %
               lbl_8046AC60.header.numFrames;
    }
    return -1;
}

BOOL fn_801E3930(THPVideoInfo* videoInfo)
{
    if (lbl_8046AC60.open) {
        memcpy(videoInfo, &lbl_8046AC60.videoInfo, sizeof(THPVideoInfo));
        return TRUE;
    }
    return FALSE;
}

BOOL fn_801E38E8(THPAudioInfo* audioInfo)
{
    if (lbl_8046AC60.open) {
        memcpy(audioInfo, &lbl_8046AC60.audioInfo, sizeof(THPAudioInfo));
        return TRUE;
    }
    return FALSE;
}

u8 fn_801E38D8(void)
{
    return lbl_8046AC60.state;
}

void fn_801E386C(void)
{
    OSMessage tex;

    if (lbl_8047B468) {
        while (TRUE) {
            tex = PopUsedTextureSet();
            if (tex == NULL) {
                break;
            }
            fn_801E4F34(tex);
        }
    }
}

static inline void PushUsedTextureSet(OSMessage msg)
{
    fn_8009F230(&lbl_8046A494, msg, OS_MESSAGE_NOBLOCK);
}

static inline OSMessage PopUsedTextureSet(void)
{
    OSMessage msg;

    if (fn_8009F2F8(&lbl_8046A494, &msg, OS_MESSAGE_NOBLOCK) == TRUE) {
        return msg;
    }
    return NULL;
}

void fn_801E3858(u32* left, u32* right)
{
    *left = lbl_80478D00;
    *right = lbl_80478D04;
}

/*
 * Pulls `sample` stereo samples out of the decoded audio buffers into the
 * stream ring, recording the end of each consumed THP audio frame in the
 * marker ring. Reconstructed helper: the same call/marker/memset sequence is
 * expanded twelve times in retail (four times in fn_801E260C, six in
 * fn_801E2CA8, twice in fn_801E34F0) and has no copy of its own.
 *
 * It replaces the SDK sample's MixAudio and keeps that routine's shape:
 * MixAudio copies its arguments into working cursors before its loop
 * (`requestSample = sample; dst = destination;`, as in the matched copies in
 * doldecomp/sms and doldecomp/mkdd), and the loop then advances the cursors.
 * The saved-register order of every expansion follows from that form: the
 * cursors come after the position and request count (retail fn_801E34F0:
 * position r25:r24, requestSample r26, dstL r27, dstR r28), and Pokemon XD's
 * build of the same routine (0x801E08A4) has the identical order. Advancing
 * the parameters in place instead mirrors the order on every MWCC from
 * GC/1.3 to GC/2.7.
 */
static inline void FillStreamBuffer(s16* left, s16* right, u32 sample)
{
    u64 position;
    u32 requestSample;
    s16* dstL;
    s16* dstR;
    s32 status;
    u32 sampleNum;

    requestSample = sample;
    dstL = left;
    dstR = right;
    position = lbl_8046A440.decodedPosition;

    while (TRUE) {
        sampleNum = fn_801E2B74(dstL, dstR, requestSample, &status);
        position += sampleNum;
        if (status == 0) {
            break;
        }
        if (status == 1) {
            requestSample -= sampleNum;
            dstL += sampleNum;
            if (dstR != NULL) {
                dstR += sampleNum;
            }
            lbl_8046A440.markers[lbl_8046A440.writeMarker] = position;
            if (++lbl_8046A440.writeMarker >= THP_STREAM_FRAMES) {
                lbl_8046A440.writeMarker = 0;
            }
            continue;
        }
        memset(dstL, 0, requestSample * sizeof(s16));
        if (dstR != NULL) {
            memset(dstR, 0, requestSample * sizeof(s16));
        }
        break;
    }
    lbl_8046A440.decodedPosition += sample;
}

BOOL fn_801E34F0(void)
{
    u32 sample = lbl_8046AC60.audioInfo.sndFrequency * 40 / 1000;

    /*
     * The pan select is not narrowed to u8 in retail (no clrlwi). Every MWCC
     * narrows an int-typed select for the u8 parameter; one whose operands
     * are both u8 needs no narrowing. Pokemon XD's C++ build, where the u8
     * prototype is mandatory, is also unnarrowed.
     */
    lbl_80478D00 = fn_8014EE40(0xFF, lbl_8047B470, sample, lbl_8046AC60.audioInfo.sndFrequency,
                               0x7F,
                               lbl_8046AC60.audioInfo.sndChannels == 2 ? (u8)0 : (u8)0x40, 0,
                               0, 0, 0, 0x30000, fn_801E260C, 1, NULL);
    if (lbl_80478D00 == 0xFFFFFFFF) {
        return FALSE;
    }

    if (lbl_8046AC60.audioInfo.sndChannels == 2) {
        lbl_80478D04 = fn_8014EE40(0xFF, lbl_8047B474, sample,
                                   lbl_8046AC60.audioInfo.sndFrequency, 0x7F, 0x7F, 0, 0, 0, 0,
                                   0x30000, fn_801E260C, 0, NULL);
        if (lbl_80478D04 == 0xFFFFFFFF) {
            sndStreamFree(lbl_80478D00);
            return FALSE;
        }
    }

    lbl_8046A440.readMarker = 0;
    lbl_8046A440.writeMarker = 0;
    lbl_8046A440.dmaPosition = 0;
    lbl_8046A440.requestedPosition = 0;
    lbl_8046A440.decodedPosition = 0;

    if (lbl_8046AC60.audioInfo.sndChannels == 2) {
        FillStreamBuffer(lbl_8047B470, lbl_8047B474, sample);
    } else {
        FillStreamBuffer(lbl_8047B470, NULL, sample);
    }

    DCFlushRange(lbl_8047B470, sample * sizeof(s16));
    fn_8014E9B4(lbl_80478D00, 0, sample, 0, 0);
    if (lbl_8046AC60.audioInfo.sndChannels == 2) {
        DCFlushRange(lbl_8047B474, sample * sizeof(s16));
        fn_8014E9B4(lbl_80478D04, 0, sample, 0, 0);
    }
    return TRUE;
}

/*
 * Resynchronises the stream ring on resume: rotates the decoded samples so
 * the position MusyX last requested lands at the start of the ring, rebases
 * the pending frame markers, then refills the part of the ring left over.
 * Retail computes the left refill pointer once, ahead of the channel test,
 * and copies it into the fill cursor in both channel paths, so it is a local
 * (`dst`) here rather than an argument expression repeated in each path.
 */
void fn_801E2CA8(void)
{
    u32 sample = lbl_8046AC60.audioInfo.sndFrequency * 40 / 1000;
    u32 requestOffset;
    u32 decodedOffset;
    u32 size;
    u32 remaining;
    s32 marker;
    s16* dst;

    if (lbl_8046A440.requestedPosition == lbl_8046A440.decodedPosition) {
        lbl_8046A440.decodedPosition = 0;
        if (lbl_8046AC60.audioInfo.sndChannels == 2) {
            FillStreamBuffer(lbl_8047B470, lbl_8047B474, sample);
        } else {
            FillStreamBuffer(lbl_8047B470, NULL, sample);
        }
    } else {
        requestOffset = lbl_8046A440.requestedPosition % sample;
        decodedOffset = lbl_8046A440.decodedPosition % sample;
        if (decodedOffset == 0) {
            decodedOffset = sample;
        }

        if (requestOffset < decodedOffset) {
            size = (decodedOffset - requestOffset) * sizeof(s16);
            memcpy(lbl_8047B470, lbl_8047B470 + requestOffset, size);
            if (lbl_8046AC60.audioInfo.sndChannels == 2) {
                memcpy(lbl_8047B474, lbl_8047B474 + requestOffset, size);
            }

            for (marker = lbl_8046A440.readMarker; marker != lbl_8046A440.writeMarker;) {
                lbl_8046A440.markers[marker] = lbl_8046A440.markers[marker] % sample - requestOffset;
                if (++marker >= THP_STREAM_FRAMES) {
                    marker = 0;
                }
            }

            dst = lbl_8047B470 + size / sizeof(s16);
            remaining = sample - size / sizeof(s16);
            lbl_8046A440.decodedPosition = sample - remaining;
            if (lbl_8046AC60.audioInfo.sndChannels == 2) {
                FillStreamBuffer(dst, lbl_8047B474 + size / sizeof(s16), remaining);
            } else {
                FillStreamBuffer(dst, NULL, remaining);
            }
        } else {
            memcpy(lbl_8046A4E0, lbl_8047B470, sample / 4);
            size = (sample - requestOffset) * sizeof(s16);
            memcpy(lbl_8047B470, lbl_8047B470 + requestOffset, size);
            memcpy(lbl_8047B470 + size / sizeof(s16), lbl_8046A4E0, sample / 4);
            if (lbl_8046AC60.audioInfo.sndChannels == 2) {
                memcpy(lbl_8046A4E0, lbl_8047B474, sample / 4);
                memcpy(lbl_8047B474, lbl_8047B474 + requestOffset, size);
                memcpy(lbl_8047B474 + size / sizeof(s16), lbl_8046A4E0, sample / 4);
            }

            for (marker = lbl_8046A440.readMarker; marker != lbl_8046A440.writeMarker;) {
                if (lbl_8046A440.markers[marker] % sample > sample / 2) {
                    lbl_8046A440.markers[marker] = lbl_8046A440.markers[marker] % sample - requestOffset;
                } else {
                    lbl_8046A440.markers[marker] =
                        lbl_8046A440.markers[marker] % sample + (sample - requestOffset);
                }
                if (++marker >= THP_STREAM_FRAMES) {
                    marker = 0;
                }
            }

            remaining = requestOffset - decodedOffset;
            lbl_8046A440.decodedPosition = sample - remaining;
            dst = lbl_8047B470 + sample - requestOffset + decodedOffset;
            if (lbl_8046AC60.audioInfo.sndChannels == 2) {
                FillStreamBuffer(dst, lbl_8047B474 + sample - requestOffset + decodedOffset,
                                 remaining);
            } else {
                FillStreamBuffer(dst, NULL, remaining);
            }
        }
    }

    lbl_8046A440.dmaPosition = 0;
    lbl_8046A440.requestedPosition = 0;
    DCFlushRange(lbl_8047B470, sample * sizeof(s16));
    fn_8014E9B4(lbl_80478D00, 0, sample, 0, 0);
    if (lbl_8046AC60.audioInfo.sndChannels == 2) {
        DCFlushRange(lbl_8047B474, sample * sizeof(s16));
        fn_8014E9B4(lbl_80478D04, 0, sample, 0, 0);
    }
}

/*
 * Copies up to `sample` samples of the current decoded audio buffer into the
 * stream ring (right channel optional). *status: 0 = request satisfied,
 * 1 = buffer drained, 2 = no decoded audio available. When the current buffer
 * is already empty the retail code returns `num` without assigning it (the
 * return value comes from a saved register never written on that path); this
 * is kept as is.
 */
u32 fn_801E2B74(s16* left, s16* right, u32 sample, s32* status)
{
    u32 num;
    s16* src;
    u32 i;

    if (lbl_8046AC60.playAudioBuffer == NULL) {
        if ((lbl_8046AC60.playAudioBuffer = (THPAudioBuffer*)fn_801E4AC4(0)) == NULL) {
            *status = 2;
            return 0;
        }
    }

    if (lbl_8046AC60.playAudioBuffer->validSample != 0) {
        if (lbl_8046AC60.playAudioBuffer->validSample >= sample) {
            num = sample;
        } else {
            num = lbl_8046AC60.playAudioBuffer->validSample;
        }
        src = lbl_8046AC60.playAudioBuffer->curPtr;

        if (right == NULL) {
            for (i = 0; i < num; i++) {
                *left++ = src[1];
                src += 2;
            }
        } else {
            for (i = 0; i < num; i++) {
                *right++ = src[0];
                *left++ = src[1];
                src += 2;
            }
        }

        lbl_8046AC60.playAudioBuffer->validSample -= num;
        lbl_8046AC60.playAudioBuffer->curPtr = src;
        if (lbl_8046AC60.playAudioBuffer->validSample == 0) {
            fn_801E4B08(lbl_8046AC60.playAudioBuffer);
            lbl_8046AC60.playAudioBuffer = NULL;
            *status = 1;
        } else {
            *status = 0;
        }
    }
    return num;
}

/* MusyX stream update callback; only the left stream (user != 0) refills. */
u32 fn_801E260C(void* buffer1, u32 len1, void* buffer2, u32 len2, u32 user)
{
    u32 size = lbl_8046AC60.audioInfo.sndFrequency * 40 / 1000;
    u64 requested;
    u32 sample;

    if (user) {
        requested = lbl_8046A440.dmaPosition + len1 + len2;
        lbl_8046A440.requestedPosition = requested;
        while (lbl_8046A440.readMarker != lbl_8046A440.writeMarker) {
            if (requested < lbl_8046A440.markers[lbl_8046A440.readMarker]) {
                break;
            }
            if (++lbl_8046A440.readMarker >= THP_STREAM_FRAMES) {
                lbl_8046A440.readMarker = 0;
            }
            lbl_8046AC60.curAudioNumber++;
        }

        if (len1 + len2 >= size / 2) {
            sample = size / 2;

            if (buffer1 == lbl_8047B470) {
                if (lbl_8046AC60.audioInfo.sndChannels == 2) {
                    FillStreamBuffer(lbl_8047B470, lbl_8047B474, sample);
                } else {
                    FillStreamBuffer(lbl_8047B470, NULL, sample);
                }
                DCFlushRange(lbl_8047B470, size);
                fn_8014E9B4(lbl_80478D00, 0, sample, 0, 0);
                if (lbl_8046AC60.audioInfo.sndChannels == 2) {
                    DCFlushRange(lbl_8047B474, size);
                    fn_8014E9B4(lbl_80478D04, 0, sample, 0, 0);
                }
            } else {
                if (lbl_8046AC60.audioInfo.sndChannels == 2) {
                    FillStreamBuffer(lbl_8047B470 + sample, lbl_8047B474 + sample, sample);
                } else {
                    FillStreamBuffer(lbl_8047B470 + sample, NULL, sample);
                }
                DCFlushRange(lbl_8047B470 + sample, size);
                fn_8014E9B4(lbl_80478D00, sample, sample, 0, 0);
                if (lbl_8046AC60.audioInfo.sndChannels == 2) {
                    /* retail flushes `sample` bytes here, not `size` as on the other three */
                    DCFlushRange(lbl_8047B474 + sample, sample);
                    fn_8014E9B4(lbl_80478D04, sample, sample, 0, 0);
                }
            }

            lbl_8046A440.dmaPosition += sample;
            return sample;
        }
    }
    return 0;
}

s32 fn_801E25C8(void)
{
    if (lbl_8046AC60.open && lbl_8046AC60.state != 0 && lbl_8046AC60.dispTextureSet != NULL) {
        return lbl_8046AC60.dispTextureSet->frameNumber + lbl_8046AC60.initReadFrame;
    }
    return -1;
}
