/**
 * @file fight_trainer_ai_irekae.c
 * @brief game/pxdvs/app/fight/fightTrainerAiIrekae.cpp -- split from colosseum_battle.c (the
 *        Colosseum battle-flow/AI bucket, 0x802405C0-0x80265EC4),
 *        address range 0x8024E578-0x80250980, 6 fns.
 *
 * XD source unit: game/pxdvs/app/fight/fightTrainerAiIrekae.cpp
 * Physically split out of the pre/post-battle mega-file by address
 * (functions located and bucketed by name via config/GC6E01/symbols.txt,
 * since this TU uses plain named C bodies with no address-comment
 * markers).
 */

#include "game/colosseum.h"
#include "game/trainer.h"
#include "game/pokemon.h"

/* =========================================================================
 * Duplicated declarations (verbatim from the original colosseum_battle.c
 * preamble, present in every split segment so each TU keeps the same
 * external visibility it had before the split)
 * ========================================================================= */
extern void* pokemonGetStatus();
extern u32   pokemonSetStatus();

/* Battle system functions */
extern void fn_801EF8F4();

/* Sound functions */
extern void soundStop();     /* Stop sound */
extern void fn_80165A20();     /* Fade out music */
extern void fn_801659FC();     /* Start BGM */

/* SDA2 float constants used by asm wrappers */
extern f32 lbl_8047E678;
extern f32 lbl_8047E67C;

/* SDA1 globals used by asm wrappers */
extern u32 lbl_8047B668;
extern u32 lbl_8047B66C;
extern u32 lbl_8047B670;

/* Data labels used by asm wrappers */
extern u8  lbl_8039A6B8[];
extern u8  lbl_8039A6A8[];
extern int lbl_804782BC[];
extern u8  lbl_804782E0[];
extern u8  lbl_804783E0[];

/* Forward declarations for functions used as addresses in asm wrappers */
void ShortCommandProc(int r3);
void ReadProc(int r3);
void WriteProc(int r3);
void __GBASyncCallback(int r3);
u32  __GBASync(int r3);
u32  __GBATransfer(int r3, u32 r4, u32 r5, u32 r6);

/* Forward declarations for asm wrapper bl targets (use () form for compat) */
extern void DSPInit();
extern void set__5GSvecFfff();
extern int  _fadeEffectGetRandom__FUl();
extern u32  pokemonBiosGetCatchTrainerRnd();
extern u32  pokemonBiosGetRnd();
extern u16  pokemonBiosGetPokemonDataId();
extern u32  savedataGetStatus();
extern int  fadeCheck();
extern int  fadeSet();
extern int  wazaSequenceSysRelease();
extern int  fn_801DADC0();
extern void OSRegisterResetFunction();
extern void OSInitAlarm();
extern void OSInitThreadQueue();
extern void* memcpy();

/* Forward declarations for converted functions */
u32 evolutionWazaLearn();
int fightTrainerAiWazaValueKuroikiri(void* ctx, u32 param1, u32 param2, u32 param3);
void fightTrainerAiWazaValueHimitunotikara(void* ctx, u32 param1, u32 param2, u32 param3);
s32 fightTrainerAiSelectIrekaeDasuFightPokemon(void* ctx, u32 param1, u32 param2, u32 param3);
u32 fightTrainerAiWazaHit045(void* trainerCtx, u32 trainerSlot, u32 resultSlot, u32 resultType);
u32 fightMenuFightTrainerGcHeroOpenMenu(void* ctx, u32 param1, u32 param2);

#if !defined(FIGHT_TRAINER_AI_IREKAE_EXACT_8024F8B4_ONLY)
/* =========================================================================
 * fightTrainerAiSelectIrekaeDasuFightPokemon - PostBattleProcessing
 *
 * Large post-battle handler (0x1224 = 4644 bytes).
 *
 * Processes the results of a completed battle:
 *   - Experience calculation and distribution
 *   - Shadow Pokemon purification progress
 *   - Prize money / Poke Coupon rewards
 *   - Story flag updates
 *   - Team state restoration (PP, status)
 * ========================================================================= */
