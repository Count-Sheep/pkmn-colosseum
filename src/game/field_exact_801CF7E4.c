/**
 * @file field_exact_801CF7E4.c
 * @brief Memory-card load: check the read image (0x801CF7E4 - 0x801CF9C8).
 *
 * Runs after a save file has been read into the work buffer.
 * fn_801CBCDC unscrambles the save data and checks it against the stored
 * digest. If it is good, the card's save count is taken over; a newer count
 * already seen elsewhere is reported (error 5) before the data is used.
 * Load tasks (kinds 1-3) copy the save data out. If the image is bad, the
 * slot header it came from is invalidated (count cleared, header checksum
 * redone) and the task goes on to the next state (0x1F).
 *
 * Built on the save-data SHA-1 / memory-card unit's flags (GC/2.5 -O4,p;
 * see configure.py). Every task-state access goes through lbl_8047B3D4, as
 * retail reloads it after each store.
 */
#include "dolphin/types.h"
#include "game/save/memcard_task.h"

u8 fn_801CBCDC(u8* data, u32 size, const u32 expected[5], u32 offset);

s32 fn_801CF7E4(void)
{
    u8* save;
    u32* header;
    u32 sum;
    s32 i;

    save = (u8*)lbl_8047B3D4->work_buffer;
    ((u32*)save)[3] = 0;
    if (fn_801CBCDC(save, 0x1DFD8, (u32*)(save + 0x1DFEC), 0x18)) {
        if (lbl_8047B3D4->task_kind != 3) {
            lbl_8047B3D4->field_2c = ((s32*)save)[1];
            if ((s32)lbl_8047B3D4->serial_hi > lbl_8047B3D4->field_2c) {
                lbl_8047B3D4->field_2c = lbl_8047B3D4->serial_hi;
                lbl_8047B3D4->error_code = 5;
                switch (lbl_8047B3D4->task_kind) {
                case 1:
                case 2:
                    *(SavedataPayload*)lbl_8047B3D4->savedata_status =
                        *(SavedataPayload*)(save + 8);
                    lbl_8047B3D4->resume_state = 0x25;
                    break;
                default:
                    lbl_8047B3D4->resume_state = 0x24;
                    break;
                }
                return 0x30;
            }
        }
        switch (lbl_8047B3D4->task_kind) {
        case 1:
        case 2:
        case 3:
            *(SavedataPayload*)lbl_8047B3D4->savedata_status =
                *(SavedataPayload*)(save + 8);
            lbl_8047B3D4->error_code = 0xC;
            lbl_8047B3D4->task_result = 3;
            if (lbl_8047B3D4->format_requested != 0) {
                return 0x11;
            }
            return 0x2C;
        }
        return 0x24;
    }

    header = (u32*)((u8*)lbl_8047B3D4->work_buffer +
                    (lbl_8047B3D4->field_20 * 0x200 + 0x1E000));
    header[1] = 0;
    header[3] = 0;
    sum = 0;
    for (i = 0; i < 8; i++) {
        sum += header[i];
    }
    header[3] = -sum;
    return 0x1F;
}
