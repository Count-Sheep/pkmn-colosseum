#include "dolphin/types.h"
#include "game/win_sprite.h"

extern void _threadSwitch(void);
extern u32 fn_8010B560(void);
extern void fn_801CB9D8(u32);

extern u32 lbl_8047A418;
extern u32 lbl_8047A420;
extern u32 lbl_8047A424;
extern u32 lbl_8047A42C;
extern u8 lbl_80266E90[];

typedef struct {
    u8 kind;
    u8 arg;
    u16 ids[8];
} NpcPokemonEvent;

void fn_800318D8(s32 unused, u8* tgt)
{
    s32 i;
    s32 kind;
    s32 arg;

    kind = 0;
    arg = 0;
    for (i = 0; i < 12; i++) {
        if (*(s16*)(tgt + 0x6) == ((NpcPokemonEvent*)lbl_80266E90)[i].ids[6]) {
            kind = ((NpcPokemonEvent*)lbl_80266E90)[i].kind;
            arg = ((NpcPokemonEvent*)lbl_80266E90)[i].arg;
        }
    }

    switch (kind) {
    case 1:
        if ((s32)lbl_8047A424 == arg) {
            winSpriteSetDisp(tgt, 1);
            return;
        }
        break;
    case 2:
        if ((s32)lbl_8047A420 == arg) {
            winSpriteSetDisp(tgt, 1);
            return;
        }
        break;
    }
    winSpriteSetDisp(tgt, 0);
}

void fn_80031A1C(void* r3, void* r4)
{
    s32 val = (s32) lbl_8047A42C;

    switch (val) {
    case 7:
    case 2:
        winSpriteSetDisp(r4, 1);
        break;
    default:
        winSpriteSetDisp(r4, 0);
        break;
    }
}

void fn_80031A70(void* r3, void* r4)
{
    s32 val = (s32) lbl_8047A42C;

    switch (val) {
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
        winSpriteSetDisp(r4, 1);
        break;
    default:
        winSpriteSetDisp(r4, 0);
        break;
    }
}

void fn_80031AC0(void* r3, void* r4)
{
    s32 val = (s32) lbl_8047A42C;

    switch (val) {
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
        winSpriteSetDisp(r4, 1);
        break;
    default:
        winSpriteSetDisp(r4, 0);
        break;
    }
}

#pragma peephole off
void fn_80031B10(void)
{
    while ((u8) fn_8010B560() != 0) {
        _threadSwitch();
    }
    fn_801CB9D8(lbl_8047A418);
}
#pragma peephole on
