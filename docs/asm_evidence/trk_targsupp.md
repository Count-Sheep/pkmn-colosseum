# MetroTRK targsupp file-I/O trap stubs in Pokémon Colosseum (GC6E01)

`trk/targsupp.c` (.text 0x800C29F0–0x800C2A10) is MetroTRK's
`Processor/ppc/Export/targsupp.s`: four eight-byte routines, each a trap
followed by `blr`. The host debugger services the trap and supplies the
result. Only these four routines are admitted, with the exact retail
mnemonics; they match the retail bytes in objdiff and in the final link.

The stubs are written `twi 31, r0, 0`, which encodes the same word as the
retail `twui r0, 0x0` (0x0FE00000); the GC/1.3.2 inline assembler does not
accept the `twui` spelling. The unit builds with `-func_align 8` so the
object lands 8-aligned at 0x800C29F0 after the preceding
`TRKTarget_residual_800C25FC` object; the zero word at 0x800C29EC is that
alignment padding (upstream places the routines under `.balign 16`).

Common origin: Metrowerks MetroTRK for Dolphin (`TRK_MINNOW_DOLPHIN`),
`Processor/ppc/Export/targsupp.s`, an assembly-language source file in the
CodeWarrior debugger library.

## TRKAccessFile

Retail address 0x800C29F0, size 0x8.

- Why it cannot be C: the body is a single unconditional trap word (`twui r0, 0`, written here as `twi 31, r0, 0`) and `blr`. MWCC never emits a trap instruction from C; the routine exists only to hand control to the host debugger through the program exception.
- Other decompilations: [zeldaret/tp, commit c8fa8c9e, `libs/TRK_MINNOW_DOLPHIN/debugger/embedded/MetroTRK/Processor/ppc/Export/targsupp.s`](https://github.com/zeldaret/tp/blob/c8fa8c9e2aab72cf4e5db0e5d1c84a9ea6ee6eb0/libs/TRK_MINNOW_DOLPHIN/debugger/embedded/MetroTRK/Processor/ppc/Export/targsupp.s#L7) and [zeldaret/tww, commit f5234ec8, `src/TRK_MINNOW_DOLPHIN/ppc/Export/targsupp.s`](https://github.com/zeldaret/tww/blob/f5234ec8f4b8f4119b8db4a018c1ca25572988b1/src/TRK_MINNOW_DOLPHIN/ppc/Export/targsupp.s#L7) keep `TRKAccessFile` as assembly (`.fn TRKAccessFile` in targsupp.s).
- Origin: Metrowerks MetroTRK for Dolphin, `targsupp.s`, hand-written assembly in the vendor source; the exact CodeWarrior/MetroTRK build in Colosseum is not established.

## TRKOpenFile

Retail address 0x800C29F8, size 0x8.

- Why it cannot be C: the body is a single unconditional trap word (`twui r0, 0`, written here as `twi 31, r0, 0`) and `blr`. MWCC never emits a trap instruction from C; the routine exists only to hand control to the host debugger through the program exception.
- Other decompilations: [zeldaret/tp, commit c8fa8c9e, `libs/TRK_MINNOW_DOLPHIN/debugger/embedded/MetroTRK/Processor/ppc/Export/targsupp.s`](https://github.com/zeldaret/tp/blob/c8fa8c9e2aab72cf4e5db0e5d1c84a9ea6ee6eb0/libs/TRK_MINNOW_DOLPHIN/debugger/embedded/MetroTRK/Processor/ppc/Export/targsupp.s#L12) and [zeldaret/tww, commit f5234ec8, `src/TRK_MINNOW_DOLPHIN/ppc/Export/targsupp.s`](https://github.com/zeldaret/tww/blob/f5234ec8f4b8f4119b8db4a018c1ca25572988b1/src/TRK_MINNOW_DOLPHIN/ppc/Export/targsupp.s#L12) keep `TRKOpenFile` as assembly (`.fn TRKOpenFile` in targsupp.s).
- Origin: Metrowerks MetroTRK for Dolphin, `targsupp.s`, hand-written assembly in the vendor source; the exact CodeWarrior/MetroTRK build in Colosseum is not established.

## TRKCloseFile

Retail address 0x800C2A00, size 0x8.

- Why it cannot be C: the body is a single unconditional trap word (`twui r0, 0`, written here as `twi 31, r0, 0`) and `blr`. MWCC never emits a trap instruction from C; the routine exists only to hand control to the host debugger through the program exception.
- Other decompilations: [zeldaret/tp, commit c8fa8c9e, `libs/TRK_MINNOW_DOLPHIN/debugger/embedded/MetroTRK/Processor/ppc/Export/targsupp.s`](https://github.com/zeldaret/tp/blob/c8fa8c9e2aab72cf4e5db0e5d1c84a9ea6ee6eb0/libs/TRK_MINNOW_DOLPHIN/debugger/embedded/MetroTRK/Processor/ppc/Export/targsupp.s#L17) and [zeldaret/tww, commit f5234ec8, `src/TRK_MINNOW_DOLPHIN/ppc/Export/targsupp.s`](https://github.com/zeldaret/tww/blob/f5234ec8f4b8f4119b8db4a018c1ca25572988b1/src/TRK_MINNOW_DOLPHIN/ppc/Export/targsupp.s#L17) keep `TRKCloseFile` as assembly (`.fn TRKCloseFile` in targsupp.s).
- Origin: Metrowerks MetroTRK for Dolphin, `targsupp.s`, hand-written assembly in the vendor source; the exact CodeWarrior/MetroTRK build in Colosseum is not established.

## TRKPositionFile

Retail address 0x800C2A08, size 0x8.

- Why it cannot be C: the body is a single unconditional trap word (`twui r0, 0`, written here as `twi 31, r0, 0`) and `blr`. MWCC never emits a trap instruction from C; the routine exists only to hand control to the host debugger through the program exception.
- Other decompilations: [zeldaret/tp, commit c8fa8c9e, `libs/TRK_MINNOW_DOLPHIN/debugger/embedded/MetroTRK/Processor/ppc/Export/targsupp.s`](https://github.com/zeldaret/tp/blob/c8fa8c9e2aab72cf4e5db0e5d1c84a9ea6ee6eb0/libs/TRK_MINNOW_DOLPHIN/debugger/embedded/MetroTRK/Processor/ppc/Export/targsupp.s#L22) and [zeldaret/tww, commit f5234ec8, `src/TRK_MINNOW_DOLPHIN/ppc/Export/targsupp.s`](https://github.com/zeldaret/tww/blob/f5234ec8f4b8f4119b8db4a018c1ca25572988b1/src/TRK_MINNOW_DOLPHIN/ppc/Export/targsupp.s#L22) keep `TRKPositionFile` as assembly (`.fn TRKPositionFile` in targsupp.s).
- Origin: Metrowerks MetroTRK for Dolphin, `targsupp.s`, hand-written assembly in the vendor source; the exact CodeWarrior/MetroTRK build in Colosseum is not established.
