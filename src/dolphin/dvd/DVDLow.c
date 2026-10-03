#include "dolphin/dvd/dvd.h"
#include "dolphin/os/OSAlarm.h"
#include "dolphin/os/OSClock.h"
#include "dolphin/os/OSInterrupt.h"
#include "dolphin/os/OSTime.h"

#define OSSecondsToTicks(sec) ((sec) * OS_TIMER_CLOCK)

extern volatile u32 __DIRegs[16] : 0xCC006000;
extern volatile u32 __PIRegs[12] : 0xCC003000;

extern volatile BOOL StopAtNextInt_8047A780;
extern DVDLowCallback Callback_8047A788;
extern volatile OSTime LastResetEnd_8047A790;
extern volatile u32 ResetOccurred_8047A798;
extern volatile BOOL WaitingCoverClose_8047A79C;
extern OSAlarm AlarmForTimeout_803FC2F8;
extern u32 lbl_8047A784;
extern DVDLowCallback lbl_8047A78C;
extern volatile BOOL lbl_8047A7A0;
extern volatile u32 WorkAroundType_8047A7A4;
extern u32 WorkAroundSeekLocation_8047A7A8;
extern volatile OSTime lbl_8047A7B0;
extern OSTime lbl_8047A7B8;
extern volatile BOOL lbl_8047A7C0;
extern volatile u32 NextCommandNumber_8047A7C4;
extern BOOL lbl_804789B8;

extern void AlarmHandlerForTimeout(OSAlarm* alarm, OSContext* context);
extern u32 __OSMaskInterrupts(u32 mask);
extern void OSClearContext(OSContext* context);
extern void OSSetCurrentContext(OSContext* context);
extern DVDDiskID* fn_800A7BCC(void);

typedef struct DVDLowCommand {
    s32 command;
    void* address;
    u32 length;
    u32 offset;
    DVDLowCallback callback;
} DVDLowCommand;

typedef struct DVDLowBuffer {
    void* address;
    u32 length;
    u32 offset;
} DVDLowBuffer;

/*
 * DVDLow's static .bss block (CommandList, AlarmForWA, AlarmForTimeout,
 * AlarmForBreak, Prev, Curr) is not owned by this unit yet. Retail reaches
 * these statics from one section base held in a register, so they are laid
 * out here as one structure over CommandList's address.
 */
typedef struct DVDLowStatics {
    DVDLowCommand commandList[3];
    u32 pad;
    OSAlarm alarmForWA;
    OSAlarm alarmForTimeout;
    OSAlarm alarmForBreak;
    DVDLowBuffer prev;
    DVDLowBuffer curr;
} DVDLowStatics;

extern DVDLowCommand CommandList_803FC290[3];

#define CommandList CommandList_803FC290
#define DVDLowBss ((DVDLowStatics*)CommandList_803FC290)

static void Read(void* address, u32 length, u32 offset,
                 DVDLowCallback callback);

static inline BOOL ProcessNextCommand(DVDLowStatics* bss)
{
    s32 n = NextCommandNumber_8047A7C4;

    if (bss->commandList[n].command == 1) {
        ++NextCommandNumber_8047A7C4;
        Read(bss->commandList[n].address, bss->commandList[n].length,
             bss->commandList[n].offset, bss->commandList[n].callback);
        return TRUE;
    } else if (bss->commandList[n].command == 2) {
        ++NextCommandNumber_8047A7C4;
        DVDLowSeek(bss->commandList[n].offset, bss->commandList[n].callback);
        return TRUE;
    }

    return FALSE;
}

