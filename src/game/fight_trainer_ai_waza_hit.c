/**
 * @file fight_trainer_ai_waza_hit.c
 * @brief Candidate fightTrainerAiWazaHit.cpp range, 0x8025C264 - 0x8025C5A4.
 */
#include "game/fight_trainer_ai_waza_hit_shared.h"

void _fightTrainerAiWazaHitCheck(void* ctx, u32 param1, u32 param2, u32 param3, u32 param4) {
    extern u32 wazaGetStatus();
    extern u32 fightTargetGetTragetPtrToRelativeHostSideFightTargetId();
    extern u32 fightFloorGetFightOutPokemonPtrAry();
    extern void fightFloorSortFightOutPokemonPtrArySub();
    extern u32 fightFloorGetStatus();
    extern u32 fn_80201D84();
    extern u32 fn_80229934();
    extern u32 fn_80229B70();
    extern u32 fn_80229BD8();
    extern u32 fn_80235B04();
    extern u32 fn_80236BFC();
    extern u32 fn_80237288();
    extern u32 fightTrainerAiCheckGuard();
    
    u32 r3 = (u32)ctx;
    u32 r4 = param1;
    u32 r5 = param2;
    u32 r6 = param3;
    u32 r7 = param4;
    u32 r23;
    u32 r24;
    u32 r25;
    u32 r26;
    u32 r27;
    u32 r28;
    u32 r29;
    u32 r30;
    u32 r31;
    u32 r0;
    u32 sp_8;

    r25 = r3;
    r26 = r4;
    r27 = r5;
    r28 = r6;
    r29 = r7;
    
    r31 = fightFloorGetStatus(0, 0, 0x14, 0);
    r31 = r31 & 0xFFFF;
    
    r31 = fightTargetGetTragetPtrToRelativeHostSideFightTargetId(r26, r31);
    
    fn_80235B04(r25, 0, 1);
    
    r30 = wazaGetStatus(0, r27, 9, 0);
    r30 = r30 & 0xFFFF;
    
    if (r28 == 0) {
        r3 = 1;
        return;
    }
    
    sp_8 = 0;
    r24 = fightFloorGetFightOutPokemonPtrAry(0, 1, 0, 0);
    r24 = r24 & 0xFFFF;
    
    fightFloorSortFightOutPokemonPtrArySub(&sp_8, 0, 8, 0);
    
    r6 = 0;
    r7 = 0;
    r5 = 0;
    
    while (1) {
        r3 = r5 & 0xFFFF;
        if (r3 >= r24) break;
        
        r3 = *(u32*)(&sp_8 + r3);
        
        if (r3 != 0) {
            if (r26 == r3) {
                r6 = r5;
            }
            if (r28 == r3) {
                r7 = r5;
            }
        }
        r5 = r5 + 1;
    }
    
    r3 = r6 & 0xFFFF;
    r0 = r7 & 0xFFFF;
    r0 = r3 - r0;
    r24 = (u32)r0 >> 31;

    if (r29 == 0xffff || r29 == 0xfffe) {

        if (r29 == 0xffff) {
            r3 = fn_80236BFC(r25, r28, 0x1d);
            r0 = r3 & 0xFF;
            if (r0 == 1) {
                r3 = fn_80201D84(r28, 0x1d);
                r3 = r3 & 0xFFFF;
                r0 = r31 & 0xFFFF;
                if (r0 == r3) {
                    r3 = -1;
                    return;
                }
            }
        }
        
        r3 = fn_80237288(r25, r26);
        r0 = r3 & 0xFF;
        if ((r0 == 1) && (r24 == 1)) {
            r3 = 0;
            return;
        }
        
        r3 = fightTrainerAiCheckGuard(r25, r28, r27);
        r0 = r3 & 0xFF;
        if (r0 == 1) {
            r3 = 0;
            return;
        }
        
        r3 = 1;
        return;
    }
    
    r3 = fn_80229934(r27, r26, r28);
    r0 = r3 & 0xFF;
    if (r0 == 1) {
        r3 = 0;
        return;
    }
    
    r3 = fightTrainerAiCheckGuard(r25, r28, r27);
    r0 = r3 & 0xFF;
    if (r0 == 1) {
        r3 = 0;
        return;
    }
    
    r3 = fn_80236BFC(r25, r28, 0x1d);
    r0 = r3 & 0xFF;
    if (r0 == 1) {
        r3 = fn_80201D84(r28, 0x1d);
        r3 = r3 & 0xFFFF;
        r0 = r31 & 0xFFFF;
        if (r3 == r0) {
            r3 = -1;
            return;
        }
    }
    
    if (r24 == 1 && r0 == 1 && r0 != 0x39 && r0 != 0xfa) {
        r3 = fn_80236BFC(r25, r28, 0x1f);
        r0 = r3 & 0xFF;
        if (r0 == 1) {
            if (r30 != 0x92) {
                if (r30 != 0x95) {
                    if (r30 != 0x98) {
                        if (r30 != 0xcf) {
                            r3 = 0;
                            return;
                        }
                    }
                }
            }
        }
        
        r3 = fn_80236BFC(r25, r28, 0x20);
        r0 = r3 & 0xFF;
        if ((r0 == 1) && (r30 != 0x93)) {
            r3 = 0;
            return;
        }
        
        r3 = fn_80236BFC(r25, r28, 0x21);
        r0 = r3 & 0xFF;
        r0 = r27 & 0xFFFF;
        r3 = 0;
        return;
    }
    
    r3 = fn_80229B70(r27);
    r0 = r3 & 0xFF;
    if (r0 == 1) {
        r3 = -1;
        return;
    }
    
    r3 = fn_80229BD8(r27);
    r0 = r3 & 0xFF;
    r3 = 1;
    if (r0 != 1) return;
    r3 = -1;

    return;
}
