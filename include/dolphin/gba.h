#ifndef DOLPHIN_GBA_H
#define DOLPHIN_GBA_H

#include "dolphin/types.h"

/* Dolphin SDK GBA (JOY Bus) library, public interface (dolphin/gba.h). */

#define GBA_CHAN0 0
#define GBA_CHAN1 1
#define GBA_CHAN2 2
#define GBA_CHAN3 3
#define GBA_MAX_CHAN 4

#define GBA_READY 0
#define GBA_NOT_READY 1
#define GBA_BUSY 2
#define GBA_JOYBOOT_UNKNOWN_STATE 3
#define GBA_JOYBOOT_ERR_INVALID 4

#define GBA_JSTAT_MASK 0x3a
#define GBA_JSTAT_FLAGS_SHIFT 4
#define GBA_JSTAT_FLAGS_MASK 0x30
#define GBA_JSTAT_PSF1 0x20
#define GBA_JSTAT_PSF0 0x10
#define GBA_JSTAT_SEND 0x08
#define GBA_JSTAT_RECV 0x02

typedef void (*GBACallback)(s32 chan, s32 ret);

void GBAInit(void);
s32 GBAGetStatus(s32 chan, u8* status);
s32 GBAGetStatusAsync(s32 chan, u8* status, GBACallback callback);
s32 GBAReset(s32 chan, u8* status);
s32 GBAResetAsync(s32 chan, u8* status, GBACallback callback);
s32 GBARead(s32 chan, u8* dst, u8* status);
s32 GBAReadAsync(s32 chan, u8* dst, u8* status, GBACallback callback);
s32 GBAWrite(s32 chan, u8* src, u8* status);
s32 GBAWriteAsync(s32 chan, u8* src, u8* status, GBACallback callback);

#endif /* DOLPHIN_GBA_H */
