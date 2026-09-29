# `psRemoveParticle` whole-TU closure

The 2026-09-28 report measures `psRemoveParticle` at 99.37956% fuzzy match
(548 bytes), but the reconstructed `particle.c` owner is not linked. Raw
objdiff for `main/game/ps_candidate_80169A48` measures 97.22628% because
the candidate is compiled from the whole included TU and its branch/data
relocations are not the retail carve's relocations. Neither score is accepted
linked progress.

The decisive instruction mismatch is still the inlined `psKillParticle` list
walk in `psRemoveParticle`: retail assigns `next`, `prev`, and `current` to
`r26`, `r27`, `r28`, while the candidate assigns them `r28`, `r26`, `r27`.
The standalone `psKillAllParticle` expansion is exact, so altering its
algorithm only to rotate registers would risk that accepted carve. The
owner's top-of-file notes record an extensive prior source-faithful search
over loop/declaration/inlining forms. This pass confirmed the same residual
and retained the existing source; there is no new semantic evidence for a
different list-walk behavior.

The `particle.c` candidate chunks are score instrumentation for a larger
retail TU with text and data ownership. They cannot be promoted to
`Matching` merely because one function approaches 100%; a valid exact
whole-object source, linkage, quality scan, and retail DOL/REL hashes remain
required. Shared-lock `all_source`/report and `ninja -j2` were up to date,
and the retail SHA-1 check passed for both `main.dol` and `common_rel.rel`.

## Resolution (2026-09-29, lane D2): exact, particle.c linked

The MWCC register replay (cadmic/mwcc-debugger with GC/2.6, which emits
the same psRemoveParticle as GC/1.3.2, including the same 14-instruction
residual) explains the wall:

- The frontend numbers inlined locals breadth-first. In psRemoveParticle
  the level-1 expansions come first (psKillAllParticle's `i`/`next`/`pp`
  @640-642, psClearPointJObj's `i` @648), then level 2 (psKillParticle's
  `p`/`prev` @653/654, its `pp` parameter @655), then psDeletePntJObj.
  Backend virtual registers run in reverse id order (r50 `i`, r49 `next`,
  r48 `pp`, r47 point-JObj counter, r46 `p`, r45 `prev`).
- Every one of these has fewer than 29 remaining neighbours, so they sit in
  one colouring level, coloured from the highest virtual register down;
  each takes the lowest free already-used saved register, or a new one
  from r31 down.
- Retail's r29/r28/r27/r26 for `i`/`p`/`prev`/`next`, with the
  point-JObj counter reusing r29, needs the order `i`, counter, `p`,
  `prev`, `next`. That puts `next` after psKillParticle's locals, which
  only a local created at or below psKillParticle's level can be.
  Declaration order, block scope and loop form only reorder ids inside
  level 1, which is why the earlier sweeps could not close it.

Fix: psKillParticle reads the successor first and returns it, and
psKillAllParticle walks each list with `pp = psKillParticle(pp)`. The code
is identical; psRemoveParticle and the standalone psKillAllParticle both
match. HAL's psKillParticle returns void (Melee's header; GNT4's live copy),
so this is a title-path rule exception (docs/RULE_EXCEPTIONS.md).

With every function exact, `game/particle.c` links as one Matching object
(.text 0x80169034-0x8016A644, .rodata 0x80273820-0x802738B8, .data
0x8036BF80-0x8036BFA4, .bss 0x804527C8-0x80452DE8, .sdata2
0x8047D5B0-0x8047D5C0). It replaces the ten function carves and their
named-literal stand-ins; `rodata_80273820.c` became `rodata_802738B8.c`, and
`sdata2_8047D560.c` was split at 0x8047D5B0 and 0x8047D5C0
(`sdata2_8047D5C0.c`). The unreferenced psKillParticle and psClearPointJObj
bodies are dead-stripped. Retail DOL/REL SHA-1 pass.
