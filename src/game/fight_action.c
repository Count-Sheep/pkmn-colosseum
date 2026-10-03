/**
 * @file fight_action.c
 * @brief fightAction section -- split from colosseum_event.c (the fight
 *        engine bucket, 0x80202810-0x80211A00), address range
 *        0x8020AE30-0x8020D968, 68 fns.
 *
 * Per-turn action dispatch: fightActionDisp / fightActionFlow state
 * machine driving one battle turn (kaisi/heijou/syuuryou/kaijou phases,
 * trainer call/item/nigeru/irekae sub-flows) plus the fightActionBios*
 * accessor farm they read/write. Corresponds to XD's fight.cpp
 * fightAction section (0x80208288-0x8020C018).
 */

#include "game/colosseum.h"
#include "game/fight_action.h"
#include "game/trainer.h"
#include "game/pokemon.h"

typedef struct ColosseumEventRow6 {
    u8 mode;
    u8 field_01;
    u16 eventIndex;
    u16 nextIndex;
} ColosseumEventRow6;

typedef struct ColosseumEventSubRow {
    u8 valueMode;
    u8 scaleMode;
    s16 scaleNumerator;
    s16 scaleDenominator;
    u16 minValue;
    u16 maxValue;
} ColosseumEventSubRow;

typedef struct ColosseumEventPairRow {
    u8 resultFuncId;
    u8 field_01;
    u16 firstLinkIndex;
    ColosseumEventSubRow slots[2];
} ColosseumEventPairRow;

/* =========================================================================
 * External declarations
 * ========================================================================= */

extern void* pokemonGetStatus();
extern u32   pokemonSetStatus();
extern void  pokemonGrowBasisStatus();
extern u32   itemGetStatus();
extern void  fn_80119ED0(void);
extern void  fn_80121ADC(void);
extern void  fn_8011B67C(void);
extern void  pokemonGetSoubiItemDataId(void);
extern void  wazaGetStatus(void);

/* SDA table pointers for event data arrays */
extern u32 lbl_80478D38;   /* Event table count */
extern ColosseumEventRow6 lbl_80478D30[]; /* Event table base (6 bytes per entry) */
extern u32 lbl_80478D28; /* Pair-row table count */
extern ColosseumEventPairRow lbl_80375A08[]; /* 0x18-byte pair rows */

#if !defined(PR409_FIGHT_ACTION_SPLIT) || defined(PR409_FIGHT_ACTION_AED0_D784)

/* 0x8020AED0 | size: 0x60 */
#pragma push
#pragma peephole on
u32 fightActionFlowWazaKiaipantiPre(void* ctx) {
    extern void fightFloorSetStatus();
    extern void fn_80211B94();
    fightFloorSetStatus(
        0, 0, 0x36, 0,
        fightActionBiosGetActorFightTargetPtr((FightAction*)ctx));
    fn_80211B94(ctx, fightActionBiosGetBuffDataPtr((FightAction*)ctx), 0);
    return 1;
}
#pragma pop

u32 _fightActionFlowTenkouInitSubGetSeqFightOutPokemonPtr__FPvUsPv(void* fightOutPokemon, u16 index, void** outFightOutPokemon);

/* Address: 0x8020AF30 | Size: 0xc4 */
u32 fightActionFlowTenkouInit(void* ctx)
{
    extern u32 tenkouDataBiosGetFightInitMsgId();
    extern void fightFloorLoopValidFightOutPokemon();
    extern s8 fightFloorGetNowTenkouDataId();
    extern void fightFloorSetStatus();
    extern void fightKoukaDoFightKoukaJoukenAndKouka();
    extern void fn_80211B94();
    u8 tenkouDataId;
    void* seqPokemon;
    u32 msgId;

    fightKoukaDoFightKoukaJoukenAndKouka(0, 1);
    tenkouDataId = fightFloorGetNowTenkouDataId(0, 0);
    seqPokemon = NULL;
    fightFloorLoopValidFightOutPokemon(
        0, _fightActionFlowTenkouInitSubGetSeqFightOutPokemonPtr__FPvUsPv, &seqPokemon, 0);
    if (tenkouDataId != 0) {
        fightFloorSetStatus(0, 0, 0x36, 0, seqPokemon);
        msgId = tenkouDataBiosGetFightInitMsgId(tenkouDataId);
        fightFloorSetStatus(0, 0, 0x50, 0, msgId);
        fn_80211B94(ctx, fightActionBiosGetBuffDataPtr((FightAction*)ctx), 0);
    }
    return 1;
}

/* Address: 0x8020AFF4 | Size: 0x5c */
u32 _fightActionFlowTenkouInitSubGetSeqFightOutPokemonPtr__FPvUsPv(void* fightOutPokemon, u16 index, void** outFightOutPokemon)
{
    if (pokemonGetStatus(fightOutPokemon, 0, 0xEE, 0) != NULL) {
        if (outFightOutPokemon != NULL) {
            *outFightOutPokemon = fightOutPokemon;
        }
        return 0;
    }
    return 1;
}

/* Address: 0x8020B050 | Size: 0x8 | Pattern: return_constant */
u32 fightActionFlowHeijou(void* action) { return 1; }

/* Address: 0x8020B058 | Size: 0x2d8 | Ghidra import */
u32 fightActionFlowSyuuryouPost(void* action)

{
    extern int fn_8006B0F8();
    extern u8 fn_8006B57C();
    extern u8 pokemonIsDarkPokemon();
    extern u8 pokemonCheckFightOut();
    extern void pokemonEvolutionAll();
    extern u32 pokemonEvolutionCheck();
    extern int savedataGetStatus();
    extern void heroCheckSetMonohiroiAllTemotiPokemon();
    extern u32 heroGetStatus();
    extern void heroBiosCopy();
    extern u16 fn_801EF634();
    extern void fn_801EFFC4();
    extern u8 fightFloorIsGcHeroWin();
    extern int fightFloorGetGcHeroFightTrainerPtr();
    extern int fightFloorGetValidFightSidePtr();
    extern u32 fightFloorGetStatus();
    extern int fightSideGetValidFightTrainerPtr();
    extern void fightTrainerBackFightPokemonToTemotiPokemon();
    extern u8 fightTrainerCheckCanGetExp();
    extern int fightTrainerCheckTemotiPokemonFightEntry();
    extern int fightTrainerGetStatus();
    extern u32 fightPokemonCheckFightOut();
  u32 uVar1;
  u16 sVar6;
  int iVar2;
  int iVar3;
  u32 uVar4;
  u8 cVar7;
  int iVar5;
  u16 uVar9;
  u32 uVar8;
  u32 uVar10;
  u16 local_28 [2];
  u8 auStack_24 [8];
  
  uVar1 = fightFloorGetStatus(0,0,0x16,0);
  sVar6 = fn_801EF634();
  if (sVar6 != 1) {
    iVar2 = fightFloorGetGcHeroFightTrainerPtr(0);
    if ((iVar2 != 0) && (iVar3 = fightTrainerGetStatus(iVar2,0,0x44,0), iVar3 != 0)) {
      fightTrainerBackFightPokemonToTemotiPokemon(iVar2,0);
      uVar4 = fn_801EF634();
      cVar7 = fightFloorIsGcHeroWin(0,uVar4);
      if (cVar7 == 1) {
        cVar7 = fightFloorGetStatus(0,0,0x24,0);
        if ((cVar7 == 1) && (cVar7 = fightTrainerCheckCanGetExp(iVar2), cVar7 == 1)) {
          for (uVar9 = 0; uVar9 < 6; uVar9 = uVar9 + 1) {
            uVar4 = heroGetStatus(iVar3,3,uVar9);
            cVar7 = pokemonCheckFightOut();
            if (((((cVar7 != 0) && (cVar7 = pokemonIsDarkPokemon(uVar4), cVar7 != 1)) &&
                 (iVar5 = fightTrainerCheckTemotiPokemonFightEntry(iVar2,uVar4), iVar5 != 0)) &&
                ((cVar7 = fightPokemonCheckFightOut(), cVar7 != 0 &&
                 (cVar7 = (int)pokemonGetStatus(iVar5,0,0xd0,0), cVar7 != 0)))) &&
               (uVar8 = pokemonEvolutionCheck(uVar4,0,0,local_28,auStack_24), (uVar8 & 0xffff) != 0)) {
              pokemonEvolutionAll(uVar4,uVar8,local_28[0],auStack_24,iVar3,1,1,0);
              fn_801EFFC4(10);
            }
          }
        }
        cVar7 = fightFloorGetStatus(0,0,0x30,0);
        if (cVar7 == 1) {
          heroCheckSetMonohiroiAllTemotiPokemon(iVar3);
        }
        fightFloorGetStatus(0,0,0x28,0);
      }
      cVar7 = fightFloorGetStatus(0,0,0x1c,0);
      if ((cVar7 == 1) && (iVar2 = savedataGetStatus(0,2), iVar2 != 0)) {
        heroBiosCopy(iVar2,iVar3);
      }
    }
    cVar7 = fn_8006B57C();
    if (cVar7 == 1) {
      for (uVar8 = 0; (uVar8 & 0xffff) < 2; uVar8 = uVar8 + 1) {
        iVar2 = fightFloorGetValidFightSidePtr(0,uVar8);
        if (iVar2 != 0) {
          for (uVar10 = 0; (uVar10 & 0xffff) < (uVar1 & 0xffff); uVar10 = uVar10 + 1) {
            iVar3 = fightSideGetValidFightTrainerPtr(iVar2,uVar10);
            if (iVar3 != 0) {
              fightTrainerBackFightPokemonToTemotiPokemon(iVar3,0);
              iVar5 = fn_8006B0F8(uVar10 + (uVar8 & 0xffff) * (uVar1 & 0xffff) & 0xff);
              if ((iVar5 != 0) && (iVar3 = fightTrainerGetStatus(iVar3,0,0x44,0), iVar3 != 0)) {
                heroBiosCopy(iVar5);
              }
            }
          }
        }
      }
    }
  }
  return 1;
}

