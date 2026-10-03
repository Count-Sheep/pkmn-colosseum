#include "dolphin/types.h"
#include "dolphin/pad/Pad.h"
#include "dolphin/si/SI.h"
#include "dolphin/os/OSContext.h"
#include "dolphin/os/OSInterrupt.h"
#include "dolphin/os/OSReset.h"
#include "dolphin/os/OSTime.h"

/* Internal PAD library state. Names kept as lbl_/fn_ where the exact
 * original SDK identifier is not yet confirmed by symbols.txt. */
extern char *lbl_80478A08;         /* __PADVersion */
extern s32 lbl_80478A0C;           /* ResettingChan */
extern u32 lbl_80478A10;           /* XPatchBits */
extern u32 lbl_80478A14;           /* AnalogMode */
extern u32 lbl_80478A18;           /* Spec */
extern u32 lbl_80478A1C;           /* MakeStatus function pointer */
extern u32 lbl_80478A20;           /* CmdReadOrigin */
extern u32 lbl_80478A24;           /* CmdCalibrate */

extern BOOL lbl_8047A8A0;          /* Initialized */
extern u32 lbl_8047A8A4;           /* EnabledBits */
extern u32 lbl_8047A8A8;           /* ResettingBits */
extern u32 lbl_8047A8AC;           /* RecalibrateBits */
extern u32 lbl_8047A8B0;           /* WaitingBits */
extern u32 lbl_8047A8B4;           /* CheckingBits */
extern u32 lbl_8047A8B8;           /* PendingBits */
extern PADSamplingCallback lbl_8047A8BC; /* SamplingCallback */
extern u32 __PADSpec;
extern u32 lbl_8047AA58;           /* __PADFixBits */

extern OSResetFunctionInfo lbl_80312500; /* ResetFunctionInfo */

/* Type[4], Origin[4] and CmdProbeDevice[4] are contiguous TU-local .bss. */
static u32 lbl_803FC5D0[4];        /* Type[4] */
static PADStatus lbl_803FC5E0[4];  /* Origin[4] */
static u32 CmdProbeDevice[4];

extern u16 __OSWirelessPadFixMode : 0x800030E0;
extern u8 GameChoice : 0x800030E3;

/* SI library helpers not yet recovered by name (unassigned SI unit). */
extern void fn_800D0338(s32 chan, u32 command);       /* SISetCommand */
extern u32 SIEnablePolling(u32 poll);
extern u32 SIDisablePolling(u32 poll);
extern BOOL SIGetResponse(s32 chan, void *data);
extern void SIGetTypeAsync(s32 chan, SITypeAndStatusCallback cb);
extern BOOL SIIsChanBusy(s32 chan);
extern u32 SIGetStatus(s32 chan);
extern void fn_800D104C(void);                        /* SIRefreshSamplingRate */
extern void OSRegisterVersion(char *version);

static void UpdateOrigin(s32 chan);
static void PADOriginCallback(s32 chan, u32 error, OSContext *context);
static void fn_800AA73C(s32 chan, u32 error, OSContext *context);
static void PADProbeCallback(s32 chan, u32 error, OSContext *context);
static void PADTypeAndStatusCallback(s32 chan, u32 type);
void PADSetSpec(u32 spec);

static void PADEnable(s32 chan) {
    u32 cmd;
    u32 chanBit;
    u32 data[2];

    chanBit = 0x80000000u >> chan;
    lbl_8047A8A4 |= chanBit;
    SIGetResponse(chan, data);
    cmd = (0x40 << 16) | lbl_80478A14;
    fn_800D0338(chan, cmd);
    SIEnablePolling(lbl_8047A8A4);
}

