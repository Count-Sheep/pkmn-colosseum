/**
 * @file fight_action_exact_801F108C.c
 * @brief fightActionFifoInit, 0x801F108C - 0x801F1170.
 *
 * Function-boundary carve of the fight-action range (see
 * fight_action_range_801F0898.c): clear the 32-entry action FIFO. No jump
 * table, no pooled constant; its data is the .bss FIFO (lbl_8046D790) and
 * the .sbss FIFO indices (lbl_8047B5E8/EC), kept extern. GC/1.3 -O4,s like
 * the range, no pragmas.
 */
#include "dolphin/types.h"

void fightActionFifoInit(void) {
    extern u8 lbl_8046D790[];
    extern u32 lbl_8047B5E8;
    extern u32 lbl_8047B5EC;
    extern void fightActionBiosSetKind(void*, u32);
    extern void fightActionBiosSetBuff(void*, u32);
    extern void fightActionBiosSetFightActionDataPtr(void*, u32);
    extern void fightActionBiosSetDispBuff(void*, u32, u32);
    extern void fightActionBiosSetBuffDataPtr(void*, u32);
    extern void fightActionBiosSetBuffDataId(void*, u32);
    extern void fightActionBiosSetActorFightTargetPtr(void*, u32);
    extern void fightActionBiosSetMotoFightActionDataPtr(void*, u32);
    extern void fightActionBiosSetFifoBanme(void*, s32);
    u32 j;
    void* action;
    u32 i;

    lbl_8047B5EC = 0;
    lbl_8047B5E8 = 0;
    for (i = 0; (u16)i < 32; i++) {
        action = &lbl_8046D790[(u16)i * 0x30];
        fightActionBiosSetKind(action, 0);
        fightActionBiosSetBuff(action, 0);
        fightActionBiosSetFightActionDataPtr(action, 0);
        for (j = 0; (u16)j < 4; j++) {
            fightActionBiosSetDispBuff(action, j, 0);
        }
        fightActionBiosSetBuffDataPtr(action, 0);
        fightActionBiosSetBuffDataId(action, 0);
        fightActionBiosSetActorFightTargetPtr(action, 0);
        fightActionBiosSetMotoFightActionDataPtr(action, 0);
        fightActionBiosSetFifoBanme(action, -1);
    }
}
