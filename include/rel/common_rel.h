#ifndef REL_COMMON_REL_H
#define REL_COMMON_REL_H

/*
 * REL 125 (common_rel, common.fsys member 0): global data tables that the
 * module's _prolog publishes into main.dol. See docs/REL_MODULES.md.
 */
#include "dolphin/types.h"

/* Song table entry (snd_song_table.c). */
typedef struct SndSong {
    /* 0x0 */ u32 fileId;
    /* 0x4 */ const char* path;
} SndSong;

/* Sample-archive table entry (snd_sample_table.c). */
typedef struct SndSampleArchive {
    /* 0x00 */ u8 unk0;
    /* 0x01 */ u8 active;
    /* 0x02 */ u16 soundId;
    /* 0x04 */ u32 unk4;
    /* 0x08 */ u32 poolId;
    /* 0x0C */ u32 projId;
    /* 0x10 */ u32 sdirId;
    /* 0x14 */ const char* path;
} SndSampleArchive;

extern SndSong lbl_125_data_111B8C[79];
extern SndSampleArchive lbl_125_data_143918[8];

#endif /* REL_COMMON_REL_H */
