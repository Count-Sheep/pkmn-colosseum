# Dolphin SDK OSCache.c cache primitives in Pokémon Colosseum (GC6E01)

OSCache.c occupies .text 0x8009B290–0x8009B914 and .data 0x803105B0–0x803107E0
as one retail translation unit. Its C functions (LCEnable, LCStoreData,
L2GlobalInvalidate, DMAErrorHandler, __OSCacheInit) cannot link without the
fifteen hand-written assembly routines between them. Only the routines named
below are admitted. Each body's instructions are the exact retail mnemonics;
the source matches the retail bytes in objdiff and in the final link.

Common origin: Nintendo Dolphin SDK, `os/OSCache.c` (the 2001–2002 GameCube
SDK revision linked into Colosseum's main.dol). The routines are written as
`asm` functions in the SDK source itself, as every Dolphin SDK decompilation
below also reproduces.

## DCEnable

Retail address 0x8009B290.

- Why it cannot be C: it reads and writes HID0 with `mfspr`/`mtspr` behind a leading `sync`, and returns without a stack frame. MWCC emits no SPR access from C except through these intrinsically asm primitives.
- Other decompilations: [zeldaret/tww, commit f5234ec8, `src/dolphin/os/OSCache.c`](https://github.com/zeldaret/tww/blob/f5234ec8f4b8f4119b8db4a018c1ca25572988b1/src/dolphin/os/OSCache.c#L6) and [doldecomp/melee, commit e78dc834, `libs/dolphin/src/dolphin/os/OSCache.c`](https://github.com/doldecomp/melee/blob/e78dc8349c587ca5d2f97322834b625d3bd1abc3/libs/dolphin/src/dolphin/os/OSCache.c#L22) keep `DCEnable` as an `asm` function body.
- Origin: Nintendo Dolphin SDK `os/OSCache.c`, hand-written assembly in the vendor source; the exact SDK build date in Colosseum is not established.

## DCInvalidateRange

Retail address 0x8009B2A4.

- Why it cannot be C: it walks the range 32 bytes at a time in a `mtctr`/`bdnz` loop around the cache-block instruction `dcbi`, and returns early with `blelr`. MWCC never emits `dcbi` from C, and the register-argument frameless body is hand-written.
- Other decompilations: [zeldaret/tww, commit f5234ec8, `src/dolphin/os/OSCache.c`](https://github.com/zeldaret/tww/blob/f5234ec8f4b8f4119b8db4a018c1ca25572988b1/src/dolphin/os/OSCache.c#L21) and [doldecomp/melee, commit e78dc834, `libs/dolphin/src/dolphin/os/OSCache.c`](https://github.com/doldecomp/melee/blob/e78dc8349c587ca5d2f97322834b625d3bd1abc3/libs/dolphin/src/dolphin/os/OSCache.c#L87) keep `DCInvalidateRange` as an `asm` function body.
- Origin: Nintendo Dolphin SDK `os/OSCache.c`, hand-written assembly in the vendor source; the exact SDK build date in Colosseum is not established.

## DCFlushRange

Retail address 0x8009B2D0.

- Why it cannot be C: it walks the range 32 bytes at a time in a `mtctr`/`bdnz` loop around the cache-block instruction `dcbf` and ends with `sc` to synchronise, and returns early with `blelr`. MWCC never emits `dcbf` from C, and the register-argument frameless body is hand-written.
- Other decompilations: [zeldaret/tww, commit f5234ec8, `src/dolphin/os/OSCache.c`](https://github.com/zeldaret/tww/blob/f5234ec8f4b8f4119b8db4a018c1ca25572988b1/src/dolphin/os/OSCache.c#L45) and [doldecomp/melee, commit e78dc834, `libs/dolphin/src/dolphin/os/OSCache.c`](https://github.com/doldecomp/melee/blob/e78dc8349c587ca5d2f97322834b625d3bd1abc3/libs/dolphin/src/dolphin/os/OSCache.c#L107) keep `DCFlushRange` as an `asm` function body.
- Origin: Nintendo Dolphin SDK `os/OSCache.c`, hand-written assembly in the vendor source; the exact SDK build date in Colosseum is not established.

## DCStoreRange

Retail address 0x8009B300.

- Why it cannot be C: it walks the range 32 bytes at a time in a `mtctr`/`bdnz` loop around the cache-block instruction `dcbst` and ends with `sc` to synchronise, and returns early with `blelr`. MWCC never emits `dcbst` from C, and the register-argument frameless body is hand-written.
- Other decompilations: [zeldaret/tww, commit f5234ec8, `src/dolphin/os/OSCache.c`](https://github.com/zeldaret/tww/blob/f5234ec8f4b8f4119b8db4a018c1ca25572988b1/src/dolphin/os/OSCache.c#L70) and [doldecomp/melee, commit e78dc834, `libs/dolphin/src/dolphin/os/OSCache.c`](https://github.com/doldecomp/melee/blob/e78dc8349c587ca5d2f97322834b625d3bd1abc3/libs/dolphin/src/dolphin/os/OSCache.c#L127) keep `DCStoreRange` as an `asm` function body.
- Origin: Nintendo Dolphin SDK `os/OSCache.c`, hand-written assembly in the vendor source; the exact SDK build date in Colosseum is not established.

## DCFlushRangeNoSync

Retail address 0x8009B330.

- Why it cannot be C: it walks the range 32 bytes at a time in a `mtctr`/`bdnz` loop around the cache-block instruction `dcbf`, and returns early with `blelr`. MWCC never emits `dcbf` from C, and the register-argument frameless body is hand-written.
- Other decompilations: [zeldaret/tww, commit f5234ec8, `src/dolphin/os/OSCache.c`](https://github.com/zeldaret/tww/blob/f5234ec8f4b8f4119b8db4a018c1ca25572988b1/src/dolphin/os/OSCache.c#L95) and [doldecomp/melee, commit e78dc834, `libs/dolphin/src/dolphin/os/OSCache.c`](https://github.com/doldecomp/melee/blob/e78dc8349c587ca5d2f97322834b625d3bd1abc3/libs/dolphin/src/dolphin/os/OSCache.c#L148) keep `DCFlushRangeNoSync` as an `asm` function body.
- Origin: Nintendo Dolphin SDK `os/OSCache.c`, hand-written assembly in the vendor source; the exact SDK build date in Colosseum is not established.

## DCStoreRangeNoSync

Retail address 0x8009B35C.

- Why it cannot be C: it walks the range 32 bytes at a time in a `mtctr`/`bdnz` loop around the cache-block instruction `dcbst`, and returns early with `blelr`. MWCC never emits `dcbst` from C, and the register-argument frameless body is hand-written.
- Other decompilations: [zeldaret/tww, commit f5234ec8, `src/dolphin/os/OSCache.c`](https://github.com/zeldaret/tww/blob/f5234ec8f4b8f4119b8db4a018c1ca25572988b1/src/dolphin/os/OSCache.c#L119) and [doldecomp/melee, commit e78dc834, `libs/dolphin/src/dolphin/os/OSCache.c`](https://github.com/doldecomp/melee/blob/e78dc8349c587ca5d2f97322834b625d3bd1abc3/libs/dolphin/src/dolphin/os/OSCache.c#L168) keep `DCStoreRangeNoSync` as an `asm` function body.
- Origin: Nintendo Dolphin SDK `os/OSCache.c`, hand-written assembly in the vendor source; the exact SDK build date in Colosseum is not established.

## DCZeroRange

Retail address 0x8009B388.

- Why it cannot be C: it walks the range 32 bytes at a time in a `mtctr`/`bdnz` loop around the cache-block instruction `dcbz`, and returns early with `blelr`. MWCC never emits `dcbz` from C, and the register-argument frameless body is hand-written.
- Other decompilations: [zeldaret/tww, commit f5234ec8, `src/dolphin/os/OSCache.c`](https://github.com/zeldaret/tww/blob/f5234ec8f4b8f4119b8db4a018c1ca25572988b1/src/dolphin/os/OSCache.c#L143) and [doldecomp/melee, commit e78dc834, `libs/dolphin/src/dolphin/os/OSCache.c`](https://github.com/doldecomp/melee/blob/e78dc8349c587ca5d2f97322834b625d3bd1abc3/libs/dolphin/src/dolphin/os/OSCache.c#L188) keep `DCZeroRange` as an `asm` function body.
- Origin: Nintendo Dolphin SDK `os/OSCache.c`, hand-written assembly in the vendor source; the exact SDK build date in Colosseum is not established.

## ICInvalidateRange

Retail address 0x8009B3B4.

- Why it cannot be C: it walks the range 32 bytes at a time in a `mtctr`/`bdnz` loop around the cache-block instruction `icbi` followed by `sync`/`isync`, and returns early with `blelr`. MWCC never emits `icbi` from C, and the register-argument frameless body is hand-written.
- Other decompilations: [zeldaret/tww, commit f5234ec8, `src/dolphin/os/OSCache.c`](https://github.com/zeldaret/tww/blob/f5234ec8f4b8f4119b8db4a018c1ca25572988b1/src/dolphin/os/OSCache.c#L167) and [doldecomp/melee, commit e78dc834, `libs/dolphin/src/dolphin/os/OSCache.c`](https://github.com/doldecomp/melee/blob/e78dc8349c587ca5d2f97322834b625d3bd1abc3/libs/dolphin/src/dolphin/os/OSCache.c#L228) keep `ICInvalidateRange` as an `asm` function body.
- Origin: Nintendo Dolphin SDK `os/OSCache.c`, hand-written assembly in the vendor source; the exact SDK build date in Colosseum is not established.

## ICFlashInvalidate

Retail address 0x8009B3E8.

- Why it cannot be C: it sets the ICFI bit by `mfspr`/`ori`/`mtspr` on HID0, a privileged special-purpose register that MWCC never accesses from C.
- Other decompilations: [zeldaret/tww, commit f5234ec8, `src/dolphin/os/OSCache.c`](https://github.com/zeldaret/tww/blob/f5234ec8f4b8f4119b8db4a018c1ca25572988b1/src/dolphin/os/OSCache.c#L194) and [doldecomp/melee, commit e78dc834, `libs/dolphin/src/dolphin/os/OSCache.c`](https://github.com/doldecomp/melee/blob/e78dc8349c587ca5d2f97322834b625d3bd1abc3/libs/dolphin/src/dolphin/os/OSCache.c#L251) keep `ICFlashInvalidate` as an `asm` function body.
- Origin: Nintendo Dolphin SDK `os/OSCache.c`, hand-written assembly in the vendor source; the exact SDK build date in Colosseum is not established.

## ICEnable

Retail address 0x8009B3F8.

- Why it cannot be C: it sets ICE in HID0 through `mfspr`/`mtspr` followed by `isync`; MWCC has no C construct that emits either an SPR move or `isync`.
- Other decompilations: [zeldaret/tww, commit f5234ec8, `src/dolphin/os/OSCache.c`](https://github.com/zeldaret/tww/blob/f5234ec8f4b8f4119b8db4a018c1ca25572988b1/src/dolphin/os/OSCache.c#L208) and [doldecomp/melee, commit e78dc834, `libs/dolphin/src/dolphin/os/OSCache.c`](https://github.com/doldecomp/melee/blob/e78dc8349c587ca5d2f97322834b625d3bd1abc3/libs/dolphin/src/dolphin/os/OSCache.c#L259) keep `ICEnable` as an `asm` function body.
- Origin: Nintendo Dolphin SDK `os/OSCache.c`, hand-written assembly in the vendor source; the exact SDK build date in Colosseum is not established.

## __LCEnable

Retail address 0x8009B40C.

- Why it cannot be C: it saves and edits the MSR with `mfmsr`/`mtmsr`, flushes 32 KB with `dcbt`/`dcbst`, sets HID2 LCE with `mtspr`, rewrites DBAT3 with `mtdbatu`/`mtdbatl`, allocates the locked cache with `dcbz_l`, and pads with `nop` for alignment. None of these is MWCC output from C, and `dcbz_l` is a Gekko-only cache op.
- Other decompilations: [zeldaret/tww, commit f5234ec8, `src/dolphin/os/OSCache.c`](https://github.com/zeldaret/tww/blob/f5234ec8f4b8f4119b8db4a018c1ca25572988b1/src/dolphin/os/OSCache.c#L223) and [doldecomp/melee, commit e78dc834, `libs/dolphin/src/dolphin/os/OSCache.c`](https://github.com/doldecomp/melee/blob/e78dc8349c587ca5d2f97322834b625d3bd1abc3/libs/dolphin/src/dolphin/os/OSCache.c#L309) keep `__LCEnable` as an `asm` function body.
- Origin: Nintendo Dolphin SDK `os/OSCache.c`, hand-written assembly in the vendor source; the exact SDK build date in Colosseum is not established.

## LCDisable

Retail address 0x8009B510.

- Why it cannot be C: it invalidates the 512 locked-cache blocks at 0xE0000000 with `dcbi` in a `bdnz` loop, then clears the LCE bit in HID2 (SPR 920) with `mfspr`/`mtspr`. MWCC never emits `dcbi` or SPR moves from C.
- Other decompilations: [zeldaret/tww, commit f5234ec8, `src/dolphin/os/OSCache.c`](https://github.com/zeldaret/tww/blob/f5234ec8f4b8f4119b8db4a018c1ca25572988b1/src/dolphin/os/OSCache.c#L299) and [doldecomp/melee, commit e78dc834, `libs/dolphin/src/dolphin/os/OSCache.c`](https://github.com/doldecomp/melee/blob/e78dc8349c587ca5d2f97322834b625d3bd1abc3/libs/dolphin/src/dolphin/os/OSCache.c#L380) keep `LCDisable` as an `asm` function body.
- Origin: Nintendo Dolphin SDK `os/OSCache.c`, hand-written assembly in the vendor source; the exact SDK build date in Colosseum is not established.

## LCStoreBlocks

Retail address 0x8009B538.

- Why it cannot be C: it queues a locked-cache DMA by writing the DMA_U and DMA_L special-purpose registers with `mtspr` in a frameless leaf. MWCC cannot emit `mtspr` from C.
- Other decompilations: [zeldaret/tww, commit f5234ec8, `src/dolphin/os/OSCache.c`](https://github.com/zeldaret/tww/blob/f5234ec8f4b8f4119b8db4a018c1ca25572988b1/src/dolphin/os/OSCache.c#L321) and [doldecomp/melee, commit e78dc834, `libs/dolphin/src/dolphin/os/OSCache.c`](https://github.com/doldecomp/melee/blob/e78dc8349c587ca5d2f97322834b625d3bd1abc3/libs/dolphin/src/dolphin/os/OSCache.c#L441) keep `LCStoreBlocks` as an `asm` function body.
- Origin: Nintendo Dolphin SDK `os/OSCache.c`, hand-written assembly in the vendor source; the exact SDK build date in Colosseum is not established.

## LCQueueLength

Retail address 0x8009B608.

- Why it cannot be C: it reads the DMA queue length field out of HID2 (SPR 920) with `mfspr`; MWCC cannot read a special-purpose register from C.
- Other decompilations: [doldecomp/melee, commit e78dc834, `libs/dolphin/src/dolphin/os/OSCache.c`](https://github.com/doldecomp/melee/blob/e78dc8349c587ca5d2f97322834b625d3bd1abc3/libs/dolphin/src/dolphin/os/OSCache.c#L539) keep `LCQueueLength` as an `asm` function body.
- Origin: Nintendo Dolphin SDK `os/OSCache.c`, hand-written assembly in the vendor source; the exact SDK build date in Colosseum is not established.

## LCQueueWait

Retail address 0x8009B614.

- Why it cannot be C: it spins on the HID2 (SPR 920) DMA queue length read with `mfspr` until it drops to the argument; MWCC cannot read a special-purpose register from C.
- Other decompilations: [zeldaret/tww, commit f5234ec8, `src/dolphin/os/OSCache.c`](https://github.com/zeldaret/tww/blob/f5234ec8f4b8f4119b8db4a018c1ca25572988b1/src/dolphin/os/OSCache.c#L362) and [doldecomp/melee, commit e78dc834, `libs/dolphin/src/dolphin/os/OSCache.c`](https://github.com/doldecomp/melee/blob/e78dc8349c587ca5d2f97322834b625d3bd1abc3/libs/dolphin/src/dolphin/os/OSCache.c#L546) keep `LCQueueWait` as an `asm` function body.
- Origin: Nintendo Dolphin SDK `os/OSCache.c`, hand-written assembly in the vendor source; the exact SDK build date in Colosseum is not established.
