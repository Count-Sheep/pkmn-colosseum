# GSmsg link campaign: lane D14 (2026-09-30)

Branch `claude/decomp-d14-gsmsg`, starting from integration head b21e57c9.

## Result

| Commit | Change |
|---|---|
| a8d58065 | fn_800FC7E0 (XD `_msgMainSub`) report-exact (98.94% -> 100%); fn_800FAEF8 97.09% -> 97.49% |
| 7e1a7c0f | **Linked** fn_800F96E4 as `gs_msg_exact_800F96E4` (0x800F96E4-0x800F9AEC), row 36 |
| 520b1074 | fn_800FD69C 90.55% -> 91.49% |

## Why the other blockers cannot link as text-only carves

Every blocker that is 100% but unlinked converts integers to float:
GSmsgGetRect (row 43), fn_800FBB34 and fn_800FD348 (row 25), and
_msgGetSize__FPCUs (rows 7/36/42). MWCC emits the conversion bias as an
anonymous `.sdata2` literal. Retail keeps the biases in the GSmsg TU pool
(lbl_8047CD10 signed, lbl_8047CD28 unsigned). Every unlinked GSmsg object
references that pool by name, so a text-only carve cannot own it.

- A **float** conversion (`fsubs`) cannot be written by hand. No GC
  compiler from 1.0 to 3.0a5.2 has `__fsubs`, and a union conversion against
  the named bias compiles to `fsub; frsp`. The `float_constants` and
  `gecko_float_typecons` pragmas do not change the literal. This rules out
  GetRect, FBB34, FD348, FAEF8, FD69C, GSmsgInitRuby, GSmsgAdjustAlign and
  fn_800FB43C/FB680/FB8C8.
- A **double** conversion (`fsub`: _msgGetSize, GSmsgExec,
  GSmsgSetFontInfo) can be written as a union against the named bias.
  For _msgGetSize, `scratchpad/d14/carve_FE010.c` gets to 6 rows. The
  only difference is that the bias and the stack value swap f1/f2. MWCC
  colours FPRs in descending virtual-register order, and the native
  conversion creates the bias register before the stack load. The union
  form creates the stack load first, whatever the operand order, inline
  parameters, out-parameters, casts or multiple definitions. Only
  `#pragma opt_propagation off` with two locals (`a = 0.5; c = bias;`)
  gives the right order. That pragma also stops constant propagation into
  the font loop's induction start, which leaves one extra `slwi r5,r5,3`.
  Not committed.

So rows 25, 26 and 43 need **one object that owns the pool**. Plan:
`.text` 0x800F9D04-0x800FE35C (F9AEC/F9C04 stay out; they don't use the
pool) plus `.sdata2` 0x8047CD00-0x8047CD50. The pool's first three entries
are named file-scope constants: two `GXColor` whites (CD00 used by FC7E0,
CD04 by SetColor) and `lbl_8047CD08` (1.0f, still named by the linked
F96E4 and GSmsgInit units). The literals then follow in creation order:
signed bias (AdjustAlign), 1.0, 0.5, unsigned bias (SetFontInfo), 2.0f,
0.5f, 0.4f (InitRuby), 8.0f, pi, 25.0f, 4.0f (FC7E0), 1/512 (FD69C). The
source's named pool externs lbl_8047CD18..CD48 must become literals in
that object. **This needs fn_800FAEF8 and fn_800FD69C exact.** Those two
walls are all that is left.

## Techniques that worked here

- **Inline-return temporaries are numbered late.** MWCC gives an
  inline's locals and parameters virtual registers in reverse creation
  order. Inlines expand round by round (top-level calls in textual order,
  then nested ones). Taking the dispatcher's entry pointer through a
  one-line inline (`msgCtrlEntry`) moved it after the renderer's other
  temporaries, giving retail's r22/r19. The select rule from D5's
  `simg_sel.py` reproduces MWCC exactly: volatiles first, then the
  lowest already-used saved register, then a new one from r31 down. Use it
  with the mwcc-debugger dump to find which colouring order the target
  needs, then find a source form that produces it.
- A local pointer to an array stops MWCC folding the array's stack address
  into the index: `saved = savedStack; ... saved[i]` hoists `addi r25,r1,20`.
- A local that holds a derived pointer and is used twice (`str = base +
  0x4D0; str[0xFF] = 0; f(..., str)`) keeps `addi r4; stb 255(r4)`.
- `/ 512.0f` instead of `* named_extern` changes FPR colouring, because
  the literal is created later.

## Walls left

### fn_800FAEF8 (XD GSprint / GSvtr_DrawText), 67 rows

1. **The set-up copy.** Retail: `addi r30,r31,0x5D0; mr r3,r30; bl memset`,
   then `mr r7,r30`. Every task-init and tail store goes through r7, except
   the first flag store `stb 1,0x5D0(r31)`. msgSetFontInfo's stores and the
   loop go through r30. XD has the identical shape. The frontend
   propagates every source-level copy (`p = work`), including under
   `opt_propagation off`, where the backend propagates it instead. So no
   copy survives. Tried: inline parameters, inline returns
   (msgInitTaskR/msgTaskOf), a parameter-modifying inline, casts, a
   manual expansion with p at every position, `static` auto-inline, and
   C++ mode (identical codegen). A backend copy (`mr`) only survives when
   CSE makes it and its uses are in other basic blocks.
2. **base/work colour order.** As soon as msgSetFontInfo goes through
   `work` (retail r30), base loses interference and is coloured after work
   and color: work r31, color r30, base r29, which costs 147 rows. Retail
   keeps base r31 / work r30 / color r29. Base is a backend temporary (the
   lis/addi of lbl_80401DE0); declaration order has no effect.

### fn_800FD69C (XD `_msgMakeTexture`), 166 rows

1. **The swizzle store.** Retail: `add lo,lo,image; stbx v,hi32,lo`, with
   the lo and hi chains interleaved. The codegen always makes the
   loop-invariant pointer the base (`stbx v,image,(lo+hi32)`) for any
   single expression: pointer, integer, casts, `&a[i]`, inline returns.
   The retail form appears only when `lo` is a multiply-defined variable
   (`lo = ...; lo += (s32)image; *(u8*)(hi + lo) = v;`, see
   `scratchpad/d14/c8*.c`). In the real function that extra variable
   shifts the whole register assignment (+150 rows). Hill-climbs over
   declaration order (30 locals, with and without `opt_propagation off`)
   found nothing.
2. **Second-loop counters.** Retail uses `li r3,0; mr r0,r3` (rowIndex r3,
   source offset r0 as a copy) and xPos r10. Writing the source row as
   `arg1 + rowIndex * widthRounded`, which MWCC strength-reduces, gets the
   order but not the copy.

### fn_800F9AEC / fn_800F9C04

Unchanged: the switch-leaf `beq` wall (D9). They are now split off by
themselves (`gs_msg_r56b_800F9AEC`), so they do not block the pool object.

## Scripts (scratchpad/d14)

`msgdiff.py` (whole-TU function diff), `tv.py` (variant runner),
`hc.py` (declaration-order hill-climb), `dbg.sh` (mwcc-debugger replay),
`simg.py`/`simg_sel.py` (D5's GPR select simulator), `carve_FE010.c`.
