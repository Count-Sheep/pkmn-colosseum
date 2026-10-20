# Dolphin OSReset.c `Reset` in Pokémon Colosseum (GC6E01)

`OSReset.c`'s static `Reset` is the routine `__OSDoHotReset` jumps to after
masking interrupts. It is hand-written assembly in the Dolphin SDK. In this
project it is linked on its own as `dolphin/os/OSReset_exact.c`
(`Reset_8009FF50`), and the copy in `src/dolphin/os/OSReset.c` is compiled
out of every unit. The body matches the retail bytes in objdiff and in the
final link (`ninja`: 26 files OK).

## Reset_8009FF50

Retail address 0x8009FF50, size 0x70 (`Reset` in `config/GC6E01/symbols.txt`,
scope local); unit `dolphin/os/OSReset_exact.c`.

- Why it cannot be C: it sets HID0's instruction-cache-flash bit (`mfspr`/`ori`/`mtspr HID0`), synchronises (`isync`, `sync`), busy-waits on the time base (`mftb r5, 268` / `mftb r6, 268`) and then writes the reset code to the processor interface register 0xCC003024, hanging in a `nop` loop. The routine is laid out as a chain of forward `b` instructions that jump over each block before branching back to it, so every block is fetched into the instruction cache before it runs. MWCC never emits `mftb`, HID0 moves or `isync` from C, and no C control flow produces this branch-over-and-back layout.
- Other decompilations: [doldecomp/dolsdk2004, commit 2328b416, `src/os/OSReset.c`](https://github.com/doldecomp/dolsdk2004/blob/2328b4164b1a98422a2255d83ce9a5a7548990cc/src/os/OSReset.c#L101-L140) and [projectPiki/pikmin2, commit 999140f3, `src/Dolphin/os/OSReset.c`](https://github.com/projectPiki/pikmin2/blob/999140f3521e22619547d5d61862a6577ea73882/src/Dolphin/os/OSReset.c#L69-L119) keep `Reset` as an `asm` function with the same instructions and branch chain.
- Origin: Nintendo Dolphin SDK, `os/OSReset.c`, hand-written assembly in the vendor source.
