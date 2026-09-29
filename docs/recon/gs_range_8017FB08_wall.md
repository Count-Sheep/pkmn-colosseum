# `fn_8017FB08` free-list matching wall

`fn_8017FB08` (`0x8017FB08`, `0x2A8` bytes) is the remaining function in
`main/game/gs_range_8017FA5C_suffix`. The 2026-09-28 baseline report scored
it at 95.4%; this object is a `CodeCandidate` and contributes no linked source.
The baseline candidate and retail each had 172 instructions.

The candidate implements the block lookup, ordered circular free-list insert,
coalescing with the previous block, and repeated absorption of following
blocks. Fresh objdiff shows these concrete differences:

- Retail initializes the inlined coalescing values in `r21`/`r20`; the
  candidate uses `r23`/`r22`. Retail carries the result of the block lookup
  through `r19`; the candidate uses `r21`.
- The previous-block end/data pair is held in `r21`/`r20` in retail and
  `r23`/`r22` in the candidate. The comparison and branch agree, but the
  values then have different lifetimes through the inline helpers.
- The `memAbsorbNext` expansion differs around retail instructions 61–110:
  retail stores two comparison operands at stack offsets `0x18`/`0x1C` and
  later the list-head and predecessor values at `0x14`; the candidate uses
  different registers and stack slots. This shifts the register used for the
  removed-list-node pointer (`r18` retail, `r20` candidate).

The source contains local pointer copies in `memFindPrev` (`a = target`,
`b = p->next`). Their presence in the current candidate is not evidence that
they were in the original C; the strict campaign policy does not accept pure
copies added solely to influence register assignment. No source-faithful
change was established in the first audit. An exact candidate must reproduce
the whole object under its declared flags and pass the quality and link gates
before the recomp can treat it as accepted evidence.

On 2026-09-29, removing the redundant `head` local in `memFindPrev` and
starting its search at `lbl_8047B1D0->next` improved the report score to
96.75883%. The source behavior is unchanged; the retail helper loads the
global head and then immediately its `next` member. This removed candidate
stack churn around the inlined predecessor search, but the function is still
unlinked. The remaining primary wall is the separate `memAbsorbNext` expansion:
retail initializes and spills both pointer-comparison locals, while this
candidate keeps one comparison operand in a register. Retail also assigns the
parent's end/data values `r21`/`r20` rather than the candidate's `r23`/`r22`.
The analogous direct-head change to `memAbsorbNext` lowered the score to
93.776474% and was reverted; a direct `target == p->next` comparison in
`memFindPrev` lowered it to 92.55882% (baseline) or 94.14706% (with the direct
head) and was also reverted. These tests distinguish the accepted semantic
cleanup from a claim of exact matching.

On 2026-09-29, the next source-faithful trial factored the byte-pointer
adjacency comparison into a small `memAreAdjacent(left, right)` inline helper.
The helper computes `left->data + left->size == right->data`, which is exactly
the existing coalescing predicate and makes the free-list operation explicit.
The guarded report now scores `fn_8017FB08` at **97.05882%**. Objdiff aligns
all retail instructions 71–79, including both comparison operands being
stored to and reloaded from stack slots `0x18`/`0x1C`; the previous candidate
kept one operand in a register. The remaining differences include the register
priority of the parent `end`/`data` pair and block lookup, the missing two
initial zero stores before the inlined absorber's null check, and list-head/
predecessor register allocation. Passing already-computed `end`/`data`
pointers to the helper scored 96.752945% and was reverted. This object remains
`CodeCandidate` and unlinked; the improvement is not port acceptance.

2026-09-29 bounded absorber-entry audit: guarded objdiff shows the current
candidate is 16 bytes shorter than the 680-byte retail function. Immediately
after the coalesced block is unlinked, retail emits `li r4,0`, `li r0,0`,
`stw r4,0x18(r1)`, and `stw r0,0x1C(r1)` around the inlined absorber's null
check (`+0xF4..+0x108`); the candidate lacks these four instructions. The
later adjacency calculation *does* write and reload both of those stack
homes on both sides, so the missing zeros are not evidence for a different
adjacency predicate. Moving that calculation into `memAbsorbNext` with natural
initialized pointer locals compiled to 94.11176% and changed the helper
expansion; it was reverted. Moving the main function's three node declarations
ahead of `end`/`data`, and separately moving the absorber's `head` declaration
after its other locals, each left 97.05882% unchanged; both were reverted.
Simply adding zero stores to force retail's stack frame would be an
unsupported compiler-shaping change. The baseline source remains unchanged;
the next correction needs original helper/local-lifetime evidence before
the isolated object can be made exact and linked.

## 2026-09-29: exact and linked (title-path exception)

Resolved on branch `claude/decomp-requests`. Two findings closed the wall:

1. The four missing instructions (`li r4,0; li r0,0; stw r4,0x18; stw r0,0x1c`
   before the inlined absorber's null check) are memAbsorbNext's own
   `u8* end = NULL; u8* data = NULL;` locals, the same shape as
   fn_8017FB08's. The adjacency test is written in the loop with those
   locals (no `memAreAdjacent` helper); their homes are 0x18/0x1C as in
   retail. Alone this scores 96.76%: the extra locals push the register
   ranking around.
2. At level 0 the frontend ranks locals for r31..r18 by reference weight,
   then declaration order (the colouring allocator never sees them).
   Retail ranks memAbsorbNext's head (r23) and memFindPrev's head (r22)
   above fn_8017FB08's end/data pair (r21/r20). memFindPrev gets a `head`
   local again (declared first, then `a`, `b`, `p`), and each helper takes
   one extra reference to its head with `(void)head;`. A `head = head;`
   self-assignment adds two references and overshoots (99.62%, the heads
   outrank memFindBlock's index and the absorber's result); one reference
   in each helper is exact.

`(void)head;` and memFindPrev's `a`/`b` copies are tagged
`RULE-EXCEPTION(title-path)` and listed in docs/RULE_EXCEPTIONS.md. The
one-function object is Matching; `ninja` passes the retail DOL/REL SHA-1.
