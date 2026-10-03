# MetroTRK exception entry and return routines in Pokémon Colosseum (GC6E01)

`trk/TRKInterrupt.c` occupies .text 0x800C0EAC–0x800C1300 as one translation
unit. It holds MetroTRK's exception entry (`TRKInterruptHandler`,
`TRKExceptionHandler`), its return path to the debuggee (`TRKSwapAndGo`,
`TRKInterruptHandlerEnableInterrupts`), the FPSCR helpers (see
`trk_fpscr.md`) and the C routine `TRKTargetAccessARAM`. The four routines
below come from MetroTRK's `Processor/ppc/Generic/targimpl.c`. Only they are
admitted, with the exact retail mnemonics; each matches the retail bytes in
objdiff and in the final link.

Common origin: Metrowerks MetroTRK for Dolphin (`TRK_MINNOW_DOLPHIN`),
`targimpl.c`, linked into Colosseum's main.dol from the CodeWarrior debugger
library.

## TRKInterruptHandler

Retail address 0x800C0EAC, size 0x194.

- Why it cannot be C: it is the common exception vector body. It runs with only r2 free, saves state through SPRG registers and SRR0/SRR1 (`mtsrr0`, `mtsrr1`, `mfsprg`, `mtsprg`), rewrites the MSR (`mtmsr` between `sync`s), returns from the exception with `rfi` on one path, and on the other stores every GPR with `stmw` and reloads the debugger's registers with `lmw`, `mtctr`, `mtxer`, `mtdsisr` and `mtdar`. MWCC emits none of these from C and cannot write code that runs without a valid stack.
- Other decompilations: [zeldaret/tp, commit c8fa8c9e, `libs/TRK_MINNOW_DOLPHIN/debugger/embedded/MetroTRK/Processor/ppc/Generic/targimpl.c`](https://github.com/zeldaret/tp/blob/c8fa8c9e2aab72cf4e5db0e5d1c84a9ea6ee6eb0/libs/TRK_MINNOW_DOLPHIN/debugger/embedded/MetroTRK/Processor/ppc/Generic/targimpl.c#L877), [zeldaret/tww, commit f5234ec8, `src/TRK_MINNOW_DOLPHIN/ppc/Generic/targimpl.c`](https://github.com/zeldaret/tww/blob/f5234ec8f4b8f4119b8db4a018c1ca25572988b1/src/TRK_MINNOW_DOLPHIN/ppc/Generic/targimpl.c#L877) and [doldecomp/melee, commit e78dc834, `src/MetroTRK/targimpl.c`](https://github.com/doldecomp/melee/blob/e78dc8349c587ca5d2f97322834b625d3bd1abc3/src/MetroTRK/targimpl.c#L498) keep `TRKInterruptHandler` as an `asm` function body.
- Origin: Metrowerks MetroTRK for Dolphin, `targimpl.c`, hand-written assembly in the vendor source; the exact CodeWarrior/MetroTRK build in Colosseum is not established.
- External branch targets: `bne TRKExceptionHandler` (0x800C0F90) enters `TRKExceptionHandler` (0x800C1040) when an exception arrives while the debugger itself is running; `b TRKPostInterruptEvent` (0x800C103C) ends the handler by jumping to `TRKPostInterruptEvent` (0x800C195C) after switching to the debugger's saved registers and stack. Both are jumps, not calls: the routine has no frame, no valid link register to return through, and never comes back. The retail relocations at those instructions name these two functions.

## TRKExceptionHandler

Retail address 0x800C1040, size 0x9C.

- Why it cannot be C: it handles an exception taken inside the debugger. It records the exception id and SRR0, steps SRR0 past the faulting instruction for the listed exception classes (`mfsrr0`/`mtsrr0`), restores CR, r2 and r3 from SPRG registers (`mtcrf`, `mfsprg`) and returns with `rfi`. MWCC emits no SRR/SPRG access and no `rfi` from C.
- Other decompilations: [zeldaret/tp, commit c8fa8c9e, `libs/TRK_MINNOW_DOLPHIN/debugger/embedded/MetroTRK/Processor/ppc/Generic/targimpl.c`](https://github.com/zeldaret/tp/blob/c8fa8c9e2aab72cf4e5db0e5d1c84a9ea6ee6eb0/libs/TRK_MINNOW_DOLPHIN/debugger/embedded/MetroTRK/Processor/ppc/Generic/targimpl.c#L986), [zeldaret/tww, commit f5234ec8, `src/TRK_MINNOW_DOLPHIN/ppc/Generic/targimpl.c`](https://github.com/zeldaret/tww/blob/f5234ec8f4b8f4119b8db4a018c1ca25572988b1/src/TRK_MINNOW_DOLPHIN/ppc/Generic/targimpl.c#L986) and [doldecomp/melee, commit e78dc834, `src/MetroTRK/targimpl.c`](https://github.com/doldecomp/melee/blob/e78dc8349c587ca5d2f97322834b625d3bd1abc3/src/MetroTRK/targimpl.c#L609) keep `TRKExceptionHandler` as an `asm` function body.
- Origin: Metrowerks MetroTRK for Dolphin, `targimpl.c`, hand-written assembly in the vendor source; the exact CodeWarrior/MetroTRK build in Colosseum is not established.

## TRKSwapAndGo

Retail address 0x800C10DC, size 0xC4.

- Why it cannot be C: it hands control back to the debuggee. It saves the debugger's GPRs with `stmw r0` and its MSR, LR, CTR, XER, DSISR and DAR by hand, masks MSR[EE|RI] (`nor`/`and`/`mtmsr`), and then either returns into the debugger or restores the debuggee's full register file (`lmw`, `mtsrr0`, `mtcrf`, `mtctr`, `mtxer`) and resumes it with `rfi`. None of this is expressible in C.
- Other decompilations: [zeldaret/tp, commit c8fa8c9e, `libs/TRK_MINNOW_DOLPHIN/debugger/embedded/MetroTRK/Processor/ppc/Generic/targimpl.c`](https://github.com/zeldaret/tp/blob/c8fa8c9e2aab72cf4e5db0e5d1c84a9ea6ee6eb0/libs/TRK_MINNOW_DOLPHIN/debugger/embedded/MetroTRK/Processor/ppc/Generic/targimpl.c#L1033), [zeldaret/tww, commit f5234ec8, `src/TRK_MINNOW_DOLPHIN/ppc/Generic/targimpl.c`](https://github.com/zeldaret/tww/blob/f5234ec8f4b8f4119b8db4a018c1ca25572988b1/src/TRK_MINNOW_DOLPHIN/ppc/Generic/targimpl.c#L1033) and [doldecomp/melee, commit e78dc834, `src/MetroTRK/targimpl.c`](https://github.com/doldecomp/melee/blob/e78dc8349c587ca5d2f97322834b625d3bd1abc3/src/MetroTRK/targimpl.c#L694) keep `TRKSwapAndGo` as an `asm` function body.
- Origin: Metrowerks MetroTRK for Dolphin, `targimpl.c`, hand-written assembly in the vendor source; the exact CodeWarrior/MetroTRK build in Colosseum is not established.
- External branch targets: `b TRKInterruptHandlerEnableInterrupts` (0x800C1154) jumps to `TRKInterruptHandlerEnableInterrupts` (0x800C11A0) when input is pending, so the debugger resumes instead of the debuggee. It is a jump, not a call: by then r1 and the link register already hold the debugger's saved state, so the routine has no frame to return to. The retail relocation at that instruction names `TRKInterruptHandlerEnableInterrupts`.

## TRKInterruptHandlerEnableInterrupts

Retail address 0x800C11A0, size 0x54.

- Why it cannot be C: it restores the debugger's saved MSR (`mtmsr` between `sync`s), LR, CTR, XER, DSISR and DAR and its whole GPR file (`lmw r3`, then r0, r1 and r2 last, so the base register is overwritten by the final load). MWCC never reloads r1/r2 or uses `mtdsisr`/`mtdar` from C.
- Other decompilations: [zeldaret/tp, commit c8fa8c9e, `libs/TRK_MINNOW_DOLPHIN/debugger/embedded/MetroTRK/Processor/ppc/Generic/targimpl.c`](https://github.com/zeldaret/tp/blob/c8fa8c9e2aab72cf4e5db0e5d1c84a9ea6ee6eb0/libs/TRK_MINNOW_DOLPHIN/debugger/embedded/MetroTRK/Processor/ppc/Generic/targimpl.c#L1089), [zeldaret/tww, commit f5234ec8, `src/TRK_MINNOW_DOLPHIN/ppc/Generic/targimpl.c`](https://github.com/zeldaret/tww/blob/f5234ec8f4b8f4119b8db4a018c1ca25572988b1/src/TRK_MINNOW_DOLPHIN/ppc/Generic/targimpl.c#L1089) and [doldecomp/melee, commit e78dc834, `src/MetroTRK/targimpl.c`](https://github.com/doldecomp/melee/blob/e78dc8349c587ca5d2f97322834b625d3bd1abc3/src/MetroTRK/targimpl.c#L770) keep `TRKInterruptHandlerEnableInterrupts` as an `asm` function body.
- Origin: Metrowerks MetroTRK for Dolphin, `targimpl.c`, hand-written assembly in the vendor source; the exact CodeWarrior/MetroTRK build in Colosseum is not established.
- External branch targets: the final `b TRKPostInterruptEvent` (0x800C11F0) jumps to `TRKPostInterruptEvent` (0x800C195C) with the debugger's registers restored. It is a tail jump, not a call: the routine has just reloaded r1 and LR from the saved state and has no frame of its own. The retail relocation at that instruction names `TRKPostInterruptEvent`.
