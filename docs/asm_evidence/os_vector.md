# Dolphin OS.c exception vector and paired-single init in Pokémon Colosseum (GC6E01)

`dolphin/sdk_range_8009A0F4.c` covers .text 0x8009A0F4–0x8009A27C: the tail of
the Dolphin SDK's `OS.c` (`OSExceptionVector`, `OSDefaultExceptionHandler`,
`__OSPSInit`, `__OSGetDIConfig`, `OSRegisterVersion`). `OSInitAlarm`, the first
function of `OSAlarm.c`, follows at 0x8009A27C in the OSAlarm.c unit
(`dolphin/os/OSAlarm.c`). Only the routines named below are admitted. Each body uses the
exact retail mnemonics and matches the retail bytes in objdiff.

`OSDefaultExceptionHandler` ends in `b __OSUnhandledException`, a tail branch
to another function; it is admitted with that target declared (README item 4),
and the unit links whole.

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

## OSDefaultExceptionHandler

Retail address 0x8009A190, size 0x58. `OSInit` installs it as the handler for
every exception that has none; `OSExceptionVector` reaches it through SRR0 and
`rfi` with r3 = exception number and r4 = context.

- Why it cannot be C: it is entered from the exception vector with only r3–r5 saved and no valid stack frame. It stores r0–r2 and r6–r31 into the context (`stmw r6, 0x18(r4)`), reads the graphics quantization registers GQR1–GQR7 with `mfspr` and the fault registers with `mfdsisr`/`mfdar`, carves an 8-byte frame with `stwu r1, -0x8(r1)` and tail-branches. MWCC never emits `mfdsisr`, `mfdar` or GQR `mfspr` from C, and a C body would save registers into its own frame before the context is filled.
- Other decompilations: [zeldaret/tww, commit f5234ec8, `src/dolphin/os/OS.c`](https://github.com/zeldaret/tww/blob/f5234ec8f4b8f4119b8db4a018c1ca25572988b1/src/dolphin/os/OS.c#L584-L616) keeps `OSDefaultExceptionHandler` as an `asm` function with the same body, ending in `stwu r1, -8(r1)` and `b __OSUnhandledException` ([L612-L613](https://github.com/zeldaret/tww/blob/f5234ec8f4b8f4119b8db4a018c1ca25572988b1/src/dolphin/os/OS.c#L612-L613)); [doldecomp/melee, commit e78dc834, `libs/dolphin/src/dolphin/os/OS.c`](https://github.com/doldecomp/melee/blob/e78dc8349c587ca5d2f97322834b625d3bd1abc3/libs/dolphin/src/dolphin/os/OS.c#L486-L497) keeps it as `asm` with `OS_EXCEPTION_SAVE_GPRS(context)` and the same final `b __OSUnhandledException` ([L495](https://github.com/doldecomp/melee/blob/e78dc8349c587ca5d2f97322834b625d3bd1abc3/libs/dolphin/src/dolphin/os/OS.c#L495)).
- Origin: Nintendo Dolphin SDK, `os/OS.c`, hand-written assembly in the vendor source.
- External branch targets: the final `b __OSUnhandledException` (0x8009A1E4) jumps to __OSUnhandledException at 0x8009C578 (OSError.c) with r3–r6 holding the exception, context, DSISR and DAR. It is a tail jump, not a call: the routine has no return path and only the 8-byte frame it builds itself, so MWCC's `bl` with a prologue would not reproduce it. The retail relocation at that instruction names __OSUnhandledException.

## __OSPSInit

Retail address 0x8009A1E8, size 0x54. The source is C (`PPCMthid2(PPCMfhid2()
| 0xA0000000)`, `ICFlashInvalidate()`, `__sync()`); only the GQR clears are an
inline `asm` block (`li r3, 0` and `mtspr GQR0`–`GQR7`).

- Why it cannot be C: the graphics quantization registers GQR0–GQR7 are special-purpose registers (SPR 912–919) that MWCC only writes through `mtspr` in assembly; there is no C intrinsic for them, and the retail routine writes all eight from r3 in a row.
- Other decompilations: [doldecomp/melee, commit e78dc834, `libs/dolphin/src/dolphin/os/OS.c`](https://github.com/doldecomp/melee/blob/e78dc8349c587ca5d2f97322834b625d3bd1abc3/libs/dolphin/src/dolphin/os/OS.c#L499) writes `__OSPSInit` as C with the same inline `asm { li r3, 0; mtspr GQR0, r3 }` block, and [zeldaret/tww, commit f5234ec8, `src/dolphin/os/OS.c`](https://github.com/zeldaret/tww/blob/f5234ec8f4b8f4119b8db4a018c1ca25572988b1/src/dolphin/os/OS.c#L618) keeps the whole routine as an `asm` function. Both are later SDK revisions that clear GQR0 only.
- Origin: Nintendo Dolphin SDK, `os/OS.c`, an inline assembly block in the vendor source.
