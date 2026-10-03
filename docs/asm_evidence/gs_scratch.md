# GSscratch locked-cache stack move in Pokémon Colosseum (GC6E01)

This is a first-party (game-code) assembly routine, admitted under the tier
described in README.md ("First-party (game) assembly"). It belongs to Genius
Sonority's GSscratch module, the GS engine's scratch allocator in the Gekko
locked cache at 0xE0000000 (32 blocks of 0x200 bytes). The retail translation
unit is 0x800EE928–0x800EEF48. Its C functions are linked in
`src/game/gs_scratch_800EE928.c` and `src/game/gs_scratch_alloc_800EEC38.c`.
This routine is the unit's last function. It sits alone in
`src/game/gs_scratch_exact_800EEDF8.c` (.text 0x800EEDF8–0x800EEF48).

No other decompilation of this code exists, so the routine is backed by a
compiler probe instead. The probe compiles the closest C candidate with every
GameCube MWCC build in `build/compilers/GC`, using the unit's own flags, and
compares the result with the retail instruction words.

## GSscratchInit

Retail address 0x800EEDF8.

- Why it cannot be C: the routine keeps a frame pointer (`mr r31, r1`), which MWCC sets up only around inline assembly. It reads LR into r3 mid-prologue (`mflr r3`) and parks it in `lbl_8047ABE4`. After the C-like bookkeeping (clear the 32 records, `bl LCEnable`, set the reserved size and base, mark the reserved blocks), it does the following when blocks were reserved:
  - it points r1 into the locked cache (`subi r1, r3, 0x8` with r3 = 0xE0000000 + size);
  - it writes a -1 back-chain word (`stw r3, 0x0(r1)`);
  - it reloads LR from `lbl_8047ABE4` and returns with `blr` on the new stack, skipping its own epilogue.

  The epilogue after that `blr` (`li r3, 0x1`, then the r31-based restore) is only reached when nothing was reserved. Moving the stack pointer and returning past the epilogue cannot be written in C.
- Compiler probe: docs/asm_evidence/probes/GSscratchInit.txt (candidate docs/asm_evidence/probes/GSscratchInit.c, the routine's bookkeeping in C) ends "verdict: no-match (20 compilers compared)" under the unit's -O4,p flags. An earlier run under the old candidate unit's -O2 flags also found no match across 20 compilers.
- Origin: Genius Sonority GS engine, GSscratch module (locked-cache scratch allocator); a C function with a block of hand-written assembly in the game's own code, kept whole as assembly here because inline asm is not admitted.