/* Address: 0x8020B330 | Size: 0x3a4 | Ghidra import */
#pragma push
#pragma peephole on
u32 fightActionFlowSyuuryou(void* ctx)
{
    extern u32 fn_800896B8();
    extern u32 fn_800896C0();
    extern void _threadSwitch();
    extern void msgctrlSetValue();
    extern void fn_80165668();
    extern u32 battleCameraIsSimple();
    extern void fn_801DA8C4();
    extern u8 fn_801DA94C();
    extern void fn_801DA9E8();
    void** new_var;
    extern void fn_801DDD28();
    extern void fn_801EF2D4();
    u16 uVar3;
    extern u32 fn_801EF634();
    extern void fn_801EF8F4();
    extern void fightMainWaitFrame();
    extern u32 fightTargetGetPtrAsNowFightType();
    extern u8 fightFloorIsGcHeroWin();
    extern u8 fightFloorGetStatus();
    extern u32 fn_801F8000();
    extern u32 fightTrainerGetNamePtr();
    extern u32 fightTrainerGetStatus();
    extern void fn_80211B94();
    extern u8 lbl_8037880F[];
    extern void fightMenuCloseMsg();
    extern void fightMenuOpenTrainerMsg();
    extern u8 lbl_80378801[];
    extern void fightMenuOpenMsg();
    u16 sVar9;
    u32 uVar1;
    u32 uVar2;
    u32 uVar4;
    u32 iVar5;
    u8 cVar10;
    u32 saved_r27;
    u32 iVar7;
    FightActionData* data;

    data = fightActionBiosGetFightActionDataPtr((FightAction*)ctx);
    sVar9 = fightActionDataBiosGetBuff(data);
    uVar1 = fightTargetGetPtrAsNowFightType(0xb, 0);
    uVar2 = fightTargetGetPtrAsNowFightType(9, uVar1);
    uVar3 = fightTrainerGetStatus(uVar2, 0, 0x43, 0);
    uVar4 = fightTrainerGetStatus(uVar2, 0, 0x4c, 0);
    iVar5 = fightTrainerGetStatus(0, uVar3, 8, 1);
    if (uVar3 == fn_800896B8()) {
        iVar7 = (0, fn_800896C0());
        if (0 == iVar7) {
            iVar7 = 0;
        } else {
            msgctrlSetValue(0x24, iVar7);
            iVar7 = 0x7531;
        }
    } else if (((s32) fightTrainerGetStatus(uVar2, 0, 0x4a, 0)) == 0) {
        iVar7 = fightTrainerGetStatus(0, uVar3, 8, 2);
    } else {
        iVar7 = fightTrainerGetStatus(0, uVar3, 8, 3);
        if (iVar7 == 0) {
            iVar7 = fightTrainerGetStatus(0, uVar3, 8, 2);
        }
    }
    msgctrlSetValue(0x22, fn_801F8000(uVar2));
    msgctrlSetValue(0x23, fightTrainerGetNamePtr(uVar2));
    msgctrlSetValue(0x13, fightTrainerGetNamePtr(uVar1));
    msgctrlSetValue(0x25, fightTrainerGetNamePtr(uVar2));
    cVar10 = fightFloorGetStatus(0, 0, 0x33, 0);
    if (cVar10 == 1) {
        if (sVar9 == 2) {
            if (iVar7 != 0) {
                fn_801DDD28(uVar4, 0x5a, 4, 0);
                saved_r27 = battleCameraIsSimple();
            }
            fn_80165668(0x3f5, 0, 0xff);
            msgctrlSetValue(0x5d, 0);
            fightMenuOpenMsg(0x766c);
            fightMenuCloseMsg();
            if (iVar7 != 0) {
                fn_801DA9E8(uVar4, 0x5a, 4);
                fightMenuOpenTrainerMsg(iVar7);
                while (1) {
                    cVar10 = fn_801DA94C(uVar4, 0x5a, 4);
                    if (cVar10 == 0) {
                        break;
                    }
                    _threadSwitch();
                }

                fn_801EF8F4(saved_r27);
                fightMenuCloseMsg();
                fn_801DA8C4(uVar4, 0x5a, 4);
            }
        } else if (sVar9 == 3) {
            if (iVar5 != 0) {
                fn_801DDD28(uVar4, 0x59, 4, 0);
                saved_r27 = battleCameraIsSimple();
            }
            fightMenuOpenMsg(0x7547);
            fightMenuCloseMsg();
            if (iVar5 != 0) {
                fn_801DA9E8(uVar4, 0x59, 4);
                fightMenuOpenTrainerMsg(iVar5);
                while (1) {
                    cVar10 = fn_801DA94C(uVar4, 0x59, 4);
                    if (cVar10 == 0) {
                        break;
                    }
                    _threadSwitch();
                }

                fn_801EF8F4(saved_r27);
                fightMenuCloseMsg();
                fn_801DA8C4(uVar4, 0x59, 4);
            }
            fightMenuOpenMsg(0x7548);
            fightMenuCloseMsg();
        } else if ((1 < ((u16) (sVar9 - 4U))) && ((sVar9 == 7) || (sVar9 == 6))) {
            fightMenuOpenMsg(0x7640);
            fightMainWaitFrame(0x40);
            fightMenuCloseMsg();
        }
    }
    uVar1 = fn_801EF634();
    cVar10 = fightFloorIsGcHeroWin(0, uVar1);
    if ((cVar10 == 1) && ((cVar10 = fightFloorGetStatus(0, 0, 0x25, 0), cVar10 == 1))) {
        fn_80211B94(ctx, (u32) lbl_80378801, 0);
        new_var = &ctx;
        fn_80211B94(*new_var, (u32) lbl_8037880F, 0);
    }
    fn_801EF2D4();
    return 1;
}
#pragma pop

/* Address: 0x8020B6D4 | Size: 0x58 | Ghidra import */
#pragma push
#pragma peephole on
u32 fightActionFlowSyuuryouPre(void* action)
{
    extern void fn_8016597C();
    extern void fightMainWaitFrame();
    extern u32 fightFloorGetStatus();
    u32 uVar1;

    uVar1 = fightFloorGetStatus(0, 0, 0x12, 0);
    if (uVar1 != 0) {
        fn_8016597C(1, 1000, 1000, 0xff);
        fightMainWaitFrame(0x3c);
    }
    return 1;
}
#pragma pop

