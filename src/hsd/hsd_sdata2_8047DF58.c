#include "dolphin/types.h"

#pragma section ".sdata2"
#define SDATA2 __declspec(section ".sdata2")

typedef union Sdata2AlignedString2 {
    u8 text[2];
    f64 align;
} Sdata2AlignedString2;

/*
 * Mixed HSD TObj/CObj/util/video/AObj .sdata2 constants and assert
 * strings. Source references and symbolmap strings tie the range to HSD code;
 * aligned string wrappers preserve compiler-emitted padding before later labels.
 */
SDATA2 const f32 lbl_8047DF58 = 0.5f;
SDATA2 const f32 lbl_8047DF5C = 1.1000000238418579f;
SDATA2 const f32 lbl_8047DF60 = 1.2000000476837158f;
SDATA2 const f32 lbl_8047DF64 = 1.3999999761581421f;
SDATA2 const f32 lbl_8047DF68 = 1.0f;
SDATA2 const f32 lbl_8047DF6C = 0.875f;
SDATA2 const f32 lbl_8047DF70 = 1.7999999523162842f;
SDATA2 const f32 lbl_8047DF74 = 2.75f;
SDATA2 const f32 lbl_8047DF78 = 65.0f;
SDATA2 const f32 lbl_8047DF7C = 40.0f;
SDATA2 const f32 lbl_8047DF80 = -30.0f;
SDATA2 const f32 lbl_8047DF84 = -8.0f;
SDATA2 const f32 lbl_8047DF88 = 30.0f;
SDATA2 const f32 lbl_8047DF8C = 1.5707963705062866f;
