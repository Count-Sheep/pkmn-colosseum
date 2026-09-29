# `GSpartGetTransform` loop-counter colouring wall

`src/game/gs_part.c` (unit `main/game/gs_part`, 0x800EE3BC–0x800EE6B4) is
99.79% in the report. The code is identical to retail apart from one
register. Both rotation/scale loop counters (`i = count`, counted down to
zero) get r24 in the candidate and r27 in retail. In retail, r27 is first the
strength-reduced byte offset of the parent-walk loop
(`stwx r24, r25, r27; addi r27, r27, 4`). r24 is the walker `jobj`, which is
dead (NULL) after that loop. Both registers are free at the counters' copy
`mr rX, r31`, and MWCC chooses r24 while retail chose r27.

XD's `GSpartGetTransform` (GXXE01 0x801002C8, trevor403/xd-asm
`func_FUN_801002c8.s`) has the same shape. There the counters take the
offset register (r26), not the walker's (r30). It confirms retail's
allocation, but not the source construct that produces it.

## Dead ends (2026-09-29, lane D4); all left the r24/r27 choice unchanged or were worse

- The counters as one function-scope `i`, or as two separate function-scope
  counters declared at each of the 64 position pairs in the declaration
  list. Position has no effect.
- `for (i = count; ...)`, `i > 0`, `s32`/`register` counters, and the copy
  after the `set__5GSvecFfff` call.
- Pre-decrement indexing (`lbl_804018B0[--i]`), `jobj` reused as the node,
  a function-scope node, and literal constants for the 0.0f/1.0f sets.
- Parent-walk variants: split `count++`, a `for` loop, an explicit byte-offset
  or index variable, `break` instead of `jobj = NULL`, `count > 15`, and
  moving `count = 0`.
- Compiler flags: GC/1.3.2, 2.0 and 2.6 give the same code, and 1.1/1.2.5
  differ widely. `-opt nopropagation`, `nocse`, `nolifetimes`, `noloop`,
  `nodeadcode`, `-inline deferred` and `-O3` are unchanged, and
  `nostrength` is worse.

The allocator's choice seems to depend on internal virtual-register order,
not declaration order. The next step is a real register replay
(`tools/local_campaign_colouring.py`; GC/2.6 replays this function
faithfully, since it emits the same code) once mwcc-debugger is installed.

## 2026-09-29 resolution (lane D4): linked

A register replay with cadmic/mwcc-debugger under GC/2.6 (which emits the
same code as GC/1.3 for this unit) showed the cause.
Named locals got virtual registers in declaration order, highest first:
`jobj` r41, `count` r40, then each block's `i` and `node` (r39/r38 and
r37/r36). The parameters were r32–r35. Nonvolatiles are coloured from the
highest virtual register down, and each value takes the lowest callee-saved
register already in use that it does not interfere with. The counters were
coloured after `jobj` and so reused its r24.

The fix declares the loop locals at the top of the function, ahead of `jobj`,
in the order rotation node, rotation index, scale node, scale index. Each
node is then coloured first and takes r26. Each index is coloured next and
takes r27, the parent walk's dead offset register, and `jobj` keeps r24. The
earlier function-scope trials put the counters before `jobj` without the
nodes, so the counters took r26. `GSpartGetTransform` is 100% and `gs_part.c`
links as Matching with retail DOL/REL SHA-1. Only declaration order changed,
so this is not a rule exception.