/* TODO: Decompile fightTrainerAiSelectIrekaeDasuFightPokemon (4644 bytes) */
s32 fightTrainerAiSelectIrekaeDasuFightPokemon(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u8 lbl_8027A434[];
    extern u32 lbl_80478B38;
    extern void fn_8000815C();
    extern void fn_800E0C54();
    extern void fightFloorGetFightTrainerFightPokemonPtrAry();
    extern void fightFloorGetFightTrainerFightOutPokemonPtrAry();
    extern void fightFloorGetFightPokemonPtrToFightTrainerPtr();
    extern void fn_801F87CC();
    extern void fightTrainerCheckCanIrekaeFightPokemon();
    extern void fightTrainerGetStatus();
    extern void fn_801FCEC4();
    extern void fightOutPokemonIsFightActionAttackWazaOut();
    extern void fightPokemonGetPokemonPtr();
    extern void fightOutPokemonCheckFightOut();
    extern void fightOutPokemonCreate();
    extern void fn_80235B04();
    extern void fn_802367CC();
    extern void fn_802369B8();
    extern void fn_8023793C();
    extern void fn_80238538();
    extern void fn_80238600();
    extern void fn_802386C8();
    extern void fn_8023881C();
    extern void fn_802389D4();
    extern void fn_80238B0C();
    extern void fn_80238E30();
    extern void fn_80239058();
    extern void fn_8023943C();
    extern void fn_80239500();
    extern void fn_802395C8();
    extern void fn_802397B8();
    extern void fn_802398E4();
    extern void fn_80239984();
    extern void fightTrainerAiAddValue();
    extern void fn_80239A40();
    extern void fn_80239EE8();
    extern void fn_8023A118();
    extern void fn_8023C530();
    extern void fn_8024FE80();
    u8 sp[0x860];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r14 = 0;
    u32 r15 = 0;
    u32 r16 = 0;
    u32 r17 = 0;
    u32 r18 = 0;
    u32 r19 = 0;
    u32 r20 = 0;
    u32 r21 = 0;
    u32 r22 = 0;
    u32 r23 = 0;
    u32 r24 = 0;
    u32 r25 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f0 = 0.0f;
    f32 f4 = 0.0f;
    f32 f8 = 0.0f;
    u32 r1 = (u32)sp;
    u32 r4 = param1;
    u32 r5 = param2;
    u32 r6 = param3;

    void (*ctr_fn)(void) = 0;
    u32 ctr = 0;

    r4 = 0x0;
    r5 = 0x43;
    r16 = r6;
    r15 = r3;
    r6 = 0x0;
    fightTrainerGetStatus();
    r4 = r3 & 0xFFFF;
    r3 = 0x0;
    r5 = 0x2;
    r6 = 0x0;
    fightTrainerGetStatus();
    r21 = r3 & 0xFFFF;
    r3 = 0x0;
    r4 = r21;
    r5 = 0x38;
    r6 = 0x0;
    fightTrainerGetStatus();
    r5 = (u32)lbl_8027A434;
    r0 = r3 & 0xFF;
    r5 = (u32)lbl_8027A434;
    r4 = 0x3;
    *(u8*)(sp + 0x7F0) = r0;
    r6 = (u32)sp + 0x14;
    r14 = 0x0;
    ctr_fn = (void(*)(void))r4;
    do {
        r3 = *(u32*)((u8*)r5 + 0x4);
        r0 = *(u32*)((u8*)r5 + 0x8);
        *(u32*)((u8*)r6 + 0x4) = r3;
        r6 += 8; *(u32*)r6 = r0;
    } while (--ctr != 0);
    r0 = *(u32*)((u8*)r5 + 0x4);
    r3 = r15;
    r4 = 0x0;
    r5 = 0x1;
    *(u32*)((u8*)r6 + 0x4) = r0;
    fn_80235B04();
    r3 = (u32)sp + 0x68;
    r5 = 0x0;
    r4 = 0x0;
    while (1) {
        r0 = r5 & 0xFFFF;
        if (r0 >= (u32)0x6) break;
        r5 = r5 + 0x1;
        *(u32*)(r3 + r0) = r4;

    }
    r4 = r16;
    r3 = (u32)sp + 0x110;
    fn_801FCEC4();
    r3 = r16;
    fightOutPokemonCheckFightOut();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r16;
        r4 = 0xe2;
        r5 = 0x0;
        fightOutPokemonIsFightActionAttackWazaOut();
        r0 = r3 & 0xFF;
        if (r0 == (u32)0x1) {
            r14 = 0x1;
    }
    }
    r3 = r15;
    r4 = (u32)sp + 0x98;
    fn_801F87CC();
    r20 = r3;
    r4 = r15;
    r5 = (u32)sp + 0x34;
    r3 = 0x0;
    r6 = 0x0;
    r7 = 0x1;
    fightFloorGetFightTrainerFightOutPokemonPtrAry();
    *(u32*)(sp + 0x7FC) = r3;
    r4 = r15;
    r5 = (u32)sp + 0xb0;
    r3 = 0x0;
    r6 = 0x1;
    r7 = 0x1;
    fightFloorGetFightTrainerFightPokemonPtrAry();
    r0 = r20 & 0xFFFF;
    *(u32*)(sp + 0x800) = r3;
    if (r0 == (u32)0x0) {
        r3 = -0x1;
        return (s32)r3;
    }
    r4 = r21;
    r3 = 0x0;
    r5 = 0x1f;
    r6 = 0x0;
    fightTrainerGetStatus();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r0 = 0x0;
        r5 = (0x1 << 16);
        *(u32*)(sp + 0x8) = r0;
        r6 = r15;
        *(u32*)(sp + 0xC) = r0;
        r7 = 0x0;
        r8 = 0x0;
        *(u32*)(sp + 0x10) = r0;
        r9 = 0x0;
        r10 = 0x0;
        fn_8023A118();
        fn_800E0C54();
        r5 = r3 & 0xFFFF;
        r4 = r20 & 0xFFFF;
        r0 = (s32)r5 / (s32)r4;
        r3 = (u32)sp + 0x98;
        r0 = r0 * r4;
        r0 = r5 - r0;
        r17 = *(u32*)(r3 + r0);
        if (r17 != (u32)0x0) {
            r4 = r17;
            r3 = 0x0;
            fightFloorGetFightPokemonPtrToFightTrainerPtr();
            r0 = r3;
            r3 = r17;
            r14 = r0;
            fightPokemonGetPokemonPtr();
            r8 = 0x0;
            r5 = (0x1 << 16);
            r0 = 0x228;
            r7 = r3;
            r6 = r14;
            *(u32*)(sp + 0xC) = r0;
            r8 = 0x0;
            r9 = 0x0;
            r10 = 0x0;
            fn_8023A118();
            r3 = r17;
            r4 = 0x0;
            r5 = 0xce;
            r6 = 0x0;
            ((void(*)(void))pokemonGetStatus)();
            r3 = (s16)r3;
            return;
    }
    }
    r0 = r14 & 0xFF;
    if (r0 == (u32)0x1) {
        r14 = (u32)sp + 0x98;
        r17 = (u32)sp + 0x80;
        r18 = (u32)sp + 0x18;
        r19 = 0x0;
        r26 = 0x0;
        while (1) {
            r0 = r26 & 0xFFFF;
            if (r0 >= (u32)0xe) break;
            r4 = 0x0;
            r0 = r3 + 0x2;
            r23 = *(s16*)(r18 + r3);
            r24 = *(s16*)(r18 + r0);
            r3 = r4;
            while (1) {
                r0 = r4 & 0xFFFF;
                if (r0 >= (u32)0x6) break;
                r4 = r4 + 0x1;
                *(u32*)(r17 + r0) = r3;

            }
            r22 = 0x0;
            r25 = r22;
            while (1) {
                r0 = r25 & 0xFFFF;
                if (r0 >= (u32)0x6) break;
                r19 = *(u32*)(r14 + r0);
                if (r19 != (u32)0x0 || (s32)r23 != (s32)r0 || (s32)r24 != (s32)r0) {
                    r0 = (s16)r23;
                    if (r19 >= (u32)0x0) {
                        r3 = r15;
                        r4 = r19;
                        fn_80238600();
                        r0 = r3 & 0xFF;

                    }
                    r0 = (s16)r24;
                    if ((s32)r23 >= (s32)r0) {
                        r3 = r15;
                        r4 = r19;
                        fn_80238538();
                        r0 = r3 & 0xFF;

                    }
                    r3 = (u32)sp + 0x80;
                    *(u32*)(r3 + r0) = r19;
                    r22 = r22 + 0x1;
                }
                r25 = r25 + 0x1;

            }
            r0 = r22 & 0xFFFF;
            r19 = r22;
            if (r0 != (u32)0x6) break;
            r26 = r26 + 0x2;

        }

        r0 = r19 & 0xFFFF;
        if (r0 != (u32)0xe) {
            fn_800E0C54();
            r5 = r3 & 0xFFFF;
            r4 = r19 & 0xFFFF;
            r0 = (s32)r5 / (s32)r4;
            r3 = (u32)sp + 0x80;
            r0 = r0 * r4;
            r0 = r5 - r0;
            r14 = *(u32*)(r3 + r0);
            if (r14 != (u32)0x0) {
                r4 = r14;
                r3 = 0x0;
                fightFloorGetFightPokemonPtrToFightTrainerPtr();
                r15 = r3;
                r3 = r14;
                fightPokemonGetPokemonPtr();
                r8 = 0x0;
                r5 = (0x1 << 16);
                r0 = 0x228;
                r7 = r3;
                r6 = r15;
                *(u32*)(sp + 0xC) = r0;
                r8 = 0x0;
                r9 = 0x0;
                r10 = 0x0;
                fn_8023A118();
                r3 = r14;
                r4 = 0x0;
                r5 = 0xce;
                r6 = 0x0;
                ((void(*)(void))pokemonGetStatus)();
                r3 = (s16)r3;
                return;
    }
    }
    }
    r4 = (0xffff << 16);
    r3 = (0x1 << 16);
    r18 = r4 + 0x1;
    r22 = (u32)sp + 0x98;
    r17 = r20 & 0xFFFF;
    r19 = 0x0;
    r23 = 0x0;
    while (1) {
        r0 = r23 & 0xFFFF;
        if (r0 >= (u32)r17) break;
        r24 = *(u32*)(r22 + r0);
        if (r24 != (u32)0x0) {
            r3 = r24;
            r4 = 0x0;
            r5 = 0xce;
            r6 = 0x0;
            ((void(*)(void))pokemonGetStatus)();
            r0 = (s16)r3;
            if (r24 >= (u32)0x0) {
                r3 = r15;
                r4 = r24;
                fn_802386C8();
                r0 = r3;
                r3 = r15;
                r25 = r0;
                r4 = r24;
                fn_802389D4();
                r0 = r3;
                r3 = r24;
                r24 = r0;
                fightPokemonGetPokemonPtr();
                r4 = 0x0;
                r5 = 0xc9;
                r6 = 0x0;
                ((void(*)(void))pokemonGetStatus)();
                r3 = r3 & 0xFFFF;
                if (r19 < r25) {
                    r19 = r25;
                }
                if ((s32)r18 < (s32)r24) {
                    r18 = r24;
                }
                r0 = r14 & 0xFFFF;
                if (r0 > r3) {
                    r14 = r3;
        }
        }
        }
        r23 = r23 + 0x1;

    }
    r4 = r21;
    r3 = 0x0;
    r5 = 0x21;
    r6 = 0x0;
    fightTrainerGetStatus();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r22 = (u32)sp + 0x98;
        r17 = r20 & 0xFFFF;
        r23 = 0x0;
        while (1) {
            r0 = r23 & 0xFFFF;
            if (r0 >= (u32)r17) break;
            r24 = *(u32*)(r22 + r0);
            if (r24 != (u32)0x0) {
                r3 = r24;
                r4 = 0x0;
                r5 = 0xce;
                r6 = 0x0;
                ((void(*)(void))pokemonGetStatus)();
                r0 = (s16)r3;
                if (r24 >= (u32)0x0) {
                    r4 = r24;
                    r3 = 0x0;
                    fightFloorGetFightPokemonPtrToFightTrainerPtr();
                    r3 = r24;
                    fightPokemonGetPokemonPtr();
                    r4 = 0x0;
                    r5 = 0xc9;
                    r6 = 0x0;
                    ((void(*)(void))pokemonGetStatus)();
                    r3 = r3 & 0xFFFF;
                    r0 = r14 & 0xFFFF;
                    if (r0 >= (u32)r3) {
                        r4 = r24;
                        r3 = 0x0;
                        fightFloorGetFightPokemonPtrToFightTrainerPtr();
                        r14 = r3;
                        r3 = r24;
                        fightPokemonGetPokemonPtr();
                        r8 = 0x0;
                        r5 = (0x1 << 16);
                        r0 = 0x228;
                        r7 = r3;
                        r6 = r14;
                        *(u32*)(sp + 0xC) = r0;
                        r8 = 0x0;
                        r9 = 0x0;
                        r10 = 0x0;
                        fn_8023A118();
                        r3 = r24;
                        r4 = 0x0;
                        r5 = 0xce;
                        r6 = 0x0;
                        ((void(*)(void))pokemonGetStatus)();
                        r3 = (s16)r3;
                        return;
            }
            }
            }
            r23 = r23 + 0x1;

        }
    }
    r0 = *(u8*)(sp + 0x7F0);
    r28 = 0x0;
    r0 = r3 + 0x1;
    *(u32*)(sp + 0x7F4) = r0;
    r0 = r20 & 0xFFFF;
    *(u32*)(sp + 0x808) = r0;
    while (1) {
        r0 = *(u32*)(sp + 0x808);
        r3 = r28 & 0xFFFF;
        if (r3 >= (u32)r0) break;
        r3 = (u32)sp + 0x98;
        r27 = *(u32*)(r3 + r30);
        if (r27 != (u32)0x0) {
            r3 = r27;
            r4 = 0x0;
            r5 = 0xce;
            r6 = 0x0;
            ((void(*)(void))pokemonGetStatus)();
            r0 = (s16)r3;
            if (r27 >= (u32)0x0) {
                r4 = r27;
                r3 = 0x0;
                fightFloorGetFightPokemonPtrToFightTrainerPtr();
                r26 = r3;
                r3 = r16;
                r4 = r27;
                r5 = 0x0;
                fightOutPokemonCreate();
                r3 = r15;
                r4 = r16;
                r5 = (u32)sp + 0x54;
                r6 = 0x0;
                r7 = 0x1;
                fn_802367CC();
                r0 = *(u32*)(sp + 0x7FC);
                r23 = r3;
                r31 = (u32)sp + 0x34;
                r14 = 0x0;
                r17 = r0 & 0xFFFF;
                r25 = 0x0;
                while (1) {
                    r0 = r25 & 0xFFFF;
                    if (r0 >= (u32)r17) break;
                    r29 = *(u32*)(r31 + r0);
                    if (r29 != (u32)0x0) {
                        r22 = r23 & 0xFFFF;
                        r24 = 0x0;
                        while (1) {
                            r0 = r24 & 0xFFFF;
                            if (r0 >= (u32)r22) break;
                            r3 = (u32)sp + 0x54;
                            r5 = *(u16*)(r3 + r0);
                            if (r5 == (u32)0x0 || r5 == (u32)0x165 || r0 == (u32)0x1) {

                                r3 = r15;
                                r4 = r16;
                                r6 = r29;
                                fn_8023C530();
                                r0 = r3 & 0xFF;

                                r14 = 0x1;
                                break;
                            }
                            r24 = r24 + 0x1;

                        }

                        r0 = r14 & 0xFF;
                        if (r0 == (u32)0x1) break;
                    }
                    r25 = r25 + 0x1;

                }

                r3 = r16;
                r4 = (u32)sp + 0x110;
                fn_801FCEC4();
                r3 = r15;
                r4 = r27;
                r5 = (u32)sp + 0x54;
                r6 = 0x0;
                r7 = 0x1;
                fn_802369B8();
                r3 = r27;
                fightPokemonGetPokemonPtr();
                r0 = 0x0;
                r5 = (0x1 << 16);
                *(u32*)(sp + 0x8) = r0;
                r0 = 0x227;
                r7 = r3;
                r29 = (u32)sp + 0x68;
                *(u32*)(sp + 0xC) = r0;
                r6 = r26;
                r0 = *(u32*)(r29 + r30);
                r8 = 0x0;
                r9 = 0x0;
                *(u32*)(sp + 0x10) = r0;
                r10 = 0x0;
                fn_8023A118();
                r3 = r15;
                r4 = r27;
                fn_80238600();
                r17 = r3;
                r0 = r3 & 0xFF;
                if (r0 == (u32)0x1) {
                    r3 = *(u32*)(r29 + r30);
                    r4 = r15;
                    r5 = 0x1a;
                    fn_80239984();
                    *(u32*)(r29 + r30) = r3;
                    r3 = r27;
                    fightPokemonGetPokemonPtr();
                    r7 = (0x1 << 16);
                    r5 = r3;
                    r4 = r26;
                    r6 = 0x0;
                    r7 = 0x0;
                    r8 = 0x0;
                    r9 = 0x0;
                    r10 = 0x1a;
                    fn_80239EE8();
                }
                r0 = r17 & 0xFF;
                if (r0 == (u32)0x2) {
                    r3 = *(u32*)(r29 + r30);
                    r4 = r15;
                    r5 = 0x1b;
                    fn_80239984();
                    *(u32*)(r29 + r30) = r3;
                    r3 = r27;
                    fightPokemonGetPokemonPtr();
                    r7 = (0x1 << 16);
                    r5 = r3;
                    r4 = r26;
                    r6 = 0x0;
                    r7 = 0x0;
                    r8 = 0x0;
                    r9 = 0x0;
                    r10 = 0x1b;
                    fn_80239EE8();
                }
                r0 = r17 & 0xFF;
                if (r0 == (u32)0x3) {
                    r3 = *(u32*)(r29 + r30);
                    r4 = r15;
                    r5 = 0x1c;
                    fn_80239984();
                    *(u32*)(r29 + r30) = r3;
                    r3 = r27;
                    fightPokemonGetPokemonPtr();
                    r7 = (0x1 << 16);
                    r5 = r3;
                    r4 = r26;
                    r6 = 0x0;
                    r7 = 0x0;
                    r8 = 0x0;
                    r9 = 0x0;
                    r10 = 0x1c;
                    fn_80239EE8();
                }
                r17 = 0x0;
                while (1) {
                    r0 = r17 & 0xFFFF;
                    if (r0 >= (u32)0x2) break;
                    r4 = r21;
                    r6 = r17;
                    r3 = 0x0;
                    r5 = 0x39;
                    fightTrainerGetStatus();
                    r5 = r3 & 0xFFFF;
                    if (r5 != (u32)0x9) {
                        r3 = r15;
                        r4 = r27;
                        fn_80238E30();
                        r0 = r3 & 0xFF;
                        if (r0 == (u32)0x1) {
                            r4 = r21;
                            r6 = r17;
                            r3 = 0x0;
                            r5 = 0x3a;
                            fightTrainerGetStatus();
                            r22 = r3 & 0xFF;
                            r3 = *(u32*)(r29 + r30);
                            r4 = r22;
                            r5 = r15;
                            r6 = 0x1d;
                            fn_802398E4();
                            *(u32*)(r29 + r30) = r3;
                            r3 = r27;
                            fightPokemonGetPokemonPtr();
                            r6 = (0x1 << 16);
                            r5 = r3;
                            r4 = r15;
                            r6 = 0x0;
                            r7 = 0x0;
                            r8 = 0x0;
                            r9 = 0x0;
                            r10 = 0x1d;
                            fn_80239A40();
                    }
                    }
                    r17 = r17 + 0x1;

                }
                r3 = r15;
                r4 = r27;
                fn_8023881C();
                r0 = r3 & 0xFF;
                if (r0 == (u32)0x1) {
                    r3 = *(u32*)(r29 + r30);
                    r4 = r15;
                    r5 = 0x1e;
                    fn_80239984();
                    *(u32*)(r29 + r30) = r3;
                    r3 = r27;
                    fightPokemonGetPokemonPtr();
                    r7 = (0x1 << 16);
                    r5 = r3;
                    r4 = r26;
                    r6 = 0x0;
                    r7 = 0x0;
                    r8 = 0x0;
                    r9 = 0x0;
                    r10 = 0x1e;
                    fn_80239EE8();
                }
                r0 = r14 & 0xFF;
                if (r0 == (u32)0x1) {
                    r3 = *(u32*)(r29 + r30);
                    r4 = r15;
                    r5 = 0x1f;
                    fn_80239984();
                    *(u32*)(r29 + r30) = r3;
                    r3 = r27;
                    fightPokemonGetPokemonPtr();
                    r7 = (0x1 << 16);
                    r5 = r3;
                    r4 = r26;
                    r6 = 0x0;
                    r7 = 0x0;
                    r8 = 0x0;
                    r9 = 0x0;
                    r10 = 0x1f;
                    fn_80239EE8();
                }
                r3 = r15;
                r4 = r27;
                fn_802386C8();
                if (r19 <= (u32)r3) {
                    r3 = *(u32*)(r29 + r30);
                    r4 = r15;
                    r5 = 0x21;
                    fn_80239984();
                    *(u32*)(r29 + r30) = r3;
                    r3 = r27;
                    fightPokemonGetPokemonPtr();
                    r7 = (0x1 << 16);
                    r5 = r3;
                    r4 = r26;
                    r6 = 0x0;
                    r7 = 0x0;
                    r8 = 0x0;
                    r9 = 0x0;
                    r10 = 0x21;
                    fn_80239EE8();
                }
                r3 = r15;
                r4 = r27;
                fn_802389D4();
                if ((s32)r18 <= (s32)r3) {
                    r3 = *(u32*)(r29 + r30);
                    r4 = r15;
                    r5 = 0x20;
                    fn_80239984();
                    *(u32*)(r29 + r30) = r3;
                    r3 = r27;
                    fightPokemonGetPokemonPtr();
                    r7 = (0x1 << 16);
                    r5 = r3;
                    r4 = r26;
                    r6 = 0x0;
                    r7 = 0x0;
                    r8 = 0x0;
                    r9 = 0x0;
                    r10 = 0x20;
                    fn_80239EE8();
                }
                r0 = *(u32*)(sp + 0x7FC);
                r24 = 0x0;
                r14 = r0 & 0xFFFF;
                while (1) {
                    r0 = r24 & 0xFFFF;
                    if (r0 >= (u32)r14) break;
                    r3 = (u32)sp + 0x34;
                    r23 = *(u32*)(r3 + r0);
                    if (r23 != (u32)0x0) {
                        r0 = *(u32*)(sp + 0x804);
                        r25 = 0x0;
                        r31 = r0 & 0xFFFF;
                        while (1) {
                            r0 = r25 & 0xFFFF;
                            if (r0 >= (u32)r31) break;
                            r3 = (u32)sp + 0x54;
                            r22 = *(u16*)(r3 + r0);
                            if (r22 != (u32)0x0) {
                                r3 = r15;
                                r4 = r22;
                                r5 = r16;
                                fn_802395C8();
                                r0 = r3 & 0xFFFF;
                                r17 = r3;
                                if (r0 != (u32)0x9) {
                                    r3 = r15;
                                    r4 = r22;
                                    r5 = 0x1;
                                    fn_8023943C();
                                    r0 = r3 & 0xFF;
                                    if (r0 != (u32)0x9) {
                                        r3 = r15;
                                        r4 = r22;
                                        fn_80239500();
                                        r6 = r3;
                                        r3 = r15;
                                        r4 = r23;
                                        r5 = r17;
                                        fn_8023793C();
                                        r0 = r3 & 0xFFFF;
                                        if (r0 == (u32)0x41) {
                                            r3 = *(u32*)(r29 + r30);
                                            r4 = r15;
                                            r5 = 0x22;
                                            fn_80239984();
                                            *(u32*)(r29 + r30) = r3;
                                            r3 = r27;
                                            fightPokemonGetPokemonPtr();
                                            r7 = (0x1 << 16);
                                            r5 = r3;
                                            r4 = r26;
                                            r6 = 0x0;
                                            r7 = 0x0;
                                            r8 = 0x0;
                                            r9 = 0x0;
                                            r10 = 0x22;
                                            fn_80239EE8();
                            }
                            }
                            }
                            }
                            r25 = r25 + 0x1;

                        }
                    }
                    r24 = r24 + 0x1;

                }
                r0 = *(u32*)(sp + 0x7FC);
                r22 = 0x0;
                r31 = r0 & 0xFFFF;
                while (1) {
                    r0 = r22 & 0xFFFF;
                    if (r0 >= (u32)r31) break;
                    r3 = (u32)sp + 0x34;
                    r23 = *(u32*)(r3 + r0);
                    if (r23 != (u32)0x0) {
                        r3 = r15;
                        r4 = r23;
                        r5 = (u32)sp + 0x54;
                        r6 = 0x0;
                        r7 = 0x0;
                        fn_802367CC();
                        r14 = r3 & 0xFFFF;
                        r17 = 0x0;
                        while (1) {
                            r0 = r17 & 0xFFFF;
                            if (r0 >= (u32)r14) break;
                            r3 = (u32)sp + 0x54;
                            r24 = *(u16*)(r3 + r0);
                            if (r24 != (u32)0x0) {
                                r3 = r15;
                                r4 = r24;
                                r5 = r23;
                                fn_802395C8();
                                r0 = r3 & 0xFFFF;
                                r25 = r3;
                                if (r0 != (u32)0x9) {
                                    r3 = r15;
                                    r4 = r24;
                                    r5 = 0x1;
                                    fn_8023943C();
                                    r0 = r3 & 0xFF;
                                    if (r0 != (u32)0x9) {
                                        r3 = r15;
                                        r4 = r24;
                                        fn_80239500();
                                        r6 = r3;
                                        r3 = r15;
                                        r4 = r27;
                                        r5 = r25;
                                        fn_80238B0C();
                                        r0 = r3 & 0xFFFF;
                                        if (r0 == (u32)0x41) {
                                            r3 = *(u32*)(r29 + r30);
                                            r4 = r15;
                                            r5 = 0x23;
                                            fn_80239984();
                                            *(u32*)(r29 + r30) = r3;
                                            r3 = r27;
                                            fightPokemonGetPokemonPtr();
                                            r7 = (0x1 << 16);
                                            r5 = r3;
                                            r4 = r26;
                                            r6 = 0x0;
                                            r7 = 0x0;
                                            r8 = 0x0;
                                            r9 = 0x0;
                                            r10 = 0x23;
                                            fn_80239EE8();
                            }
                            }
                            }
                            }
                            r17 = r17 + 0x1;

                        }
                    }
                    r22 = r22 + 0x1;

                }
                r4 = r21;
                r3 = 0x0;
                r5 = 0x20;
                r6 = 0x0;
                fightTrainerGetStatus();
                r0 = r3 & 0xFF;
                if (r0 == (u32)0x1) {
                    r17 = 0x0;
                    while (1) {
                        r0 = lbl_80478B38;
                        r3 = r17 & 0xFFFF;
                        if (r3 >= (u32)r0) break;
                        r0 = r17 & 0xFFFF;
                        if (r0 != (u32)0x9) {
                            r3 = r15;
                            r4 = r27;
                            r5 = r17;
                            r6 = 0x1;
                            fn_80238B0C();
                            r0 = r3 & 0xFFFF;
                            if (r0 == (u32)0x41) {
                                r0 = *(u32*)(sp + 0x800);
                                r14 = (u32)sp + 0xb0;
                                r22 = 0x0;
                                r23 = r0 & 0xFFFF;
                                while (1) {
                                    r0 = r22 & 0xFFFF;
                                    if (r0 >= (u32)r23) break;
                                    r24 = *(u32*)(r14 + r0);
                                    if (r24 != (u32)0x0) {
                                        r4 = r24;
                                        r3 = 0x0;
                                        fightFloorGetFightPokemonPtrToFightTrainerPtr();
                                        if (r3 != (u32)0x0) {
                                            r4 = r24;
                                            fightTrainerCheckCanIrekaeFightPokemon();
                                            r0 = r3 & 0xFF;
                                    }
                                    }
                                    if (r0 != (u32)0x1 || r0 != (u32)0x2 && r0 != (u32)0x3 || r0 != (u32)0x2 && r0 != (u32)0x3) {

                                        if (r0 == (u32)0x2 || r0 == (u32)0x3) {

                                            r3 = r15;
                                            r4 = r24;
                                            r5 = r17;
                                            r6 = 0x1;
                                            fn_80238B0C();
                                            r0 = r3 & 0xFFFF;
                                            if (r0 == (u32)0x41) {
                                                r3 = *(u32*)(r29 + r30);
                                                r4 = r15;
                                                r5 = 0x24;
                                                fn_80239984();
                                                *(u32*)(r29 + r30) = r3;
                                                r3 = r27;
                                                fightPokemonGetPokemonPtr();
                                                r7 = (0x1 << 16);
                                                r5 = r3;
                                                r4 = r26;
                                                r6 = 0x0;
                                                r7 = 0x0;
                                                r8 = 0x0;
                                                r9 = 0x0;
                                                r10 = 0x24;
                                                fn_80239EE8();
                            }
                                        }
                                    }
                                    r22 = r22 + 0x1;

                                }
                    }
                        }
                        r17 = r17 + 0x1;

                    }
                }
                r3 = r15;
                r4 = r27;
                fn_8024FE80();
                r0 = r3 & 0xFFFF;
                r14 = r3;
                if (r3 != (u32)r0) {
                    r3 = *(u32*)(r29 + r30);
                    r4 = r15;
                    r5 = r14;
                    fn_80239984();
                    *(u32*)(r29 + r30) = r3;
                    r3 = r27;
                    fightPokemonGetPokemonPtr();
                    r6 = (0x1 << 16);
                    r5 = r3;
                    r4 = r26;
                    r10 = r14;
                    r6 = 0x0;
                    r7 = 0x0;
                    r8 = 0x0;
                    r9 = 0x0;
                    fn_80239EE8();
                }
                r0 = *(u32*)(sp + 0x7F8);
                r0 = r0 & 0xFF;
                if (r0 == (u32)0x2) {
                    r3 = r15;
                    r4 = r27;
                    r5 = 0x21;
                    fn_80239058();
                    r0 = r3 & 0xFF;
                    if (r0 != (u32)0x1) {
                        r3 = r15;
                        r4 = r27;
                        r5 = 0x2c;
                        fn_80239058();
                        r0 = r3 & 0xFF;
                        if (r0 != (u32)0x1) goto L_8024F710;
                    }
                    r3 = *(u32*)(r29 + r30);
                    r4 = r15;
                    r5 = 0x29;
                    fn_80239984();
                    *(u32*)(r29 + r30) = r3;
                    r3 = r27;
                    fightPokemonGetPokemonPtr();
                    r7 = (0x1 << 16);
                    r5 = r3;
                    r4 = r26;
                    r6 = 0x0;
                    r7 = 0x0;
                    r8 = 0x0;
                    r9 = 0x0;
                    r10 = 0x29;
                    fn_80239EE8();

                } else {
                    if (r0 == (u32)0x1) {
                        r3 = r15;
                        r4 = r27;
                        r5 = 0x22;
                        fn_80239058();
                        r0 = r3 & 0xFF;
                        if (r0 == (u32)0x1) {
                            r3 = *(u32*)(r29 + r30);
                            r4 = r15;
                            r5 = 0x2a;
                            fn_80239984();
                            *(u32*)(r29 + r30) = r3;
                            r3 = r27;
                            fightPokemonGetPokemonPtr();
                            r7 = (0x1 << 16);
                            r5 = r3;
                            r4 = r26;
                            r6 = 0x0;
                            r7 = 0x0;
                            r8 = 0x0;
                            r9 = 0x0;
                            r10 = 0x2a;
                            fn_80239EE8();
                        }
                    } else {
                    if (r0 == (u32)0x3) {
                        r3 = r15;
                        r4 = r27;
                        r14 = 0x0;
                        r5 = 0x8;
                        fn_80239058();
                        r0 = r3 & 0xFF;
                        if (r0 == (u32)0x1) {
                            r14 = 0x1;
                        }
                        r3 = r15;
                        r4 = r27;
                        r5 = 0x8;
                        fn_80238E30();
                        r0 = r3 & 0xFF;
                        if (r0 != (u32)0x1) {
                            r3 = r15;
                            r4 = r27;
                            r5 = 0x5;
                            fn_80238E30();
                            r0 = r3 & 0xFF;
                            if (r0 != (u32)0x1) {
                                r3 = r15;
                                r4 = r27;
                                r5 = 0x4;
                                fn_80238E30();
                                r0 = r3 & 0xFF;
                                if (r0 == (u32)0x1) {
                        }
                            }
                            r14 = 0x1;
                                }
                        r0 = r14 & 0xFF;
                        if (r0 == (u32)0x1) {
                            r3 = *(u32*)(r29 + r30);
                            r4 = r15;
                            r5 = 0x2b;
                            fn_80239984();
                            *(u32*)(r29 + r30) = r3;
                            r3 = r27;
                            fightPokemonGetPokemonPtr();
                            r7 = (0x1 << 16);
                            r5 = r3;
                            r4 = r26;
                            r6 = 0x0;
                            r7 = 0x0;
                            r8 = 0x0;
                            r9 = 0x0;
                            r10 = 0x2b;
                            fn_80239EE8();
                        }
                        goto L_8024F710;
                    }
                    }
                    if (r0 == (u32)0x4) {
                        r3 = r15;
                        r4 = r27;
                        r5 = 0xf;
                        fn_80238E30();
                        r0 = r3 & 0xFF;
                        if (r0 == (u32)0x1) {
                            r3 = *(u32*)(r29 + r30);
                            r4 = r15;
                            r5 = 0x2c;
                            fn_80239984();
                            *(u32*)(r29 + r30) = r3;
                            r3 = r27;
                            fightPokemonGetPokemonPtr();
                            r7 = (0x1 << 16);
                            r5 = r3;
                            r4 = r26;
                            r6 = 0x0;
                            r7 = 0x0;
                            r8 = 0x0;
                            r9 = 0x0;
                            r10 = 0x2c;
                            fn_80239EE8();
                }
                    }
                }
            L_8024F710:
                fn_8000815C();
                r0 = r3 & 0xFF;
                if (r0 == (u32)0x1) {
                    fn_800E0C54();
                    r0 = *(u32*)(sp + 0x7F4);
                    r5 = r3 & 0xFFFF;
                    r3 = *(u32*)(r29 + r30);
                    r4 = (s32)r5 / (s32)r0;
                    r0 = r4 * r0;
                    r4 = r5 - r0;
                    r0 = *(u8*)(sp + 0x7F0);
                    r14 = r4 - r0;
                    r4 = r14;
                    fightTrainerAiAddValue();
                    *(u32*)(r29 + r30) = r3;
                    r3 = r27;
                    fightPokemonGetPokemonPtr();
                    r0 = 0x0;
                    r5 = (0x1 << 16);
                    *(u32*)(sp + 0x8) = r0;
                    r0 = 0x225;
                    r7 = r3;
                    r6 = r26;
                    *(u32*)(sp + 0xC) = r0;
                    r8 = 0x0;
                    r9 = 0x0;
                    r10 = 0x0;
                    fn_8023A118();
                }
                r3 = r27;
                fightPokemonGetPokemonPtr();
                r0 = 0x0;
                r5 = (0x1 << 16);
                *(u32*)(sp + 0x8) = r0;
                r0 = 0x226;
                r7 = r3;
                r6 = r26;
                *(u32*)(sp + 0xC) = r0;
                r0 = *(u32*)(r29 + r30);
                r8 = 0x0;
                r9 = 0x0;
                r10 = 0x0;
                *(u32*)(sp + 0x10) = r0;
                fn_8023A118();
            }
        }
        r28 = r28 + 0x1;

    }
    r4 = r20;
    r3 = (u32)sp + 0x68;
    r5 = 0x1;
    fn_802397B8();
    if ((s32)r3 < (s32)0x0) {
        r3 = -0x1;
        return;
    }
    r14 = r3 << 2;
    r3 = (u32)sp + 0x98;
    r15 = *(u32*)(r3 + r14);
    if (r15 == (u32)0x0) {
        r3 = -0x1;
        return;
    }
    r4 = r15;
    r3 = 0x0;
    fightFloorGetFightPokemonPtrToFightTrainerPtr();
    r16 = r3;
    r3 = r15;
    fightPokemonGetPokemonPtr();
    r0 = 0x0;
    r4 = (u32)sp + 0x68;
    *(u32*)(sp + 0x8) = r0;
    r0 = 0x228;
    r5 = (0x1 << 16);
    r7 = r3;
    *(u32*)(sp + 0xC) = r0;
    r6 = r16;
    r8 = 0x0;
    r0 = *(u32*)(r4 + r14);
    r9 = 0x0;
    *(u32*)(sp + 0x10) = r0;
    r10 = 0x0;
    fn_8023A118();
    r3 = r15;
    r4 = 0x0;
    r5 = 0xce;
    r6 = 0x0;
    ((void(*)(void))pokemonGetStatus)();
    r3 = (s16)r3;

    return;
}

