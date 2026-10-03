# MetroTRK mpc_7xx_603e.c extended-register save/restore in Pokémon Colosseum (GC6E01)

`trk/TRKSaveState.c` occupies .text 0x800C2A10–0x800C2D80 as one retail
translation unit: MetroTRK's `mpc_7xx_603e.c`, which consists of exactly these
two hand-written assembly routines and no data. Only the routines named below
are admitted. Each body uses the exact retail mnemonics; the source matches the
retail bytes in objdiff and in the final link.

Common origin: Metrowerks MetroTRK (CodeWarrior target resident kernel for
Dolphin, `TRK_MINNOW_DOLPHIN`), `Processor/ppc/Generic/mpc_7xx_603e.c`, linked
into Colosseum's main.dol from the CodeWarrior debugger library. The routines are
written as `asm` functions in the vendor source, as every decompilation below
also reproduces.

## TRKSaveExtended1Block

Retail address 0x800C2A10, size 0x1B8.

- Why it cannot be C: it copies the sixteen segment registers (`mfsr`), the time base (`mftb`/`mftbu`), the instruction and data BAT registers (`mfibatu`/`mfibatl`/`mfdbatu`/`mfdbatl`), DAR/DSISR (`mfdar`/`mfdsisr`), PVR (`mfpvr`) and a long list of special-purpose registers (`mfspr`) into `gTRKCPUState` with `stmw`, using r2 as its base without saving it, and contains processor-variant code that is skipped by unconditional branches. MWCC never emits segment, BAT, time-base or SPR moves from C, and never clobbers r2 or r16–r31 without a frame.
- Other decompilations: [zeldaret/tp, commit c8fa8c9e, `libs/TRK_MINNOW_DOLPHIN/debugger/embedded/MetroTRK/Processor/ppc/Generic/mpc_7xx_603e.c`](https://github.com/zeldaret/tp/blob/c8fa8c9e2aab72cf4e5db0e5d1c84a9ea6ee6eb0/libs/TRK_MINNOW_DOLPHIN/debugger/embedded/MetroTRK/Processor/ppc/Generic/mpc_7xx_603e.c#L6), [zeldaret/tww, commit f5234ec8, `src/TRK_MINNOW_DOLPHIN/ppc/Generic/mpc_7xx_603e.c`](https://github.com/zeldaret/tww/blob/f5234ec8f4b8f4119b8db4a018c1ca25572988b1/src/TRK_MINNOW_DOLPHIN/ppc/Generic/mpc_7xx_603e.c#L6) and [doldecomp/melee, commit e78dc834, `src/MetroTRK/mpc_7xx_603e.c`](https://github.com/doldecomp/melee/blob/e78dc8349c587ca5d2f97322834b625d3bd1abc3/src/MetroTRK/mpc_7xx_603e.c#L9) keep `TRKSaveExtended1Block` as an `asm` function body.
- Origin: Metrowerks MetroTRK for Dolphin, `mpc_7xx_603e.c`, hand-written assembly in the vendor source; the exact CodeWarrior/MetroTRK build in Colosseum is not established.

## TRKRestoreExtended1Block

Retail address 0x800C2BC8, size 0x1B8.

- Why it cannot be C: it is the inverse of `TRKSaveExtended1Block`: it reloads the block from `gTRKCPUState` with `lmw` and writes the time base (`mttbl`/`mttbu`), segment registers (`mtsr`), BAT registers (`mtibatu`/`mtibatl`/`mtdbatu`/`mtdbatl`), DAR/DSISR (`mtdar`/`mtdsisr`) and special-purpose registers (`mtspr`), consuming the `gTRKRestoreFlags` bytes, again with r2 as base and r16–r31 clobbered without a frame. MWCC never emits these privileged register moves from C.
- Other decompilations: [zeldaret/tp, commit c8fa8c9e, `libs/TRK_MINNOW_DOLPHIN/debugger/embedded/MetroTRK/Processor/ppc/Generic/mpc_7xx_603e.c`](https://github.com/zeldaret/tp/blob/c8fa8c9e2aab72cf4e5db0e5d1c84a9ea6ee6eb0/libs/TRK_MINNOW_DOLPHIN/debugger/embedded/MetroTRK/Processor/ppc/Generic/mpc_7xx_603e.c#L124), [zeldaret/tww, commit f5234ec8, `src/TRK_MINNOW_DOLPHIN/ppc/Generic/mpc_7xx_603e.c`](https://github.com/zeldaret/tww/blob/f5234ec8f4b8f4119b8db4a018c1ca25572988b1/src/TRK_MINNOW_DOLPHIN/ppc/Generic/mpc_7xx_603e.c#L124) and [doldecomp/melee, commit e78dc834, `src/MetroTRK/mpc_7xx_603e.c`](https://github.com/doldecomp/melee/blob/e78dc8349c587ca5d2f97322834b625d3bd1abc3/src/MetroTRK/mpc_7xx_603e.c#L143) keep `TRKRestoreExtended1Block` as an `asm` function body.
- Origin: Metrowerks MetroTRK for Dolphin, `mpc_7xx_603e.c`, hand-written assembly in the vendor source; the exact CodeWarrior/MetroTRK build in Colosseum is not established.
