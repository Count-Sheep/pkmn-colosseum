# MetroTRK TRKLoadContext in Pokémon Colosseum (GC6E01)

`TRKLoadContext` (.text 0x800C3414–0x800C349C, the `trk/TRKBoard_candidate_800C3414`
unit, source in `src/trk/TRKBoard_range_800C33BC.c`) comes from MetroTRK's
`Os/dolphin/dolphin_trk_glue.c`. It is the only routine in that unit and is
admitted with the exact retail mnemonics; it matches the retail bytes in
objdiff and in the final link.

## TRKLoadContext

Retail address 0x800C3414, size 0x88.

- Why it cannot be C: it loads a saved `OSContext` into the live CPU before entering the debugger. It overwrites r0, r1 and r2 first, reloads either r5–r31 or r13–r31 with `lmw` depending on the context's state bit, restores CR, LR, CTR and XER by hand (`mtcrf`, `mtlr`, `mtctr`, `mtxer`), clears MSR[EE|RI] with `mfmsr`/`rlwinm`/`mtmsr`, and parks r2, r3 and r4 in SPRG1–3 (`mtsprg`) the way the exception vector expects. MWCC never writes r1/r2, SPRGs or the MSR from C, and C code cannot run while the stack pointer is being replaced.
- Other decompilations: [zeldaret/tp, commit c8fa8c9e, `libs/TRK_MINNOW_DOLPHIN/debugger/embedded/MetroTRK/Os/dolphin/dolphin_trk_glue.c`](https://github.com/zeldaret/tp/blob/c8fa8c9e2aab72cf4e5db0e5d1c84a9ea6ee6eb0/libs/TRK_MINNOW_DOLPHIN/debugger/embedded/MetroTRK/Os/dolphin/dolphin_trk_glue.c#L134), [zeldaret/tww, commit f5234ec8, `src/TRK_MINNOW_DOLPHIN/Os/dolphin/dolphin_trk_glue.c`](https://github.com/zeldaret/tww/blob/f5234ec8f4b8f4119b8db4a018c1ca25572988b1/src/TRK_MINNOW_DOLPHIN/Os/dolphin/dolphin_trk_glue.c#L134) and [doldecomp/melee, commit e78dc834, `src/MetroTRK/dolphin_trk_glue.c`](https://github.com/doldecomp/melee/blob/e78dc8349c587ca5d2f97322834b625d3bd1abc3/src/MetroTRK/dolphin_trk_glue.c#L16) keep `TRKLoadContext` as an `asm` function body.
- Origin: Metrowerks MetroTRK for Dolphin, `dolphin_trk_glue.c`, hand-written assembly in the vendor source; the exact CodeWarrior/MetroTRK build in Colosseum is not established.
- External branch targets: the final `b TRKInterruptHandler` (0x800C3498) enters `TRKInterruptHandler` (0x800C0EAC) with the loaded context in place and r3 holding the exception id, exactly as if the exception vector had fired. It is a jump, not a call: the stack pointer and link register now belong to the loaded context, so there is no frame and nothing to return to. The retail relocation at that instruction names `TRKInterruptHandler`.
