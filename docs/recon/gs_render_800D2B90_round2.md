# `fn_800D2B90` render/fog context (second pass)

This 0x258-byte function is still a text-only `CodeCandidate` carve in
`src/game/gs_render_util.c`. It is not linked or accepted decompilation
progress. The 2026-09-28 baseline report measured 76.96% fuzzy match.

A source-faithful multiplication spelling, `scale * converted.channel`,
matches retail's operand order better than `converted.channel * scale`.
The final report measures 77.09333% fuzzy (raw objdiff 77.06%). This is a
small exact-source improvement only, not a change in linked coverage.

The larger remaining differences are in the reference decrement's generated
register/temporary sequence and in how four fog-color byte values are
converted, stored, and rescaled. A direct equivalent spelling of the
reference-count decrement regressed to 69.83%; moving the fog-enable store
between the first and remaining channel conversions regressed to 72.96%.
Both probes were reverted. The existing nested null guard and the
`GSRenderColor` structure reconstruction were preserved. No asm, undefined
behavior, artificial target-instruction emulation, or score-only layout
coercion was introduced.

Shared-lock `all_source`/report and full `ninja -j2` builds passed. The
retail SHA-1 checks passed for `main.dol` and `common_rel.rel`, which remain
based on the linked objects rather than this candidate.