static void PADDisable(s32 chan) {
    BOOL enabled;
    u32 chanBit;

    enabled = OSDisableInterrupts();

    chanBit = 0x80000000u >> chan;
    SIDisablePolling(chanBit);
    lbl_8047A8A4 &= ~chanBit;
    lbl_8047A8B0 &= ~chanBit;
    lbl_8047A8B4 &= ~chanBit;
    lbl_8047A8B8 &= ~chanBit;
    OSSetWirelessID(chan, 0);

    OSRestoreInterrupts(enabled);
}

static void DoReset(void) {
    u32 chanBit;

    lbl_80478A0C = __cntlzw(lbl_8047A8A8);
    if (lbl_80478A0C != 32) {
        chanBit = 0x80000000u >> lbl_80478A0C;
        lbl_8047A8A8 &= ~chanBit;

        memset(&lbl_803FC5E0[lbl_80478A0C], 0, sizeof(PADStatus));
        SIGetTypeAsync(lbl_80478A0C, PADTypeAndStatusCallback);
    }
}

/*
 * fn_800AA4D4 = UpdateOrigin
 */
static void UpdateOrigin(s32 chan) {
    PADStatus *origin = &lbl_803FC5E0[chan];
    u32 chanBit = 0x80000000u >> chan;

    switch (lbl_80478A14 & 0x00000700) {
    case 0x00000000:
    case 0x00000500:
    case 0x00000600:
    case 0x00000700:
        origin->triggerLeft &= ~0xF;
        origin->triggerRight &= ~0xF;
        origin->analogA &= ~0xF;
        origin->analogB &= ~0xF;
        break;
    case 0x00000100:
        origin->substickX &= ~0xF;
        origin->substickY &= ~0xF;
        origin->analogA &= ~0xF;
        origin->analogB &= ~0xF;
        break;
    case 0x00000200:
        origin->substickX &= ~0xF;
        origin->substickY &= ~0xF;
        origin->triggerLeft &= ~0xF;
        origin->triggerRight &= ~0xF;
        break;
    case 0x00000300:
    case 0x00000400:
        break;
    }

    origin->stickX -= 128;
    origin->stickY -= 128;
    origin->substickX -= 128;
    origin->substickY -= 128;

    if (lbl_80478A10 & chanBit) {
        if (origin->stickX > 0x40) {
            if ((SIGetType(chan) & 0xFFFF0000u) == 0x09000000u) {
                origin->stickX = 0;
            }
        }
    }
}

/*
 * fn_800AA678 = PADOriginCallback
 */
static void PADOriginCallback(s32 chan, u32 error, OSContext *context) {
    if (!(error & 0xF)) {
        UpdateOrigin(lbl_80478A0C);
        PADEnable(lbl_80478A0C);
    }
    DoReset();
}

/*
 * fn_800AA73C = PADOriginUpdateCallback
 */
static void fn_800AA73C(s32 chan, u32 error, OSContext *context) {
    if (!(lbl_8047A8A4 & (0x80000000u >> chan))) {
        return;
    }

    if (!(error & 0xF)) {
        UpdateOrigin(chan);
    }

    if (error & 0x8) {
        PADDisable(chan);
    }
}

/*
 * fn_800AA7FC = PADProbeCallback
 */
static void PADProbeCallback(s32 chan, u32 error, OSContext *context) {
    if (!(error & 0xF)) {
        PADEnable(lbl_80478A0C);
        lbl_8047A8B0 |= 0x80000000u >> lbl_80478A0C;
    }
    DoReset();
}

/*
 * fn_800AA8D4 = PADTypeAndStatusCallback
 */
