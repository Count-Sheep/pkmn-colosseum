#ifndef DOLPHIN_OS_OSMODULE_H
#define DOLPHIN_OS_OSMODULE_H

#include "dolphin/types.h"

/*
 * Relocatable module (REL) layout used by the Dolphin SDK OSLink.c family
 * (0x8009E7A8 - 0x8009F1D0). Field layout follows the public SDK
 * <dolphin/os/OSModule.h>.
 */

typedef struct OSModuleInfo OSModuleInfo;

typedef struct OSModuleLink {
    OSModuleInfo* next;
    OSModuleInfo* prev;
} OSModuleLink;

struct OSModuleInfo {
    u32 id;
    OSModuleLink link;
    u32 numSections;
    u32 sectionInfoOffset;
    u32 nameOffset;
    u32 nameSize;
    u32 version;
};

typedef struct OSSectionInfo {
    u32 offset;
    u32 size;
} OSSectionInfo;

typedef struct OSImportInfo {
    u32 id;
    u32 offset;
} OSImportInfo;

typedef struct OSRel {
    u16 offset;
    u8 type;
    u8 section;
    u32 addend;
} OSRel;

typedef struct OSModuleHeader {
    OSModuleInfo info;
    u32 bssSize;
    u32 relOffset;
    u32 impOffset;
    u32 impSize;
    u8 prologSection;
    u8 epilogSection;
    u8 unresolvedSection;
    u8 bssSection;
    u32 prolog;
    u32 epilog;
    u32 unresolved;
    u32 align;
    u32 bssAlign;
    u32 fixSize;
} OSModuleHeader;

typedef struct OSModuleQueue {
    OSModuleInfo* head;
    OSModuleInfo* tail;
} OSModuleQueue;

#define OS_SECTIONINFO_EXEC 0x1
#define OS_SECTIONINFO_OFFSET(offset) ((offset) & ~OS_SECTIONINFO_EXEC)

#define R_DOLPHIN_NOP 201
#define R_DOLPHIN_SECTION 202
#define R_DOLPHIN_END 203

#endif
