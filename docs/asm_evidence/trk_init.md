# MetroTRK dolphin_trk.c entry and ARAM transfer routines in Pokémon Colosseum (GC6E01)

MetroTRK's `Os/dolphin/dolphin_trk.c` occupies .text 0x800C2D80–0x800C33BC in
Colosseum as one translation unit (`src/trk/TRKInit.c`). Its entry points
`InitMetroTRK` and `InitMetroTRK_BBA` are hand-written assembly; the two ARAM
transfer routines are C, but each contains inline-assembly cache-block
instructions that the vendor source writes as `asm { ... }` blocks. Only the
routines named below are admitted, each with the exact mnemonics listed in
the registry. All of them match the retail bytes in objdiff and in the final
link.

Common origin: Metrowerks MetroTRK for Dolphin (`TRK_MINNOW_DOLPHIN`),
`Os/dolphin/dolphin_trk.c`, linked into Colosseum's main.dol from the
CodeWarrior debugger library.

## TRK__write_aram

Retail address 0x800C2EAC, size 0x1EC.

- Why it cannot be C: before each partial-line ARAM read it invalidates the 32-byte bounce buffer with `dcbi r0, bf`. MWCC (GC/1.3) has no `__dcbi` intrinsic; a call to one compiles to an external function call, so the instruction can only come from an inline `asm` block. (The `dcbf` flushes in the same routine are emitted by the `__dcbf` intrinsic and stay C.)
- Other decompilations: [zeldaret/tp, commit c8fa8c9e, `libs/TRK_MINNOW_DOLPHIN/debugger/embedded/MetroTRK/Os/dolphin/dolphin_trk.c`](https://github.com/zeldaret/tp/blob/c8fa8c9e2aab72cf4e5db0e5d1c84a9ea6ee6eb0/libs/TRK_MINNOW_DOLPHIN/debugger/embedded/MetroTRK/Os/dolphin/dolphin_trk.c#L138) keeps the same `dcbi` statements as inline `asm` blocks (through its `__dcbi` macro, line 94).
- Origin: Metrowerks MetroTRK for Dolphin, `dolphin_trk.c`, inline assembly in the vendor source; the exact CodeWarrior/MetroTRK build in Colosseum is not established.

## TRK__read_aram

Retail address 0x800C3098, size 0x134.

- Why it cannot be C: before the ARAM-to-RAM DMA it invalidates the destination range one 32-byte block at a time with `dcbi counter, c` (unrolled by eight in the retail loop). MWCC (GC/1.3) has no `__dcbi` intrinsic; a call to one compiles to an external function call, so the instruction can only come from an inline `asm` block.
- Other decompilations: [zeldaret/tp, commit c8fa8c9e, `libs/TRK_MINNOW_DOLPHIN/debugger/embedded/MetroTRK/Os/dolphin/dolphin_trk.c`](https://github.com/zeldaret/tp/blob/c8fa8c9e2aab72cf4e5db0e5d1c84a9ea6ee6eb0/libs/TRK_MINNOW_DOLPHIN/debugger/embedded/MetroTRK/Os/dolphin/dolphin_trk.c#L97) keeps the same `dcbi` statement as an inline `asm` block (through its `__dcbi` macro, line 94).
- Origin: Metrowerks MetroTRK for Dolphin, `dolphin_trk.c`, inline assembly in the vendor source; the exact CodeWarrior/MetroTRK build in Colosseum is not established.

## InitMetroTRK

Retail address 0x800C2D80, size 0x98. The final, unreachable `blr` at
0x800C2E14 is part of the original assembly body; the unconditional tail branch
to `TRK_main` immediately before it explains why automated analysis had split it
into a gap.

- Why it cannot be C: it is the debugger entry point called from `__start` through `blrl`. It saves every GPR into `gTRKCPUState` with `stmw r0` and restores them with `lmw r0` (a load-multiple form MWCC never emits), stores LR, CR and the MSR by hand (`mflr`, `mfcr`, `mfmsr`, `mtsrr1`), masks MSR[EE] with `ori`/`xori` and `mtmsr`, clears IABR and DABR with `mtspr`, and replaces the stack pointer with `_db_stack_addr`. None of this is expressible in C.
- Other decompilations: [zeldaret/tp, commit c8fa8c9e, `libs/TRK_MINNOW_DOLPHIN/debugger/embedded/MetroTRK/Os/dolphin/dolphin_trk.c`](https://github.com/zeldaret/tp/blob/c8fa8c9e2aab72cf4e5db0e5d1c84a9ea6ee6eb0/libs/TRK_MINNOW_DOLPHIN/debugger/embedded/MetroTRK/Os/dolphin/dolphin_trk.c#L216), [zeldaret/tww, commit f5234ec8, `src/TRK_MINNOW_DOLPHIN/Os/dolphin/dolphin_trk.c`](https://github.com/zeldaret/tww/blob/f5234ec8f4b8f4119b8db4a018c1ca25572988b1/src/TRK_MINNOW_DOLPHIN/Os/dolphin/dolphin_trk.c#L90) and [doldecomp/melee, commit e78dc834, `src/MetroTRK/dolphin_trk.c`](https://github.com/doldecomp/melee/blob/e78dc8349c587ca5d2f97322834b625d3bd1abc3/src/MetroTRK/dolphin_trk.c#L56) keep `InitMetroTRK` as an `asm` function body.
- Origin: Metrowerks MetroTRK for Dolphin, `dolphin_trk.c`, hand-written assembly in the vendor source; the exact CodeWarrior/MetroTRK build in Colosseum is not established.
- External branch targets: `b TRK_main` (0x800C2E10) jumps to `TRK_main` at 0x800C33BC once `InitMetroTRKCommTable` succeeds. It is a tail jump, not a call: the routine has just replaced r1 with the debugger stack and never returns, so there is no frame and no `bl`. The retail relocation at that instruction names `TRK_main`.

## InitMetroTRK_BBA

Retail address 0x800C2E18, size 0x94.

- Why it cannot be C: the broadband-adapter variant of `InitMetroTRK`, called from `__start` when `__get_debug_bba` is set. It performs the same full GPR save and restore with `stmw r0`/`lmw r0`, hand-written LR/CR/MSR/SRR1 handling (here setting MSR[EE] with `ori` and `mtmsr`), the `mtspr` IABR/DABR clears and the switch to `_db_stack_addr`, then initialises the comm table for hardware id 2. None of this is expressible in C.
- Other decompilations: [zeldaret/tp, commit c8fa8c9e, `libs/TRK_MINNOW_DOLPHIN/debugger/embedded/MetroTRK/Os/dolphin/dolphin_trk.c`](https://github.com/zeldaret/tp/blob/c8fa8c9e2aab72cf4e5db0e5d1c84a9ea6ee6eb0/libs/TRK_MINNOW_DOLPHIN/debugger/embedded/MetroTRK/Os/dolphin/dolphin_trk.c#L277) keeps `InitMetroTRK_BBA` as an `asm` function body.
- Origin: Metrowerks MetroTRK for Dolphin, `dolphin_trk.c`, hand-written assembly in the vendor source; the exact CodeWarrior/MetroTRK build in Colosseum is not established.
- External branch targets: `b TRK_main` (0x800C2EA4) jumps to `TRK_main` at 0x800C33BC once `InitMetroTRKCommTable` succeeds; like `InitMetroTRK`, it runs on the freshly installed debugger stack and never returns, so it is a tail jump rather than a call. The retail relocation at that instruction names `TRK_main`.
