/**
 * @file ps_r56_801735EC_prefix.c
 * @brief Particle-system getFloat and psSetBillboardCamera,
 *        0x801735EC - 0x80173718.
 *
 * Standalone source for this split range; the bodies are the ones previously
 * reached through the ps_range_80168C64.c include wrapper.
 */
#include "dolphin/types.h"

#define ref_INC hsd_inline_ref_INC
#include "hsd/hsd_object.h"
#undef ref_INC
#include "hsd/hsd_class.h"

extern void __assert(const char* file, u32 line, const char* condition);

/* object.h's assertion strings live in the shared .rodata split. */
extern const char lbl_802739B0[];  /* "object.h" */
extern const char lbl_802739BC[];  /* "HSD_OBJ(o)->ref_count != HSD_OBJ_NOREF" */

typedef union PSFloatBytes {
    u8 bytes[4];
    f32 value;
} PSFloatBytes;

extern PSFloatBytes lbl_8047B178;
extern HSD_Obj* lbl_8047B190;

/* object.h ref_INC (assert line 93) */
static inline void ref_INC(void* o)
{
    if (o != NULL) {
        HSD_OBJ(o)->ref_count++;
        if (HSD_OBJ(o)->ref_count == HSD_OBJ_NOREF) {
            __assert(lbl_802739B0, 93, lbl_802739BC);
        }
    }
}

u8* getFloat(u8* stream, f32* out) {
    lbl_8047B178.bytes[0] = *stream++;
    lbl_8047B178.bytes[1] = *stream++;
    lbl_8047B178.bytes[2] = *stream++;
    lbl_8047B178.bytes[3] = *stream++;
    *out = lbl_8047B178.value;
    return stream;
}

void psSetBillboardCamera(HSD_Obj* obj) {
    HSD_Obj* old_obj;

    if (obj != (old_obj = lbl_8047B190)) {
        if (old_obj != NULL) {
            if (old_obj != NULL && ref_DEC(old_obj)) {
                if (old_obj != NULL) {
                    HSD_CLASS_METHOD(old_obj)->release((HSD_Class*)old_obj);
                    HSD_CLASS_METHOD(old_obj)->destroy((HSD_Class*)old_obj);
                }
            }
        }
        if (obj != NULL) {
            ref_INC(obj);
        }
        lbl_8047B190 = obj;
    }
}
