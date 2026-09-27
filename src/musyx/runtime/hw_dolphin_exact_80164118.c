/**
 * @file hw_dolphin_exact_80164118.c
 * @brief MusyX hw_dolphin.c salAiGetDest and salInitDsp,
 *        0x80164118 - 0x80164204.
 *
 * Follows the reference MusyX runtime's hw_dolphin.c. dsp_task and
 * dram_image (.bss), the DSP ucode and the state words stay extern.
 * DSPTaskInfo is the Dolphin SDK task descriptor (0x50 bytes).
 */
#include "dolphin/types.h"

typedef void (*DSPCallback)(void* task);

typedef struct DSPTaskInfo {
    volatile u32 state;
    volatile u32 priority;
    volatile u32 flags;
    u16* iram_mmem_addr;
    u32 iram_length;
    u32 iram_addr;
    u16* dram_mmem_addr;
    u32 dram_length;
    u32 dram_addr;
    u16 dsp_init_vector;
    u16 dsp_resume_vector;
    DSPCallback init_cb;
    DSPCallback res_cb;
    DSPCallback done_cb;
    DSPCallback req_cb;
    struct DSPTaskInfo* next;
    struct DSPTaskInfo* prev;
    s64 t_context;
    s64 t_task;
} DSPTaskInfo;

extern u8 lbl_8047B0A0;                 /* salAIBufferIndex */
extern void* lbl_8047B09C;              /* salAIBufferBase */
extern volatile u32 lbl_8047B088;       /* salDspInitIsDone */
extern DSPTaskInfo lbl_804504A0;        /* dsp_task */
extern u16 lbl_80450500[];              /* dram_image */
extern u8 lbl_8036A520[];               /* dspSlave */
extern u16 lbl_80478C08;                /* dspSlaveLength */

extern void fn_80163F88(void* task);    /* dspInitCallback */
extern void dspResumeCallback(void* task);
extern void DSPInit(void);
extern DSPTaskInfo* DSPAddTask(DSPTaskInfo* task);
extern void hwEnableIrq(void);
extern void hwDisableIrq(void);

void* salAiGetDest(void)
{
    u8 index;

    index = (lbl_8047B0A0 + 2) % 4;
    return (void*)((u8*)lbl_8047B09C + index * 0x280);
}

u32 fn_80164148(void) /* salInitDsp */
{
    lbl_804504A0.iram_mmem_addr = (u16*)lbl_8036A520;
    lbl_804504A0.iram_length = lbl_80478C08;
    lbl_804504A0.iram_addr = 0;
    lbl_804504A0.dram_mmem_addr = lbl_80450500;
    lbl_804504A0.dram_length = 0x2000;
    lbl_804504A0.dram_addr = 0;
    lbl_804504A0.dsp_init_vector = 0x10;
    lbl_804504A0.dsp_resume_vector = 0x30;
    lbl_804504A0.init_cb = fn_80163F88;
    lbl_804504A0.res_cb = dspResumeCallback;
    lbl_804504A0.done_cb = NULL;
    lbl_804504A0.req_cb = NULL;
    lbl_804504A0.priority = 0;
    DSPInit();
    DSPAddTask(&lbl_804504A0);
    lbl_8047B088 = FALSE;
    hwEnableIrq();
    while (!lbl_8047B088) {
    }
    hwDisableIrq();
    return TRUE;
}