/* Address: 0x8024E578 | Size: 0x118 (280 bytes) */
u32 fightTrainerAiSelectFightActionIrekae(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u8 lbl_80375D30[];
    int new_var2;
    extern u32 fightFloorGetFightOutPokemonPtrToFightTrainerPtr(u32, u32);
    extern u32 fightTrainerGetStatus(u32, u32, u32, u32);
    extern void fightOutPokemonCreateFightAction(u32, u32, u32, u32, void*, s32);
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern void fn_8023A118(u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, s32);
    long long new_var;
    extern s32 fightTrainerAiSelectIrekaeDasuFightPokemon(void*, u32, u32, u32);
    extern s32 fightTrainerAiGetFightPokemonIrekaeModosuValue();
    u32 choice;
    s32 score;
    u32 field;

    fightTrainerGetStatus(0, fightTrainerGetStatus((u32)ctx, 0, 0x43, 0) & 0xffff, 2, 0);
    field = fightFloorGetFightOutPokemonPtrToFightTrainerPtr(0, param1);
    score = fightTrainerAiGetFightPokemonIrekaeModosuValue(ctx, param1, param2);
    if (score <= 0) {
        return 0;
    }
    new_var = !ctx;
    if (new_var) {
    }
    new_var2 = 0;
    fn_8023A118(0xec63, 0xec04, 0xec05, field, fightOutPokemonGetPokemonPtr(param1), 0, new_var2, new_var2, 0,
                (((((0x228 & 0xFFFF) & 0xFFFF) & 0xFFFF) & 0xFFFF) & 0xFFFF) & 0xFFFF, score);
    choice = fightTrainerAiSelectIrekaeDasuFightPokemon(ctx, param2, 1, param1);
    if ((s16)choice < 0) {
        return 0;
    }
    new_var = choice;
    fightOutPokemonCreateFightAction(param1, new_var2, 9, 0, lbl_80375D30, (s16)new_var);
    return 1;
}

