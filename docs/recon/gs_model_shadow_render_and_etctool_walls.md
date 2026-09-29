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

## Etctool unit 0x801DF474-0x801E0FB4 (lane D2, 2026-09-29)

### Unit boundary and settings

- **Start:** the unit begins at `fn_801DF474`, not 0x801DF1D0. Its
  `.sdata2` pool is 0x8047E3F0-0x8047E428, in this order: 2.0, 0.0, 0.7,
  the signed and unsigned int-to-float biases, 1.0, 1.5, 0.6, 0.5, 0.8.
  The pool at 0x8047E3C8-0x8047E3E8 repeats some of these values. A single
  TU would have pooled the duplicates, so fn_801DE698-fn_801DF3D4 is a
  separate unit.
- **End:** the unit ends with `etctoolSetPokemonNakigoe`, at 0x801E0FB4.
  An earlier note put the end at 0x801E1170; that was wrong.
  `fn_801E0FB4` is XD's `_vtrUpdateFunc__FUlUl`. XD keeps it with the
  GSvtr functions, and XD's etctool unit also ends with
  `etctoolSetPokemonNakigoe`. `fn_801E0FB4` reads no pool constant, so it
  is linked as the data-free carve `gs_exact_801E0FB4`
  (0x801E0FB4-0x801E1170), next to the GSvtr carves after it.
- **Data it owns:** the `lbl_80279A00` sequence table (.rodata, 0x68
  bytes) and jump tables 80375100, 80375120, 80375160 and 803751B8. It
  presumably also owns `lbl_803750C8` (.data, 0x38 bytes: three GSvecs
  and a u16 0..8 table that no code references).
- **Compiler:** GC/1.3 (GC/1.3.2 and GC/2.6 give the same code), with the
  pool constants written as literals. Extern float stand-ins change the
  scheduling. `fn_801DF474` still reads 2.0f through `extern lbl_8047E3F0`
  and needs a literal in the whole-unit build.

### Status

| Function | Score |
|---|---|
| fn_801DF474 | 100% |
| fn_801DF790 | 100% (one tagged getter) |
| fn_801DFC30 | 100% (three tagged helpers) |
| fn_801E03D4 | 100% |
| fn_801E075C | 100% |
| etctoolSetPokemonNakigoe | 100% |
| fn_801E09E0 | 99.17% (two instructions short) |

### How the last rows closed

- **`fn_801DF790`:**
  - State 11 expands XD's `pokemonWazaForget`/`pokemonWazaCopy`
    (0x8013E9A0/0x8013EA24; XD asm has the same loop). The `u16 srcSlot`
    parameter gives retail's in-place `clrlwi r24`, and the `dst` local
    gives the direct `mr r27,r3`.
  - The state-8 move-id copy needs an unevidenced
    `fieldWazaGetId` getter (tagged).
  - The getter's return temp adds one interference edge to `running`, which
    then colours ahead of `wazaId`. `fieldWazaCountValid`'s invalid path
    written as `count = 0;` with an else, instead of `return 0;`, removes
    one temp and restores the order. The replay plus `simg.py` with K=29
    (r1, r2 and r13 reserved) reproduces all three variants.
- **`fn_801DF474`:** retail's `mr r26,r28` after the selection loop is
  the frontend's hoisted copy of `(u32)selectedItem`, so `selectedItem`
  is `s32`. The pcboxDelItem arguments must be masks (`& 0xFFFF`), not
  `(u16)` casts. The frontend hoists a cast as a `rlwinm`+`mr` pair, which
  the scheduler moves ahead of the copy. The backend hoists a mask as a
  single `rlwinm`, which stays behind the copy, as in retail.

### Remaining wall: `fn_801E09E0`

Retail sets up the call like this:

```
mr   r0,r3
addi r3,r1,20
mr   r29,r0
addi r4,r30,0
```

Ours is:

```
mr   r29,r3
mr   r4,r30
addi r3,r1,20
```

At the prologue, retail's `addi r30,r4,lo(lbl_803750C8)` goes straight to
r30. We emit `addi r0` followed by `mr r30,r0`.

- **The +0 is the whole problem.** Once `&sequencePositions[0]` is an
  `addi` rather than a copy, the scheduler keeps `r3` live across the
  object copy (the r0 routing follows; the direct-global form shows it).
  That also frees the prologue coalesce.
- **MWCC 1.3 never produced the unfolded +0.** The frontend folds `p + 0`
  in every spelling tried: member offsets, `&p->x`, `(u8*)p + 0`, `const`
  and enum zeros, inline helpers with a zero index, loops that run once,
  and index variables. With `opt_propagation off`, a zero index survives,
  but only as `li/mulli/add`.
- **Pooled data can't be the source either.** An `addi rX,rBase,0` does
  appear in this build when a function reaches pooled data at offset 0
  (memcard `...rodata.0`, objalloc, pslist `.bss`). But no compiler version
  or `-inline deferred` pools initialised `.data`, and retail's r30 is
  reached only at +0/+12/+24.
- **Other forms tried:** pointer-to-array, `f32*` and `u8*` bases, struct
  wrappers, assignment placement, a named call-result local, and
  per-function pragmas (`opt_propagation`, `opt_dead_assignments`,
  `opt_lifetimes`, `opt_loop_invariants`, `opt_common_subs`,
  optimisation levels 1-3). None gives retail's `addi r4,r30,0`. The best
  is still the committed literal source (32 aligned rows, 24 of them
  pool-label names).
- **GC/1.1p1 and 1.2.5n** do emit this exact shape, because they print
  every copy as `addi`. The rest of the function rules them out
  (300+ rows).