void __DVDInterruptHandler(__OSInterrupt interrupt, OSContext* context)
{
    DVDLowStatics* bss = DVDLowBss;
    DVDLowCallback cb;
    OSContext exceptionContext;
    u32 cause = 0;
    u32 reg;
    u32 intr;
    u32 mask;

    if (lbl_8047A7C0) {
        lbl_8047A7B0 = __OSGetSystemTime();
        lbl_804789B8 = FALSE;
        bss->prev.address = bss->curr.address;
        bss->prev.length = bss->curr.length;
        bss->prev.offset = bss->curr.offset;
        if (StopAtNextInt_8047A780 == TRUE) {
            cause |= 8;
        }
    }

    lbl_8047A7C0 = FALSE;
    StopAtNextInt_8047A780 = FALSE;
    reg = __DIRegs[0];
    mask = reg & 0x2A;
    intr = (reg & 0x54) & (mask << 1);

    if (intr & 0x40) {
        cause |= 8;
    }

    if (intr & 0x10) {
        cause |= 1;
    }

    if (intr & 4) {
        cause |= 2;
    }

    if (cause) {
        ResetOccurred_8047A798 = FALSE;
        OSCancelAlarm(&bss->alarmForTimeout);
    }

    __DIRegs[0] = intr | mask;

    if (ResetOccurred_8047A798 &&
        (__OSGetSystemTime() - LastResetEnd_8047A790) < OSMillisecondsToTicks(200)) {
        reg = __DIRegs[1];
        mask = reg & 2;
        intr = (reg & 4) & (mask << 1);
        if (intr & 4) {
            if (lbl_8047A78C) {
                lbl_8047A78C(4);
            }
            lbl_8047A78C = NULL;
        }

        __DIRegs[1] = __DIRegs[1];
    } else if (WaitingCoverClose_8047A79C) {
        reg = __DIRegs[1];
        mask = reg & 2;
        intr = (reg & 4) & (mask << 1);

        if (intr & 4) {
            cause |= 4;
        }

        __DIRegs[1] = intr | mask;
        WaitingCoverClose_8047A79C = FALSE;
    } else {
        __DIRegs[1] = 0;
    }

    if ((cause & 8) && !lbl_8047A7A0) {
        cause &= ~8;
    }

    if ((cause & 1)) {
        if (ProcessNextCommand(bss)) {
            return;
        }
    } else {
        bss->commandList[0].command = -1;
        NextCommandNumber_8047A7C4 = 0;
    }

    OSClearContext(&exceptionContext);
    OSSetCurrentContext(&exceptionContext);

    if (cause) {
        cb = Callback_8047A788;
        Callback_8047A788 = NULL;
        if (cb) {
            cb(cause);
        }

        lbl_8047A7A0 = FALSE;
    }

    OSClearContext(&exceptionContext);
    OSSetCurrentContext(context);
}

void fn_800A41D0(OSAlarm* alarm, OSContext* context)
{
    BOOL error = ProcessNextCommand(DVDLowBss);
}

void AlarmHandlerForTimeout(OSAlarm* alarm, OSContext* context)
{
    DVDLowCallback callback;
    OSContext exceptionContext;

    (void)alarm;
    __OSMaskInterrupts(0x400);
    OSClearContext(&exceptionContext);
    OSSetCurrentContext(&exceptionContext);
    callback = Callback_8047A788;
    Callback_8047A788 = NULL;
    if (callback != NULL) {
        callback(0x10);
    }
    OSClearContext(&exceptionContext);
    OSSetCurrentContext(context);
}

static inline void SetTimeoutAlarm(OSTime timeout)
{
    OSCreateAlarm(&AlarmForTimeout_803FC2F8);
    OSSetAlarm(&AlarmForTimeout_803FC2F8, timeout, AlarmHandlerForTimeout);
}

static inline void SetTimeoutAlarmFrom(DVDLowStatics* bss, OSTime timeout)
{
    OSCreateAlarm(&bss->alarmForTimeout);
    OSSetAlarm(&bss->alarmForTimeout, timeout, AlarmHandlerForTimeout);
}

static void Read(void* address, u32 length, u32 offset,
                 DVDLowCallback callback)
{
    DVDLowStatics* bss = DVDLowBss;

    StopAtNextInt_8047A780 = FALSE;
    lbl_8047A7C0 = TRUE;
    Callback_8047A788 = callback;
    lbl_8047A7B8 = __OSGetSystemTime();

    __DIRegs[2] = 0xA8000000;
    __DIRegs[3] = offset / 4;
    __DIRegs[4] = length;
    __DIRegs[5] = (u32)address;
    __DIRegs[6] = length;
    lbl_8047A784 = length;
    __DIRegs[7] = 3;

    if (length > 0xA00000) {
        SetTimeoutAlarmFrom(bss, OSSecondsToTicks(20));
    } else {
        SetTimeoutAlarmFrom(bss, OSSecondsToTicks(10));
    }
}