#endif /* !FIGHT_TRAINER_AI_IREKAE_EXACT_8024F8B4_ONLY */

/* Address: 0x8024F8B4 | Size: 0x5CC (1484 bytes) */
s32 fightTrainerAiGetFightPokemonIrekaeModosuValue(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern s32 fightTrainerGetStatus(u32, u32, u32, u32);
    extern void* fightFloorGetFightOutPokemonPtrToFightTrainerPtr(u32, u32);
    extern u16 fn_80236E9C(void* ctx, u32 pokemon);
    extern u16 fn_802367CC(void* ctx, u32 pokemon, u16* moves, u32 zero, u32 one);
    extern u16 fightFloorGetFightTrainerFightOutPokemonPtrAry(u32, void*, u32*, u32, u32);
    extern u16 fn_802376EC(void* ctx, u32 pokemon);
    extern u8 fn_8023C530(void* ctx, u32 pokemon, u32 move, u32 target);
    extern u8 fn_80237288(void* ctx, u32 pokemon);
    extern u8 fn_8023943C(void* ctx, u32 move, u32 one);
    extern s32 fn_8023C370(void* ctx, u32 pokemon, u32 move, u32 target, u32 zero);
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern void fn_8023A118(u32, u32, u32, void*, u32, u32, u32, u32, u32, u32, s32);
    extern u8 fn_8023785C(void* ctx, u32 pokemon);
    extern s32 fn_80239984(s32 value, void* ctx, u32 id);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    extern u8 fn_8023753C(void* ctx, u32 pokemon);
    extern u8 fn_80235714(void* ctx, u32 pokemon);
    extern u8 fn_8023565C(void* ctx, u32 pokemon);
    extern s32 fightTrainerAiGetFightOutPokemonIrekaeJoutaiBadJoutaiAddsbuDataId();
    u16 moves[16];
    u32 pokemon[8];
    u8 canHit;
    u16 count;
    u16 moveCount;
    u16 level;
    u16 speed;
    s32 value;
    u16 i;
    u32 move;
    u16 j;
    void* trainer;
    u32 other;
    u8 faster;
    u8 hittable;
    s32 id;

    fightTrainerGetStatus(0, (u16)fightTrainerGetStatus((u32)ctx, 0, 0x43, 0), 2, 0);
    value = 0;
    trainer = fightFloorGetFightOutPokemonPtrToFightTrainerPtr(0, param1);
    level = fn_80236E9C(ctx, param1);
    moveCount = fn_802367CC(ctx, param1, moves, 0, 1);
    count = fightFloorGetFightTrainerFightOutPokemonPtrAry(0, ctx, pokemon, 0, 1);

    faster = 0;
    hittable = 0;
    for (i = 0; i < count; i++) {
        other = pokemon[i];
        if (other == 0) {
            continue;
        }
        speed = fn_802376EC(ctx, other);
        for (j = 0; j < moveCount; j++) {
            move = moves[j];
            if (move == 0 || move == 0x165) {
                continue;
            }
            canHit = fn_8023C530(ctx, param1, move, other);
            if (canHit == 1) {
                hittable = 1;
            }
            if (fn_80237288(ctx, other) == 1) {
                hittable = 1;
            }
            if (fn_8023943C(ctx, move, 1) == 0) {
                continue;
            }
            if (speed >= fn_8023C370(ctx, param1, move, other, 0)) {
                continue;
            }
            if (canHit == 1) {
                faster = 1;
            }
        }
    }

    fn_8023A118(0xEC63, 0xEC04, 0xEC05, trainer, fightOutPokemonGetPokemonPtr(param1),
                0, 0, 0, 0, 0x227, 0);

    if (fn_8023785C(ctx, param1)) {
        value = fn_80239984(value, ctx, 1);
        fn_80239EE8(0xEC63, trainer, fightOutPokemonGetPokemonPtr(param1), 0, 0, 0, 0, 1);
    }
    if (!fn_8023753C(ctx, param1)) {
        value = fn_80239984(value, ctx, 2);
        fn_80239EE8(0xEC63, trainer, fightOutPokemonGetPokemonPtr(param1), 0, 0, 0, 0, 2);
    } else if (fn_8023785C(ctx, param1)) {
        value = fn_80239984(value, ctx, 3);
        fn_80239EE8(0xEC63, trainer, fightOutPokemonGetPokemonPtr(param1), 0, 0, 0, 0, 3);
    }
    if (fn_80235714(ctx, param1) == 1) {
        value = fn_80239984(value, ctx, 4);
        fn_80239EE8(0xEC63, trainer, fightOutPokemonGetPokemonPtr(param1), 0, 0, 0, 0, 4);
    }
    if (fn_8023565C(ctx, param1) == 1) {
        value = fn_80239984(value, ctx, 5);
        fn_80239EE8(0xEC63, trainer, fightOutPokemonGetPokemonPtr(param1), 0, 0, 0, 0, 5);
    }
    if (!hittable) {
        value = fn_80239984(value, ctx, 6);
        fn_80239EE8(0xEC63, trainer, fightOutPokemonGetPokemonPtr(param1), 0, 0, 0, 0, 6);
    }
    if (faster == 1) {
        value = fn_80239984(value, ctx, 7);
        fn_80239EE8(0xEC63, trainer, fightOutPokemonGetPokemonPtr(param1), 0, 0, 0, 0, 7);
    }
    if (fn_8023753C(ctx, param1) == 1) {
        for (i = 0; i < count; i++) {
            if (pokemon[i] == 0) {
                continue;
            }
            if (fn_80236E9C(ctx, pokemon[i]) > level) {
                value = fn_80239984(value, ctx, 8);
                fn_80239EE8(0xEC63, trainer, fightOutPokemonGetPokemonPtr(param1), 0, 0, 0, 0, 8);
            }
        }
    }
    id = fightTrainerAiGetFightOutPokemonIrekaeJoutaiBadJoutaiAddsbuDataId(ctx, param1);
    if ((u16)id != 0) {
        value = fn_80239984(value, ctx, id);
        fn_80239EE8(0xEC63, trainer, fightOutPokemonGetPokemonPtr(param1), 0, 0, 0, 0, id);
    }

    fn_8023A118(0xEC63, 0xEC04, 0xEC05, trainer, fightOutPokemonGetPokemonPtr(param1),
                0, 0, 0, 0, 0x226, value);
    return value;
}

