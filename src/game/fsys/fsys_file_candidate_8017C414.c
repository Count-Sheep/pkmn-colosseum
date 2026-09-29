/**
 * @file fsys_file_candidate_8017C414.c
 * @brief FSYS chunked-read step (0x8017C414 - 0x8017C568).
 *
 * Steps the chunked DVD read (fn_8017A624); when the read is complete,
 * advances the active slot's status exactly as the fn_8017A814/fn_8017A95C
 * completion callbacks do and closes its external file.
 *
 * Standalone level-0 code like the rest of fsys (see configure.py; it used
 * to be scored from fsys_file_candidates.c under a local optimization
 * pragma, 93.9%). 99.76%: the only difference is the r4/r5 wall the two
 * callbacks share (retail builds the lbl_80453FEC address in r5); see
 * docs/recon/fsys_8017C414_wall.md. Not linked.
 */
#include "dolphin/types.h"
#include "game/fsys/fsys.h"

extern FSYSManager lbl_80453FEC;
extern s32 fn_8017A624(FSYSSlot* slot);
extern u32 OSDisableInterrupts(void);
extern void OSRestoreInterrupts(u32 level);
extern void fn_80167E64(void* file);

/* Address: 0x8017C414 | size: 0x154 */
s32 fn_8017C414(FSYSSlot* request)
{
    FSYSSlot* slot;
    u32 enabled;

    (void)request; /* second reference: retail keeps the argument in r29 */
    if (fn_8017A624(request)) {
        enabled = OSDisableInterrupts();
        slot = lbl_80453FEC.activeSlot;
        switch (slot->status) {
        case 1: slot->status = 2; break;
        case 3: slot->status = 4; break;
        case 0x65: slot->status = 0x96; break;
        case 0xC8: slot->status = 0xC9; break;
        case 0x12F: slot->status = 0x130; break;
        case 0x190: slot->status = 0x191; break;
        case 2: case 4: case 0x64: case 0xC9: case 0x12D: break;
        default: slot->status = 0x3E8; slot->archiveHandle = 1; break;
        }
        if (slot->tocBuffer) {
            fn_80167E64(slot->tocBuffer);
            slot->tocBuffer = NULL;
        }
        OSRestoreInterrupts(enabled);
    }
    return 1;
}
