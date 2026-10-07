/**
 * Floor-event script dispatch, 0x80116D30 - 0x80116E6C.
 * The shared log pool remains with its original data owner.
 */
#include "dolphin/types.h"

extern "C" {
void GSlogWrite(const char*, ...);
u32 fn_800FF56C(void);
void EvlogSet__FScUl(s8, u32);
s32 scriptSetEventColID(s32);
u32 fn_800F7434(u32, u32, ...);
extern u8 lbl_80272708[];
extern u32 lbl_80478EC4;
extern u32 lbl_80478EC0;
}

// RULE-EXCEPTION(user-approved): scalar wrappers and C++ compilation with
// -opt nopeephole,nopropagation retain retail's register allocation.
// See docs/RULE_EXCEPTIONS.md and docs/recon/fn_80116D30_retry_20261008.md.
template <class T> struct Scalar {
    T value;

    inline Scalar() {}
    inline Scalar(T v) : value(v) {}
    inline operator T() const { return value; }
    inline Scalar& operator=(T v) { value = v; return *this; }
    inline Scalar& operator+=(T v) { value += v; return *this; }
};

extern "C" void fn_80116D30(s8 eventKind, u32 arg)
{
    Scalar<s32> skind;
    u32 floor;
    Scalar<u32> offset;
    u8* rec;
    u8* text;
    u32 kind;

    text = lbl_80272708;
    EvlogSet__FScUl(eventKind, arg);
    skind = (s8)eventKind;
    switch ((s32)skind) {
    case 1:
        GSlogWrite((const char*)(text + 0x14), arg);
        break;
    case 2:
        GSlogWrite((const char*)(text + 0x28), arg);
        break;
    case 3:
        GSlogWrite((const char*)(text + 0x3C), arg);
        break;
    case 4:
        GSlogWrite((const char*)(text + 0x50), arg);
        break;
    }

    floor = (u32)fn_800FF56C();
    kind = 0;
    offset = 0;
    while (kind < *(u32*)lbl_80478EC0) {
        rec = (u8*)(lbl_80478EC4 + (u32)offset);
        if (*(u16*)(rec + 2) == floor &&
            (s32)*(u8*)rec == (s32)skind &&
            *(u32*)(rec + 4) == arg &&
            *(u32*)(rec + 8) != 0) {
            scriptSetEventColID((u32)arg);
            fn_800F7434(*(u32*)(rec + 8), 4, *(u32*)(rec + 0xC),
                        *(u32*)(rec + 0x10), *(u32*)(rec + 0x14),
                        *(u32*)(rec + 0x18));
        }
        offset += 0x1C;
        kind++;
    }
}
