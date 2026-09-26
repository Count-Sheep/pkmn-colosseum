/**
 * @file hsd_id_tail_candidate_8019C3C4.c
 * @brief HSD_SetInitParameter candidate (0x8019C3C4-0x8019C690).
 *
 * Formerly compiled by including the old combined hsd_fog.c, whose tail this
 * function was; fog.c is now its own HAL translation unit (linked on its
 * own), and the candidate keeps compiling it ahead of the function so the
 * candidate object's layout is unchanged.
 */
#include "src/hsd/fog.c"

typedef struct HSD_VaList {
    u8 gpr;
    u8 fpr;
    u16 padding;
    u32* overflow_arg_area;
    u32* reg_save_area;
} HSD_VaList;
typedef HSD_VaList HSD_VaListArray[1];

extern void* __va_arg(void* ap, u32 type);
extern u32 lbl_80478C74;
extern u32 lbl_80478C7C;
extern u32 lbl_80478C80;
extern u32 lbl_8047B270;
extern u32 lbl_8047B274;
extern s32 lbl_8047B280;
extern u32 lbl_8047B284;
extern u32 lbl_8047B288;
extern s32 lbl_8047B28C;
extern u32 fn_8009F3D4(void);
extern void OSReport(const char* fmt, ...);
extern void _HSD_MemSetCallbacks(void* callbacks, u32 size);

#define HSD_VA_START(ap, last) ((void) last, __builtin_va_info(&(ap)))
#define HSD_VA_ARG(ap, type) (*(type*) __va_arg((ap), 1))

s32 fn_8019C3C4(u32 cmd, ...)
{
    HSD_VaListArray ap;
    u32 callbacks[5];
    u32 result = 0;
    u32 arena_lo;
    u32 arena_hi;
    s32 sval;

    if (lbl_8047B280 != 0) {
        if (lbl_8047B28C == 0) {
            OSReport("init parameter should be set before invoking HSD_Init().\n");
            lbl_8047B28C = 1;
        }
        return result;
    }

    HSD_VA_START(ap, cmd);
    switch (cmd) {
    case 0: {
        u32 fifo_size = HSD_VA_ARG(ap, u32);
        if (fifo_size != 0) {
            lbl_80478C7C = fifo_size;
            result = 1;
        }
    } break;
    case 1: {
        u32 xfb_max_num = HSD_VA_ARG(ap, u32);
        if (xfb_max_num != 0) {
            lbl_80478C80 = xfb_max_num;
            result = 1;
        }
    } break;
    case 2: {
        u32 heap_size = HSD_VA_ARG(ap, u32);
        if (heap_size < fn_8009F3D4()) {
            lbl_8047B284 = heap_size;
            result = 1;
        }
    } break;
    case 3:
        arena_lo = HSD_VA_ARG(ap, u32);
        arena_hi = HSD_VA_ARG(ap, u32);
        lbl_8047B270 = arena_lo;
        lbl_8047B274 = arena_hi;
        result = 1;
        break;
    case 4:
        callbacks[0] = HSD_VA_ARG(ap, u32);
        callbacks[1] = HSD_VA_ARG(ap, u32);
        callbacks[2] = HSD_VA_ARG(ap, u32);
        callbacks[3] = HSD_VA_ARG(ap, u32);
        callbacks[4] = HSD_VA_ARG(ap, u32);
        if (HSD_VA_ARG(ap, u32) != 0) {
            OSReport("ERROR in HSD_SetInitParameter():\n");
            OSReport("  HSD_INIT_MEMORY_CALLBACKS was given invalid arguments.\n");
            break;
        }
        _HSD_MemSetCallbacks(callbacks, 0x14);
        lbl_8047B288 = 1;
        result = 1;
        break;
    case 5: {
        u32 render_mode = HSD_VA_ARG(ap, u32);
        if (render_mode != 0) {
            lbl_80478C74 = render_mode;
            result = 1;
        }
    } break;
    case 6:
        OSReport("ERROR in HSD_SetInitParameter():\n");
        OSReport("  HSD_INIT_HEAP_MAX_NUM is obsolete since 1.3.0.0. \n");
        sval = HSD_VA_ARG(ap, s32);
        if (sval == 0) {
            result = 1;
        }
        break;
    case 7:
        OSReport("ERROR in HSD_SetInitParameter():\n");
        OSReport("  HSD_INIT_AUDIO_HEAP_SIZE is obsolete since 1.3.0.0. \n");
        break;
    }
    return result;
}

#undef HSD_VA_ARG
#undef HSD_VA_START
