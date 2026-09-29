# modelShadowRender and fn_801E09E0: remaining walls (lane D4, 2026-09-29)

## `modelShadowRender__FP10GSgfxLayer` (96.13%)

- Literal pool: the shadow TU's `.sdata2` pool is 0x8047CBC0–0x8047CBE8
  (3.0f, 0.01f, 0.0f, 30.0f, 0.1f, 3000.0f, "shadow", and the unsigned
  int-to-float bias), padded to 8 at both ends. Only modelShadowRender and
  the linked `gs_model_shadow_candidate_800E9358` read it, and the latter
  reads only 3.0f (`lbl_8047CBC0`, through `modelShadowGetAvgScl`). If this
  function becomes exact, its carve can own 0x8047CBC4–0x8047CBE8, keeping
  3.0f as the extern the linked carve needs. That requires the constants
  as literals and "shadow" as a string literal emitted in `.sdata2` ahead of
  the bias. With literals the pool already comes out in retail order (the
  3.0f is emitted only while the helper reads a literal).
- The code is still about 170 rows of allocation away. The candidate uses
  r22–r31, one more than retail, because it hoists `&model->bound` out of
  the cast-model loop; retail rematerialises `addi r6, r26, 0x4c` at every
  use. The XD-named inline `modelShadowAddReceive(model, receiveModel,
  light, bound)` (XD `_modelShadowAddReceive__FP8_GSmodelP8_GSmodelP7GSlightP7GSbound`,
  UNUSED 0x2CC) aligns the loop tail with retail (70.0% raw vs 69.2%), but
  the bound is still hoisted. The same happens when the helper computes the
  bound itself, or also contains the light/scale checks.
  `-opt nopropagation`, `-O3`, `noloop`, `nolifetimes`, `-inline deferred`
  and `-O4,s` do not help.
- Nothing was committed. The next step is a construct that stops MWCC's
  loop-invariant motion of the bound address, which is needed before the
  pool can be owned.

## `fn_801E09E0` (95.66%) and `fn_801E0FB4` (100%, blocked)

The wait loops convert `fn_800D3088()` (u32) and `fn_800D37CC()` (s32) to
float. In retail those conversions use the TU-shared biases
`lbl_8047E400`/`lbl_8047E408`. The pool 0x8047E3D8–0x8047E428 is shared
with `gs_range_candidate_801DF1D0` (0x8047E3D8–0x8047E3F0) and
`field_range_801DF790` (0x8047E3F4–0x8047E408). Both are asm-backed
CodeCandidates whose objects reference those labels by name. A carve of
`gs_candidate_801E09E0` would emit its own conversion doubles, so it cannot
own or share the biases. A hand-written union conversion against the extern
bias compiles to `fsub`+`frsp`, not retail's `fsubs`, so it is not an
option either. This function can only link as part of the whole TU from
0x801DF1D0 (every function exact, with the TU owning the pool). The float
constants' scheduling differences (`lfd` biases before the `lfs` limit)
are consistent with a literal pool, and would be the first thing to
revisit in that whole-TU build.
