# MetroWerks PowerPC EABI runtime in Pokémon Colosseum (GC6E01)

runtime.c from the MetroWerks CodeWarrior PowerPC EABI runtime library
(Runtime.PPCEABI.H.a) occupies .text 0x800C46B0–0x800C4D8C and owns its
`__constants` table at .rodata 0x8026FE58–0x8026FE70. Every routine in it is
hand-written assembly in the vendor source; the object has no C function, so
it can only link with these bodies. The source (`src/crt/runtime.c`) copies
the retail instructions, which agree with the independently matching tww
source below; objdiff and the final link SHA-1s prove the bytes.

None of these routines branches to another function, so none declares
`branch_targets`. The `entry` statements in the four save/restore routines
are listed as mnemonics because the scan reads them as statements; they emit
no instruction, only the global `_savefpr_N`/`_restfpr_N`/`_savegpr_N`/
`_restgpr_N` labels that symbols.txt records at those addresses.

## __cvt_fp2unsigned

Retail address 0x800C46B0.

- Why it cannot be C: it compares f1 against three doubles from `__constants`, addressed with a raw `lis`/`ori` pair of @h/@l halves (MWCC addresses data with @ha/@l and `addi`), uses `fcmpu` into cr6 and cr7 and converts with `fctiwz` through a hand-built 16-byte frame. It is the routine MWCC calls to convert a double to unsigned int, so C for it would call itself.
- Other decompilations: [zeldaret/tww, commit f5234ec8, `src/PowerPC_EABI_Support/Runtime/Src/runtime.c`](https://github.com/zeldaret/tww/blob/f5234ec8f4b8f4119b8db4a018c1ca25572988b1/src/PowerPC_EABI_Support/Runtime/Src/runtime.c#L110) keeps `__cvt_fp2unsigned` as an `asm` function body.
- Origin: MetroWerks CodeWarrior for GameCube, PowerPC EABI runtime library (Runtime.PPCEABI.H.a), `runtime.c`. The exact CodeWarrior release linked into Colosseum is not established.

## __save_fpr

Retail address 0x800C470C.

- Why it cannot be C: it is one routine with eighteen global `entry` points (_savefpr_14 through _savefpr_31), one before each `stfd` of f14-f31 at negative offsets from r11. These are the out-of-line prologue/epilogue helpers that MWCC's own generated code calls (`bl _savefpr_NN`); C has no way to declare entry points in the middle of a function, to address a frame through r11, or to save registers the compiler itself reserves.
- Other decompilations: [zeldaret/tww, commit f5234ec8, `src/PowerPC_EABI_Support/Runtime/Src/runtime.c`](https://github.com/zeldaret/tww/blob/f5234ec8f4b8f4119b8db4a018c1ca25572988b1/src/PowerPC_EABI_Support/Runtime/Src/runtime.c#L141) keeps `__save_fpr` as an `asm` function body.
- Origin: MetroWerks CodeWarrior for GameCube, PowerPC EABI runtime library (Runtime.PPCEABI.H.a), `runtime.c`. The exact CodeWarrior release linked into Colosseum is not established.

## __restore_fpr

Retail address 0x800C4758.

- Why it cannot be C: it is one routine with eighteen global `entry` points (_restfpr_14 through _restfpr_31), one before each `lfd` of f14-f31 at negative offsets from r11. These are the out-of-line prologue/epilogue helpers that MWCC's own generated code calls (`bl _restfpr_NN`); C has no way to declare entry points in the middle of a function, to address a frame through r11, or to save registers the compiler itself reserves.
- Other decompilations: [zeldaret/tww, commit f5234ec8, `src/PowerPC_EABI_Support/Runtime/Src/runtime.c`](https://github.com/zeldaret/tww/blob/f5234ec8f4b8f4119b8db4a018c1ca25572988b1/src/PowerPC_EABI_Support/Runtime/Src/runtime.c#L179) keeps `__restore_fpr` as an `asm` function body.
- Origin: MetroWerks CodeWarrior for GameCube, PowerPC EABI runtime library (Runtime.PPCEABI.H.a), `runtime.c`. The exact CodeWarrior release linked into Colosseum is not established.

## __save_gpr

Retail address 0x800C47A4.

- Why it cannot be C: it is one routine with eighteen global `entry` points (_savegpr_14 through _savegpr_31), one before each `stw` of r14-r31 at negative offsets from r11. These are the out-of-line prologue/epilogue helpers that MWCC's own generated code calls (`bl _savegpr_NN`); C has no way to declare entry points in the middle of a function, to address a frame through r11, or to save registers the compiler itself reserves.
- Other decompilations: [zeldaret/tww, commit f5234ec8, `src/PowerPC_EABI_Support/Runtime/Src/runtime.c`](https://github.com/zeldaret/tww/blob/f5234ec8f4b8f4119b8db4a018c1ca25572988b1/src/PowerPC_EABI_Support/Runtime/Src/runtime.c#L217) keeps `__save_gpr` as an `asm` function body.
- Origin: MetroWerks CodeWarrior for GameCube, PowerPC EABI runtime library (Runtime.PPCEABI.H.a), `runtime.c`. The exact CodeWarrior release linked into Colosseum is not established.

## __restore_gpr

Retail address 0x800C47F0.

- Why it cannot be C: it is one routine with eighteen global `entry` points (_restgpr_14 through _restgpr_31), one before each `lwz` of r14-r31 at negative offsets from r11. These are the out-of-line prologue/epilogue helpers that MWCC's own generated code calls (`bl _restgpr_NN`); C has no way to declare entry points in the middle of a function, to address a frame through r11, or to save registers the compiler itself reserves.
- Other decompilations: [zeldaret/tww, commit f5234ec8, `src/PowerPC_EABI_Support/Runtime/Src/runtime.c`](https://github.com/zeldaret/tww/blob/f5234ec8f4b8f4119b8db4a018c1ca25572988b1/src/PowerPC_EABI_Support/Runtime/Src/runtime.c#L258) keeps `__restore_gpr` as an `asm` function body.
- Origin: MetroWerks CodeWarrior for GameCube, PowerPC EABI runtime library (Runtime.PPCEABI.H.a), `runtime.c`. The exact CodeWarrior release linked into Colosseum is not established.

## __div2u

Retail address 0x800C483C.

- Why it cannot be C: it divides the 64-bit value in r3:r4 by r5:r6 with a hand-scheduled shift-subtract loop built on carry chains (`adde`, `subfc`, `subfe.`, `addic`) and `cntlzw` normalisation, in a leaf with no MWCC prologue. It is the helper MWCC calls for 64-bit unsigned division itself: writing it as `long long` C makes the compiler emit `bl __div2u`, so it cannot be C.
- Other decompilations: [zeldaret/tww, commit f5234ec8, `src/PowerPC_EABI_Support/Runtime/Src/runtime.c`](https://github.com/zeldaret/tww/blob/f5234ec8f4b8f4119b8db4a018c1ca25572988b1/src/PowerPC_EABI_Support/Runtime/Src/runtime.c#L299) keeps `__div2u` as an `asm` function body.
- Origin: MetroWerks CodeWarrior for GameCube, PowerPC EABI runtime library (Runtime.PPCEABI.H.a), `runtime.c`. The exact CodeWarrior release linked into Colosseum is not established.

## __div2i

Retail address 0x800C4928.

- Why it cannot be C: it divides the 64-bit value in r3:r4 by r5:r6 with a hand-scheduled shift-subtract loop built on carry chains (`adde`, `subfc`, `subfe.`, `addic`) and `cntlzw` normalisation, in a leaf with no MWCC prologue (it keeps the two sign words in a hand-built 16-byte frame and negates with `subfic`/`subfze`). It is the helper MWCC calls for 64-bit signed division itself: writing it as `long long` C makes the compiler emit `bl __div2i`, so it cannot be C.
- Other decompilations: [zeldaret/tww, commit f5234ec8, `src/PowerPC_EABI_Support/Runtime/Src/runtime.c`](https://github.com/zeldaret/tww/blob/f5234ec8f4b8f4119b8db4a018c1ca25572988b1/src/PowerPC_EABI_Support/Runtime/Src/runtime.c#L373) keeps `__div2i` as an `asm` function body.
- Origin: MetroWerks CodeWarrior for GameCube, PowerPC EABI runtime library (Runtime.PPCEABI.H.a), `runtime.c`. The exact CodeWarrior release linked into Colosseum is not established.

## __mod2u

Retail address 0x800C4A60.

- Why it cannot be C: it divides the 64-bit value in r3:r4 by r5:r6 with a hand-scheduled shift-subtract loop built on carry chains (`adde`, `subfc`, `subfe.`, `addic`) and `cntlzw` normalisation, in a leaf with no MWCC prologue (early returns use `bgtlr`). It is the helper MWCC calls for 64-bit unsigned remainder itself: writing it as `long long` C makes the compiler emit `bl __mod2u`, so it cannot be C.
- Other decompilations: [zeldaret/tww, commit f5234ec8, `src/PowerPC_EABI_Support/Runtime/Src/runtime.c`](https://github.com/zeldaret/tww/blob/f5234ec8f4b8f4119b8db4a018c1ca25572988b1/src/PowerPC_EABI_Support/Runtime/Src/runtime.c#L472) keeps `__mod2u` as an `asm` function body.
- Origin: MetroWerks CodeWarrior for GameCube, PowerPC EABI runtime library (Runtime.PPCEABI.H.a), `runtime.c`. The exact CodeWarrior release linked into Colosseum is not established.

## __mod2i

Retail address 0x800C4B44.

- Why it cannot be C: it divides the 64-bit value in r3:r4 by r5:r6 with a hand-scheduled shift-subtract loop built on carry chains (`adde`, `subfc`, `subfe.`, `addic`) and `cntlzw` normalisation, in a leaf with no MWCC prologue (it negates with `subfic`/`subfze` and returns early with `bgelr`). It is the helper MWCC calls for 64-bit signed remainder itself: writing it as `long long` C makes the compiler emit `bl __mod2i`, so it cannot be C.
- Other decompilations: [zeldaret/tww, commit f5234ec8, `src/PowerPC_EABI_Support/Runtime/Src/runtime.c`](https://github.com/zeldaret/tww/blob/f5234ec8f4b8f4119b8db4a018c1ca25572988b1/src/PowerPC_EABI_Support/Runtime/Src/runtime.c#L544) keeps `__mod2i` as an `asm` function body.
- Origin: MetroWerks CodeWarrior for GameCube, PowerPC EABI runtime library (Runtime.PPCEABI.H.a), `runtime.c`. The exact CodeWarrior release linked into Colosseum is not established.

## __shl2i

Retail address 0x800C4C50.

- Why it cannot be C: it shifts the 64-bit value in r3:r4 by r5 with the branch-free `subfic`/`subic`/`slw`/`srw` sequence on the register pair. MWCC emits `bl __shl2i` for a variable 64-bit shift, so the helper cannot be written in C.
- Other decompilations: [zeldaret/tww, commit f5234ec8, `src/PowerPC_EABI_Support/Runtime/Src/runtime.c`](https://github.com/zeldaret/tww/blob/f5234ec8f4b8f4119b8db4a018c1ca25572988b1/src/PowerPC_EABI_Support/Runtime/Src/runtime.c#L630) keeps `__shl2i` as an `asm` function body.
- Origin: MetroWerks CodeWarrior for GameCube, PowerPC EABI runtime library (Runtime.PPCEABI.H.a), `runtime.c`. The exact CodeWarrior release linked into Colosseum is not established.

## __shr2u

Retail address 0x800C4C74.

- Why it cannot be C: it shifts the 64-bit value in r3:r4 by r5 with the branch-free `subfic`/`subic`/`srw`/`slw` sequence on the register pair. MWCC emits `bl __shr2u` for a variable 64-bit shift, so the helper cannot be written in C.
- Other decompilations: [zeldaret/tww, commit f5234ec8, `src/PowerPC_EABI_Support/Runtime/Src/runtime.c`](https://github.com/zeldaret/tww/blob/f5234ec8f4b8f4119b8db4a018c1ca25572988b1/src/PowerPC_EABI_Support/Runtime/Src/runtime.c#L645) keeps `__shr2u` as an `asm` function body.
- Origin: MetroWerks CodeWarrior for GameCube, PowerPC EABI runtime library (Runtime.PPCEABI.H.a), `runtime.c`. The exact CodeWarrior release linked into Colosseum is not established.

## __shr2i

Retail address 0x800C4C98.

- Why it cannot be C: it shifts the 64-bit value in r3:r4 right arithmetically by r5 with `subfic`/`subic.`/`sraw`/`srw`/`slw` and a conditional `ble` on the pair. MWCC emits `bl __shr2i` for a variable signed 64-bit shift, so the helper cannot be written in C.
- Other decompilations: [zeldaret/tww, commit f5234ec8, `src/PowerPC_EABI_Support/Runtime/Src/runtime.c`](https://github.com/zeldaret/tww/blob/f5234ec8f4b8f4119b8db4a018c1ca25572988b1/src/PowerPC_EABI_Support/Runtime/Src/runtime.c#L660) keeps `__shr2i` as an `asm` function body.
- Origin: MetroWerks CodeWarrior for GameCube, PowerPC EABI runtime library (Runtime.PPCEABI.H.a), `runtime.c`. The exact CodeWarrior release linked into Colosseum is not established.

## __cvt_dbl_usll

Retail address 0x800C4CC0.

- Why it cannot be C: it spills f1 to a hand-built stack slot, splits the IEEE fields with `extrwi`/`clrlwi`/`oris` and shifts the mantissa across the r3:r4 pair with `slw`/`srw`, saturating with `lis`/`ori` constants, in a frameless leaf. MWCC emits `bl __cvt_dbl_usll` for a double to 64-bit conversion, so C for it would call itself.
- Other decompilations: [zeldaret/tww, commit f5234ec8, `src/PowerPC_EABI_Support/Runtime/Src/runtime.c`](https://github.com/zeldaret/tww/blob/f5234ec8f4b8f4119b8db4a018c1ca25572988b1/src/PowerPC_EABI_Support/Runtime/Src/runtime.c#L677) keeps `__cvt_dbl_usll` as an `asm` function body.
- Origin: MetroWerks CodeWarrior for GameCube, PowerPC EABI runtime library (Runtime.PPCEABI.H.a), `runtime.c`. The exact CodeWarrior release linked into Colosseum is not established.