/* Address: 0x8024FE80 | Size: 0x1F0 (496 bytes) */
typedef struct IrekaeTypeTable {
    u16 ids[10];
} IrekaeTypeTable;

s32 fightTrainerAiGetFightPokemonIrekaeDasuTokuseiAddsubDataId(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u8 fn_80235B04(void* ctx, u32 zero, u32 one);
    extern u16 fightFloorGetFightTrainerFightOutPokemonPtrAry(u32, void*, u32*, u32, u32);
    extern u8 fn_80239058(void* ctx, u32 elem, u32 type);
    extern u32 fn_8023715C(void* ctx, u32 pokemon);
    extern u32 fn_80236FFC(void* ctx, u32 pokemon);
    extern u8 fn_80237F74(void* ctx, u32 pokemon, u32 type);
    /* {0x01, 0x02, 0x23, 0x24, 0x2D, 0x32, 0x35, 0x36, 0x3B, 0x46} */
    extern const IrekaeTypeTable lbl_8027A420;
    u8 notFirst = fn_80235B04(ctx, 0, 1);
    IrekaeTypeTable types = lbl_8027A420;
    u32 pokemon[11];
    u16 count;
    u16 i;
    u16 j;
    u8 found;
    u32 hp;
    u32 maxHp;

    count = fightFloorGetFightTrainerFightOutPokemonPtrAry(0, ctx, pokemon, 0, 1);

    if (fn_80239058(ctx, param1, 0x16) == 1) {
        for (i = 0; i < count; i++) {
            hp = fn_8023715C(ctx, pokemon[i]);
            maxHp = fn_80236FFC(ctx, pokemon[i]);
            if ((u16)hp >= (u16)maxHp) {
                return 0x25;
            }
        }
    }

    if (fn_80239058(ctx, param1, 0x24) == 1) {
        for (i = 0; i < count; i++) {
            found = 0;
            for (j = 0; j < 10; j++) {
                if (fn_80237F74(ctx, pokemon[i], types.ids[j]) == 1) {
                    found = 1;
                    break;
                }
            }
            if (found != 1) {
                return 0x26;
            }
        }
    }

    if (notFirst) {
        if (fn_80239058(ctx, param1, 0x4D) == 1) {
            return 0x27;
        }
        if (fn_80239058(ctx, param1, 0xD) == 1) {
            return 0x28;
        }
    }
    return 0;
}