static inline BOOL HitCache(DVDLowBuffer* cur, DVDLowBuffer* prev)
{
    u32 prevBlock = (prev->offset + prev->length - 1) >> 15;
    u32 curBlock = cur->offset >> 15;
    u32 cacheBlocks = (fn_800A7BCC()->streaming ? TRUE : FALSE) ? 5 : 15;

    if ((curBlock > prevBlock - 2) || (curBlock < prevBlock + cacheBlocks + 3)) {
        return TRUE;
    }
    return FALSE;
}

static inline void DoJustRead(DVDLowStatics* bss, void* address, u32 length,
                              u32 offset, DVDLowCallback callback)
{
    bss->commandList[0].command = -1;
    NextCommandNumber_8047A7C4 = 0;
    Read(address, length, offset, callback);
}

static void SeekTwiceBeforeRead(void* address, u32 length, u32 offset,
                                DVDLowCallback callback)
{
    DVDLowCommand* commands = CommandList;
    u32 newOffset;

    if ((offset & ~0x7FFF) == 0) {
        newOffset = 0;
    } else {
        newOffset = (offset & ~0x7FFF) + WorkAroundSeekLocation_8047A7A8;
    }

    commands[0].command = 2;
    commands[0].offset = newOffset;
    commands[0].callback = callback;
    commands[1].command = 1;
    commands[1].address = address;
    commands[1].length = length;
    commands[1].offset = offset;
    commands[1].callback = callback;
    commands[2].command = -1;
    NextCommandNumber_8047A7C4 = 0;
    DVDLowSeek(newOffset, callback);
}

static inline void WaitBeforeRead(DVDLowStatics* bss, void* address, u32 length,
                                  u32 offset, DVDLowCallback callback,
                                  OSTime timeout)
{
    bss->commandList[0].command = 1;
    bss->commandList[0].address = address;
    bss->commandList[0].length = length;
    bss->commandList[0].offset = offset;
    bss->commandList[0].callback = callback;
    bss->commandList[1].command = -1;
    NextCommandNumber_8047A7C4 = 0;
    OSCreateAlarm(&bss->alarmForWA);
    OSSetAlarm(&bss->alarmForWA, timeout, fn_800A41D0);
}

BOOL DVDLowRead(void* address, u32 length, u32 offset,
                DVDLowCallback callback)
{
    DVDLowStatics* bss = DVDLowBss;
    OSTime diff;
    u32 prev;

    __DIRegs[6] = length;
    bss->curr.address = address;
    bss->curr.length = length;
    bss->curr.offset = offset;

    if (WorkAroundType_8047A7A4 == 0) {
        DoJustRead(bss, address, length, offset, callback);
    } else if (WorkAroundType_8047A7A4 == 1) {
        if (lbl_804789B8) {
            SeekTwiceBeforeRead(address, length, offset, callback);
        } else {
            if (!HitCache(&bss->curr, &bss->prev)) {
                DoJustRead(bss, address, length, offset, callback);
            } else {
                prev = (bss->prev.offset + bss->prev.length - 1) >> 15;
                if (prev == bss->curr.offset >> 15 ||
                    prev + 1 == bss->curr.offset >> 15)
                {
                    diff = __OSGetSystemTime() - lbl_8047A7B0;
                    if (OSMillisecondsToTicks(5) < diff) {
                        DoJustRead(bss, address, length, offset, callback);
                    } else {
                        WaitBeforeRead(bss, address, length, offset, callback,
                                       OSMillisecondsToTicks(5) - diff +
                                           OSMicrosecondsToTicks(500));
                    }
                } else {
                    SeekTwiceBeforeRead(address, length, offset, callback);
                }
            }
        }
    }
    return TRUE;
}

BOOL fn_800A48DC(DVDLowCallback callback)
{
    Callback_8047A788 = callback;
    StopAtNextInt_8047A780 = FALSE;
    __DIRegs[2] = 0xE0000000;
    __DIRegs[7] = 1;
    SetTimeoutAlarm(OSSecondsToTicks(10));
    return TRUE;
}

BOOL DVDLowBreak(void)
{
    StopAtNextInt_8047A780 = TRUE;
    lbl_8047A7A0 = TRUE;
    return TRUE;
}