/* Address: 0x8020B72C | Size: 0x1e4 | Ghidra import */
#pragma push
#pragma peephole on
u32 fightActionFlowFightOutPokemonOutWaza(void* ctx)
{
    extern void wazaSetStatus(void*, s32, s32, s32, u16);
    extern u32 wazaGetStatus();
    extern u32 fightTargetGetRelativeHostSideFightTargetIdToTragetPtr();
    extern void fightFloorSetStatus();
    extern u32 fightFloorGetStatus();
    extern u16 fightOutPokemonGetMotoWazaDataId();
    extern u32 fightOutPokemonGetPokemonPtr();
    extern void fightWazaSetUseWazaStatus(void*, u16);
    extern u8 fightWazaCheckValid();
    extern void fn_802128D0(void*, u16);
    extern u32 fn_8022B2CC();
    u16 targetId;
    void* actorTarget;
    void* waza;
    void* target;
    void* pokemon;
    u16 motoWaza;
    s8 slot;
    void* result;

    targetId = fightFloorGetStatus(0, 0, 0x14, 0);
    actorTarget = fightActionBiosGetActorFightTargetPtr((FightAction*)ctx);
    waza = pokemonGetStatus(actorTarget, 0, 0xd9, 0);
    if (fightWazaCheckValid(waza) == 0) {
        return 0;
    }
    target = (void*)fightTargetGetRelativeHostSideFightTargetIdToTragetPtr(
        (u16)wazaGetStatus(waza, 0, 0x29, 0), targetId);
    fightFloorSetStatus(0, 0, 0x36, 0, actorTarget);
    fightFloorSetStatus(0, 0, 0x42, 0, target);
    pokemon = (void*)fightOutPokemonGetPokemonPtr(actorTarget);
    waza = pokemonGetStatus(actorTarget, 0, 0xd9, 0);
    motoWaza = fightOutPokemonGetMotoWazaDataId(actorTarget);
    slot = wazaGetStatus(waza, 0, 0x26, 0);
    if ((u8)wazaGetStatus(waza, 0, 0x32, 0) == 0) {
        if ((u16)motoWaza != (u16)(u32)pokemonGetStatus(pokemon, 0, 0x7f, (u8)slot)) {
            motoWaza = (u16)(u32)pokemonGetStatus(pokemon, 0, 0x7f, (u8)slot);
            wazaSetStatus(waza, 0, 0x27, 0, motoWaza);
            fightWazaSetUseWazaStatus(waza, motoWaza);
            result = (void*)fn_8022B2CC(actorTarget, motoWaza, targetId, 0, 1, 0, -1);
            fightFloorSetStatus(0, 0, 0x43, 0, result);
        }
    }
    fn_802128D0(ctx, motoWaza);
    return 1;
}
#pragma pop

/* 0x8020B910 | size: 0x104 */
#pragma push
#pragma peephole on
u32 fightActionFlowFightTrainerUseItem(void* ctx)
{
    extern u32 fightTargetGetRelativeHostSideFightTargetIdToTragetPtr();
    extern void fightFloorSetStatus();
    extern u16 fightFloorGetStatus();
    extern void fn_80211E18();
    u32 actionValue;
    void* itemData;
    u16 itemId;
    u32 actorFightTarget;
    u32 partyCount;
    u8 slotType;
    u32 selectedTarget;

    partyCount = fightFloorGetStatus(0, 0, 0x14, 0);
    actionValue = (u32)fightActionBiosGetActorFightTargetPtr((FightAction*)ctx);
    actorFightTarget = actionValue;
    fightFloorSetStatus(0, 0, 0x36, 0, actionValue);
    itemData = pokemonGetStatus((void*)actorFightTarget, 0, 0xE5, 0);
    itemId = (u16)itemGetStatus(itemData, 0, 0x1E, 0);
    slotType = (u8)itemGetStatus(0, itemId, 0x2, 0);
    if (slotType == 1) {
        selectedTarget = (u32)fightTargetGetRelativeHostSideFightTargetIdToTragetPtr(
            (u16)itemGetStatus(itemData, 0, 0x1F, 0), partyCount);
    } else {
        selectedTarget = actorFightTarget;
    }
    fightFloorSetStatus(0, 0, 0x42, 0, selectedTarget);
    actionValue = itemId;
    fn_80211E18(ctx, actionValue);
    return 1;
}
#pragma pop

/* 0x8020BA14 | size: 0x6c */
#pragma push
#pragma peephole on
u32 fightActionFlowFightTrainerCall(void* ctx) {
    extern void fightFloorSetStatus();
    extern void fightFloorGetStatus();
    extern void fn_80212D6C();
    void* d908val;
    fightFloorGetStatus(0, 0, 0x14, 0);
    d908val = fightActionBiosGetActorFightTargetPtr((FightAction*)ctx);
    fightFloorSetStatus(0, 0, 0x36, 0, d908val);
    fn_80212D6C(ctx);
    return 1;
}
#pragma pop

/* Address: 0x8020BA80 | Size: 0x78 | Ghidra import */
#pragma push
#pragma peephole on
u32 fightActionFlowFightOutPokemonIrekae(void* ctx)
{
    extern void fightFloorSetStatus();
    extern void fn_80213158();
  void* uVar1;
  short sVar2;

  uVar1 = fightActionBiosGetActorFightTargetPtr((FightAction*)ctx);
  fightFloorSetStatus(0,0,0x45,0,uVar1);
  sVar2 = fightActionBiosGetBuffDataId((FightAction*)ctx);
  pokemonSetStatus(uVar1,0,0x121,0,(int)sVar2);
  fn_80213158(ctx);
  return 1;
}
#pragma pop

/* 0x8020BAF8 | size: 0xAC */
u32 fightActionFlowFightNigeru(void* ctx) {
    extern u8 fightTargetIsHostSide();
    extern u8 fightFloorSetFightResultId();
    extern void fightFloorSetStatus();
    extern u32 fightFloorGetStatus();
    extern void fightSeqSpecificationActionCounterInit();
    void* obj;
    u16 tableId;
    u8 result;
    tableId = fightFloorGetStatus(NULL, 0, 0x14, 0);
    obj = fightActionBiosGetActorFightTargetPtr((FightAction*)ctx);
    fightSeqSpecificationActionCounterInit(obj);
    if (fightTargetIsHostSide(obj, tableId) == 1) {
        result = fightFloorSetFightResultId(0, 4);
    } else {
        result = fightFloorSetFightResultId(0, 5);
    }
    if (result == 1) {
        fightFloorSetStatus(0, 0, 0x44, 0, obj);
    }
    return 1;
}

/* Address: 0x8020BBA4 | Size: 0x58 | Ghidra import */
#pragma push
#pragma peephole on
u32 fightActionFlowOneTurnPost(void* ctx) {
    extern u16 fn_801EF634();
    extern void fightMainWaitFrame();
    extern void fightFloorInitFightTarget();
    extern void fightSeqPost();
    u16 sVar1;
    sVar1 = fn_801EF634();
    if (sVar1 != 0) { return 1; }
    fightSeqPost(ctx);
    fightMainWaitFrame(5);
    fightFloorInitFightTarget(0);
    return 1;
}
#pragma pop

/* 0x8020BBFC | size: 0x98 */
#pragma push
#pragma peephole on
u32 fightActionFlowAllFightOutPokemonDoFightAction(void* ctx) {
    extern u16 fn_801EF634();
    extern void fightFloorSortFightOutPokemonPtrAry();
    extern void fightFloorCreateFightOutPokemonPtrAry();
    extern u32 _fightActionFlowAllFightOutPokemonDoFightActionOneLoop__FP11FIGHT_FLOORUc();
    extern void fn_80211A00();
    u32 uVar1;
    u16 sVar3;
    u32 uVar2;
    fightFloorCreateFightOutPokemonPtrAry(0);
    fightFloorSortFightOutPokemonPtrAry(0, 1);
    uVar1 = _fightActionFlowAllFightOutPokemonDoFightActionOneLoop__FP11FIGHT_FLOORUc(0, 0);
    if ((u8)uVar1 != 1) { return uVar1; }
    sVar3 = fn_801EF634();
    if (sVar3 != 0) { return 1; }
    fn_80211A00(ctx);
    uVar2 = _fightActionFlowAllFightOutPokemonDoFightActionOneLoop__FP11FIGHT_FLOORUc(0, 1);
    uVar1 = 1;
    if ((u8)uVar2 != 1) {
        uVar1 = uVar2;
    }
    return uVar1;
}
#pragma pop

