# Dolphin PPCArch.c primitives in Pokémon Colosseum (GC6E01)

`dolphin/os/PPCArch.c` covers .text 0x80097FFC–0x80098108 of the Dolphin
SDK's `base/PPCArch.c`. The SPR/MSR primitives (`PPCMfmsr` .. `PPCMtwpar`,
`PPCSync`, `PPCHalt`) are frameless `asm` functions that move a special
register to or from r3; `PPCSetFpNonIEEEMode`, the next PPCArch.c routine in
retail (0x80098108), sits at the head of the following range unit. The two
FPSCR accessors are C functions whose bodies are inline `asm` blocks with a
memory round trip. All of them match the retail bytes in objdiff and in the
final link.

The special-register instructions (`mfmsr`, `mtmsr`, `mfspr`, `mtspr`,
`mtdec`, `mtfsb1`, `sc`) have no C form: MWCC emits none of them from C
source, so each primitive can only exist as assembly.

## PPCMffpscr

Retail address 0x80098080, size 0x20.

- Why it cannot be C: it reads the FPSCR with `mffs` into fp31 and spills it with `stfd` into a stack union to return the low word. The asm names fp31 explicitly, which makes MWCC save and restore f31 around the block (the `stfd f31`/`lfd f31` frame in retail); the compiler intrinsic `__mffs` allocates f0 and produces no such frame.
- Other decompilations: [zeldaret/tww, commit f5234ec8, `src/dolphin/base/PPCArch.c`](https://github.com/zeldaret/tww/blob/f5234ec8f4b8f4119b8db4a018c1ca25572988b1/src/dolphin/base/PPCArch.c#L63-L72) writes `PPCMffpscr` as C with the same inline `asm { mffs fp31; stfd fp31, m.f }` block.
- Origin: Nintendo Dolphin SDK, `base/PPCArch.c`, inline assembly in the vendor source.

## PPCMtfpscr

Retail address 0x800980A0, size 0x28.

- Why it cannot be C: it builds a double from a zero pad word and the new FPSCR value with `li`/`stw`, loads it into fp31 with `lfd` and writes the FPSCR with `mtfsf 0xff`. As above, naming fp31 in the asm gives retail's f31 save/restore frame; `__setflm` and plain C allocate f0 and emit no frame.
- Other decompilations: [zeldaret/tww, commit f5234ec8, `src/dolphin/base/PPCArch.c`](https://github.com/zeldaret/tww/blob/f5234ec8f4b8f4119b8db4a018c1ca25572988b1/src/dolphin/base/PPCArch.c#L74-L84) writes `PPCMtfpscr` as C with the same inline `asm { li r4, 0; stw r4, m.u.fpscr_pad; stw newFPSCR, m.u.fpscr; lfd fp31, m.f; mtfsf 0xff, fp31 }` block.
- Origin: Nintendo Dolphin SDK, `base/PPCArch.c`, inline assembly in the vendor source.

## PPCMfmsr

Retail address 0x80097FFC, size 0x8; unit `dolphin/os/PPCArch.c`.

- Why it cannot be C: it returns the machine state register with `mfmsr r3; blr`, no frame. MWCC never emits `mfmsr` from C.
- Other decompilations: [doldecomp/dolsdk2004, commit 2328b416, `src/base/PPCArch.c`](https://github.com/doldecomp/dolsdk2004/blob/2328b4164b1a98422a2255d83ce9a5a7548990cc/src/base/PPCArch.c#L4-L8) and [zeldaret/tww, commit f5234ec8, `src/dolphin/base/PPCArch.c`](https://github.com/zeldaret/tww/blob/f5234ec8f4b8f4119b8db4a018c1ca25572988b1/src/dolphin/base/PPCArch.c#L5) keep `PPCMfmsr` as an `asm` function with the same body.
- Origin: Nintendo Dolphin SDK, `base/PPCArch.c`, hand-written assembly in the vendor source.

## PPCMtmsr

Retail address 0x80098004, size 0x8; unit `dolphin/os/PPCArch.c`.

- Why it cannot be C: it writes its argument to the machine state register with `mtmsr r3; blr`, no frame. MWCC never emits `mtmsr` from C.
- Other decompilations: [doldecomp/dolsdk2004, commit 2328b416, `src/base/PPCArch.c`](https://github.com/doldecomp/dolsdk2004/blob/2328b4164b1a98422a2255d83ce9a5a7548990cc/src/base/PPCArch.c#L10-L14) and [zeldaret/tww, commit f5234ec8, `src/dolphin/base/PPCArch.c`](https://github.com/zeldaret/tww/blob/f5234ec8f4b8f4119b8db4a018c1ca25572988b1/src/dolphin/base/PPCArch.c#L11) keep `PPCMtmsr` as an `asm` function with the same body.
- Origin: Nintendo Dolphin SDK, `base/PPCArch.c`, hand-written assembly in the vendor source.

## PPCMfhid0

Retail address 0x8009800C, size 0x8; unit `dolphin/os/PPCArch.c`.

- Why it cannot be C: it reads hardware-implementation register 0 with `mfspr r3, HID0` (SPR 1008). MWCC never emits `mfspr` from C.
- Other decompilations: [doldecomp/dolsdk2004, commit 2328b416, `src/base/PPCArch.c`](https://github.com/doldecomp/dolsdk2004/blob/2328b4164b1a98422a2255d83ce9a5a7548990cc/src/base/PPCArch.c#L37-L41) and [zeldaret/tww, commit f5234ec8, `src/dolphin/base/PPCArch.c`](https://github.com/zeldaret/tww/blob/f5234ec8f4b8f4119b8db4a018c1ca25572988b1/src/dolphin/base/PPCArch.c#L17) keep `PPCMfhid0` as an `asm` function with the same body.
- Origin: Nintendo Dolphin SDK, `base/PPCArch.c`, hand-written assembly in the vendor source.

## PPCMthid0

Retail address 0x80098014, size 0x8; unit `dolphin/os/PPCArch.c`.

- Why it cannot be C: it writes HID0 with `mtspr HID0, r3` (SPR 1008). MWCC never emits `mtspr` from C.
- Other decompilations: [doldecomp/dolsdk2004, commit 2328b416, `src/base/PPCArch.c`](https://github.com/doldecomp/dolsdk2004/blob/2328b4164b1a98422a2255d83ce9a5a7548990cc/src/base/PPCArch.c#L43-L47) and [zeldaret/tww, commit f5234ec8, `src/dolphin/base/PPCArch.c`](https://github.com/zeldaret/tww/blob/f5234ec8f4b8f4119b8db4a018c1ca25572988b1/src/dolphin/base/PPCArch.c#L23) keep `PPCMthid0` as an `asm` function with the same body.
- Origin: Nintendo Dolphin SDK, `base/PPCArch.c`, hand-written assembly in the vendor source.

## PPCMfl2cr

Retail address 0x8009801C, size 0x8; unit `dolphin/os/PPCArch.c`.

- Why it cannot be C: it reads the L2 cache control register with `mfspr r3, L2CR` (SPR 1017). MWCC never emits `mfspr` from C.
- Other decompilations: [doldecomp/dolsdk2004, commit 2328b416, `src/base/PPCArch.c`](https://github.com/doldecomp/dolsdk2004/blob/2328b4164b1a98422a2255d83ce9a5a7548990cc/src/base/PPCArch.c#L55-L59) and [zeldaret/tww, commit f5234ec8, `src/dolphin/base/PPCArch.c`](https://github.com/zeldaret/tww/blob/f5234ec8f4b8f4119b8db4a018c1ca25572988b1/src/dolphin/base/PPCArch.c#L29) keep `PPCMfl2cr` as an `asm` function with the same body.
- Origin: Nintendo Dolphin SDK, `base/PPCArch.c`, hand-written assembly in the vendor source.

## PPCMtl2cr

Retail address 0x80098024, size 0x8; unit `dolphin/os/PPCArch.c`.

- Why it cannot be C: it writes L2CR with `mtspr L2CR, r3` (SPR 1017). MWCC never emits `mtspr` from C.
- Other decompilations: [doldecomp/dolsdk2004, commit 2328b416, `src/base/PPCArch.c`](https://github.com/doldecomp/dolsdk2004/blob/2328b4164b1a98422a2255d83ce9a5a7548990cc/src/base/PPCArch.c#L61-L65) and [zeldaret/tww, commit f5234ec8, `src/dolphin/base/PPCArch.c`](https://github.com/zeldaret/tww/blob/f5234ec8f4b8f4119b8db4a018c1ca25572988b1/src/dolphin/base/PPCArch.c#L35) keep `PPCMtl2cr` as an `asm` function with the same body.
- Origin: Nintendo Dolphin SDK, `base/PPCArch.c`, hand-written assembly in the vendor source.

## PPCMtdec

Retail address 0x8009802C, size 0x8; unit `dolphin/os/PPCArch.c`.

- Why it cannot be C: it writes the decrementer with `mtdec r3`. MWCC never emits `mtdec` (`mtspr 22`) from C.
- Other decompilations: [doldecomp/dolsdk2004, commit 2328b416, `src/base/PPCArch.c`](https://github.com/doldecomp/dolsdk2004/blob/2328b4164b1a98422a2255d83ce9a5a7548990cc/src/base/PPCArch.c#L67-L71) and [zeldaret/tww, commit f5234ec8, `src/dolphin/base/PPCArch.c`](https://github.com/zeldaret/tww/blob/f5234ec8f4b8f4119b8db4a018c1ca25572988b1/src/dolphin/base/PPCArch.c#L41) keep `PPCMtdec` as an `asm` function with the same body.
- Origin: Nintendo Dolphin SDK, `base/PPCArch.c`, hand-written assembly in the vendor source.

## PPCSync

Retail address 0x80098034, size 0x8; unit `dolphin/os/PPCArch.c`.

- Why it cannot be C: it executes `sc` (system call) and returns; the SDK uses the system-call vector as a synchronising primitive. MWCC never emits `sc` from C.
- Other decompilations: [doldecomp/dolsdk2004, commit 2328b416, `src/base/PPCArch.c`](https://github.com/doldecomp/dolsdk2004/blob/2328b4164b1a98422a2255d83ce9a5a7548990cc/src/base/PPCArch.c#L79-L83) and [zeldaret/tww, commit f5234ec8, `src/dolphin/base/PPCArch.c`](https://github.com/zeldaret/tww/blob/f5234ec8f4b8f4119b8db4a018c1ca25572988b1/src/dolphin/base/PPCArch.c#L47) keep `PPCSync` as an `asm` function with the same body.
- Origin: Nintendo Dolphin SDK, `base/PPCArch.c`, hand-written assembly in the vendor source.

## PPCHalt

Retail address 0x8009803C, size 0x14; unit `dolphin/os/PPCArch.c`.

- Why it cannot be C: it executes `sync` and then spins forever in a `nop; li r3, 0; nop; b` loop with no frame and no return. MWCC never emits `sync` from plain C (only through the `__sync` intrinsic, which does not give this frameless non-returning layout).
- Other decompilations: [doldecomp/dolsdk2004, commit 2328b416, `src/base/PPCArch.c`](https://github.com/doldecomp/dolsdk2004/blob/2328b4164b1a98422a2255d83ce9a5a7548990cc/src/base/PPCArch.c#L102-L110) and [zeldaret/tww, commit f5234ec8, `src/dolphin/base/PPCArch.c`](https://github.com/zeldaret/tww/blob/f5234ec8f4b8f4119b8db4a018c1ca25572988b1/src/dolphin/base/PPCArch.c#L53) keep `PPCHalt` as an `asm` function with the same body.
- Origin: Nintendo Dolphin SDK, `base/PPCArch.c`, hand-written assembly in the vendor source.

## PPCMtmmcr0

Retail address 0x80098050, size 0x8; unit `dolphin/os/PPCArch.c`.

- Why it cannot be C: it writes performance-monitor control register 0 with `mtspr MMCR0, r3` (SPR 952). MWCC never emits `mtspr` from C.
- Other decompilations: [doldecomp/dolsdk2004, commit 2328b416, `src/base/PPCArch.c`](https://github.com/doldecomp/dolsdk2004/blob/2328b4164b1a98422a2255d83ce9a5a7548990cc/src/base/PPCArch.c#L118-L122) keeps `PPCMtmmcr0` as an `asm` function with the same body.
- Origin: Nintendo Dolphin SDK, `base/PPCArch.c`, hand-written assembly in the vendor source.

## PPCMtmmcr1

Retail address 0x80098058, size 0x8; unit `dolphin/os/PPCArch.c`.

- Why it cannot be C: it writes performance-monitor control register 1 with `mtspr MMCR1, r3` (SPR 956). MWCC never emits `mtspr` from C.
- Other decompilations: [doldecomp/dolsdk2004, commit 2328b416, `src/base/PPCArch.c`](https://github.com/doldecomp/dolsdk2004/blob/2328b4164b1a98422a2255d83ce9a5a7548990cc/src/base/PPCArch.c#L130-L134) keeps `PPCMtmmcr1` as an `asm` function with the same body.
- Origin: Nintendo Dolphin SDK, `base/PPCArch.c`, hand-written assembly in the vendor source.

## PPCMtpmc1

Retail address 0x80098060, size 0x8; unit `dolphin/os/PPCArch.c`.

- Why it cannot be C: it writes performance-monitor counter 1 with `mtspr PMC1, r3` (SPR 953). MWCC never emits `mtspr` from C.
- Other decompilations: [doldecomp/dolsdk2004, commit 2328b416, `src/base/PPCArch.c`](https://github.com/doldecomp/dolsdk2004/blob/2328b4164b1a98422a2255d83ce9a5a7548990cc/src/base/PPCArch.c#L142-L146) keeps `PPCMtpmc1` as an `asm` function with the same body.
- Origin: Nintendo Dolphin SDK, `base/PPCArch.c`, hand-written assembly in the vendor source.

## PPCMtpmc2

Retail address 0x80098068, size 0x8; unit `dolphin/os/PPCArch.c`.

- Why it cannot be C: it writes performance-monitor counter 2 with `mtspr PMC2, r3` (SPR 954). MWCC never emits `mtspr` from C.
- Other decompilations: [doldecomp/dolsdk2004, commit 2328b416, `src/base/PPCArch.c`](https://github.com/doldecomp/dolsdk2004/blob/2328b4164b1a98422a2255d83ce9a5a7548990cc/src/base/PPCArch.c#L154-L158) keeps `PPCMtpmc2` as an `asm` function with the same body.
- Origin: Nintendo Dolphin SDK, `base/PPCArch.c`, hand-written assembly in the vendor source.

## PPCMtpmc3

Retail address 0x80098070, size 0x8; unit `dolphin/os/PPCArch.c`.

- Why it cannot be C: it writes performance-monitor counter 3 with `mtspr PMC3, r3` (SPR 957). MWCC never emits `mtspr` from C.
- Other decompilations: [doldecomp/dolsdk2004, commit 2328b416, `src/base/PPCArch.c`](https://github.com/doldecomp/dolsdk2004/blob/2328b4164b1a98422a2255d83ce9a5a7548990cc/src/base/PPCArch.c#L166-L170) keeps `PPCMtpmc3` as an `asm` function with the same body.
- Origin: Nintendo Dolphin SDK, `base/PPCArch.c`, hand-written assembly in the vendor source.

## PPCMtpmc4

Retail address 0x80098078, size 0x8; unit `dolphin/os/PPCArch.c`.

- Why it cannot be C: it writes performance-monitor counter 4 with `mtspr PMC4, r3` (SPR 958). MWCC never emits `mtspr` from C.
- Other decompilations: [doldecomp/dolsdk2004, commit 2328b416, `src/base/PPCArch.c`](https://github.com/doldecomp/dolsdk2004/blob/2328b4164b1a98422a2255d83ce9a5a7548990cc/src/base/PPCArch.c#L178-L182) keeps `PPCMtpmc4` as an `asm` function with the same body.
- Origin: Nintendo Dolphin SDK, `base/PPCArch.c`, hand-written assembly in the vendor source.

## PPCMfhid2

Retail address 0x800980C8, size 0x8; unit `dolphin/os/PPCArch.c`.

- Why it cannot be C: it reads Gekko's HID2 with `mfspr r3, 920`. MWCC never emits `mfspr` from C.
- Other decompilations: [doldecomp/dolsdk2004, commit 2328b416, `src/base/PPCArch.c`](https://github.com/doldecomp/dolsdk2004/blob/2328b4164b1a98422a2255d83ce9a5a7548990cc/src/base/PPCArch.c#L219-L223) and [zeldaret/tww, commit f5234ec8, `src/dolphin/base/PPCArch.c`](https://github.com/zeldaret/tww/blob/f5234ec8f4b8f4119b8db4a018c1ca25572988b1/src/dolphin/base/PPCArch.c#L86) keep `PPCMfhid2` as an `asm` function with the same body.
- Origin: Nintendo Dolphin SDK, `base/PPCArch.c`, hand-written assembly in the vendor source.

## PPCMthid2

Retail address 0x800980D0, size 0x8; unit `dolphin/os/PPCArch.c`.

- Why it cannot be C: it writes Gekko's HID2 with `mtspr 920, r3`. MWCC never emits `mtspr` from C.
- Other decompilations: [doldecomp/dolsdk2004, commit 2328b416, `src/base/PPCArch.c`](https://github.com/doldecomp/dolsdk2004/blob/2328b4164b1a98422a2255d83ce9a5a7548990cc/src/base/PPCArch.c#L225-L229) and [zeldaret/tww, commit f5234ec8, `src/dolphin/base/PPCArch.c`](https://github.com/zeldaret/tww/blob/f5234ec8f4b8f4119b8db4a018c1ca25572988b1/src/dolphin/base/PPCArch.c#L92) keep `PPCMthid2` as an `asm` function with the same body.
- Origin: Nintendo Dolphin SDK, `base/PPCArch.c`, hand-written assembly in the vendor source.

## PPCMtwpar

Retail address 0x800980D8, size 0x8; unit `dolphin/os/PPCArch.c`.

- Why it cannot be C: it writes the write-gather pipe address register with `mtspr WPAR, r3` (SPR 921). MWCC never emits `mtspr` from C.
- Other decompilations: [doldecomp/dolsdk2004, commit 2328b416, `src/base/PPCArch.c`](https://github.com/doldecomp/dolsdk2004/blob/2328b4164b1a98422a2255d83ce9a5a7548990cc/src/base/PPCArch.c#L238-L242) and [zeldaret/tww, commit f5234ec8, `src/dolphin/base/PPCArch.c`](https://github.com/zeldaret/tww/blob/f5234ec8f4b8f4119b8db4a018c1ca25572988b1/src/dolphin/base/PPCArch.c#L98) keep `PPCMtwpar` as an `asm` function with the same body.
- Origin: Nintendo Dolphin SDK, `base/PPCArch.c`, hand-written assembly in the vendor source.

## PPCSetFpNonIEEEMode

Retail address 0x80098108, size 0x8; unit `dolphin/sdk_r48_80098108_prefix.c` (source `src/dolphin/sdk_range_80098108.c`).

- Why it cannot be C: it sets FPSCR[NI] with `mtfsb1 29; blr`, no frame. MWCC never emits `mtfsb1` from C.
- Other decompilations: [doldecomp/dolsdk2004, commit 2328b416, `src/base/PPCArch.c`](https://github.com/doldecomp/dolsdk2004/blob/2328b4164b1a98422a2255d83ce9a5a7548990cc/src/base/PPCArch.c#L288-L292) and [zeldaret/tww, commit f5234ec8, `src/dolphin/base/PPCArch.c`](https://github.com/zeldaret/tww/blob/f5234ec8f4b8f4119b8db4a018c1ca25572988b1/src/dolphin/base/PPCArch.c#L108) keep `PPCSetFpNonIEEEMode` as an `asm` function with the same body.
- Origin: Nintendo Dolphin SDK, `base/PPCArch.c`, hand-written assembly in the vendor source.
