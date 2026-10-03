# Dolphin PPCArch.c FPSCR accessors in Pokémon Colosseum (GC6E01)

`dolphin/os/PPCArch.c` covers .text 0x80097FFC–0x80098108 of the Dolphin
SDK's `base/PPCArch.c`. The SPR/MSR primitives (`PPCMfmsr` .. `PPCMtwpar`,
`PPCSync`, `PPCHalt`) are `asm` functions using only the hardware allowlist.
The two FPSCR accessors below are C functions whose bodies are inline `asm`
blocks with a memory round trip, so they are registered here. Both match the
retail bytes.

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