/* Address: 0x8020BC94 | Size: 0x1a4 | Ghidra import */
u32 _fightActionFlowAllFightOutPokemonDoFightActionOneLoop__FP11FIGHT_FLOORUc(void* fightFloor, u8 phase)
{
    extern u16 fn_801EF634();
    extern void fightFloorInitFightTarget();
    extern int fightFloorGetStatus();
    extern u8 fightOutPokemonCheckFightOut();
  int iVar3;
  u8 cVar7;
  int iVar4;
  u16 sVar6;
  int iVar5;
  u32 *puVar8;
  u32 *puVar9;
  u32 uVar10;
  u32 local_48[12];
  
  for (uVar10 = 0; (uVar10 & 0xffff) < 8; uVar10++) {
    iVar3 = fightFloorGetStatus(fightFloor,0,0x59,uVar10);
    if (iVar3 != 0) {
      cVar7 = fightOutPokemonCheckFightOut();
      if (cVar7 == 0) {
        pokemonSetStatus(iVar3,0,0x112,0,1);
      }
      else {
        iVar4 = (int)pokemonGetStatus(iVar3,0,0xfe,0);
        if (iVar4 == 0) {
          pokemonSetStatus(iVar3,0,0x112,0,1);
        }
        else {
          cVar7 = fightActionCheckValid((void*)iVar4);
          if (cVar7 == 0) {
            pokemonSetStatus(iVar3,0,0x112,0,1);
          }
          else {
            if (phase == 0) {
              sVar6 = fightActionGetKindDataId((void*)iVar4);
              if (sVar6 != 8) {
                continue;
              }
            } else {
              sVar6 = fightActionGetKindDataId((void*)iVar4);
              if (sVar6 == 8) {
                continue;
              }
            }
            iVar5 = (int)pokemonGetStatus(iVar3,0,0x112,0);
            if (iVar5 != 1) {
              pokemonSetStatus(iVar3,0,0x112,0,1);
              puVar9 = local_48;
              puVar8 = (u32 *)iVar4;
              iVar3 = 6;
              do {
                puVar9[0] = puVar8[0];
                puVar9[1] = puVar8[1];
                puVar8 += 2;
                puVar9 += 2;
                iVar3 = iVar3 + -1;
              } while (iVar3 != 0);
              fightActionFlowFifo(local_48);
              if (phase != 0) {
                fightFloorInitFightTarget(0);
                sVar6 = fn_801EF634();
                if (sVar6 != 0) {
                  return 1;
                }
              }
            }
          }
        }
      }
    }
  }
  return 1;
}

/* 0x8020BE38 | size: 0x108 */
u32 fightActionFlowAllFightTrainerSelectFightAction(void* action) {
    extern u32 fn_80008174();
    extern void fightFloorLoopValidFightTrainer();
    extern void* fightFloorGetValidFightSidePtr();
    extern u32 fightFloorGetStatus();
    extern void* fightSideGetValidFightTrainerPtr();
    extern u8 fightMenuFightTrainerGcHeroOpenMenu();
    extern u32 _fightActionFlowFightTrainerSelectFightAction__FPvUsPv();
    u32 checkResult;
    void* side;
    u16 slotCount;
    u32 partyCount;
    u32 j;
    u32 i;
    void* trainer;

    checkResult = fn_80008174();
    if ((u8)checkResult == 1) {
        partyCount = fightFloorGetStatus(0, 0, 0x14, 0) & 0xFFFF;
        slotCount = fightFloorGetStatus(0, 0, 0x16, 0);
        for (i = 0; (u16)i < 2; i++) {
        retrySide:
            side = fightFloorGetValidFightSidePtr(0, i);
            if (side == NULL) {
                continue;
            }
            for (j = 0; (u16)j < slotCount; j++) {
            retryTrainer:
                trainer = fightSideGetValidFightTrainerPtr(side, j);
                if (trainer == NULL) {
                    continue;
                }
                if (fightMenuFightTrainerGcHeroOpenMenu(trainer, partyCount, checkResult) != 0) {
                    continue;
                }
                if ((u16)i == 0) {
                    goto retryTrainer;
                }
                i--;
                goto retrySide;
            }
        }
    } else {
        fightFloorLoopValidFightTrainer(0, _fightActionFlowFightTrainerSelectFightAction__FPvUsPv, 0, 1);
    }
    return 1;
}

/* 0x8020BF40 | size: 0x60 */
#pragma push
#pragma peephole on
u32 _fightActionFlowFightTrainerSelectFightAction__FPvUsPv(void* ctx, u32 param) {
    extern u16 fn_801EF634();
    extern void fightFloorSetTuusinErrorFightResult();
    extern u8 fightTrainerSelectFightAction();
    if (fn_801EF634() != 0) {
        return 1;
    }
    if (fightTrainerSelectFightAction(ctx, param) == 0) {
        fightFloorSetTuusinErrorFightResult(0);
    }
    return 1;
}
#pragma pop

/* 0x8020BFA0 | size: 0x120 */
#pragma push
#pragma peephole on
u32 fightActionFlowKaisiPost(void* ctx) {
    extern FightActionData lbl_80375CC8[];
    extern u8 lbl_80378AA0[];
    extern u16 fn_800E0C54();
    extern void fn_801DA7AC();
    extern void fightFloorCreateFightPokemonEnemyAryEnemySideAll();
    extern void fightFloorRegistFightTrainerEnemyPokemonFightSideAll();
    extern void fightFloorSetMeetEnemyFightPokemonEnemySideAll();
    extern void fightFloorLoopValidFightOutPokemon();
    extern void fightFloorSortFightOutPokemonPtrAry();
    extern void fightFloorCreateFightOutPokemonPtrAry();
    extern void fightFloorSetStatus();
    extern void fightSeqInit();
    extern void fightSeqFightActionCreateAndFlowFifo(
        void* motoAction, void* actorTarget, u32 kind, u32 buff,
        FightActionData* actionData, void* buffData);
    extern void fn_8022E1C4();
    extern void fn_8022E314();
    extern s32 _fightActionFlowKaisiPostSubFightOutPokemonSoubiItemCheckAppear__FPvUsPv();
    extern s32 _fightActionFlowKaisiPostSubFightOutPokemonTokuseiCheckAppear__FPvUsPv();
    extern s32 _fightActionFlowKaisiPostSubFightOutPokemonDarkCheckAppear__FPvUsPv();
    u8 localBuf[0x10];

    fightFloorCreateFightOutPokemonPtrAry(0);
    fightFloorSortFightOutPokemonPtrAry(0, 0);
    fightSeqInit();
    localBuf[0] = 0;
    fightFloorLoopValidFightOutPokemon(0, (u32)_fightActionFlowKaisiPostSubFightOutPokemonDarkCheckAppear__FPvUsPv, &localBuf[0], 0);
    fightSeqFightActionCreateAndFlowFifo(
        fightActionBiosGetFightActionDataPtr((FightAction*)ctx), 0, 6, 0,
        lbl_80375CC8, lbl_80378AA0);
    fightFloorLoopValidFightOutPokemon(0, (u32)_fightActionFlowKaisiPostSubFightOutPokemonTokuseiCheckAppear__FPvUsPv, 0, 1);
    fn_8022E314(1);
    fn_8022E1C4();
    fightFloorLoopValidFightOutPokemon(0, (u32)_fightActionFlowKaisiPostSubFightOutPokemonSoubiItemCheckAppear__FPvUsPv, 0, 1);
    localBuf[0] = 1;
    fightFloorLoopValidFightOutPokemon(0, _fightActionFlowKaisiPostSubFightOutPokemonDarkCheckAppear__FPvUsPv, &localBuf[0], 0);
    fightFloorSetMeetEnemyFightPokemonEnemySideAll(0);
    fightFloorRegistFightTrainerEnemyPokemonFightSideAll(0);
    fightFloorCreateFightPokemonEnemyAryEnemySideAll(0);
    fightFloorSetStatus(0, 0, 0x5B, 0, fn_800E0C54());
    fn_801DA7AC();
    return 1;
}
#pragma pop

/* 0x8020C0C0 | size: 0x24 | small */
/* _fightActionFlowKaisiPostSubFightOutPokemonSoubiItemCheckAppear__FPvUsPv | Size: 0x24 | Call fn_8022D084 and return 1 */
#pragma push
#pragma peephole on
s32 _fightActionFlowKaisiPostSubFightOutPokemonSoubiItemCheckAppear__FPvUsPv(void) {
    extern void fn_8022D084(void);
    fn_8022D084();
    return 1;
}
#pragma pop

/* 0x8020C0E4 | size: 0x24 | small */
/* _fightActionFlowKaisiPostSubFightOutPokemonTokuseiCheckAppear__FPvUsPv | Size: 0x24 | Call fn_8022E410 and return 1 */
#pragma push
#pragma peephole on
s32 _fightActionFlowKaisiPostSubFightOutPokemonTokuseiCheckAppear__FPvUsPv(void) {
    extern void fn_8022E410(void);
    fn_8022E410();
    return 1;
}
#pragma pop

/* _fightActionFlowKaisiPostSubFightOutPokemonDarkCheckAppear__FPvUsPv | Size: 0x54 | Apply effect with optional data parameter */
s32 _fightActionFlowKaisiPostSubFightOutPokemonDarkCheckAppear__FPvUsPv(void* ctx, u32 unused, u8* data) {
    extern void fn_8022E6F0(void* ctx, u32 value);
    if (data != NULL) {
        fn_8022E6F0(ctx, data[0]);
    } else {
        fn_8022E6F0(ctx, 0);
        fn_8022E6F0(ctx, 1);
    }
    return 1;
}

/* Address: 0x8020C15C | Size: 0x6e4 | Ghidra import */
u32 fightActionFlowKaisiPre(void* action)

