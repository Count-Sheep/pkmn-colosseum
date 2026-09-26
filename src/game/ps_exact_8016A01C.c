/**
 * @file ps_exact_8016A01C.c
 * @brief psInitParticle (0x8016A01C - 0x8016A17C) and the particle module's
 *        per-bank tables in .bss (0x804527C8 - 0x80452DE8).
 *
 * psInitParticle resets the particle link lists (_psLinkInit, pslist.c),
 * the live/peak particle counters and every per-bank table: six 64-entry
 * tables and the eight camera/JObj slots after them. It is Melee's particle.c
 * psInit counterpart (hsd_80398A08), with Colosseum's 64 banks and the
 * link-list reset moved into pslist.c.
 *
 * Pooled .bss. The tables are defined in this unit, so MWCC (1.3.2 and
 * later) loads their common base once and reaches each table with its own
 * add, as retail does (GC/1.3 folds them into one register). Retail lays
 * them out 0x804527C8 .. 0x80452DC8 while psInitParticle touches them in the
 * opposite order (the 0x80452CC8 table first); under -inline deferred MWCC
 * lays pooled .bss out in reverse definition order, so they are defined from
 * the last to the first. The rest of the particle module (still candidates
 * and generated assembly) reaches them through their symbols.
 *
 * 0x80452AC8 and 0x80452BC8 are two separate tables (cleared at +0x300 and
 * +0x400 from the base); symbols.txt had them as one 0x200 object.
 *
 * The bank loop counter is unsigned and separate from the slot counter, as
 * in pslist.c's _psLinkInit: retail has no trip-count guard in front of the
 * unrolled loop. Built with GC/1.3.2 -O4,p -inline deferred and no local
 * pragmas. The symbols keep their address names.
 */
#include "dolphin/types.h"

extern s32 _psLinkInit(s32 count);
extern u16 lbl_8047B114; /* peak live particles */
extern u16 lbl_8047B11A; /* live particles */

void* lbl_80452DC8[8];         /* camera/JObj slots */
s32 lbl_80452CC8[64];
void* lbl_80452BC8[64];
void* lbl_80452AC8[64];
void* lbl_804529C8[64];         /* per-bank object data */
void** lbl_804528C8[64];
void* lbl_804527C8[64];         /* per-bank script data */

void psInitParticle(s32 count)
{
    u32 bank;
    s32 i;

    _psLinkInit(count);
    lbl_8047B11A = 0;
    lbl_8047B114 = 0;

    for (bank = 0; bank < 64; bank++) {
        lbl_80452CC8[bank] = 0;
        lbl_80452BC8[bank] = NULL;
        lbl_80452AC8[bank] = NULL;
        lbl_804529C8[bank] = NULL;
        lbl_804528C8[bank] = NULL;
        lbl_804527C8[bank] = NULL;
    }
    for (i = 0; i < 8; i++) {
        lbl_80452DC8[i] = NULL;
    }
}
