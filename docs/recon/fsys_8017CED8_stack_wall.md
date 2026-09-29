# `fn_8017CED8` FSYS TOC callback

`src/game/fsys/fsys_file_candidate_8017CED8.c` is a text-only
`CodeCandidate` carve (0x8017CED8-0x8017D3A0), not a linked whole retail
object. Its current 1224-byte function measures 99.767975% fuzzy match.
This is not accepted decompilation progress until exact whole-object linkage
and retail hash/quality gates pass.

The candidate has the same 0xC0-byte stack frame, instruction count, opcode
sequence, and non-stack operands as retail. All 71 printed objdiff
differences are stack-slot offsets. For example, the first inlined cache
pass stores to 0x5C/0x60 in retail but 0x48/0x4C in the candidate; the
entry-loop index home is 0x8 in retail but 0x70 in the candidate. The source
already preserves the observed two size reloads in the second eviction pass
and the copied entry index/sub-entry state in the trailer-reset loop.

2026-09-28 bounded source-faithful probes moved the entry-loop index and
entry/sub-entry pointer declarations between function and loop scope. MWCC
produced the same score in each case. All probes were reverted, preserving
the pre-existing reconstruction. No artificial stack-local padding, asm,
pragma, or other layout coercion was used.

Final shared-lock `all_source`/report and `ninja -j2` completed; retail
SHA-1 checks passed for `main.dol` and `common_rel.rel`. Those hashes are for
the currently linked image, which does not include this candidate carve.

2026-09-28 re-audit: current retail and candidate objects each contain 306
instructions. The opcode-mnemonic sequence is identical, and the same 71
formatted differences are stack offsets only. The owner source is unchanged;
no new behavior or data-flow evidence supports changing the helper or loop
merely to force stack placement.

Further bounded probes confirmed that directly spelling out the source-backed
`fsysGetEntry` lookup in the callback worsens the match to 99.08497%, while
putting the entry-loop locals in a nested block or moving the entry/sub-entry
pointer declarations to the function opening leaves 99.767975% unchanged.
Reordering both eviction helpers' local declarations to `handleID, n,
freeSize` worsens the match to 96.153595%; the other tested orders
(`freeSize, handleID, n` and `n, freeSize, handleID`) leave it unchanged.
All edits were restored. A 45-second semantics-preserving rewrite search
evaluated 146 variants and found no improvement. The remaining question is
the original source's stack-home/lifetime arrangement, not an observed FSYS
control-flow or data-flow difference; do not link or port this carve yet.

2026-09-29 shared-lock follow-up: moving `entryIndex` ahead of the other
function locals and declaring/initializing it with `entry` and `sub` inside
the entry loop each retained 306 instructions, 99.767975%, and the same 71
stack-only operand deltas. Expanding the full-archive eviction helper as a
literal, source-faithful block in the caller instead emitted 311 instructions
and fell to 94.26471%; that experiment was reverted. The 71 differences do
not justify stack-layout padding or accepting this unlinked `CodeCandidate`.

Later 2026-09-29 owner-claim check: objdiff's stack map confirms the first
eviction's scratch slots are systematically lower in the candidate (retail
`0x5C/0x60`, candidate `0x48/0x4C`), while the trailer loop's copied index
is near the bottom of retail's frame (`0x8`) but near the top of the
candidate's (`0x70`). Moving the unchanged, source-backed `fsysGetEntry`
inline definition after the eviction helpers retained the same 306
instructions, 99.767975% raw match, and 71 deltas. Extracting the trailer
reset loop into an inline helper was also source-equivalent, but produced 309
instructions and only 98.44118% raw match. Both were reverted. Definition
order alone does not explain the stack homes; the inlined-loop boundary is
not supported by retail. The original local-lifetime arrangement remains
unidentified, so the candidate stays unlinked.
