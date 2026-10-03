# Dolphin db.c exception destination in Pokémon Colosseum (GC6E01)

`dolphin/db/DB.c` covers .text 0x800A2C74–0x800A2CCC of the Dolphin SDK's
`db.c`: `__DBExceptionDestinationAux` (C) and `__DBExceptionDestination`. The
rest of db.c (`DBInit`, `__DBIsExceptionMarked`, `DBPrintf` and its neighbour)
links from its own units. The routine below uses the exact retail mnemonics and
matches the retail bytes.

## __DBExceptionDestination

Retail address 0x800A2CBC, size 0x10. `DBInit` stores its physical address in
the debugger interface (0x80000048) as the exception destination, so the
debug monitor jumps to it with address translation off.

- Why it cannot be C: it runs with the MMU off and re-enables instruction and data translation by setting MSR[IR|DR] (`mfmsr`, `ori r3, r3, 0x30`, `mtmsr`) before anything touches virtual memory, then tail-branches without a frame. MWCC never emits `mtmsr` from C, and a C body would build a stack frame (through a translated stack pointer) before the MSR write.
- Other decompilations: [zeldaret/tww, commit f5234ec8, `src/dolphin/db/db.c`](https://github.com/zeldaret/tww/blob/f5234ec8f4b8f4119b8db4a018c1ca25572988b1/src/dolphin/db/db.c#L25-L31) keeps `__DBExceptionDestination` as an `asm` function with the same four instructions, ending in `b __DBExceptionDestinationAux` ([L30](https://github.com/zeldaret/tww/blob/f5234ec8f4b8f4119b8db4a018c1ca25572988b1/src/dolphin/db/db.c#L30)); [doldecomp/melee, commit e78dc834, `libs/dolphin/src/dolphin/db/db.c`](https://github.com/doldecomp/melee/blob/e78dc8349c587ca5d2f97322834b625d3bd1abc3/libs/dolphin/src/dolphin/db/db.c#L38-L46) does the same ([L44](https://github.com/doldecomp/melee/blob/e78dc8349c587ca5d2f97322834b625d3bd1abc3/libs/dolphin/src/dolphin/db/db.c#L44)).
- Origin: Nintendo Dolphin SDK, `db/db.c`, hand-written assembly in the vendor source.
- External branch targets: the final `b __DBExceptionDestinationAux` (0x800A2CC8) jumps to __DBExceptionDestinationAux at 0x800A2C74 (same unit, C), which reports and dumps the context and halts. It is a tail jump, not a call: the routine has no frame and never returns, so MWCC's `bl` with a prologue would not reproduce it. The retail relocation at that instruction names __DBExceptionDestinationAux.