static void PADTypeAndStatusCallback(s32 chan, u32 type) {
    u32 chanBit;
    u32 recalibrate;
    BOOL rc = TRUE;
    u32 error;

    chanBit = 0x80000000u >> lbl_80478A0C;
    error = type & 0xFF;
    recalibrate = lbl_8047A8AC & chanBit;
    lbl_8047A8AC &= ~chanBit;

    if (error & 0xF) {
        DoReset();
        return;
    }

    type &= ~0xFF;
    lbl_803FC5D0[lbl_80478A0C] = type;

    if ((type & 0x18000000) != 0x08000000 || !(type & 0x01000000)) {
        DoReset();
        return;
    }

    if (lbl_80478A18 < 2) {
        PADEnable(lbl_80478A0C);
        DoReset();
        return;
    }

    if (!(type & 0x80000000) || (type & 0x04000000)) {
        if (recalibrate) {
            rc = SITransfer(lbl_80478A0C, &lbl_80478A24, 3, &lbl_803FC5E0[lbl_80478A0C], 10,
                            PADOriginCallback, 0);
        } else {
            rc = SITransfer(lbl_80478A0C, &lbl_80478A20, 1, &lbl_803FC5E0[lbl_80478A0C], 10,
                            PADOriginCallback, 0);
        }
    } else if ((type & 0x00100000) && !(type & 0x00080000) && !(type & 0x00040000)) {
        if (type & 0x40000000) {
            rc = SITransfer(lbl_80478A0C, &lbl_80478A20, 1, &lbl_803FC5E0[lbl_80478A0C], 10,
                            PADOriginCallback, 0);
        } else {
            rc = SITransfer(lbl_80478A0C, &CmdProbeDevice[lbl_80478A0C], 3,
                            &lbl_803FC5E0[lbl_80478A0C], 8, PADProbeCallback, 0);
        }
    }

    if (!rc) {
        lbl_8047A8B8 |= chanBit;
        DoReset();
        return;
    }
}

/*
 * fn_800AAC00 = PADReceiveCheckCallback
 */
static void fn_800AAC00(s32 chan, u32 type) {
    u32 error;
    u32 chanBit;

    chanBit = 0x80000000u >> chan;
    if (lbl_8047A8A4 & chanBit) {
        error = type & 0xFF;
        type &= ~0xFF;

        lbl_8047A8B0 &= ~chanBit;
        lbl_8047A8B4 &= ~chanBit;

        if (!(error & 0xF) && (type & 0x80000000) && (type & 0x00100000) &&
            (type & 0x40000000) && !(type & 0x04000000) && !(type & 0x00080000) &&
            !(type & 0x00040000)) {
            SITransfer(chan, &lbl_80478A20, 1, &lbl_803FC5E0[chan], 10, fn_800AA73C, 0);
        } else {
            PADDisable(chan);
        }
    }
}

/*
 * fn_800AAD34 = PADReset
 */
BOOL fn_800AAD34(u32 mask) {
    BOOL enabled;
    u32 disableBits;

    enabled = OSDisableInterrupts();

    mask |= lbl_8047A8B8;
    lbl_8047A8B8 = 0;
    mask &= ~(lbl_8047A8B0 | lbl_8047A8B4);
    lbl_8047A8A8 |= mask;
    disableBits = lbl_8047A8A8 & lbl_8047A8A4;
    lbl_8047A8A4 &= ~mask;

    if (lbl_80478A18 == 4) {
        lbl_8047A8AC |= mask;
    }

    SIDisablePolling(disableBits);

    if (lbl_80478A0C == 32) {
        DoReset();
    }

    OSRestoreInterrupts(enabled);
    return TRUE;
}

/*
 * fn_800AAE34 = PADRecalibrate
 */
BOOL fn_800AAE34(u32 mask) {
    BOOL enabled;
    u32 disableBits;

    enabled = OSDisableInterrupts();

    mask |= lbl_8047A8B8;
    lbl_8047A8B8 = 0;
    mask &= ~(lbl_8047A8B0 | lbl_8047A8B4);
    lbl_8047A8A8 |= mask;
    disableBits = lbl_8047A8A8 & lbl_8047A8A4;
    lbl_8047A8A4 &= ~mask;

    if (!(GameChoice & 0x40)) {
        lbl_8047A8AC |= mask;
    }

    SIDisablePolling(disableBits);

    if (lbl_80478A0C == 32) {
        DoReset();
    }

    OSRestoreInterrupts(enabled);
    return TRUE;
}

