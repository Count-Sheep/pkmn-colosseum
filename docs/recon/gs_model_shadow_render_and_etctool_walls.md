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

## Etctool unit 0x801DF474-0x801E1170 (lane D2, 2026-09-29)

### Unit boundary and settings

- **Start:** the unit begins at `fn_801DF474`, not 0x801DF1D0. Its
  `.sdata2` pool is 0x8047E3F0-0x8047E428, in this order: 2.0, 0.0, 0.7,
  the signed and unsigned int-to-float biases, 1.0, 1.5, 0.6, 0.5, 0.8.
  The pool at 0x8047E3C8-0x8047E3E8 repeats some of these values. A single
  TU would have pooled the duplicates, so fn_801DE698-fn_801DF3D4 is a
  separate unit.
- **End:** the unit runs through `fn_801E0FB4` (end 0x801E1170).
- **Data it owns:** the `lbl_80279A00` sequence table (.rodata, 0x68
  bytes) and jump tables 80375100, 80375120, 80375160 and 803751B8.
- **Compiler:** GC/1.3 (GC/1.3.2 and GC/2.6 give the same code), with the
  pool constants written as literals. Extern float stand-ins change the
  scheduling. `fn_801E075C` scored 48% under the old GC/1.2.5n chunk
  setting and 100% under GC/1.3.

### Status

| Function | Score |
|---|---|
| fn_801E075C | 100% |
| fn_801DFC30 | 100% |
| fn_801E03D4 | 100% |
| etctoolSetPokemonNakigoe | 100% |
| fn_801E0FB4 | 100% |
| fn_801E09E0 | 99.17% |
| fn_801DF790 | 99.22% |
| fn_801DF474 | 98.42% |

The unit cannot link until all eight functions are exact.

### Remaining walls

- **`fn_801E09E0`: two instructions.** Retail sets up the call to open the
  sequence object like this:

  ```
  mr   r0,r3
  mr   r29,r0
  addi r4,r30,0
  ```

  This is the object result routed through r0, then
  `&sequencePositions[0]` as an explicit `+0` from the base register.
  Retail's r0 routing is the fingerprint of an inline helper's return. Our
  build either folds the `+0` into `mr` or rematerialises `lis/addi` for
  the global.

  Every form tried is worse or equal (28 rows is the best, 32 is the
  committed literal baseline):
  - the direct global;
  - local pointer spellings (`sequencePositions + 0`, `&*sequencePositions`);
  - inline open-object helpers taking the data, an index or the positions
    array;
  - `GSvecCopy` wrapper inlines taking a global plus index, pointer plus
    index, or pointer arithmetic. These were tried at one call site and at
    all four, and give 28-35 rows (111 rows with all four using the global
    wrapper).

  In every inline-helper version, the helper's result is coloured to r24
  instead of r29.
- **`fn_801DF790`: 8 rows.**
  - Two rows are the move-id copy. An unevidenced
    `static inline u32 fieldWazaGetId(u16, u16)` getter fixes them. It is
    not applied, because nothing names that helper.
  - The other six are case 11. Retail truncates `nextSlot` in place
    (`clrlwi r24,r24,16`) and moves the destination pointer straight into
    its home register (`mr r27,r3`). We emit `mr r0,r3; mr r27,r0`, which
    means our pointer value is still routed through a temporary. Sweeps
    over declaration order, block locals, u16/s32/u32 types and helper
    shapes (the f10-f13 sweeps) did not close it.
- **`fn_801DF474`: 5-7 rows.** Retail copies the selected item entry into
  r26 after the weighted-selection loop, a hoisted copy. Our build keeps
  it in the loop register. Retyping the index, cursor and weight, casts,
  a pointer-parameter helper and a selection helper all failed to move it.

The next step for all three is register-replay work (mwcc-debugger), to
find which virtual register ordering puts these copies where retail has
them. It is not the source structure: that matches everywhere else.
