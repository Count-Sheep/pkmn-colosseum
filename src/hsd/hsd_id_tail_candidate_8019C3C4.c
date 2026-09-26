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

/* HSD_SetInitParameter's parameter ids, numbered as retail's jump table
 * (jumptable_8036C8C0) dispatches them. 0-4 keep Melee's HSD_InitParam
 * order; 5-7 are the newer library's additions (the names of 2, 3 and 7
 * are spelled by the function's own error strings; 5 and 6 are named for
 * what they set: the heap size checked against fn_8009F3D4 and the arena
 * lo/hi pair). The case bodies stay in retail's code order. */
enum {
    HSD_INIT_FIFO_SIZE,
    HSD_INIT_XFB_MAX_NUM,
    HSD_INIT_HEAP_MAX_NUM,     /* obsolete since 1.3.0.0 */
    HSD_INIT_AUDIO_HEAP_SIZE,  /* obsolete since 1.3.0.0 */
    HSD_INIT_RENDER_MODE_OBJ,
    HSD_INIT_HEAP_SIZE,
    HSD_INIT_ARENA,
    HSD_INIT_MEMORY_CALLBACKS
};

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
    case HSD_INIT_FIFO_SIZE: {
        u32 fifo_size = HSD_VA_ARG(ap, u32);
        if (fifo_size != 0) {
            lbl_80478C7C = fifo_size;
            result = 1;
        }
    } break;
    case HSD_INIT_XFB_MAX_NUM: {
        u32 xfb_max_num = HSD_VA_ARG(ap, u32);
        if (xfb_max_num != 0) {
            lbl_80478C80 = xfb_max_num;
            result = 1;
        }
    } break;
    case HSD_INIT_HEAP_SIZE: {
        u32 heap_size = HSD_VA_ARG(ap, u32);
        if (heap_size < fn_8009F3D4()) {
            lbl_8047B284 = heap_size;
            result = 1;
        }
    } break;
    case HSD_INIT_ARENA:
        arena_lo = HSD_VA_ARG(ap, u32);
        arena_hi = HSD_VA_ARG(ap, u32);
        lbl_8047B270 = arena_lo;
        lbl_8047B274 = arena_hi;
        result = 1;
        break;
    case HSD_INIT_MEMORY_CALLBACKS:
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
    case HSD_INIT_RENDER_MODE_OBJ: {
        u32 render_mode = HSD_VA_ARG(ap, u32);
        if (render_mode != 0) {
            lbl_80478C74 = render_mode;
            result = 1;
        }
    } break;
    case HSD_INIT_HEAP_MAX_NUM:
        OSReport("ERROR in HSD_SetInitParameter():\n");
        OSReport("  HSD_INIT_HEAP_MAX_NUM is obsolete since 1.3.0.0. \n");
        sval = HSD_VA_ARG(ap, s32);
        if (sval == 0) {
            result = 1;
        }
        break;
    case HSD_INIT_AUDIO_HEAP_SIZE:
        OSReport("ERROR in HSD_SetInitParameter():\n");
        OSReport("  HSD_INIT_AUDIO_HEAP_SIZE is obsolete since 1.3.0.0. \n");
        break;
    }
    return result;
}

#undef HSD_VA_ARG
#undef HSD_VA_START
