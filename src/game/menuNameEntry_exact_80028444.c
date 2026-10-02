/**
 * @file menuNameEntry_exact_80028444.c
 * @brief Name-entry selection labels and button callback.
 */
#include "dolphin/types.h"

extern u32 GSmsgGetGSchar(u32 messageId);
extern void msgctrlSetValue(s32 slot, u32 value);
extern u16* windowGetKeyInfo(void);

s32 menuNameEntrySelectDrawSelText3(void* window, u8* output)
{
    u8* outputBytes;
    void* context;
    void* messages;
    void* message;

    outputBytes = output;
    context = *(void**)((u8*)window + 0x60);
    messages = *(void**)context;
    message = *(void**)((u8*)messages + 8);
    msgctrlSetValue(0x37, GSmsgGetGSchar((u32)message));
    *(u32*)(outputBytes + 0x4C) = 0xCF;
    return 0;
}

s32 menuNameEntrySelectDrawSelText2(void* window, u8* output)
{
    u8* outputBytes;
    void* context;
    void* messages;
    void* message;

    outputBytes = output;
    context = *(void**)((u8*)window + 0x60);
    messages = *(void**)context;
    message = *(void**)((u8*)messages + 4);
    msgctrlSetValue(0x37, GSmsgGetGSchar((u32)message));
    *(u32*)(outputBytes + 0x4C) = 0xCF;
    return 0;
}

s32 menuNameEntrySelectDrawSelText1(void* window, u8* output)
{
    u8* outputBytes;
    void* context;
    void* messages;
    void* message;

    outputBytes = output;
    context = *(void**)((u8*)window + 0x60);
    messages = *(void**)context;
    message = *(void**)messages;
    msgctrlSetValue(0x37, GSmsgGetGSchar((u32)message));
    *(u32*)(outputBytes + 0x4C) = 0xCF;
    return 0;
}

void menuNameEntrySelectButton(void* window)
{
    u8* work;
    u16* keys;

    work = window;
    keys = windowGetKeyInfo();
    if ((keys[0] & 0x20) == 0) {
        if ((keys[2] & 0x10) != 0) {
            work[0x98] = 1;
        }
    }
}
