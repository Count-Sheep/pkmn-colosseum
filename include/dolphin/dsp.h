#ifndef DOLPHIN_DSP_H
#define DOLPHIN_DSP_H

#include "dolphin/types.h"

/* Dolphin SDK DSP task descriptor (dolphin/dsp.h). */
typedef void (*DSPCallback)(void* task);

typedef struct STRUCT_DSP_TASK {
    /* 0x00 */ volatile u32 state;
    /* 0x04 */ volatile u32 priority;
    /* 0x08 */ volatile u32 flags;
    /* 0x0C */ u16* iram_mmem_addr;
    /* 0x10 */ u32 iram_length;
    /* 0x14 */ u32 iram_addr;
    /* 0x18 */ u16* dram_mmem_addr;
    /* 0x1C */ u32 dram_length;
    /* 0x20 */ u32 dram_addr;
    /* 0x24 */ u16 dsp_init_vector;
    /* 0x26 */ u16 dsp_resume_vector;
    /* 0x28 */ DSPCallback init_cb;
    /* 0x2C */ DSPCallback res_cb;
    /* 0x30 */ DSPCallback done_cb;
    /* 0x34 */ DSPCallback req_cb;
    /* 0x38 */ struct STRUCT_DSP_TASK* next;
    /* 0x3C */ struct STRUCT_DSP_TASK* prev;
    /* 0x40 */ OSTime t_context;
    /* 0x48 */ OSTime t_task;
} DSPTaskInfo;

void DSPInit(void);

#endif /* DOLPHIN_DSP_H */