{
    extern void _threadSwitch();
    extern void menuGetKeyInfo();
    extern void msgctrlSetValue();
    extern void fn_80165A20();
    extern s8 fadeCheck();
    extern void fadeSet();
    extern void fn_801DA4E8();
    extern void fn_801DA8C4();
    extern s8 fn_801DA94C();
    extern void fn_801DA9B4();
    extern void fn_801DA9E8();
    extern void fn_801EF7C4();
    extern u32 fightTargetGetPtrAsNowFightType();
    extern s8 fn_801F1888();
    extern u32 fightFloorGetStatus();
    extern u32 fn_801F8000();
    extern u32 fightTrainerGetNamePtr();
    extern u32 fightTrainerGetStatus();
    extern u32 fightEncountDataBiosGetSyoukaiWzxDataId();
    extern void* fightEncountDataBiosGetPtr();
    extern void fightMenuCloseMsg();
    extern void fightMenuOpenTrainerMsg();
    extern u16 lbl_8047B5F8;
    extern f32 lbl_8047E520;
  u32 bVar1;
  volatile u8 bVar2;

  u16 uVar12;
  u32 uVar3;
  u32 uVar4;
  u32 uVar5;
  u16 uVar13;
  u32 uVar6;
  u32 uVar7;
  u32 uVar8;
  int iVar9;
  u8 cVar14;
  int iVar10;
  u32 uVar11;

  u16 local_74 [14];
  u16 local_58 [14];
  u16 local_3c [14];
  
  bVar2 = 0;
  uVar12 = fightFloorGetStatus(0,0,0xe,0);
  uVar3 = fightTargetGetPtrAsNowFightType(0xb,0);
  uVar4 = fightTrainerGetStatus(uVar3,0,0x4c,0);
  uVar3 = fightTargetGetPtrAsNowFightType(9,uVar3);
  uVar5 = fightTrainerGetStatus(uVar3,0,0x4c,0);
  uVar13 = fightTrainerGetStatus(uVar3,0,0x43,0);
  uVar6 = fightFloorGetStatus(0,0,0x10,0);
  uVar12 = fightFloorGetStatus(0,uVar12,0xd,0);
  fightEncountDataBiosGetPtr(uVar12);
  uVar7 = fightEncountDataBiosGetSyoukaiWzxDataId();
  uVar8 = fightTrainerGetStatus(uVar3,uVar13,7,0);
  if (uVar8 == 0) {
    uVar8 = 0x5f;
  }
  iVar9 = fightTrainerGetStatus(0,uVar13,8,0);
  cVar14 = fn_801F1888(0);
  if (cVar14 == 0) {
    if (uVar6 != 0) {
      if (uVar7 != 0) {
        fn_801DA9E8(uVar5,uVar7 & 0xffff,4);
      }
      fn_801DA9E8(uVar5,uVar6 & 0xffff,4);
      while (1) {
        if (0) {
          bVar1 = 0;
        }
        else {
          menuGetKeyInfo(local_3c,1);
          cVar14 = fadeCheck(0);
          bVar1 = bVar2;
          if ((cVar14 == 0) && ((local_3c[0] & 0x20) != 0)) {
            bVar1 = 1;
            bVar2 = bVar1;
          }
        }
        if (bVar1) goto LAB_00209430;
        cVar14 = fn_801DA94C(uVar5,uVar6 & 0xffff,4);
        if (cVar14 == 0) break;
        _threadSwitch();
      }
      cVar14 = fightFloorGetStatus(0,0,0x33,0);
      if (cVar14 == 1) {
        fn_801DA9E8(uVar5,lbl_8047B5F8,4);
        while (1) {
          if (0) {
            bVar1 = 0;
          }
          else {
            menuGetKeyInfo(local_58,1);
            cVar14 = fadeCheck(0);
            bVar1 = bVar2;
            if ((cVar14 == 0) && ((local_58[0] & 0x20) != 0)) {
              bVar1 = 1;
              bVar2 = bVar1;
            }
          }
          if (bVar1) goto LAB_00209430;
          cVar14 = fn_801DA94C(uVar5,lbl_8047B5F8,4);
          if (cVar14 == 0) break;
          _threadSwitch();
        }
      }
      if (uVar7 != 0) {
        while (1) {
          if (0) {
            bVar1 = 0;
          }
          else {
            menuGetKeyInfo(local_74,1);
            cVar14 = fadeCheck(0);
            bVar1 = bVar2;
            if ((cVar14 == 0) && ((local_74[0] & 0x20) != 0)) {
              bVar1 = 1;
              bVar2 = bVar1;
            }
          }
          if ((bVar1) || (cVar14 = fn_801DA94C(uVar5,uVar7 & 0xffff,4), cVar14 == 0)) break;
          _threadSwitch();
        }
      }
    }
LAB_00209430:
    if (bVar2) {
      fadeSet((double)lbl_8047E520,3);
      fadeCheck(1);
      if (uVar7 != 0) {
        fn_801DA9B4(uVar5,uVar7 & 0xffff,4);
      }
      fn_801DA9B4(uVar5,uVar6 & 0xffff,4);
      cVar14 = fightFloorGetStatus(0,0,0x33,0);
      if (cVar14 == 1) {
        fn_801DA9B4(uVar5,lbl_8047B5F8,4);
      }
    }
    if (iVar9 != 0) {
      fn_801DA9E8(uVar5,0x5f,4);
      if (bVar2) {
        fadeSet((double)lbl_8047E520,2);
        bVar2 = 0;
      }
      fightMenuOpenTrainerMsg(iVar9);
      while (1) {
        cVar14 = fn_801DA94C(uVar5,0x5f,4);
        if (cVar14 == 0) break;
        _threadSwitch();
      }
      fightMenuCloseMsg();
    }
    iVar10 = fightFloorGetStatus(0,0,0x11,0);
    if (iVar10 != 0) {
      fn_80165A20(iVar10,0,0xff);
    }
    cVar14 = fightFloorGetStatus(0,0,0x33,0);
    if (cVar14 == 1) {
      uVar4 = fn_801F8000(uVar3);
      msgctrlSetValue(0x22,uVar4);
      uVar3 = fightTrainerGetNamePtr(uVar3);
      msgctrlSetValue(0x23,uVar3);
      fn_801DA9E8(uVar5,uVar8 & 0xffff,4);
      if (bVar2) {
        fadeSet((double)lbl_8047E520,2);
        bVar2 = 0;
      }
      fightMenuOpenTrainerMsg(0x766d);
      while (1) {
        cVar14 = fn_801DA94C(uVar5,uVar8 & 0xffff,4);
        if (cVar14 == 0) break;
        _threadSwitch();
      }
      fightMenuCloseMsg();
    }
    if (bVar2) {
      fadeSet((double)lbl_8047E520,2);
    }
    if (uVar6 != 0) {
      if (uVar7 != 0) {
        fn_801DA8C4(uVar5,uVar7 & 0xffff,4);
      }
      fn_801DA8C4(uVar5,uVar6 & 0xffff,4);
      cVar14 = fightFloorGetStatus(0,0,0x33,0);
      if (cVar14 == 1) {
        fn_801DA8C4(uVar5,lbl_8047B5F8,4);
      }
    }
    if (iVar9 != 0) {
      fn_801DA8C4(uVar5,0x5f,4);
    }
    cVar14 = fightFloorGetStatus(0,0,0x33,0);
    if (cVar14 == 1) {
      fn_801DA8C4(uVar5,uVar8 & 0xffff,4);
    }
  }
  else {
    fn_801EF7C4(0);
    fn_801DA4E8(uVar4,1);
    fn_801DA9E8(uVar4,0x54,4);
    while (1) {
      cVar14 = fn_801DA94C(uVar4,0x54,4);
      if (cVar14 == 0) break;
      _threadSwitch();
    }
    fn_801EF7C4(0);
    fn_801DA4E8(uVar5,1);
    fn_801DA9E8(uVar5,0x55,4);
    cVar14 = fightFloorGetStatus(0,0,0x33,0);
    if (cVar14 == 1) {
      uVar11 = fn_801F8000(uVar3);
      msgctrlSetValue(0x22,uVar11);
      uVar3 = fightTrainerGetNamePtr(uVar3);
      msgctrlSetValue(0x23,uVar3);
      fightMenuOpenTrainerMsg(0x766d);
    }
    while (1) {
      cVar14 = fn_801DA94C(uVar5,0x55,4);
      if (cVar14 == 0) break;
      _threadSwitch();
    }
    cVar14 = fightFloorGetStatus(0,0,0x33,0);
    if (cVar14 == 1) {
      fightMenuCloseMsg();
    }
    fn_801EF7C4(1);
    fn_801DA9E8(uVar4,0x56,4);
    while (1) {
      cVar14 = fn_801DA94C(uVar4,0x56,4);
      if (cVar14 == 0) break;
      _threadSwitch();
    }
    fn_801DA8C4(uVar4,0x54,4);
    fn_801DA8C4(uVar5,0x55,4);
    fn_801DA8C4(uVar4,0x56,4);
    iVar9 = fightFloorGetStatus(0,0,0x11,0);
    if (iVar9 != 0) {
      fn_80165A20(iVar9,0,0xff);
    }
  }
  return 1;
}

