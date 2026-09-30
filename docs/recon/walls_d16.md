# Register walls: lane D16 notes (2026-09-30)

## Method

MWCC's GPR allocation can be reproduced from an mwcc-debugger replay. The
simulator is in the session scratchpad, `d16/simp.py`, with `trace.py` and
`wi*.py` beside it.

Simplify:

- K = 29.
- Repeated ascending sweeps over the virtual registers (vreg >= 32). Each
  sweep pushes every node whose current degree is below K.
- When a sweep stalls, the next node is picked by the minimum
  cost / current degree. On a tie, the highest vreg wins.
- The degree counts every neighbour in the dump, including physical
  registers, r3-precoloured call-result copies, and coalesced-away nodes.
- `simg_sel.select` (D5) then gives the exact registers.

This reproduced the coloring order of every function below.

Two rules decide most walls:

- Spill-phase colour order depends on the degree at the stall. Extra
  precoloured neighbours (for example the result copy of a one-line inline
  such as fightPokemonGetPokemonPtr) raise a node's degree without changing
  the code.
- Non-spilling webs colour in descending vreg order. The caller's params and
  locals take the lowest vregs. Inline locals are numbered in reverse
  creation order: by inline level, then source order, with parameter temps
  after all inline locals. Frontend-optimizer temps (loop-invariant hoists,
  lifetime-split webs) come last and so take the lowest inline vregs.

## Linked

- fightOutPokemonSetMeetEnemyFightPokemonEnemySideAll (fight_out_pokemon unit
  linked): the 0xCC lookups go through fightPokemonGetPokemonPtr and
  pokemonCheckValid takes its pointer. That adds three precoloured neighbours
  to the enemy node, so its degree is 29 at the stall.
- pokemonSetWazaStatus: RULE-EXCEPTION. The row is in docs/RULE_EXCEPTIONS.md.
- fn_801425E8: the sort compare reads both ids into u16 locals first. This was
  not a register wall; the operand evaluation order was wrong.

## pokemonSetLevelBasisStatus (93.49%, pokemon_range_8012795C): walled

This has two independent problems.

1. Scheduling of the /100 magic constant.
   - The pre-RA list scheduler hoists the SeikakuDataBiosGetPtr argument copy
     (`mr r3, nature`) above `addi rX, lis, -0x7ae1`. The lis temp and r3 are
     then live together, and lis gets r4.
   - Retail's pre-RA order must have had `mr r3` after the addi, which is how
     lis gets r3.
   - The DAG in backend-10 is identical to what retail implies.
   - These had no effect:
     - `#pragma scheduling` 750/604/7400 (no change) and 601/603 (worse);
     - `goto`/label block boundaries before the call or inside AdjustStatus;
     - GC 1.2.5n, 1.3, 1.3.2 and 2.0;
     - -O3 and -O4 with and without `,p`. -O4,p is best.
2. obj and oldMaxHp colour order.
   - Both are spill candidates. At the last stall both have degree 29, and
     cost/degree then picks oldMaxHp (5/29) before obj (71/29).
   - Retail needs obj trivially colourable one sweep earlier, so obj must have
     one fewer persistent neighbour than oldMaxHp.
   - obj's persistent-neighbour set equals oldMaxHp's: 11 physical registers
     plus 17 coalesced or precoloured nodes.
   - No single liveness change can give obj fewer neighbours, because obj is
     live over oldMaxHp's whole range.
   - Retail's prologue copies level (`mr r27, r4`) before obj (`mr r30, r3`).
     That hints that obj's web starts later or is a separate web.
   - Next step: try an obj copy that starts after level, for example an
     inline whose param is obj. Then re-run `trace.py` to watch obj's degree
     at sweep 5.

## fn_80072A00 (99.71%, pkjb_candidate_80072A00): walled

- The simulator confirms that retail needs key's vreg in (35, 37): above the
  result group (@153, a lifetime-split web of pkjbWait60's result that the
  frontend optimizer creates) and below the hoisted idle-callback pointer
  (@151). Moving the result group below key (vreg < 33) also works.
- So key would have to be a frontend-optimizer temp (a CSE, hoist or split
  web), or the caller's `result` would have to be the group's surviving
  (lowest) node.
- Tried:
  - `chan + 1` written twice: MWCC does not CSE it across the calls and
    recomputes it (96.0%).
  - key through a one-line inline return (`pkjbKeyOf`): 99.75%. result is now
    right, but key becomes an inline-time temp above all the others and takes
    r31.
  - A Wait60 wrapper inline: no change.
  - Reusing `result`, `key` or `chan` across the two webs: no change.
  - `return 0` in Wait60 (97.0%).
  - Partial open-coding of Wait60 with the loop in an inline: 97.1%.
  - Assignment-in-condition spellings: no change.
- Next step: find a source form in which the frontend's lifetime splitter
  creates key's web after @151, for example key with two defs whose first web
  is dead code the backend removes.