DVDLowCallback fn_800A4C94(void)
{
    DVDLowCallback old;

    __DIRegs[1] = 0;
    old = Callback_8047A788;
    Callback_8047A788 = NULL;
    return old;
}

BOOL DVDLowSeek(u32 offset, DVDLowCallback callback)
{
    Callback_8047A788 = callback;
    StopAtNextInt_8047A780 = FALSE;
    __DIRegs[2] = 0xAB000000;
    __DIRegs[3] = offset / 4;
    __DIRegs[7] = 1;
    SetTimeoutAlarm(OSSecondsToTicks(10));
    return TRUE;
}

BOOL DVDLowWaitCoverClose(DVDLowCallback callback)
{
    Callback_8047A788 = callback;
    WaitingCoverClose_8047A79C = TRUE;
    StopAtNextInt_8047A780 = FALSE;
    __DIRegs[1] = 2;
    return TRUE;
}

BOOL DVDLowReadDiskID(DVDDiskID* diskID, DVDLowCallback callback)
{
    Callback_8047A788 = callback;
    StopAtNextInt_8047A780 = FALSE;
    __DIRegs[2] = 0xA8000040;
    __DIRegs[3] = 0;
    __DIRegs[4] = sizeof(DVDDiskID);
    __DIRegs[5] = (u32) diskID;
    __DIRegs[6] = sizeof(DVDDiskID);
    __DIRegs[7] = 3;
    SetTimeoutAlarm(OSSecondsToTicks(10));
    return TRUE;
}

BOOL DVDLowStopMotor(DVDLowCallback callback)
{
    Callback_8047A788 = callback;
    StopAtNextInt_8047A780 = FALSE;
    __DIRegs[2] = 0xE3000000;
    __DIRegs[7] = 1;
    SetTimeoutAlarm(OSSecondsToTicks(10));
    return TRUE;
}

BOOL DVDLowInquiry(DVDDriveInfo* info, DVDLowCallback callback)
{
    Callback_8047A788 = callback;
    StopAtNextInt_8047A780 = FALSE;
    __DIRegs[2] = 0x12000000;
    __DIRegs[4] = sizeof(DVDDriveInfo);
    __DIRegs[5] = (u32) info;
    __DIRegs[6] = sizeof(DVDDriveInfo);
    __DIRegs[7] = 3;
    SetTimeoutAlarm(OSSecondsToTicks(10));
    return TRUE;
}

BOOL DVDLowAudioStream(u32 subcmd, u32 length, u32 offset,
                       DVDLowCallback callback)
{
    Callback_8047A788 = callback;
    StopAtNextInt_8047A780 = FALSE;
    __DIRegs[2] = subcmd | 0xE1000000;
    __DIRegs[3] = offset >> 2;
    __DIRegs[4] = length;
    __DIRegs[7] = 1;
    SetTimeoutAlarm(OSSecondsToTicks(10));
    return TRUE;
}

BOOL DVDLowRequestAudioStatus(u32 subcmd, DVDLowCallback callback)
{
    Callback_8047A788 = callback;
    StopAtNextInt_8047A780 = FALSE;
    __DIRegs[2] = subcmd | 0xE2000000;
    __DIRegs[7] = 1;
    SetTimeoutAlarm(OSSecondsToTicks(10));
    return TRUE;
}

BOOL DVDLowAudioBufferConfig(BOOL enable, u32 size,
                             DVDLowCallback callback)
{
    Callback_8047A788 = callback;
    StopAtNextInt_8047A780 = FALSE;
    __DIRegs[2] = 0xE4000000 | (enable != 0 ? 0x10000 : 0) | size;
    __DIRegs[7] = 1;
    SetTimeoutAlarm(OSSecondsToTicks(10));
    return TRUE;
}

void DVDLowReset(void)
{
    u32 reg;
    OSTime resetStart;

    __DIRegs[1] = 2;
    reg = __PIRegs[9];
    __PIRegs[9] = (reg & ~4) | 1;

    resetStart = __OSGetSystemTime();
    while ((__OSGetSystemTime() - resetStart) < OSMicrosecondsToTicks(12)) {
    }

    __PIRegs[9] = reg | 5;
    ResetOccurred_8047A798 = TRUE;
    LastResetEnd_8047A790 = __OSGetSystemTime();
}
