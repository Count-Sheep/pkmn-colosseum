/**
 * @file fstload.c
 * @brief Dolphin SDK dvd/fstload.c, 0x800A839C - 0x800A85DC, with its data:
 *   .data  0x80312078 the OSReport format strings
 *   .bss   0x803FC418 bb2Buf, __fstLoad's block
 *   .sdata 0x804789E8 the short strings
 *   .sbss  0x8047A838 status, bb2, idTmp
 */
#include "dolphin/dvd/dvd.h"

typedef struct {
    u8 diskID[0x20];
    u8 padding[0x18];
    void* fstLocation;
    u32 fstMaxLength;
} FSTBootInfo;

typedef struct {
    u32 bootFilePosition;
    u32 fstPosition;
    u32 fstLength;
    u32 fstMaxLength;
    void* fstAddress;
} FSTBootBlock;

extern BOOL DVDReadAbsAsyncForBS(DVDCommandBlock* block, void* addr, s32 length,
                                 s32 offset, DVDCBCallback callback);
extern void* OSGetArenaHi(void);
extern void OSSetArenaHi(void* arenaHi);
extern void OSReport(const char* format, ...);
extern void* memcpy(void* destination, const void* source, u32 length);



static s32 status;
static u8 bb2Buf[32 + 31];
static FSTBootBlock* bb2;
static DVDDiskID* idTmp;

static void cb(s32 result, DVDCommandBlock* block) {
    if (result > 0) {
        switch (status) {
        case 0:
            status = 1;
            DVDReadAbsAsyncForBS(block, bb2, 0x20, 0x420, cb);
            break;
        case 1:
            status = 2;
            DVDReadAbsAsyncForBS(block, bb2->fstAddress,
                                 (bb2->fstLength + 0x1F) & ~0x1F,
                                 bb2->fstPosition, cb);
            break;
        }
    } else if (result == -1) {
    } else if (result == -4) {
        status = 0;
        DVDReset();
        DVDReadDiskID(block, idTmp, cb);
    }
}

void __fstLoad(void)
{
    static DVDCommandBlock block;
    FSTBootInfo* bootInfo;
    DVDDiskID* id;
    u8 idTmpBuffer[sizeof(DVDDiskID) + 31];
    void* arenaHi;

    arenaHi = OSGetArenaHi();
    bootInfo = (FSTBootInfo*)0x80000000;
    idTmp = (DVDDiskID*)(((u32)idTmpBuffer + 31) & ~31);
    bb2 = (FSTBootBlock*)(((u32)bb2Buf + 31) & ~31);
    DVDReset();
    DVDReadDiskID(&block, idTmp, cb);
    while (DVDGetDriveStatus() != 0) {
    }
    bootInfo->fstLocation = bb2->fstAddress;
    bootInfo->fstMaxLength = bb2->fstMaxLength;
    id = (DVDDiskID*)bootInfo;
    memcpy(id, idTmp, sizeof(DVDDiskID));
    OSReport("\n");
    OSReport("  Game Name ... %c%c%c%c\n", id->gameName[0], id->gameName[1],
             id->gameName[2], id->gameName[3]);
    OSReport("  Company ..... %c%c\n", id->company[0], id->company[1]);
    OSReport("  Disk # ...... %d\n", id->diskNumber);
    OSReport("  Game ver .... %d\n", id->gameVersion);
    OSReport("  Streaming ... %s\n", id->streaming == 0 ? "OFF" : "ON");
    OSReport("\n");
    OSSetArenaHi(bb2->fstAddress);
}
