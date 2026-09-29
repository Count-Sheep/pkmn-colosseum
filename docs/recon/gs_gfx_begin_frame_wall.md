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

### Same probe on `fn_800D461C` (render-command interpreter)

Retail reads each command's arguments, advances the cursor, and then calls
the handler. Writing the cases that way (`p += n; handler(p[-n], ...)`,
with the matrix cases 63-65 and case 39's trailing block kept
call-then-advance) takes the candidate from 72.63% to 98.97% under the
default flags. Case 91 also passes `(s8)p[24]` (after the leading `*p++`),
not `p[25]`: the old source read one word too far. Under `-opt
nopropagation` the raw diff narrows further (the in-place cursor updates
around cases 39 and 67 then match), leaving case 80's `mr r3`/`addi r4`
order and case 91's advance-before-call, which only a named view of the
cursor (`data = p; p += 25; fn(arg, data, data[24])`) reproduces. Two
functions of this range thus point to a propagation-off build, but the
unit-wide flag is still unproven (see above). Linking this unit would also
need its jump table (`.data` 0x80314188, 0x170 bytes) added to the split.

## 2026-09-29 (lane D4): `fn_800D461C` linked

Default flags, no `-opt nopropagation`. Case 80 matches with the plain
`argument = *p++; memcpy(&copied, p, 4); p++;` (the earlier source read
`p[0]` and then incremented it separately, and MWCC folded the increment).
Case 39 matches with the three arguments read by `*p++` into named locals
before the call. Case 91 needs the cursor copy `data = p; p += 25;
fn_800DB900(argument, data, (s8)data[24]);`, which is recorded as a title-path
exception in docs/RULE_EXCEPTIONS.md. Forms that failed: the call before
`p += 25`; `p += 26` with `p - 25`/`p[-1]`; a named `flag` local. The
`(p += 25)` in-argument forms also match, but they are unsequenced, so they
were not used. The unit now owns its switch table (`.data` 0x80314188–0x803142F8);
`game/data/data_80314188.c` was removed. The object links as Matching and the
retail DOL/REL SHA-1 pass.

## 2026-09-29 (lane D4): `fn_800D3FA4` linked

The pass body is now the helper `gfxRenderPass(flags, setupCamera, pass)`.
In retail it expands twice, with the same calls and constants apart from the
pass index. Inside it are `gfxSetupLayerCamera(layer, pass)` (six expansions)
and `gfxRunQueue()` (four). All three are admitted by repeated expansion. The
second pass's mask is applied inside the helper, `if (pass != 0) flags &=
~0x101;`, rather than at the call site. Under `-opt nopropagation` this
gives retail's exact code, including the `flags` colouring (r30) that the
call-site mask could not reproduce. Under the default flags MWCC still folds
the dead bit-0 re-test.

The flag applies to this one-function unit only, which is consistent with the
TU layout. The `.sdata2` literal pools are per-TU and padded to 8. They put
0x800D2DE8–0x800D3E4C on 0x8047CA00–0x8047CA20, fn_800D3FA4 alone on
0x8047CA20–0x8047CA30 (0.0f, 640.0f, 480.0f), and fn_800D55D0/fn_800D5648
on 0x8047CA30–0x8047CA40. So gs_gfx_exact_800D56C0, which changes under
the flag, sits after a later TU's pool and is not fn_800D3FA4's TU sibling.
The float-free candidates for its TU, gs_gfx_core and gs_gfx_exact
800D45F8/800D4F98, are byte-identical with and without the flag.
fn_800D461C matches only under the default flags (case 80's argument order)
and stays in the following TU. The constants are literals, and the unit owns
`.sdata2` 0x8047CA20–0x8047CA30 (`sdata2_8047C9B0.c` now ends at 0x8047CA20).
The object links as Matching and the retail DOL/REL SHA-1 pass.
