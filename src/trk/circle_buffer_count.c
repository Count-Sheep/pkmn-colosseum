#include "dolphin/types.h"

typedef struct CircleBuffer {
    u8* readPtr;
    u8* writePtr;
    u8* buffer;
    u32 size;
    u32 used;
    u32 free;
} CircleBuffer;

/* NAME-T1 fn_800C41A4 */
u32 CBGetBytesAvailableForRead(CircleBuffer* circle) {
    return circle->used;
}