#if !defined(FIGHT_TRAINER_AI_IREKAE_EXACT_8024F8B4_ONLY)
/* Address: 0x80250070 | Size: 0x27C (636 bytes) */
s32 fightTrainerAiGetFightOutPokemonIrekaeJoutaiBadJoutaiAddsbuDataId(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u8 fn_80236BFC();
    extern u8 fn_8023753C();

    if (fn_80236BFC(ctx, param1, 0x9) == 1) {
        return 0x9;
    }
    if (fn_80236BFC(ctx, param1, 0xA) == 1) {
        return 0xA;
    }
    if (fn_80236BFC(ctx, param1, 0x1E) == 1 && fn_8023753C(ctx, param1) == 0) {
        return 0xB;
    }
    if (fn_80236BFC(ctx, param1, 0xE) == 1) {
        return 0xC;
    }
    if (fn_80236BFC(ctx, param1, 0x17) == 1) {
        return 0xD;
    }
    if (fn_80236BFC(ctx, param1, 0x18) == 1) {
        return 0xE;
    }
    if (fn_80236BFC(ctx, param1, 0x19) == 1) {
        return 0xF;
    }
    if (fn_80236BFC(ctx, param1, 0x1B) == 1) {
        return 0x10;
    }
    if (fn_80236BFC(ctx, param1, 0x1C) == 1) {
        return 0x11;
    }
    if (fn_80236BFC(ctx, param1, 0x1D) == 1) {
        return 0x12;
    }
    if (fn_80236BFC(ctx, param1, 0x26) == 1) {
        return 0x13;
    }
    if (fn_80236BFC(ctx, param1, 0x27) == 1) {
        return 0x14;
    }
    if (fn_80236BFC(ctx, param1, 0x28) == 1) {
        return 0x15;
    }
    if (fn_80236BFC(ctx, param1, 0x29) == 1) {
        return 0x16;
    }
    if (fn_80236BFC(ctx, param1, 0x2A) == 1) {
        return 0x17;
    }
    return fn_80236BFC(ctx, param1, 0x30) == 1 ? 0x18 : 0;
}

