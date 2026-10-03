/* Tail split preserving the surrounding GC/2.5 -O4,p profile. */
#include "src/game/menu/cardesavedata.c"

s32 fn_80087AE8(u8* work, int flags)
{
    extern u32 fn_800D0F44(s32);
    extern u8 menuIsCheck(s32);
    extern u8* windowGetKeyInfo(void);
    extern s8 winMsgCheck(void);
    extern void _threadSwitch(void);
    u8 initialized;
    s8 check;
    u32 status;

    *(u32*)(work + 0x28) = 0;
    initialized = 1;
    if ((s8)work[0x21] >= 0
     && fn_800D0F44((s8)work[0x21]) == 0x40000) {
        initialized = 0;
    }

    for (;;) {
        for (;;) {
            if (menuIsCheck(0x10C) == 0) {
                break;
            }
            _threadSwitch();
        }
        if ((flags & 2) != 0 && (*(u16*)(windowGetKeyInfo() + 4) & 0x20) != 0) {
            *(u32*)(work + 0x28) = 2;
            return 0;
        }
        if (*(u32*)(work + 0x28) == 8) {
            return 0;
        }
        if ((flags & 1) != 0) {
            check = winMsgCheck();
            if (check == 0) {
                *(u32*)(work + 0x28) = 1;
                return 1;
            }
            if ((*(u16*)(windowGetKeyInfo() + 4) & 0x10) != 0 && check == -1) {
                *(u32*)(work + 0x28) = 1;
                return 1;
            }
        }
        if ((flags & 4) != 0) {
            status = fn_800D0F44(((s8*)work)[0x21]);
            if (status != 0x80) {
                if (initialized != 0) {
                    if (status == 0x40000) {
                        *(u32*)(work + 0x28) = 4;
                        return 1;
                    }
                } else if (status != 0x40000) {
                    initialized = 1;
                }
            }
        }
        _threadSwitch();
    }
}