void fightActionFlowKaisiPreSubLoad(void)
{
    extern u32 fightTargetGetPtrAsNowFightType(u32 type, u32 relative);
    extern u32 fightTrainerGetStatus(u32 trainer, u32 index, u32 status, u32 subindex);
    extern u32 fightFloorGetStatus(u32 floor, u32 index, u32 status, u32 subindex);
    extern u32 fightEncountDataBiosGetPtr(u16 id);
    extern u32 fightEncountDataBiosGetSyoukaiWzxDataId(void);
    extern u8 fn_801F1888(u32 floor);
    extern void fn_801DDD28(u32 owner, u16 id, u32 type, u32 arg);
    extern u16 fn_800E0C54(void);
    extern u8 lbl_80478D18;
    extern u16 lbl_80375970[];
    extern u16 lbl_8047B5F8;
    u32 secondaryMessage;
    u32 heroTarget;
    u32 introMessage;
    u32 resource;
    u32 introResource;
    u16 encounterIndex;
    u32 enemyTarget;
    u32 enemyOwner;
    u32 heroOwner;
    u32 trainerIndex;

    encounterIndex = fightFloorGetStatus(0, 0, 0xE, 0);
    heroTarget = fightTargetGetPtrAsNowFightType(0xB, 0);
    heroOwner = fightTrainerGetStatus(heroTarget, 0, 0x4C, 0);
    enemyTarget = fightTargetGetPtrAsNowFightType(9, heroTarget);
    enemyOwner = fightTrainerGetStatus(enemyTarget, 0, 0x4C, 0);
    trainerIndex = fightTrainerGetStatus(enemyTarget, 0, 0x43, 0) & 0xFFFF;
    introResource = fightFloorGetStatus(0, 0, 0x10, 0);
    encounterIndex = fightFloorGetStatus(0, encounterIndex, 0xD, 0);
    fightEncountDataBiosGetPtr(encounterIndex);
    resource = fightEncountDataBiosGetSyoukaiWzxDataId();
    introMessage = fightTrainerGetStatus(enemyTarget, trainerIndex, 7, 0);
    if (introMessage == 0) {
        introMessage = 0x5F;
    }
    secondaryMessage = fightTrainerGetStatus(0, trainerIndex, 8, 0);

    if (fn_801F1888(0) == 0) {
        if (introResource != 0) {
            if (resource != 0) {
                fn_801DDD28(enemyOwner, resource, 4, 0);
            }
            fn_801DDD28(enemyOwner, (u16)introResource, 4, 0);
            if ((u8)fightFloorGetStatus(0, 0, 0x33, 0) == 1) {
                u16 index = fn_800E0C54() % lbl_80478D18;
                lbl_8047B5F8 = index;
                lbl_8047B5F8 = lbl_80375970[index];
                fn_801DDD28(enemyOwner, lbl_8047B5F8, 4, 0);
            } else {
                lbl_8047B5F8 = 0;
            }
        }
        if (secondaryMessage != 0) {
            fn_801DDD28(enemyOwner, 0x5F, 4, 0);
        }
        if ((u8)fightFloorGetStatus(0, 0, 0x33, 0) == 1) {
            fn_801DDD28(enemyOwner, (u16)introMessage, 4, 0);
        }
    } else {
        fn_801DDD28(heroOwner, 0x54, 4, 0);
        fn_801DDD28(enemyOwner, 0x55, 4, 0);
        fn_801DDD28(heroOwner, 0x56, 4, 0);
    }
}

/* Address: 0x8020CA98 | Size: 0x548 | Ghidra import */
u32 fightActionFlowKaisiNyuujouPokemon(void* action)

{
    extern void fn_8010AE2C();
    extern u32 pokemonCreateSequence();
    extern void msgctrlSetValue();
    extern void battleGridUpdate();
    extern void battleGridAddPokemon();
    extern u32 fightTargetGetPtr(u16, s32, u16);
    extern void fightFloorSetStatus();
    extern u32 fightFloorGetStatus();
    extern int fightSideGetValidFightTrainerPtr();
    extern u8 fightSideGetDoFightTrainerCount();
    extern u8 fightSideCheckValid();
    extern u32 fn_801F8000();
    extern u32 fightTrainerGetNamePtr();
    extern int fightTrainerGetValidFightOutPokemonPtr();
    extern u8 fightTrainerGetDoFightOutFightOutPokemonCount();
    extern int fightTrainerGetStatus();
    extern void fightTrainerBallThrowEffect();
    extern u8 fightOutPokemonIsGcHeroFightOutPokemon();
    extern void fightOutPokemonSetOnDarkPokemonFlag();
    extern void fightOutPokemonSetOnZukanFlag();
    extern void* fightPokemonGetPokemonPtr();
    extern u32 fightPokemonCheckFightOut();
    extern void fightOutPokemonCreate();
    extern void fightOutPokemonRegWzxLoad();
    extern void fightOutPokemonDasuEffect();
    extern void _fightActionFlowKaisiNyuujouPokemonSubAppearMsg__FP13FIGHT_TRAINERP15FightOutPokemonUsUsUsUsUc();
    extern void fightMenuCloseMsg();
    extern void fn_8026532C(void*, u16, s32);
    extern void fn_80265598(void*, u16, s32);
    void* side;
    void* trainer;
    void* fop;
    void* pokemon;
    int sequence;
    int grid;
    u32 prevTarget;
    u32 buff;
    u16 sideId;
    u32 pokemonCount;
    u32 trainerCount;
    u32 i;
    u32 j;
    u32 num;
    u32 doFightTrainerCount;
    u32 doFightOutCount;

    buff = (u16)fightActionDataBiosGetBuff(fightActionBiosGetFightActionDataPtr((FightAction*)action));
    sideId = fightFloorGetStatus(0, 0, 0x14, 0);
    pokemonCount = (u16)fightFloorGetStatus(0, 0, 0x18, 0);
    trainerCount = (u16)fightFloorGetStatus(0, 0, 0x16, 0);
    side = (void*)fightTargetGetPtr(buff, 0, sideId);
    if (fightSideCheckValid(side) == 0) {
        return 0;
    }
    doFightTrainerCount = fightSideGetDoFightTrainerCount(side);

    for (i = 0; (u16)i < trainerCount; i++) {
        trainer = (void*)fightSideGetValidFightTrainerPtr(side, i);
        if (trainer == 0) {
            continue;
        }
        grid = fightTrainerGetStatus(trainer, 0, 0x4c, 0);
        if (grid == 0) {
            continue;
        }
        fightTrainerGetDoFightOutFightOutPokemonCount(trainer);
        num = 0;
        for (j = 0; (u16)j < 6; j++) {
            if ((u16)num >= pokemonCount || (u16)num >= 2) {
                break;
            }
            pokemon = (void*)fightTrainerGetStatus(trainer, 0, 0x45, j);
            if ((u8)fightPokemonCheckFightOut(pokemon) == 0) {
                continue;
            }
            fn_8010AE2C(pokemon, 0, 0);
            sequence = pokemonCreateSequence(fightPokemonGetPokemonPtr(pokemon));
            fop = (void*)fightTrainerGetStatus(trainer, 0, 0x46, j);
            fightOutPokemonCreate(fop, pokemon, sequence);
            num++;
            fightOutPokemonRegWzxLoad(fop);
            if ((u8)fightFloorGetStatus(0, 0, 0x1e, 0) == 1 &&
                fightOutPokemonIsGcHeroFightOutPokemon(fop) == 0) {
                fightOutPokemonSetOnZukanFlag(fop, 0);
                fightOutPokemonSetOnDarkPokemonFlag(fop, 0);
            }
            battleGridAddPokemon(grid, sequence);
        }
    }

    for (i = 0; (u16)i < trainerCount; i++) {
        trainer = (void*)fightSideGetValidFightTrainerPtr(side, i);
        if (trainer == 0) {
            continue;
        }
        for (j = 0; (u16)j < pokemonCount; j++) {
            if ((fop = (void*)fightTrainerGetValidFightOutPokemonPtr(trainer, j)) != 0) {
                break;
            }
        }
        fightTrainerBallThrowEffect(trainer, fop, 0);
        for (j = 0; (u16)j < pokemonCount; j++) {
            fop = (void*)fightTrainerGetValidFightOutPokemonPtr(trainer, j);
            if (fop != 0) {
                fightOutPokemonDasuEffect(fop, 0);
            }
        }
    }

    for (i = 0; (u16)i < trainerCount; i++) {
        trainer = (void*)fightSideGetValidFightTrainerPtr(side, i);
        if (trainer == 0) {
            continue;
        }
        doFightOutCount = fightTrainerGetDoFightOutFightOutPokemonCount(trainer);
        for (j = 0; (u16)j < pokemonCount; j++) {
            fop = (void*)fightTrainerGetValidFightOutPokemonPtr(trainer, j);
            if (fop != 0) {
                _fightActionFlowKaisiNyuujouPokemonSubAppearMsg__FP13FIGHT_TRAINERP15FightOutPokemonUsUsUsUsUc(
                    trainer, fop, doFightTrainerCount, doFightOutCount, i, j, 0);
            }
        }
        for (j = 0; (u16)j < pokemonCount; j++) {
            if ((fop = (void*)fightTrainerGetValidFightOutPokemonPtr(trainer, j)) != 0) {
                break;
            }
        }
        battleGridUpdate();
        fightTrainerBallThrowEffect(trainer, fop, 1);
        msgctrlSetValue(0x22, fn_801F8000(trainer));
        msgctrlSetValue(0x23, fightTrainerGetNamePtr(trainer));
        msgctrlSetValue(0x25, fightTrainerGetNamePtr(trainer));
        _fightActionFlowKaisiNyuujouPokemonSubAppearMsg__FP13FIGHT_TRAINERP15FightOutPokemonUsUsUsUsUc(
            trainer, fop, doFightTrainerCount, doFightOutCount, i, j, 1);
        fightTrainerBallThrowEffect(trainer, fop, 2);
        for (j = 0; (u16)j < pokemonCount; j++) {
            fop = (void*)fightTrainerGetValidFightOutPokemonPtr(trainer, j);
            if (fop != 0) {
                prevTarget = fightFloorGetStatus(0, 0, 0x36, 0);
                fightFloorSetStatus(0, 0, 0x36, 0, fop);
                fightOutPokemonDasuEffect(fop, 1);
                if (fightOutPokemonIsGcHeroFightOutPokemon(fop) == 0) {
                    fn_80265598(fop, sideId, 0);
                } else {
                    fn_80265598(fop, sideId, 1);
                }
                fightOutPokemonDasuEffect(fop, 2);
                fightOutPokemonDasuEffect(fop, 3);
                fightOutPokemonDasuEffect(fop, 4);
                fn_8026532C(fop, sideId, 0);
                fightFloorSetStatus(0, 0, 0x36, 0, prevTarget);
            }
        }
    }

    fightMenuCloseMsg();
    for (i = 0; (u16)i < trainerCount; i++) {
        trainer = (void*)fightSideGetValidFightTrainerPtr(side, i);
        if (trainer == 0) {
            continue;
        }
        for (j = 0; (u16)j < pokemonCount; j++) {
            if ((fop = (void*)fightTrainerGetValidFightOutPokemonPtr(trainer, j)) != 0) {
                break;
            }
        }
        fightTrainerBallThrowEffect(trainer, fop, 3);
        for (j = 0; (u16)j < pokemonCount; j++) {
            fop = (void*)fightTrainerGetValidFightOutPokemonPtr(trainer, j);
            if (fop != 0) {
                fightOutPokemonDasuEffect(fop, 5);
            }
        }
    }
    return 1;
}

