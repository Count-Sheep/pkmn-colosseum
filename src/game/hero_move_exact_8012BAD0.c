/**
 * @file hero_move_exact_8012BAD0.c
 * @brief heroMove TU: heroMoveAddAutoEvent and heroMoveSetEventList,
 *        .text 0x8012BAD0-0x8012BBA8.
 *
 * Text-only unit carved from the hero_move range at function boundaries.
 * Neither function touches the TU's .sdata2 pool (the {100, 101} resource
 * table and 0.0f are first used before 0x8012BAD0), so this carve owns no
 * data. The hero-move state (.bss lbl_80426BD0) stays extern. Built with the
 * hero_move TU's GC/1.3 -O4,p flags. The same bodies are in
 * src/game/hero_move.c.
 */
#include "game/hero_move.h"

extern HeroMoveWork lbl_80426BD0;
extern void* memcpy(void* dst, const void* src, u32 n);

/* Sets the five automatic-event parameters. */
void heroMoveAddAutoEvent(u32 a, u32 b, u32 c, u32 d, u32 e) {
    u8* base = ((u8*)&lbl_80426BD0);
    *(u32*)(base + 0x18C) = a;
    *(u32*)(base + 0x190) = b;
    *(u32*)(base + 0x194) = c;
    *(u32*)(base + 0x198) = d;
    *(u32*)(base + 0x19C) = e;
}

/* Copies an event list (0xD0 bytes) and its value into slot 1, 2 or 0. */
void heroMoveSetEventList(u8 type, void* src, u32 val) {
    switch (type) {
        case 1:
            memcpy(lbl_80426BD0.eventList[1], src, 0xd0);
            lbl_80426BD0.eventValue[1] = val;
            break;
        case 2:
            memcpy(lbl_80426BD0.eventList[2], src, 0xd0);
            lbl_80426BD0.eventValue[2] = val;
            break;
        case 3:
            memcpy(lbl_80426BD0.eventList[0], src, 0xd0);
            lbl_80426BD0.eventValue[0] = val;
            break;
    }
}
