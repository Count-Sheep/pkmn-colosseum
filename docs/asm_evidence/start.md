# Dolphin SDK __start.c bootstrap routines in Pokémon Colosseum (GC6E01)

`crt/__start.c` occupies .init 0x80003100–0x80003458 and .sbss
0x8047A770–0x8047A778 (`Debug_BBA`) as one retail translation unit. Its C
functions (`__check_pad3`, `__set_debug_bba`, `__get_debug_bba`,
`__init_data`) cannot link without the four hand-written assembly routines
between them. Only the routines named below are admitted. Each body uses the
exact retail mnemonics; the source matches the retail bytes in objdiff and in
the final link.

The Colosseum layout (`__set_debug_bba`/`__get_debug_bba`, `InitMetroTRK_BBA`,
`__OSFPRInit` in `__init_hardware`, all in one .init object) is the same
revision that zeldaret/tp decompiles, at the same addresses
(0x80003100–0x80003458).

Common origin: Nintendo Dolphin SDK, `os/__start.c` (the bootstrap object of
the 2002-era GameCube SDK linked into Colosseum's main.dol). The routines are
written as `asm` functions in the SDK source itself, as every Dolphin SDK
decompilation below also reproduces.

## __start

Retail address 0x80003154, size 0x15C, weak.

- Why it cannot be C: it is the program entry point. It runs before any stack, small-data base or C runtime exists, calls `__init_registers` to create them, builds the first stack frame by hand (`stwu r1,-8(r1)` storing -1 back-chain words), keeps `argc`/`argv` in the callee-saved registers r14/r15 across calls without saving them, calls `InitMetroTRK` through `mtlr`/`blrl`, and ends in a tail branch to `exit` with no return. MWCC never emits a frameless function that sets up its own stack or uses r14/r15 without a prologue.
- Other decompilations: [zeldaret/tp, commit c8fa8c9e, `libs/dolphin/src/os/__start.c`](https://github.com/zeldaret/tp/blob/c8fa8c9e2aab72cf4e5db0e5d1c84a9ea6ee6eb0/libs/dolphin/src/os/__start.c#L38) (same revision, identical body), [zeldaret/tww, commit f5234ec8, `src/dolphin/os/__start.c`](https://github.com/zeldaret/tww/blob/f5234ec8f4b8f4119b8db4a018c1ca25572988b1/src/dolphin/os/__start.c#L16) and [doldecomp/melee, commit e78dc834, `libs/dolphin/src/dolphin/os/init/__start.c`](https://github.com/doldecomp/melee/blob/e78dc8349c587ca5d2f97322834b625d3bd1abc3/libs/dolphin/src/dolphin/os/init/__start.c#L56) keep `__start` as an `asm` function body.
- Origin: Nintendo Dolphin SDK `os/__start.c`, hand-written assembly in the vendor source; the exact SDK build date in Colosseum is not established.
- External branch targets: the final `b exit` (0x800032AC) jumps to `exit` after `main` returns, with r3 still holding main's return value as the exit status. It is a tail jump, not a call: `__start` never returns and has no frame of its own to return through, so no `bl` is emitted. The retail relocation at that instruction names `exit`.

## __init_registers

Retail address 0x800032B0, size 0x90, local.

- Why it cannot be C: it zeroes every general-purpose register except r1, r2 and r13, then loads r1 with `_stack_addr` and the small-data base registers r2/r13 with `_SDA2_BASE_`/`_SDA_BASE_` through `lis`/`ori` pairs. MWCC never writes r1, r2 or r13 from C, and the routine runs before those registers hold valid values.
- Other decompilations: [zeldaret/tp, commit c8fa8c9e, `libs/dolphin/src/os/__start.c`](https://github.com/zeldaret/tp/blob/c8fa8c9e2aab72cf4e5db0e5d1c84a9ea6ee6eb0/libs/dolphin/src/os/__start.c#L152) (same revision, identical body), [zeldaret/tww, commit f5234ec8, `src/dolphin/os/__start.c`](https://github.com/zeldaret/tww/blob/f5234ec8f4b8f4119b8db4a018c1ca25572988b1/src/dolphin/os/__start.c#L121) and [doldecomp/melee, commit e78dc834, `libs/dolphin/src/dolphin/os/init/__start.c`](https://github.com/doldecomp/melee/blob/e78dc8349c587ca5d2f97322834b625d3bd1abc3/libs/dolphin/src/dolphin/os/init/__start.c#L165) keep `__init_registers` as an `asm` function body.
- Origin: Nintendo Dolphin SDK `os/__start.c`, hand-written assembly in the vendor source; the exact SDK build date in Colosseum is not established.

## __init_hardware

Retail address 0x80003400, size 0x24.

- Why it cannot be C: it sets MSR[FP] with `mfmsr`/`ori`/`mtmsr`, then calls `__OSPSInit`, `__OSFPRInit` and `__OSCacheInit` while holding its return address in r31 (`mflr r31`/`mtlr r31`) instead of a stack frame, because no stack exists yet. MWCC emits no MSR access from C and never keeps the link register in r31 without saving r31.
- Other decompilations: [zeldaret/tp, commit c8fa8c9e, `libs/dolphin/src/os/__start.c`](https://github.com/zeldaret/tp/blob/c8fa8c9e2aab72cf4e5db0e5d1c84a9ea6ee6eb0/libs/dolphin/src/os/__start.c#L231) (same revision, with `__OSFPRInit`), [zeldaret/tww, commit f5234ec8, `src/dolphin/os/__start.c`](https://github.com/zeldaret/tww/blob/f5234ec8f4b8f4119b8db4a018c1ca25572988b1/src/dolphin/os/__start.c#L172) and [doldecomp/melee, commit e78dc834, `libs/dolphin/src/dolphin/os/init/__ppc_eabi_init.c`](https://github.com/doldecomp/melee/blob/e78dc8349c587ca5d2f97322834b625d3bd1abc3/libs/dolphin/src/dolphin/os/init/__ppc_eabi_init.c#L12) keep `__init_hardware` as an `asm` function body.
- Origin: Nintendo Dolphin SDK `os/__start.c` (`__ppc_eabi_init` in older SDK revisions), hand-written assembly in the vendor source; the exact SDK build date in Colosseum is not established.

## __flush_cache

Retail address 0x80003424, size 0x34.

- Why it cannot be C: it walks the range eight bytes at a time with the cache-block instructions `dcbst` and `icbi` separated by `sync`, counts down with the record form `subic.`, and ends with `isync`. MWCC never emits `dcbst`, `icbi` or `isync` from C, and the register-argument frameless body is hand-written.
- Other decompilations: [zeldaret/tp, commit c8fa8c9e, `libs/dolphin/src/os/__start.c`](https://github.com/zeldaret/tp/blob/c8fa8c9e2aab72cf4e5db0e5d1c84a9ea6ee6eb0/libs/dolphin/src/os/__start.c#L247), [zeldaret/tww, commit f5234ec8, `src/dolphin/os/__start.c`](https://github.com/zeldaret/tww/blob/f5234ec8f4b8f4119b8db4a018c1ca25572988b1/src/dolphin/os/__start.c#L186) and [doldecomp/melee, commit e78dc834, `libs/dolphin/src/dolphin/os/init/__ppc_eabi_init.c`](https://github.com/doldecomp/melee/blob/e78dc8349c587ca5d2f97322834b625d3bd1abc3/libs/dolphin/src/dolphin/os/init/__ppc_eabi_init.c#L25) keep `__flush_cache` as an `asm` function body.
- Origin: Nintendo Dolphin SDK `os/__start.c` (`__ppc_eabi_init` in older SDK revisions), hand-written assembly in the vendor source; the exact SDK build date in Colosseum is not established.
