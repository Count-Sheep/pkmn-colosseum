# HAL sysdolphin debug.c HSD_SaveContext in Pokémon Colosseum (GC6E01)

`hsd/debug.c` covers .text 0x80196CE0–0x80196EB4 of HAL Laboratory's
sysdolphin `baselib/debug.c`: `HSD_SaveContext`, `HSD_Panic` and `__assert`,
with the panic context `lbl_80465080` in its .bss. The routine below uses the
exact retail mnemonics and matches the retail bytes; the rest of the unit is C.

## HSD_SaveContext

Retail address 0x80196CE0, size 0x98. `HSD_Panic` calls it to capture the
machine state into the panic context before reporting.

- Why it cannot be C: it is a hand-written variant of the SDK's `OSSaveContext`. It parks r3 in SPRG0 (`mtsprg 0`/`mfsprg 0`) so that every GPR, r3 included, can be stored with one `stmw r0, 0(r3)` into the context, reads all eight graphics quantization registers with `mfspr GQR0`–`GQR7`, stores CR, LR, CTR, XER, SRR0 and SRR1 (`mfcr`, `mflr`, `mfctr`, `mfxer`, `mfsrr0`, `mfsrr1`), sets the context's FP-saved state bit and tail-branches to `OSFillFPUContext` with r3 still pointing at the context. It has no frame and must not disturb any register before saving it, which a C function's prologue would. MWCC never emits SPRG, SRR or GQR moves from C.
- Other decompilations: [doldecomp/gnt4, commit b6c32473, `asm/sysdolphin/debug.s`](https://github.com/doldecomp/gnt4/blob/b6c32473fca8d39facf1cad70f49a3c305e18cf0/asm/sysdolphin/debug.s#L5-L44) keeps `HSD_SaveContext` (sysdolphin in Naruto: Gekitou Ninja Taisen 4) as assembly with the same body, ending in `b OSFillFPUContext` ([L44](https://github.com/doldecomp/gnt4/blob/b6c32473fca8d39facf1cad70f49a3c305e18cf0/asm/sysdolphin/debug.s#L44)); [doldecomp/kar, commit 3ed51a1a, `asm/sysdolphin/debug.s`](https://github.com/doldecomp/kar/blob/3ed51a1a008a05c33225cc286c046b8f90fb3d0e/asm/sysdolphin/debug.s#L5-L44) does the same for Kirby Air Ride, where the target is still unnamed (`b func_803D4BBC`, [L44](https://github.com/doldecomp/kar/blob/3ed51a1a008a05c33225cc286c046b8f90fb3d0e/asm/sysdolphin/debug.s#L44)). doldecomp/melee's older sysdolphin has no HSD_SaveContext; its `HSD_Panic` calls the SDK's `OSSaveContext` instead.
- Origin: HAL Laboratory sysdolphin library, `baselib/debug.c`, hand-written assembly in the vendor source.
- External branch targets: the final `b OSFillFPUContext` (0x80196D74) jumps to the Dolphin SDK's OSFillFPUContext at 0x8009C1B4 (OSContext.c) with r3 = the context, which saves the FPRs, paired-single halves and FPSCR and returns to HSD_SaveContext's caller. It is a tail jump, not a call: HSD_SaveContext has no frame and must leave LR as its caller's, so MWCC's `bl` with a prologue would not reproduce it. The retail relocation at that instruction names OSFillFPUContext.
