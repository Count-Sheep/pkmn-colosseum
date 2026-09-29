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
