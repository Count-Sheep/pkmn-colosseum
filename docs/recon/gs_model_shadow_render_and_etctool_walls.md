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

### 2026-09-29 second pass (lane D4): 96.13% to 99.79%, not exact

I replayed each step with mwcc-debugger (GC/2.6, same code as GC/1.3) and
checked colouring orders with a copy of D5's GPR simulator
(`simg.py` plus a select pass; K between 26 and 30 reproduces the dump).

- A pragma scoped to an inline's definition has no effect; only the pragma
  state at the function being compiled counts. `opt_common_subs off` on
  the whole function does not remove the hoist.
- The hoist is the frontend's loop-invariant motion. It creates
  `@N = model + 0x4c` in the j-loop preheader (visible in
  `frontend-01-ast-after-optimizations`). `#pragma opt_loop_invariants off`
  removes it. Then CSE shares one bound address across the three calls
  within an iteration, which still costs r22. Writing the argument three
  distinct ways (`(u8*)model + 0x4cU`, `(s32)model + 0x4c`,
  `(u32)model + 0x4c`, found by a 360-variant sweep) gives retail's
  r23-r31 frame and per-call `addi r6, r26, 0x4c`. Loop syntax (while, goto,
  a combined if) does not stop the hoist, and neither does routing the
  bound through the XD-named `modelShadowAddReceive` inline.
- Stack slots: nesting `modelShadowBoundToSize` one inline level deeper
  (a single-use `modelShadowSetReceiver`) makes its GSvec temporary take
  the lowest slot, as in retail (0x8, then 0x20/0x14 in the list loop).
  Inline locals are allocated breadth-first.
- The XD-named `modelShadowInitReceiveList` inline gives the first loop
  retail's volatile counter and a zero constant that is not shared with the
  model-loop counter.
- `count = 0` before `set__5GSvecFfff(&avg, ...)` lets the scheduler put
  `li r26,0` ahead of the call, as in retail. Separate named locals
  `receiver`/`k` for the receiver loop, and the declaration order
  receiver, i, j, model, count, slot, k, castModel, light, list, index,
  valid, give retail's registers everywhere else.
- Float literals fix the final `size < 0.1f` register order.

What is left is 9 rows in the object-list loop: retail colours the
searched model r27 and the list r23; the candidate gives r23/r24. The
searched model is a backend-split web (a new, highest vreg). Its degree
(17, below K) puts it in the last colouring group, where it takes light's
dead r23. The simulator shows that retail's result needs that web coloured
between `count` and `slot`. That means either `slot` at degree below K
(retail's slot web would have to interfere with about 9 fewer values), or
the searched model at degree 31-32. Renaming the web's variable, a
separate named local, an `if (model != NULL)` body and a `while` loop all
leave it unchanged.

For the link: with `-str reuse,readonly` the unit's pool comes out exactly
as retail's 0x8047CBC4-0x8047CBE8 contents (0.01f, 0.0f, 30.0f, 0.1f,
3000.0f, "shadow", pad, bias). But the object's `.sdata2` is 8-aligned, so
the unit must own 0x8047CBC0 as well. That means defining the 3.0f in
this unit, probably as a `lbl_8047CBC0` global so the linked 800E9358
carve still resolves. It also needs the unit split out of its
list-comprehension flag group.

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
