# GSgfx BeginFrame (`fn_800D3FA4`)

The retail function occupies `0x800D3FA4`–`0x800D45F8` (0x654 bytes). It is
currently built from `gs_gfx_candidate_800D3FA4.c` as a `CodeCandidate`, not
linked source.

The 2026-09-28 audit found a real argument mismatch in the first pass setup.
Retail loads byte `0x49C` from `lbl_8047AA80` immediately before its first
`fn_800DA2BC` call; the candidate passed zero. Passing that state byte in
`gs_gfx_range_800D3FA4.c` raises the authoritative objdiff score from
94.54815% to 95.39012%. Retail and candidate now each contain 407
instructions. The changed call is observable behavior, not a register probe.

On 2026-09-29, the candidate's long-lived local alias for the stats buffer was
removed. Directly addressing `lbl_804001F0` at each stats access preserves the
same reads and writes and lets MWCC produce the retail initial address load and
post-`HSD_FogSet` reload. The canonical report now measures `fn_800D3FA4` at
**96.98025%**, still a `CodeCandidate` in an incomplete, unlinked object. The
function remains 0x654 bytes; the current objdiff instruction view has 406
instructions on each side. The owner source SHA-256 is
`ceebd5c6a65cacb70762dcab45cbac731149dfadb6afaa3418896e145300f478`.

The first remaining difference is in the prologue: retail saves `r24`–`r31`
at stack offset `0x10`, while this candidate saves `r22`–`r31` at `0x08`.
That register-lifetime difference persists through camera and timing work.
Around retail instructions 249–267 the shape diverges further: retail masks
`flags` with `-0x102`, then tests bits again for the second pass, including a
call guarded by bit 0. The candidate's compiler precomputes the second-pass
tests and omits that guarded call after seeing that the mask clears bit 0.
The repeated pass bodies suggest a source-structure question, but do not yet
establish an original inline helper or a safe change to force code generation.

The source remains a candidate. Guarded `ninja -j2 all_source
build/GC6E01/report.json` and `ninja -j2` pass; both retail SHA-1 checks pass
through asm fallback. The 27 quality-scan tests pass. These checks do not prove
BeginFrame matches or is linked. A future promotion must recover the
second-pass source structure naturally, reach an exact full object, and pass
the normal quality and link gates.

The 2026-09-29 second-pass audit confirmed the retail data flow precisely:
`li r0,-0x102; and r30,r30,r0; clrlwi. r0,r30,31; beq; bl
fn_801E17A8`. The masked value and the retested value are the **same saved
register**, not a reloaded or aliased field. There is no source evidence for a
pointer escape, volatile parameter, or mutable global that would make bit 0
live again. A guarded source probe changed the function's `flags` type from
`u32` to `s32` to align with the currently reconstructed caller declaration in
`fn_801E0FB4`; the report stayed at 96.98025%, and the candidate emitted a
signed `cmpwi` where retail uses an unsigned bit test. The probe was reverted;
the owner source hash remains
`ceebd5c6a65cacb70762dcab45cbac731149dfadb6afaa3418896e145300f478`.
The current candidate already has the retail-observed behavior for all input
flags; the remaining unreachable branch and extra candidate register lifetimes
are a compiler/source-structure matching problem, not grounds for inventing
effects to keep dead code. No `CodeCandidate` acceptance or link status changed.

## 2026-09-29 (lane claude/recomp-blockers-gfx): optimiser-flag probe

With the unit-wide option `-opt nopropagation` (GC/1.3), the unchanged
candidate source compiles to retail's exact size (0x654), saves r24-r31 at
0x10 like retail, and keeps retail's second-pass `li r0,-0x102; and
r30,r30,r0; clrlwi. r0,r30,31` sequence (no hoisted first-pass tests). The
raw diff drops to register colouring only (91.4%, 37 rows): retail colours
`flags` first (r30) ahead of oldMode/oldMask (r29/r28) and
resetQueue/setupCamera (r27/r26); the candidate colours `flags` last (r26).
Local declaration order moves oldMode/oldMask but not `flags`, and neither do
parameter types or the form of the mask statement.

The flag was **not** adopted. Evidence for a TU-wide setting is incomplete:
of the Matching gs_gfx siblings 800D3074, 3190, 3410, 361C, 377C, 45F8,
4F98, 67BC and 6A00 are byte-identical under `-opt nopropagation`, but
gs_gfx_exact_800D56C0 changes, and the TU boundary is not established. The
default-flag form cannot reproduce retail's unfolded second-pass test: an
inline pass helper (parameter `flags & ~0x101`) or the in-place mask are
both folded by value numbering, and a two-iteration loop is not unrolled.
A pass-helper structure is still likely (the pass body is expanded twice and
the layer camera setup six times), but it does not match on its own.
