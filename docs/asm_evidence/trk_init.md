# MetroTRK dolphin_trk.c ARAM transfer routines in Pokémon Colosseum (GC6E01)

MetroTRK's `Os/dolphin/dolphin_trk.c` occupies .text 0x800C2D80–0x800C33BC in
Colosseum (`src/trk/TRKInit.c`, built as the `TRKInit_r53_800C2D80_prefix`,
`TRKInit_r53_800C3218_lmw_on` and `TRKInit_exact_800C3344` units). The two
ARAM transfer routines below are C, but each contains inline-assembly
cache-block instructions that the vendor source writes as `asm { ... }`
blocks. Only those inline blocks are admitted, with the exact mnemonics listed
in the registry; the rest of each function is ordinary C. Both functions match
the retail bytes in objdiff.

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
