/**
 * @file fight_range_candidate_80219838.c
 * @brief Fight sequence commands, 0x80219838 - 0x80219D98 (fn_80219838,
 *        fn_80219964, fn_80219B2C, fn_80219CF4).
 *
 * fn_80219964 and fn_80219B2C are one body with different pokemon status
 * ids, so they share an inline. As an inline its locals are numbered in
 * reverse declaration order, which gives retail's register order (floor,
 * power, target id, move, attacker) and lets the move result coalesce
 * without an extra copy.
 */
#include "dolphin/types.h"

extern u8* lbl_8047B610;
extern u16 fn_800E0C54(void);

extern u32 pokemonGetStatus();
extern void pokemonSetStatus();
extern s8 pokemonSearchWazaDataId();
extern void wazaGetStatus(u32, u16, u32, u32);
extern void wazaSetStatus();
extern u32 GSmsgGetGSchar();
extern void msgctrlSetValue();
extern u32 fightTargetGetPtrAsNowFightType();
extern int fightTargetGetRelativeHostSideFightTargetIdToTragetPtr();
extern u32 fightFloorGetStatus();
extern void fightFloorSetStatus();
extern u8 fightSideIsJoutaiDataId();
extern u32 fightSideGetJoutaiUserFightTargetId();
extern u32 fightOutPokemonGetPokemonPtr();
extern u8 fightOutPokemonIsAlly();
extern u8 fightOutPokemonCheckFightOut();
extern u8 figthOutPokemonGetLevel();
extern void fn_80201B2C();
extern void fn_8020248C();
extern u8 fn_802025B8();

void fn_80219838(void)
{
    u32 target;
    u32 pokemon;
    u16 lastMove;
    u16 move;
    s8 slot;
    u8 pp;

    target = fightTargetGetPtrAsNowFightType(0x12, 0);
    pokemon = fightOutPokemonGetPokemonPtr(target);
    lastMove = pokemonGetStatus(target, 0, 0xf0, 0);
    slot = pokemonSearchWazaDataId(pokemon, lastMove);
    if (slot < 0) {
        pp = 0;
    } else {
        pp = pokemonGetStatus(pokemon, 0, 0x80, slot);
    }
    if (fn_802025B8(target, 0x29) == 2 && slot >= 0 && pp != 0) {
        move = pokemonGetStatus(pokemon, 0, 0x7f, slot);
        wazaGetStatus(0, move, 1, 0);
        msgctrlSetValue(0xd, GSmsgGetGSchar());
        fn_8020248C(target, 0x29, 0);
        fn_80201B2C(target, 0x29, move);
        lbl_8047B610 += 5;
        return;
    }
    lbl_8047B610 = *(u8**)(lbl_8047B610 + 1);
}

/*
 * Checks the target stored in the attacker's status (targetStatus) and, if
 * it is still in battle and not an ally, sets the move's power from
 * powerStatus and aims the move there, unless a Follow Me user (side joutai
 * 0x4d) on the target's side redirects it.
 */
static inline void fightSeqAimStoredTarget(u32 targetStatus, u32 powerStatus)
{
    int target;
    u32 userId;
    int redirected;
    u32 followMe;
    u16 floor2;
    u32 attacker;
    u32 move;
    u16 targetId;
    s16 power;
    u16 floor;

    floor = fightFloorGetStatus(0, 0, 0x14, 0);
    attacker = fightTargetGetPtrAsNowFightType(0x11, 0);
    move = pokemonGetStatus(attacker, 0, 0xd9, 0);
    targetId = pokemonGetStatus(attacker, 0, targetStatus, 0);
    power = pokemonGetStatus(attacker, 0, powerStatus, 0);
    if (targetId == 0 || power == 0) {
        goto fail;
    }
    target = fightTargetGetRelativeHostSideFightTargetIdToTragetPtr(targetId, floor);
    if (target == 0) {
        goto fail;
    }
    if (fightOutPokemonIsAlly(attacker, target) != 0) {
        goto fail;
    }
    if (fightOutPokemonCheckFightOut(target) == 1) {
        wazaSetStatus(move, 0, 0x2d, 0, power << 1);
        attacker = fightTargetGetPtrAsNowFightType(3, attacker);
        floor2 = fightFloorGetStatus(0, 0, 0x14, 0);
        followMe = 0;
        if (fightSideIsJoutaiDataId(attacker, 0x4d) == 1) {
            userId = fightSideGetJoutaiUserFightTargetId(attacker, 0x4d);
            if ((userId & 0xffff) != 0) {
                redirected = fightTargetGetRelativeHostSideFightTargetIdToTragetPtr(userId, floor2);
                if (redirected != 0 && fightOutPokemonCheckFightOut(redirected) == 1) {
                    followMe = redirected;
                }
            }
        }
        if (followMe != 0) {
            target = followMe;
        }
        fightFloorSetStatus(0, 0, 0x43, 0, target);
        lbl_8047B610 += 5;
        return;
    }
fail:
    pokemonSetStatus(attacker, 0, 0x118, 0, 1);
    lbl_8047B610 = *(u8**)(lbl_8047B610 + 1);
}

void fn_80219964(void)
{
    fightSeqAimStoredTarget(0x105, 0x104);
}

void fn_80219B2C(void)
{
    fightSeqAimStoredTarget(0x103, 0x102);
}

/* Sets the move's power to level * (50 + 10 * (rand % 11)) / 100. */
void fn_80219CF4(void)
{
    u32 attacker;
    u32 move;
    u32 level;
    u16 rng;

    attacker = fightTargetGetPtrAsNowFightType(0x11, 0);
    move = pokemonGetStatus(attacker, 0, 0xd9, 0);
    level = figthOutPokemonGetLevel(attacker);
    rng = fn_800E0C54();
    wazaSetStatus(move, 0, 0x2d, 0, (s32)(level * ((rng % 11) * 10 + 50)) / 100);
    lbl_8047B610++;
}