/* Address: 0x802502EC | Size: 0x694 (1684 bytes) */
s32 fightTrainerAiSelectFightActionItem(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u8 lbl_80375D70[];
    extern s32 fightTrainerGetStatus(u32, u32, u32, u32);
    extern u16 fightTrainerGetTemotiNormalItemDataIdAry(void* ctx, u16* items, u32 max, u32 mode);
    extern u16 fightFloorGetFightTrainerFightOutPokemonPtrAry(u32, void*, u32*, u32, u32);
    extern u16 fightFloorGetFightTrainerFightPokemonPtrAry(u32, void*, u32*, u32, u32);
    extern u8 fightOutPokemonCheckFightOut(u32 pokemon);
    extern u8 fn_80235714(void* ctx, u32 pokemon);
    extern u8 fn_80142984(u32 item);
    extern u8 fightSeqGetItemType(u32 item, u32 pokemon);
    extern s16 itemUse2PokemonSimulation(void* work, u32 zero, void* status, u32 item, u32 flag);
    extern u8 fn_8023753C(void* ctx, u32 pokemon);
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u8 pokemonIsDarkPokemon(u32 pokemon);
    extern s32 fn_80239984(s32 value, void* ctx, u32 id);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    extern u8 fn_80237310(void* ctx, u32 pokemon);
    extern u8 fn_80236C80(void* ctx, u32 pokemon);
    extern u8 fn_8023785C(void* ctx, u32 pokemon);
    extern u8 fightPokemonCheckFightOut(u32 pokemon);
    extern void fn_8023A118(u32, u32, u32, void*, u32, u32, u32, u32, u32, u32, s32);
    extern s32 fightTrainerAiGetValueAryMaxBanme(s32* values, u16 count, u32 one);
    extern u32 fightTargetGetTragetPtrToRelativeHostSideFightTargetId(u32 pokemon, u32 target);
    extern void fightOutPokemonCreateFightActionUseItem(u32, u32, u32, u32, void*, u32, u32, s32, u32);
    u8 work[0xF4];
    s32 values[20];
    u32 fightPokemon[24];
    u16 items[20];
    u32 outPokemon[8];
    u32 other;
    s32 best;
    u16 itemIdx;
    u16 itemCount;
    u16 outCount;
    u16 outIdx;
    u8 usable;
    u32 item;
    u8 flag;
    u16 fightIdx;
    u8 type;
    u16 searchIdx;
    u16 fightCount;
    u16 zeroIdx;

    fightTrainerGetStatus(0, (u16)fightTrainerGetStatus((u32)ctx, 0, 0x43, 0), 2, 0);
    flag = 0;
    itemCount = fightTrainerGetTemotiNormalItemDataIdAry(ctx, items, 0x14, 1);
    if (itemCount == 0) {
        return 0;
    }
    outCount = fightFloorGetFightTrainerFightOutPokemonPtrAry(0, ctx, outPokemon, 1, 1);
    fightCount = fightFloorGetFightTrainerFightPokemonPtrAry(0, ctx, fightPokemon, 1, 1);

    for (outIdx = 0; outIdx < outCount; outIdx++) {
        other = outPokemon[outIdx];
        if (other == 0) {
            continue;
        }
        if (!fightOutPokemonCheckFightOut(other)) {
            continue;
        }
        if (fn_80235714(ctx, other) == 1) {
            flag = 0;
        }
    }

    for (zeroIdx = 0; zeroIdx < 20; zeroIdx++) {
        values[zeroIdx] = 0;
    }

    for (itemIdx = 0; itemIdx < itemCount; itemIdx++) {
        item = items[itemIdx];
        if (item == 0) {
            continue;
        }
        if (!fn_80142984(item)) {
            continue;
        }
        type = fightSeqGetItemType(item, param1);
        if (type == 7) {
            continue;
        }
        usable = itemUse2PokemonSimulation(work, 0, pokemonGetStatus(param1, 0, 0xD5, 0), item, 0) > 0;
        if (!usable) {
            continue;
        }

        if (type == 2 || type == 1) {
            if (fn_8023753C(ctx, param1) == 1 &&
                !pokemonIsDarkPokemon(fightOutPokemonGetPokemonPtr(param1))) {
                values[itemIdx] = fn_80239984(values[itemIdx], ctx, 0x2E);
                fn_80239EE8(0xEC65, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, 0, item, 0x2E);
            }
        }
        if (type == 3 || type == 1) {
            if (!fn_80237310(ctx, param1) &&
                !pokemonIsDarkPokemon(fightOutPokemonGetPokemonPtr(param1))) {
                values[itemIdx] = fn_80239984(values[itemIdx], ctx, 0x2F);
                fn_80239EE8(0xEC65, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, 0, item, 0x2F);
            }
        }
        if (type == 5 && !fn_80235714(ctx, param1)) {
            values[itemIdx] = fn_80239984(values[itemIdx], ctx, 0x30);
            fn_80239EE8(0xEC65, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, 0, item, 0x30);
        }
        if (type == 4 && fn_80236C80(ctx, param1) == 2) {
            values[itemIdx] = fn_80239984(values[itemIdx], ctx, 0x31);
            fn_80239EE8(0xEC65, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, 0, item, 0x31);
        }
        if (type == 6 && flag == 1) {
            values[itemIdx] = fn_80239984(values[itemIdx], ctx, 0x32);
            fn_80239EE8(0xEC65, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, 0, item, 0x32);
        }
        if (fn_8023785C(ctx, param1) != 2 && fn_8023785C(ctx, param1) != 3) {
            for (fightIdx = 0; fightIdx < fightCount; fightIdx++) {
                if ((u32)pokemonGetStatus(param1, 0, 0xD5, 0) == fightPokemon[fightIdx]) {
                    continue;
                }
                if (fn_8023785C(ctx, param1) == 2 || fn_8023785C(ctx, param1) == 3) {
                    if (fightPokemonCheckFightOut(fightPokemon[fightIdx]) == 1) {
                        values[itemIdx] = fn_80239984(values[itemIdx], ctx, 0x33);
                        fn_80239EE8(0xEC65, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, 0, item, 0x33);
                        break;
                    }
                }
            }
        }
        fn_8023A118(0xEC65, 0xEC46, 0xF238, ctx, fightOutPokemonGetPokemonPtr(param1),
                    0, 0, 0, item, 0x226, values[itemIdx]);
    }

    for (searchIdx = 0; searchIdx < itemCount; searchIdx++) {
        if (values[searchIdx] > 0) {
            break;
        }
    }
    if (searchIdx >= itemCount) {
        return 0;
    }
    best = fightTrainerAiGetValueAryMaxBanme(values, itemCount, 1);
    if (best < 0) {
        return 0;
    }
    item = items[searchIdx];
    if (item == 0) {
        return 0;
    }
    fn_8023A118(0xEC65, 0xEC46, 0xF238, ctx, fightOutPokemonGetPokemonPtr(param1),
                0, 0, 0, item, 0x228, values[best]);
    fightOutPokemonCreateFightActionUseItem(param1, 0, 0x12, 0, lbl_80375D70, item,
        fightTargetGetTragetPtrToRelativeHostSideFightTargetId(param1, param2), -1, 0);
    return 1;
}

#endif /* !FIGHT_TRAINER_AI_IREKAE_EXACT_8024F8B4_ONLY */
