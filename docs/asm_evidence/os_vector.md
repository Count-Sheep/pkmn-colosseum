# Dolphin OS.c exception vector and paired-single init in Pokémon Colosseum (GC6E01)

`dolphin/sdk_range_8009A0F4.c` covers .text 0x8009A0F4–0x8009A2C8: the tail of
the Dolphin SDK's `OS.c` (`OSExceptionVector`, `OSDefaultExceptionHandler`,
`__OSPSInit`, `__OSGetDIConfig`, `OSRegisterVersion`) followed by
`OSInitAlarm`. Only the routines named below are admitted. Each body uses the
exact retail mnemonics and matches the retail bytes in objdiff.

`OSDefaultExceptionHandler` is not admitted: it ends in `b
__OSUnhandledException`, a branch to another function, which the quality scan
rejects. Until that is resolved the unit stays a code candidate and is not
linked.

Common origin: Nintendo Dolphin SDK, `os/OS.c`, linked into Colosseum's
main.dol. The exact SDK build is not established; `__OSPSInit` clears all eight
GQRs, as the older SDK revisions do (later ones clear only GQR0).

## OSExceptionVector

Retail address 0x8009A0F4, size 0x9C (local symbol). The labels `__OSEVStart`
(0x8009A0F4), `__DBVECTOR` (0x8009A14C), `__OSEVSetNumber` (0x8009A15C) and
`__OSEVEnd` (0x8009A18C) are `entry` points inside it.

- Why it cannot be C: it is the code `OSInit` copies into the low-memory exception vectors. It runs with address translation off, saves r3–r5 through SPRG0 (`mtsprg`/`mfsprg`), stores CR, LR, CTR, XER, SRR0 and SRR1 (`mfcr`, `mfxer`, `mfsrr0`, `mfsrr1`) into the context found at physical 0xC0, sets SRR1[IR|DR] from the MSR (`mfmsr`, `mtsrr1`), loads the handler from the table at 0x3000 into SRR0 (`mtsrr0`) and leaves with `rfi`. It has no prologue, keeps nothing in the ABI's registers, and exposes four `entry` labels that `OSInit` patches (`__OSEVSetNumber`'s `li r3, 0` is rewritten per exception). MWCC never emits `rfi`, SPRG/SRR moves or `entry` points from C.
- Other decompilations: [zeldaret/tww, commit f5234ec8, `src/dolphin/os/OS.c`](https://github.com/zeldaret/tww/blob/f5234ec8f4b8f4119b8db4a018c1ca25572988b1/src/dolphin/os/OS.c#L498) and [doldecomp/melee, commit e78dc834, `libs/dolphin/src/dolphin/os/OS.c`](https://github.com/doldecomp/melee/blob/e78dc8349c587ca5d2f97322834b625d3bd1abc3/libs/dolphin/src/dolphin/os/OS.c#L399) keep `OSExceptionVector` as an `asm` function body with the same `entry` labels.
- Origin: Nintendo Dolphin SDK, `os/OS.c`, hand-written assembly in the vendor source.

## __OSPSInit

Retail address 0x8009A1E8, size 0x54. The source is C (`PPCMthid2(PPCMfhid2()
| 0xA0000000)`, `ICFlashInvalidate()`, `__sync()`); only the GQR clears are an
inline `asm` block (`li r3, 0` and `mtspr GQR0`–`GQR7`).

- Why it cannot be C: the graphics quantization registers GQR0–GQR7 are special-purpose registers (SPR 912–919) that MWCC only writes through `mtspr` in assembly; there is no C intrinsic for them, and the retail routine writes all eight from r3 in a row.
- Other decompilations: [doldecomp/melee, commit e78dc834, `libs/dolphin/src/dolphin/os/OS.c`](https://github.com/doldecomp/melee/blob/e78dc8349c587ca5d2f97322834b625d3bd1abc3/libs/dolphin/src/dolphin/os/OS.c#L499) writes `__OSPSInit` as C with the same inline `asm { li r3, 0; mtspr GQR0, r3 }` block, and [zeldaret/tww, commit f5234ec8, `src/dolphin/os/OS.c`](https://github.com/zeldaret/tww/blob/f5234ec8f4b8f4119b8db4a018c1ca25572988b1/src/dolphin/os/OS.c#L618) keeps the whole routine as an `asm` function. Both are later SDK revisions that clear GQR0 only.
- Origin: Nintendo Dolphin SDK, `os/OS.c`, an inline assembly block in the vendor source.
