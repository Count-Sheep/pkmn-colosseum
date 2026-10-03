# Dolphin SDK OSContext.c context primitives in Pokémon Colosseum (GC6E01)

OSContext.c occupies .text 0x8009B914–0x8009C2E0 and .data 0x803107E0–0x803109B8
as one retail translation unit. Its C functions (OSGetCurrentContext,
OSClearContext, OSDumpContext, __OSContextInit) cannot link without the
eleven hand-written assembly routines between them. Only the routines named
below are admitted. Each body's instructions are the exact retail mnemonics;
the source matches the retail bytes in objdiff and in the final link.

Common origin: Nintendo Dolphin SDK, `os/OSContext.c` (the GameCube SDK
revision linked into Colosseum's main.dol). The routines are written as `asm`
functions in the SDK source itself, as the Dolphin SDK decompilations below
also reproduce.

## __OSLoadFPUContext

Retail address 0x8009B914.

- Why it cannot be C: it restores FPSCR with `mtfsf`, reads HID2 with `mfspr` to decide whether to restore the paired singles with 32 `psq_l`, then reloads all 32 FPRs with `lfd`, frameless, with the context in r4. MWCC emits no `mtfsf`, `psq_l` or SPR reads from C.
- Other decompilations: [zeldaret/tww, commit f5234ec8, `src/dolphin/os/OSContext.c`](https://github.com/zeldaret/tww/blob/f5234ec8f4b8f4119b8db4a018c1ca25572988b1/src/dolphin/os/OSContext.c#L9) and [doldecomp/melee, commit e78dc834, `libs/dolphin/src/dolphin/os/OSContext.c`](https://github.com/doldecomp/melee/blob/e78dc8349c587ca5d2f97322834b625d3bd1abc3/libs/dolphin/src/dolphin/os/OSContext.c#L11) keep `__OSLoadFPUContext` as an `asm` function body.
- Origin: Nintendo Dolphin SDK `os/OSContext.c`, hand-written assembly in the vendor source; the exact SDK build date in Colosseum is not established.

## __OSSaveFPUContext

Retail address 0x8009BA38.

- Why it cannot be C: it saves all 32 FPRs with `stfd`, reads FPSCR with `mffs`, reads HID2 with `mfspr` and saves the paired singles with 32 `psq_st`, frameless, with the context in r5. MWCC emits none of `mffs`, `psq_st` or SPR reads from C.
- Other decompilations: [zeldaret/tww, commit f5234ec8, `src/dolphin/os/OSContext.c`](https://github.com/zeldaret/tww/blob/f5234ec8f4b8f4119b8db4a018c1ca25572988b1/src/dolphin/os/OSContext.c#L97) and [doldecomp/melee, commit e78dc834, `libs/dolphin/src/dolphin/os/OSContext.c`](https://github.com/doldecomp/melee/blob/e78dc8349c587ca5d2f97322834b625d3bd1abc3/libs/dolphin/src/dolphin/os/OSContext.c#L97) keep `__OSSaveFPUContext` as an `asm` function body.
- Origin: Nintendo Dolphin SDK `os/OSContext.c`, hand-written assembly in the vendor source; the exact SDK build date in Colosseum is not established.

## OSSaveFPUContext

Retail address 0x8009BB60.

- Why it cannot be C: it is a frameless tail call that moves the context into r5 (`addi r5, r3, 0`) and branches to __OSSaveFPUContext, keeping r3/r4 untouched as that routine's unused arguments; MWCC would set up a call with a frame.
- Other decompilations: [zeldaret/tww, commit f5234ec8, `src/dolphin/os/OSContext.c`](https://github.com/zeldaret/tww/blob/f5234ec8f4b8f4119b8db4a018c1ca25572988b1/src/dolphin/os/OSContext.c#L185) and [doldecomp/melee, commit e78dc834, `libs/dolphin/src/dolphin/os/OSContext.c`](https://github.com/doldecomp/melee/blob/e78dc8349c587ca5d2f97322834b625d3bd1abc3/libs/dolphin/src/dolphin/os/OSContext.c#L196) keep `OSSaveFPUContext` as an `asm` function body.
- Origin: Nintendo Dolphin SDK `os/OSContext.c`, hand-written assembly in the vendor source; the exact SDK build date in Colosseum is not established.
- External branch targets: the final `b __OSSaveFPUContext` (0x8009BB64) jumps to __OSSaveFPUContext at 0x8009BA38 with r5 set up as the context; it is a tail jump, not a call, so no frame or `bl` is emitted. The retail relocation at that instruction names __OSSaveFPUContext.

## OSSetCurrentContext

Retail address 0x8009BB68.

- Why it cannot be C: it stores the current and physical context pointers at 0x800000D4/0x800000C0 and toggles MSR[FP] and SRR1[FP] with `mfmsr`/`mtmsr` and `isync`, frameless. MWCC emits no `mtmsr`/`isync` from C.
- Other decompilations: [zeldaret/tww, commit f5234ec8, `src/dolphin/os/OSContext.c`](https://github.com/zeldaret/tww/blob/f5234ec8f4b8f4119b8db4a018c1ca25572988b1/src/dolphin/os/OSContext.c#L196) and [doldecomp/melee, commit e78dc834, `libs/dolphin/src/dolphin/os/OSContext.c`](https://github.com/doldecomp/melee/blob/e78dc8349c587ca5d2f97322834b625d3bd1abc3/libs/dolphin/src/dolphin/os/OSContext.c#L205) keep `OSSetCurrentContext` as an `asm` function body.
- Origin: Nintendo Dolphin SDK `os/OSContext.c`, hand-written assembly in the vendor source; the exact SDK build date in Colosseum is not established.

## OSSaveContext

Retail address 0x8009BBD0.

- Why it cannot be C: it saves r13-r31 with `stmw`, GQR1-7 with `mfspr`, and CR, LR, MSR, CTR and XER with `mfcr`/`mflr`/`mfmsr`/`mfctr`/`mfxer`, and returns 0 to the caller while recording 1 as the resumed return value. That setjmp-style register capture is not expressible in C.
- Other decompilations: [zeldaret/tww, commit f5234ec8, `src/dolphin/os/OSContext.c`](https://github.com/zeldaret/tww/blob/f5234ec8f4b8f4119b8db4a018c1ca25572988b1/src/dolphin/os/OSContext.c#L234) and [doldecomp/melee, commit e78dc834, `libs/dolphin/src/dolphin/os/OSContext.c`](https://github.com/doldecomp/melee/blob/e78dc8349c587ca5d2f97322834b625d3bd1abc3/libs/dolphin/src/dolphin/os/OSContext.c#L246) keep `OSSaveContext` as an `asm` function body.
- Origin: Nintendo Dolphin SDK `os/OSContext.c`, hand-written assembly in the vendor source; the exact SDK build date in Colosseum is not established.

## OSLoadContext

Retail address 0x8009BC50.

- Why it cannot be C: it restores every GPR (`lmw`), GQR1-7 (`mtspr`), CR (`mtcrf`), LR, CTR and XER, clears MSR[EE/RI], loads SRR0/SRR1 and returns with `rfi`; it also rewinds SRR0 out of OSDisableInterrupts. MWCC never emits `rfi` or these SPR writes from C.
- Other decompilations: [zeldaret/tww, commit f5234ec8, `src/dolphin/os/OSContext.c`](https://github.com/zeldaret/tww/blob/f5234ec8f4b8f4119b8db4a018c1ca25572988b1/src/dolphin/os/OSContext.c#L280) and [doldecomp/melee, commit e78dc834, `libs/dolphin/src/dolphin/os/OSContext.c`](https://github.com/doldecomp/melee/blob/e78dc8349c587ca5d2f97322834b625d3bd1abc3/libs/dolphin/src/dolphin/os/OSContext.c#L288) keep `OSLoadContext` as an `asm` function body.
- Origin: Nintendo Dolphin SDK `os/OSContext.c`, hand-written assembly in the vendor source; the exact SDK build date in Colosseum is not established.

## OSGetStackPointer

Retail address 0x8009BD28.

- Why it cannot be C: it returns r1 (`mr r3, r1`) without a frame; C has no access to the stack pointer register.
- Other decompilations: [zeldaret/tww, commit f5234ec8, `src/dolphin/os/OSContext.c`](https://github.com/zeldaret/tww/blob/f5234ec8f4b8f4119b8db4a018c1ca25572988b1/src/dolphin/os/OSContext.c#L349) and [doldecomp/melee, commit e78dc834, `libs/dolphin/src/dolphin/os/OSContext.c`](https://github.com/doldecomp/melee/blob/e78dc8349c587ca5d2f97322834b625d3bd1abc3/libs/dolphin/src/dolphin/os/OSContext.c#L362) keep `OSGetStackPointer` as an `asm` function body.
- Origin: Nintendo Dolphin SDK `os/OSContext.c`, hand-written assembly in the vendor source; the exact SDK build date in Colosseum is not established.

## OSSwitchFiber

Retail address 0x8009BD30.

- Why it cannot be C: it saves LR, builds a frame on the new stack (`stwu r5, -8(r4)`), moves r1 to it, calls the fiber with `blrl` and restores the old stack pointer from the frame. Switching r1 is not expressible in C.
- Other decompilations: [doldecomp/melee, commit e78dc834, `libs/dolphin/src/dolphin/os/OSContext.c`](https://github.com/doldecomp/melee/blob/e78dc8349c587ca5d2f97322834b625d3bd1abc3/libs/dolphin/src/dolphin/os/OSContext.c#L382) keep `OSSwitchFiber` as an `asm` function body.
- Origin: Nintendo Dolphin SDK `os/OSContext.c`, hand-written assembly in the vendor source; the exact SDK build date in Colosseum is not established.

## OSInitContext

Retail address 0x8009BD84.

- Why it cannot be C: it fills the context with 47 `stw`s, including r2 and r13 (the small-data bases) read straight from the registers, frameless, then tail-branches to OSClearContext. C cannot read r2/r13, and MWCC would not emit the frameless tail branch.
- Other decompilations: [zeldaret/tww, commit f5234ec8, `src/dolphin/os/OSContext.c`](https://github.com/zeldaret/tww/blob/f5234ec8f4b8f4119b8db4a018c1ca25572988b1/src/dolphin/os/OSContext.c#L369) and [doldecomp/melee, commit e78dc834, `libs/dolphin/src/dolphin/os/OSContext.c`](https://github.com/doldecomp/melee/blob/e78dc8349c587ca5d2f97322834b625d3bd1abc3/libs/dolphin/src/dolphin/os/OSContext.c#L410) keep `OSInitContext` as an `asm` function body.
- Origin: Nintendo Dolphin SDK `os/OSContext.c`, hand-written assembly in the vendor source; the exact SDK build date in Colosseum is not established.
- External branch targets: the final `b OSClearContext` jumps to OSClearContext at 0x8009BD60 after filling the context; it is a tail jump, not a call, so OSInitContext has no frame. The retail relocation at that instruction names OSClearContext.

## OSSwitchFPUContext

Retail address 0x8009C0E8.

- Why it cannot be C: it is the FP-unavailable exception handler: it enables MSR[FP] (`mfmsr`/`mtmsr`/`isync`), sets SRR1[FP] with `mtsrr1`, saves and loads FPU contexts, restores CR/LR/CTR/XER and SRR0 (`mtcrf`, `mtsrr0`) and returns with `rfi`. MWCC never emits `rfi`, `mtsrr0` or `mtsrr1` from C.
- Other decompilations: [zeldaret/tww, commit f5234ec8, `src/dolphin/os/OSContext.c`](https://github.com/zeldaret/tww/blob/f5234ec8f4b8f4119b8db4a018c1ca25572988b1/src/dolphin/os/OSContext.c#L478) and [doldecomp/melee, commit e78dc834, `libs/dolphin/src/dolphin/os/OSContext.c`](https://github.com/doldecomp/melee/blob/e78dc8349c587ca5d2f97322834b625d3bd1abc3/libs/dolphin/src/dolphin/os/OSContext.c#L532) keep `OSSwitchFPUContext` as an `asm` function body.
- Origin: Nintendo Dolphin SDK `os/OSContext.c`, hand-written assembly in the vendor source; the exact SDK build date in Colosseum is not established.

## OSFillFPUContext

Retail address 0x8009C1B4.

- Why it cannot be C: it enables MSR[FP] with `mfmsr`/`mtmsr`/`isync`, stores all 32 FPRs with `stfd`, FPSCR via `mffs`, and the paired singles with `psq_st` when HID2 (read with `mfspr`) says they are enabled. MWCC emits none of these from C.
- Other decompilations: [zeldaret/tww, commit f5234ec8, `src/dolphin/os/OSContext.c`](https://github.com/zeldaret/tww/blob/f5234ec8f4b8f4119b8db4a018c1ca25572988b1/src/dolphin/os/OSContext.c#L530) and [doldecomp/melee, commit e78dc834, `libs/dolphin/src/dolphin/os/OSContext.c`](https://github.com/doldecomp/melee/blob/e78dc8349c587ca5d2f97322834b625d3bd1abc3/libs/dolphin/src/dolphin/os/OSContext.c#L584) keep `OSFillFPUContext` as an `asm` function body.
- Origin: Nintendo Dolphin SDK `os/OSContext.c`, hand-written assembly in the vendor source; the exact SDK build date in Colosseum is not established.