/*
 * fn_800AAF38 = PADInit
 */
BOOL fn_800AAF38(void) {
    s32 chan;

    if (lbl_8047A8A0) {
        return TRUE;
    }

    OSRegisterVersion(lbl_80478A08);

    if (__PADSpec) {
        PADSetSpec(__PADSpec);
    }

    lbl_8047A8A0 = TRUE;

    if (lbl_8047AA58 != 0) {
        OSTime time = OSGetTime();
        __OSWirelessPadFixMode =
            (u16)((((time) & 0xffff) + ((time >> 16) & 0xffff) + ((time >> 32) & 0xffff) +
                   ((time >> 48) & 0xffff)) &
                  0x3fffu);
        lbl_8047A8AC = 0xF0000000;
    }

    for (chan = 0; chan < 4; ++chan) {
        CmdProbeDevice[chan] =
            (0x4D << 24) | (chan << 22) | ((__OSWirelessPadFixMode & 0x3fffu) << 8);
    }

    fn_800D104C();
    OSRegisterResetFunction(&lbl_80312500);

    return fn_800AAD34(0xF0000000);
}

/*
 * fn_800AB150 = PADRead
 */
u32 fn_800AB150(PADStatus *status) {
    BOOL enabled;
    s32 chan;
    u32 data[2];
    u32 chanBit;
    u32 sr;
    int chanShift;
    u32 motor;

    enabled = OSDisableInterrupts();

    motor = 0;
    for (chan = 0; chan < 4; chan++, status++) {
        chanBit = 0x80000000u >> chan;
        chanShift = 8 * (4 - 1 - chan);

        if (lbl_8047A8B8 & chanBit) {
            fn_800AAD34(0);
            status->err = PAD_ERR_NOT_READY;
            memset(status, 0, 10);
            continue;
        }

        if ((lbl_8047A8A8 & chanBit) || lbl_80478A0C == chan) {
            status->err = PAD_ERR_NOT_READY;
            memset(status, 0, 10);
            continue;
        }

        if (!(lbl_8047A8A4 & chanBit)) {
            status->err = (s8)PAD_ERR_NO_CONTROLLER;
            memset(status, 0, 10);
            continue;
        }

        if (SIIsChanBusy(chan)) {
            status->err = PAD_ERR_TRANSFER;
            memset(status, 0, 10);
            continue;
        }

        sr = SIGetStatus(chan);
        if (sr & 0x8) {
            SIGetResponse(chan, data);

            if (lbl_8047A8B0 & chanBit) {
                status->err = (s8)PAD_ERR_NONE;
                memset(status, 0, 10);

                if (!(lbl_8047A8B4 & chanBit)) {
                    lbl_8047A8B4 |= chanBit;
                    SIGetTypeAsync(chan, fn_800AAC00);
                }
                continue;
            }

            PADDisable(chan);

            status->err = (s8)PAD_ERR_NO_CONTROLLER;
            memset(status, 0, 10);
            continue;
        }

        if (!(SIGetType(chan) & 0x20000000)) {
            motor |= chanBit;
        }

        if (!SIGetResponse(chan, data)) {
            status->err = PAD_ERR_TRANSFER;
            memset(status, 0, 10);
            continue;
        }

        if (data[0] & 0x80000000) {
            status->err = PAD_ERR_TRANSFER;
            memset(status, 0, 10);
            continue;
        }

        ((void (*)(s32, PADStatus *, u32 *))lbl_80478A1C)(chan, status, data);

        if (status->button & 0x2000) {
            status->err = PAD_ERR_TRANSFER;
            memset(status, 0, 10);
            SITransfer(chan, &lbl_80478A20, 1, &lbl_803FC5E0[chan], 10, fn_800AA73C, 0);
            continue;
        }

        status->err = PAD_ERR_NONE;
        status->button &= ~0x0080;
    }

    OSRestoreInterrupts(enabled);
    return motor;
}