/* Address: 0x8020CFE0 | Size: 0x21c */
void _fightActionFlowKaisiNyuujouPokemonSubAppearMsg__FP13FIGHT_TRAINERP15FightOutPokemonUsUsUsUsUc(
    u32 trainer, u32 fightOutPokemon, u16 trainerCount, u16 fightOutCount, u16 trainerIndex,
    u16 pokemonIndex, u8 mode)
{
    extern void msgctrlSetValue();
    extern u32 fn_801F18DC();
    extern u32 fn_801F8000();
    extern u32 fightOutPokemonIsGcHeroFightOutPokemon();
    extern u32 fightOutPokemonGetPokemonPtr();
    extern void fightMenuOpenMsg();
    u32 pokemonName;
    u32 isHero;
    u32 playerFlag;
    u32 trainerKind;

    isHero = (u32)__cntlzw(1 - (fightOutPokemonIsGcHeroFightOutPokemon(fightOutPokemon) & 0xff)) >> 5;
    playerFlag = (u32)__cntlzw(1 - (fn_801F18DC(0) & 0xff)) >> 5;
    trainerKind = fn_801F8000(trainer);
    if ((trainerKind == 0) && ((isHero & 0xff) == 0)) {
        playerFlag = 1;
    }
    pokemonName = fightOutPokemonGetPokemonPtr(fightOutPokemon);
    pokemonName = (int)pokemonGetStatus(pokemonName, 0, 0x77, 0);
    if (mode == 0) {
        if ((playerFlag & 0xff) == 1) {
            if (pokemonIndex == 0) {
                msgctrlSetValue(0x14, pokemonName);
                msgctrlSetValue(0x16, pokemonName);
            } else {
                msgctrlSetValue(0x15, pokemonName);
                msgctrlSetValue(0x17, pokemonName);
            }
        } else if ((isHero & 0xff) == 1) {
            if (pokemonIndex == 0) {
                msgctrlSetValue(0x15, pokemonName);
                msgctrlSetValue(0x17, pokemonName);
            } else {
                msgctrlSetValue(0x14, pokemonName);
                msgctrlSetValue(0x16, pokemonName);
            }
        } else if (pokemonIndex == 0) {
            msgctrlSetValue(0x14, pokemonName);
            msgctrlSetValue(0x16, pokemonName);
        } else {
            msgctrlSetValue(0x15, pokemonName);
            msgctrlSetValue(0x17, pokemonName);
        }
    } else if (mode == 1) {
        if ((trainerCount <= 1) && (1 < fightOutCount)) {
            if ((playerFlag & 0xff) == 1) {
                pokemonName = 0x7674;
            } else if ((isHero & 0xff) == 1) {
                pokemonName = 0x7679;
            } else {
                pokemonName = 0x7671;
            }
        } else {
            msgctrlSetValue(0x14, pokemonName);
            msgctrlSetValue(0x16, pokemonName);
            if ((playerFlag & 0xff) == 1) {
                pokemonName = 0x7673;
            } else if ((isHero & 0xff) == 1) {
                pokemonName = 0x7678;
            } else {
                pokemonName = 0x7670;
            }
        }
        fightMenuOpenMsg(pokemonName);
    }
}

/* Address: 0x8020D1FC | Size: 0x49c | Ghidra import */
u32 fightActionFlowKaisiNyuujouTrainer(void* action)

