# Dolphin OSTime.c time-base reads in Pokémon Colosseum (GC6E01)

`dolphin/os/OSTime_range_800A2778.c` covers .text 0x800A2778–0x800A27FC, the
head of the Dolphin SDK's `OSTime.c`: `OSGetTime`, `OSGetTick` and
`__OSGetSystemTime` (C). The calendar half of the file links as
`dolphin/os/OSTime.c`. The two routines below use the exact retail mnemonics
and match the retail bytes.

## OSGetTime

Retail address 0x800A2778, size 0x18.

- Why it cannot be C: it reads the 64-bit time base with `mftb` (TBU, SPR 269, then TBL, SPR 268, then TBU again) and retries with `bne` back to its own first instruction until the two upper reads agree, returning the pair in r3:r4 without a frame. MWCC has no C intrinsic for the time base and never emits `mftb`. The only branch is that retry to the routine's own start, which needs no branch-target declaration (README item 4).
- Other decompilations: [zeldaret/tww, commit f5234ec8, `src/dolphin/os/OSTime.c`](https://github.com/zeldaret/tww/blob/f5234ec8f4b8f4119b8db4a018c1ca25572988b1/src/dolphin/os/OSTime.c#L8-L23) keeps `OSGetTime` as an `asm` function with the same `mftbu`/`mftb`/`mftbu`, `cmpw`, `bne OSGetTime`, `blr` body ([L18](https://github.com/zeldaret/tww/blob/f5234ec8f4b8f4119b8db4a018c1ca25572988b1/src/dolphin/os/OSTime.c#L18)); [doldecomp/melee, commit e78dc834, `libs/dolphin/src/dolphin/os/OSTime.c`](https://github.com/doldecomp/melee/blob/e78dc8349c587ca5d2f97322834b625d3bd1abc3/libs/dolphin/src/dolphin/os/OSTime.c#L15-L31) does the same ([L27](https://github.com/doldecomp/melee/blob/e78dc8349c587ca5d2f97322834b625d3bd1abc3/libs/dolphin/src/dolphin/os/OSTime.c#L27)).
- Origin: Nintendo Dolphin SDK, `os/OSTime.c`, hand-written assembly in the vendor source.

## OSGetTick

Retail address 0x800A2790, size 0x8.

- Why it cannot be C: it returns the lower time-base word with a single `mftb r3, 268` and `blr`. MWCC never emits `mftb` from C.
- Other decompilations: [zeldaret/tww, commit f5234ec8, `src/dolphin/os/OSTime.c`](https://github.com/zeldaret/tww/blob/f5234ec8f4b8f4119b8db4a018c1ca25572988b1/src/dolphin/os/OSTime.c#L25-L34) and [doldecomp/melee, commit e78dc834, `libs/dolphin/src/dolphin/os/OSTime.c`](https://github.com/doldecomp/melee/blob/e78dc8349c587ca5d2f97322834b625d3bd1abc3/libs/dolphin/src/dolphin/os/OSTime.c#L33-L41) keep `OSGetTick` as an `asm` function with the same two instructions.
- Origin: Nintendo Dolphin SDK, `os/OSTime.c`, hand-written assembly in the vendor source.
