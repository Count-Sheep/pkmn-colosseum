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

## Third pass (2026-09-29, lane D2): exact, not yet linked

fn_800D2B90 is now 100% in the report, with no pragma (the old local
`optimization_level 2` is gone). Three source facts closed it:

1. The fog-colour tail is fn_800D36B4's body expanded with the colour
   passed **by value**. Retail copies `color` word by word into a stack
   temporary (8(r1)) before converting it. A `static inline
   gsSetFogColor(GSRenderColor)` reproduces that copy.
2. The scale is the TU's pooled **literal** 255.0f (0x8047C9F0, used only
   by fn_800D2B90 and fn_800D36B4). As an `extern f32` it is reloaded after
   every byte store, and even `extern const` changes the schedule. As a
   literal it is loaded once, as in retail (lane D4 found the same pattern
   in the 0x800D-0x8010 gfx area).
3. The bytes are stored straight from float (`state->fogColorR = 255.0f *
   color.r`). Going through `s32` values lets the forward peephole drop the
   `clrlwi` truncations before the first scheduling pass, which reorders the
   int-to-float conversions. A diagnostic `#pragma peephole off` build
   showed that retail was scheduled with the truncations present.

The previous-fog release also only clears `lbl_8047AA8C` when a fog
existed (retail's first `beq` skips the store).

Linking is blocked by the literal pool: a one-function carve would own
.sdata2 0x8047C9F0-0x8047CA00 as compiler literals, but fn_800D36B4's
retail object references `lbl_8047C9F0` by name, and a defined
`const f32 lbl_8047C9F0` is folded into a second literal. The route is one
object covering 0x800D2B90-0x800D377C (with the already-exact
fn_800D2DE8-fn_800D3690 carves) once fn_800D36B4 is exact.
