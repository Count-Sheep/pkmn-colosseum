#ifndef GAME_SAVE_MEMCARD_TASK_H
#define GAME_SAVE_MEMCARD_TASK_H

#include "dolphin/types.h"

/*
 * Memory-card task state (lbl_8047B3D4) and the card image it reads and
 * writes. Field names are descriptive; the offsets are what the memory-card
 * code (0x801CDB04 - 0x801D0AA0) uses.
 */

typedef struct SavedataBody {
    u8 data[0x1DFD0];
} SavedataBody;

/* Same size as SavedataBody; copied by value between the card image and the
 * live save data. */
typedef struct SavedataPayload {
    u8 bytes[0x1DFD0];
} SavedataPayload;

/* One 0x200-byte slot header after the save data (three of them). Word 1
 * holds the save count and word 3 makes the eight first words sum to zero. */
typedef struct MemcardSlotHeader {
    u32 words[0x80];
} MemcardSlotHeader;

typedef struct MemcardWorkBuffer {
    u8 header[8];
    SavedataBody savedata;
    u8 random[0x14];
    u8 hash[0x14];
    MemcardSlotHeader slot[3];
} MemcardWorkBuffer;

typedef struct MemcardDiskId {
    u32 game_code;
    u16 company_code;
    u8 disk_number;
    u8 game_version;
    u8 streaming;
    u8 streaming_buffer_size;
    u8 padding[0x16];
} MemcardDiskId;

typedef struct MemcardTaskState {
    s32 task_kind;
    s32 error_code;
    s32 task_result;
    s32 state;
    s32 resume_state;
    s32 card_channel;
    s32 sector_size;
    s32 memory_size;
    s32 field_20;
    s32 retry_count;
    s32 card_result;
    s32 field_2c;
    u32 serial_hi;
    s32 random_delay;
    u8 field_38[4];
    u8 callback_finished;
    u8 field_3d;
    u8 dialog_result;
    u8 initial_dialog_result;
    u8 format_requested;
    u8 serial_check_enabled;
    u8 field_42[6];
    u32 card_serial[2];
    MemcardWorkBuffer* work_buffer;
    void* card_work_area;
    SavedataBody* savedata_status;
    s32 card_work_size;
    s32 next_state_after_delay;
    void* gapp;
    MemcardDiskId* disk_id;
    MemcardDiskId mounted_disk_id;
    struct {
        s32 chan;
        s32 file_no;
        u32 offset;
        u32 length;
        u16 start_block;
    } file_info;
} MemcardTaskState;

extern MemcardTaskState* lbl_8047B3D4;

#endif /* GAME_SAVE_MEMCARD_TASK_H */