{
    extern u32 fn_8006B0F8();
    extern u8 fn_8006B57C();
    extern u8 pokemonCheckFightOut();
    extern u8 pokemonCheckValid();
    extern u32 heroGetStatus();
    extern void heroBiosCopy();
    extern void battleGridUpdate();
    extern void battleGridAddTrainer();
    extern void fn_801DA4E8();
    extern u32 fightTargetGetPtr();
    extern u32 fightFloorGetFightPokemonEntryCntInc();
    extern u32 fightFloorGetStatus();
    extern int fightSideGetValidFightTrainerPtr();
    extern void fightSideGetFightTrainerGridParam();
    extern u8 fightSideGetDoFightTrainerCount();
    extern u8 fightSideCheckValid();
    extern u32 fightSideGetStatus();
    extern u32 fightTrainerCreateSequence(u16);
    extern u32 fightTrainerCheckTemotiPokemonFightEntry();
    extern void fightTrainerSortFightTrainerDataIdToHeroTemotiPokemon();
    extern void fightTrainerCreateFightTrainerDataIdToHero(u16, u16, void*);
    extern u8 fightTrainerCheckTrainerDataIdValid(u16, u16);
    extern u8 fightTrainerCheckValid();
    extern void fightTrainerCreate(void*, void*, u16, u16, u32);
    extern u32 fightTrainerGetStatus();
    extern u8 fightTrainerIsGcHero();
    extern void fightPokemonGetFriendFormPokemonFriendFilterId();
    extern void fightPokemonCreate();
    extern u16 fightEncountDataBiosGetGSInputDevice();
    extern u16 fightEncountDataBiosGetFightTrainerDataId();
    extern u32 fightEncountDataBiosGetPtr();
    u8 hero[0xB1C];
    u8 gridX;
    u8 gridY;
    void* side;
    void* trainer;
    void* heroData;
    void* pokemon;
    void* fightPokemon;
    void* encount;
    int grid;
    u16 buff;
    u16 sideId;
    u16 trainerCount;
    u16 maxCount;
    u16 pokemonCount;
    u32 gridSide;
    u32 base;
    u32 i;
    u32 j;
    u32 index;
    u16 dataId;
    u16 device;
    s8 count;
    u32 doFightTrainerCount;

    buff = fightActionDataBiosGetBuff(fightActionBiosGetFightActionDataPtr((FightAction*)action));
    encount = (void*)fightEncountDataBiosGetPtr((u16)fightFloorGetStatus(0, 0, 0xd, 0));
    sideId = fightFloorGetStatus(0, 0, 0x14, 0);
    trainerCount = fightFloorGetStatus(0, 0, 0x16, 0);
    maxCount = fightFloorGetStatus(0, 0, 0x17, 0);
    pokemonCount = fightFloorGetStatus(0, 0, 0x18, 0);
    side = (void*)fightTargetGetPtr(buff, 0, sideId);
    if (fightSideCheckValid(side) == 0) {
        return 0;
    }
    base = buff != 4;
    gridSide = (u16)fightSideGetStatus(side, 0, 5, 0);
    for (i = 0; (u16)i < trainerCount; i++) {
        trainer = (void*)fightSideGetStatus(side, 0, 7, i);
        index = (u8)(i + (u16)base * trainerCount);
        dataId = fightEncountDataBiosGetFightTrainerDataId(encount, index);
        device = fightEncountDataBiosGetGSInputDevice(encount, index);
        if (fightTrainerCheckTrainerDataIdValid(dataId, device) == 0) {
            continue;
        }
        if ((u8)fn_8006B57C() == 1) {
            heroBiosCopy(hero, fn_8006B0F8(index));
        } else {
            fightTrainerCreateFightTrainerDataIdToHero(dataId, device, hero);
        }
        fightTrainerCreate(trainer, hero, dataId, device, fightTrainerCreateSequence(dataId));
        heroData = (void*)fightTrainerGetStatus(trainer, 0, 0x44, 0);
        if (fightTrainerCheckValid(trainer) == 0) {
            continue;
        }
        fightTrainerSortFightTrainerDataIdToHeroTemotiPokemon(trainer, maxCount, pokemonCount);
        count = 0;
        for (j = 0; (u16)j < 6; j++) {
            if (count >= (int)pokemonCount || count >= (int)maxCount || count >= 6) {
                break;
            }
            pokemon = (void*)heroGetStatus(heroData, 3, j);
            if (pokemonCheckFightOut(pokemon) == 0 ||
                fightTrainerCheckTemotiPokemonFightEntry(trainer, pokemon) != 0) {
                continue;
            }
            fightPokemon = (void*)fightTrainerGetStatus(trainer, 0, 0x45, count);
            fightPokemonCreate(fightPokemon, pokemon, fightFloorGetFightPokemonEntryCntInc(0));
            if ((u8)fightFloorGetStatus(0, 0, 0x27, 0) == 1 &&
                (u8)fightFloorGetStatus(0, 0, 0x2e, 0) == 1 &&
                fightTrainerIsGcHero(trainer) == 1) {
                fightPokemonGetFriendFormPokemonFriendFilterId(fightPokemon, 3);
            }
            count++;
        }
        for (j = 0; (u16)j < 6; j++) {
            if (count >= (int)maxCount || count >= 6) {
                break;
            }
            pokemon = (void*)heroGetStatus(heroData, 3, j);
            if (pokemonCheckValid(pokemon) == 0 ||
                fightTrainerCheckTemotiPokemonFightEntry(trainer, pokemon) != 0) {
                continue;
            }
            fightPokemon = (void*)fightTrainerGetStatus(trainer, 0, 0x45, count);
            fightPokemonCreate(fightPokemon, pokemon, fightFloorGetFightPokemonEntryCntInc(0));
            if ((u8)fightFloorGetStatus(0, 0, 0x27, 0) == 1 &&
                (u8)fightFloorGetStatus(0, 0, 0x2e, 0) == 1 &&
                fightTrainerIsGcHero(trainer) == 1) {
                fightPokemonGetFriendFormPokemonFriendFilterId(fightPokemon, 3);
            }
            count++;
        }
    }
    doFightTrainerCount = fightSideGetDoFightTrainerCount(side);
    for (i = 0; (u16)i < trainerCount; i++) {
        trainer = (void*)fightSideGetValidFightTrainerPtr(side, i);
        if (trainer == 0) {
            continue;
        }
        grid = fightTrainerGetStatus(trainer, 0, 0x4c, 0);
        if (grid == 0) {
            continue;
        }
        fightSideGetFightTrainerGridParam(gridSide, doFightTrainerCount, i, &gridX, &gridY);
        battleGridAddTrainer(grid, gridX, gridY);
        battleGridUpdate();
        fn_801DA4E8(grid, 1);
    }
    return 1;
}

/* Address: 0x8020D698 | Size: 0xec | Ghidra import */
u32 fightActionFlowKaijou(void* action)
{
    extern u32 fn_801EF624();
    extern u32 fightTargetGetPtr();
    extern void fightFloorInitFightStart();
    extern void fightFloorInit();
    extern u32 fightFloorGetStatus();
    u32 fightTarget;
    extern void fightSideCreate();
    extern u32 fightEncountDataBiosGetFightFloorDataId();
    extern u32 fightEncountDataBiosGetPtr();
    u32 encountData;
    u32 floorDataId;
    u16 sideNo;
    u32 initData;
    u16 trainerCount;

    initData = fn_801EF624();
    encountData = fightEncountDataBiosGetPtr();
    fightFloorInit(fightFloorGetStatus(0, 0, 0, 0), initData);
    fightFloorInitFightStart(0);
    sideNo = fightFloorGetStatus(0, 0, 0x14, 0);
    floorDataId = fightEncountDataBiosGetFightFloorDataId(encountData);
    fightTarget = fightTargetGetPtr(4, 0, sideNo);
    trainerCount = fightFloorGetStatus(0, floorDataId, 3, 0);
    fightSideCreate(fightTarget, trainerCount);
    fightTarget = fightTargetGetPtr(5, 0, sideNo);
    trainerCount = fightFloorGetStatus(0, floorDataId, 3, 1);
    fightSideCreate(fightTarget, trainerCount);
    return 1;
}

#endif

#if !defined(PR409_FIGHT_ACTION_SPLIT) || defined(PR409_FIGHT_ACTION_D784_D844)

/* Address: 0x8020D784 | Size: 0x8 | Pattern: return_constant */
u32 fightActionFlowNullFunc(void* action) { return 1; }

/* Address: 0x8020D78C | Size: 0x10 | Pattern: nullcheck_setter */
void fightActionBiosSetFifoBanme(u8* ptr, u32 val) {
    if (ptr == NULL) { return; }
    *(u32*)(&ptr[0x1C]) = val;
}

/* Address: 0x8020D79C | Size: 0x18 | Pattern: nullcheck_getter */
u32 fightActionKindDataBiosGetDispFuncPtr(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u32*)(&ptr[0x8]);
}

/* Address: 0x8020D7B4 | Size: 0x18 | Pattern: nullcheck_getter */
u32 fightActionKindDataBiosGetFlowFuncPtr(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u32*)(&ptr[0x4]);
}

/* fightActionKindDataBiosGetPri | Size: 0x1C | Read signed byte, return -128 if NULL */
s32 fightActionKindDataBiosGetPri(u8* ptr) {
    if (ptr == NULL) {
        return -128;
    }
    return (s8)ptr[0];
}

/* fightActionKindDataBiosGetPtr | Size: 0x2C | Look up entry in 12-byte table (u16 index) */
void* fightActionKindDataBiosGetPtr(u16 index) {
    extern u8 lbl_80375BB8[];
    extern u32 lbl_80478D48;
    if (index >= lbl_80478D48) {
        return NULL;
    }
    return &lbl_80375BB8[index * 12];
}

/* Address: 0x8020D814 | Size: 0x18 | Pattern: nullcheck_getter */
u32 fightActionDataBiosGetBuff(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u32*)(&ptr[0x4]);
}

/* Address: 0x8020D82C | Size: 0x18 | Pattern: nullcheck_getter */
u16 fightActionDataBiosGetKind(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u16*)(&ptr[0x0]);
}

#endif

#if !defined(PR409_FIGHT_ACTION_SPLIT) || defined(PR409_FIGHT_ACTION_D844_D868)

/* fightActionBiosSetDispBuff | Size: 0x24 | Store value at indexed slot (max 4) */
#pragma push
#pragma peephole on
void fightActionBiosSetDispBuff(FightAction* action, u32 index, u32 value) {
    if (action == NULL) {
        return;
    }
    if ((u16)index >= 4) {
        return;
    }
    action->displayBuff[(u16)index] = value;
}
#pragma pop

#endif
