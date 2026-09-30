#include "dolphin/types.h"

#pragma section ".rodata"
#define RODATA __declspec(section ".rodata")

typedef struct TitleMessagePairEntry {
    u16 id;
    u16 pad;
    u32 messageId;
} TitleMessagePairEntry;

typedef struct TitleMessageChoiceEntry {
    u16 id;
    u16 pad;
    u32 messageId;
    u32 followupMessageId;
} TitleMessageChoiceEntry;

/* Follows the menuSub TU .rodata (0x80266C20-0x80266C30). */
RODATA const TitleMessageChoiceEntry lbl_80266C30[3] = {
    { 0x21F, 0, 0x3AF6, 0x3AF8 },
    { 0x220, 0, 0x4275, 0x3AF9 },
    { 0x221, 0, 0x4276, 0x3AFA },
};

RODATA const TitleMessagePairEntry lbl_80266C54[5] = {
    { 0x21A, 0, 0x3B34 },
    { 0x21D, 0, 0x3B36 },
    { 0x21C, 0, 0x3B38 },
    { 0x21B, 0, 0x3B30 },
    { 0x223, 0, 0x44C5 },
};
