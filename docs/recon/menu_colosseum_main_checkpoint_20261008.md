# Battle Menu Main Matching Checkpoint

`menuColosseumBattleMain` at `0x80059BDC` remains an incomplete candidate in
`main/game/menu/menuColosseumBattle_candidate_80059BDC`. It is not accepted,
linked, or a claim that the native battle path is cleared.

The retained GC/1.3 source measures 99.14683% in the independent full-function
scratch comparison. Retail text is 12,532 bytes; candidate text is 12,528 bytes.
The generated switch table is 376 bytes. A complete-owner acceptance check
cannot pass while the text differs.

The freshly rebuilt canonical report scores the retained owner at 99.15161%
(the direct scratch comparison scores 99.14683%). Both are incomplete; neither
rounded similarity nor the successful canonical link accepts this candidate.
The full link still uses retail fallback for this owner. All seven canonical
hashes passed after integrating the separately exact owners.

## Retained Changes

- Inline the repeated party-menu close/wait/open sequence through
  `menuCBOpenParty`, preserving its arguments and calls.
- Give unrelated switch cases separate locals instead of reusing the previous
  command for unrelated values.
- Use existing `MenuMiddleWork` party-slot fields in B6 and signed byte
  pointers for the B3 table arguments. B3 advances an explicit 0x1660-byte
  slot offset, with local pointer/counter carriers. This recovers retail's
  offset register and increment order; pointer/counter registers still differ.
- Use a layout overlay for the rules table and preserve the signed comparison
  of the save-status return value.
- Disable loop-invariant motion locally to preserve the two distinct hero
  addresses in case 105. The numeric command/address carrier is intentional
  byte-match-first shaping, not a recovered semantic type.
- Keep the case-105 name pointer in a one-field local carrier. This changes
  only two register operands from r22 to retail's r24.
- Keep the first case-B1 party pointer in an integer carrier. This restores
  retail's intermediate copy and its scheduling before the next save-status
  lookup, reducing the remaining text-size gap from eight bytes to four.

These forms are tagged under the existing exception policy. No assembly,
new policy category, split change, or data ownership change is introduced.

## Remaining Differences

The previous-command local and several case-local results still occupy the
wrong registers. The AA lookup lacks retail's zero-offset address instruction;
AC/AE use different table-address forms. B3/B6 loop allocations, several zero
copies, and local instruction scheduling also differ.

Named static table reconstruction, a single static table aggregate, compiler
version probes, local optimization switches, helper boundaries, scalar reuse,
and small scalar-carrier trials did not produce an exact function. Failed
variants remain ignored build artifacts, not accepted source changes.

A small local permuter seed was independently checked against the retained
candidate's bytes and relocations before searching. Three bounded batches
produced no improvement. Diagnostic compiler runs captured early trees but
did not reach the requested full-function allocation graph within their
deadlines; there is no successful final allocator trace to cite.

All builds and comparisons used the campaign build lock. The original
accepted-unit work and incomplete trainer dependency are tracked separately.
