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

## Lane D2 follow-up (2026-09-29)

- **Compiler:** GC/1.3 through 2.7 give the same 176-row aligned diff, so
  this is not a compiler-version wall.
- **Literals:** turning the extern pool constants and "shadow" into
  literals leaves 179 differing rows. The literal pool matters for linking,
  but it is not the wall.
- **Replay** (mwcc-debugger, GC/2.6 = GC/1.3): the extra register comes from
  the frontend, not the backend. The first backend dump already has the
  inner (`j`) loop preheader holding hoisted address temps
  `@189/@190/@191 = &model->scale.{y,x,z}`, `@192 = &model->bound` and
  `model + 0x58`. The scale and +0x58 temps are folded back into their
  loads by add-propagation. `@192` feeds only call arguments
  (`mr r6, r73` x3, `mr r3, r73`), so it stays in a callee-saved register;
  retail instead rematerialises `addi r6, r26, 76` at each call.
- `#pragma opt_common_subs off` restores retail's r23-r31 frame (144
  differing rows) but reloads the model flags that retail CSEs, so it is
  not a whole-function answer. `opt_loop_invariants off`, `opt_lifetimes
  off`, `opt_propagation off` and `opt_strength_reduction off` either keep
  the hoist or are worse. So do a `(GSshadowBound*)((u8*)model + 0x4C)`
  spelling and the loop-shape variants.
- The first loops also differ in coalescing. Retail reuses the init loop's
  zero register as the outer byte offset and loads `i` with `li r28, 0`;
  ours copies the zero (`mr`). The inner loop has the same pattern (retail
  `li r31, 0` for the offset, ours `mr r31, r26`). Both point at the
  offsets/counters being distinct virtual registers created in a different
  order, the same kind of priority effect as the GSpartGetTransform fix.
