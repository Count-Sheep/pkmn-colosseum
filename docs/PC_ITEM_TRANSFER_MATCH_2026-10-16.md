# PC Item Management Acceptance

`fn_8003A10C` (0x8003A10C), unit
`main/game/menu/pda_candidate_8003A10C`, is 100% and linked. Its standalone
source contains the complete 1,044-byte function and no other emitted code or
storage. This clears the decomp gate for item-management choices 0 and 2 in
native `fn_8003A520`. Mode 0 withdraws an item stack; mode 1 confirms discarding
one. No native implementation or headed test is included.

## Matching Fix

The initial 99.02299% candidate already had the right control flow and size.
Its differences were register operands in repeated inlined PC-box scans.

- Give the initial item-ID lookup its own local declaration order.
- Disable lifetime splitting locally, retaining the existing peephole setting.
- Return the withdrawal ID through its finished scan index and the discard ID
  through its finished match counter. Both are initialized before use and have
  no further scan use after the return.
- Declare the confirmed message ID as u16.
- Give the withdrawal count lookup the local order slots, index, match counter,
  item pointer. This fixes the final four r25/r26 operand differences.

These callsite-specific helpers, scalar reuse and compiler controls are
explicit byte-match-first exceptions in `docs/RULE_EXCEPTIONS.md`, not proof
of original source identities. No assembly, uninitialized reads, additional
observable accesses, instruction rewriting or shared-data imports were added.
Existing linked PDA siblings and their shared source remain unchanged.

## Evidence

The ignored `build/decomp-work/rea-audit/verify_item_transfer_acceptance.py`
reproduces acceptance and writes `item-transfer-acceptance.json` with HEAD,
tracked-diff and report digests, section contents/alignment/flags, exports,
relocations, policy, canonical hashes and full-inventory regression checks.

- `.text`: 1,044 identical bytes, alignment 4, flags 6.
- Sole export: `fn_8003A10C`; all 54 relocation destinations identical.
- Text SHA-256: `33eed24d957423c534ea73456e057506938f864ee58fa1d7d9c0ac08214f4daa`.
- Relocation record SHA-256: `f030928ba1b6f1f8edcceeeb3acb8c3ba767f901e38d3e5e68003c913106be71`.
- All 26 canonical DOL/REL hashes pass; no regressions across 8,677 functions.
- Fresh native policy accepts both scoped pragmas with their adjacent tag.

The unfiltered static closure contains 261 reachable functions, four
thread-register/switch assembly verification findings, and 16 compiler
save/restore helper names absent from the game-function report. These are
recorded for native host-thread/runtime review, not silently accepted or
counted as newly unmatched game functions. The separate People branch still
reaches unlinked `fn_80058150` (96.376625%).

## Harness Lessons

An isolated source probe first reproduced the live full-translation-unit
function bytes exactly. This made subsequent compiler/objdiff tests fast
without editing the active source or comparing an invalid reduced context.
The m2c draft required missing item-pointer arguments repaired and still
scored at most 87.30%; it was analysis input, not an accepted transplant.
Caller declaration swaps, blanket helper permutations, separate branch locals,
narrower prototypes, extra branch helpers, dead stores and broad optimization
controls did not finish the match. The meaningful remaining difference was
the lifetime and return-local identity of each inline expansion.

The CSV and JSON fix ledger retain these results for pipeline improvements.
The dashboard's last saved headed stop was still the already native-bound
`fn_8003842C` from October 13; this work clears a known static branch gate,
not a claimed newly observed runtime stop.
