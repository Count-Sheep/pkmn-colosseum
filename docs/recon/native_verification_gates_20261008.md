# Native verification gates: acceptance audit, 2026-10-08

Read-only audit by CodexAcceptance at decomp HEAD
`7263c5817e4ea83856c77e77573abdedbf979166`. Only this standalone note was added;
no source, config, shared documentation, native, or harness edits, commit, or
push. Existing source-agent edits were left alone. Every build, objdiff, and
verification command ran through
`python3 tools/local_campaign.py build --worker CodexAcceptance -- COMMAND`.

## Decision for the parent/native handoff

Parent follow-up: the missing init exception marker was added beside its
conditional compilation block. Full linkage after the fight-side promotion
passes all seven canonical hashes, and the strict object-map check and all
56 policy tests pass. The final committed revision and fresh-report hash are
recorded in the communication handoff after integration. These checks do not
remove the conditional dependency caveats below.

The three function bodies pass raw unlinked-byte and normalized-relocation
acceptance under the existing byte-match-first exception policy. They are
exception-backed acceptance, not strict-policy wins or new linked progress.
The pragma questions now have measured answers; a pragma alone is not a reason
to leave these bodies at an unexplained needs-verification gate. The parent
retains the fresh full-build/seven-hash check after source agents finish.

| Function | Current report | Raw function bytes | Normalized relocations |
|---|---|---|---|
| menuColosseumBattleInit, 0x8005CD88 | 100%, complete/Matching | 352/352 identical | 26/26 equal |
| updateChat__F15HEROMOVE_MEMBER, 0x8012C0B4 | 100%, complete/Matching | 1,164/1,164 identical | 63/63 equal |
| fn_801CBA90 | 100%, complete/Matching | 40/40 identical | 1/1 equal |

The final observed report is dated **2026-10-08 19:03:39 UTC**, SHA-256
`b1c22c550acb0298812746a437c40d593e693fe0f33f2858bc7896c0261e6746`.
The parent refreshed it during the audit; all three statuses and the direct
dependency caveats below were rechecked under the lock. The audited object
hashes stayed unchanged. The initial report was dated 06:12:10 UTC, SHA-256
`a265481b380027e1d8dc1d80fa0ef4c7540518ed863a700d521ff1e852ac3627`.
All three owners remain `Matching` in configure.py and linked in build.ninja.
Targeted Ninja validation of their three source objects returned no work to do.
This auditor did not regenerate the shared report.

**This note supersedes the stale main.dol-mismatch prose in the
menuColosseumBattleInit/Exit row of docs/RULE_EXCEPTIONS.md.** The parent reports
that the latest canonical seven hashes pass. Independently, this audit found
all three function slices identical to retail in the existing linked DOL
(06:10:44 UTC), including all 34 hero-move functions and its full pool. This
is corroborating snapshot evidence, not a new full-link/hash run. Do not carry
the old mismatch forward as a current blocker.

## Byte, relocation, and data evidence

Raw unlinked function SHA-256, target and source object equal:

- Init: `db24896d2fc44bce408bf357c0607f6f55d968574f5f9a71278f01a99618abda`
- updateChat: `4553e83b545023ace8abf88f6118edc132a5c6427732d334f89f09fa955f68e7`
- fn_801CBA90: `4f57558bfeb3f073f0c1f6d5773ab3b3fd42eb6b4ff9a17324d872915bf7b8ff`

ELF parsing used tools/decomp_work/permuter/owner_extract.py. Relocations were
compared by function-relative offset, relocation type, and destination:
undefined symbols retain name/addend; named text functions retain name/addend;
owned data resolves to section plus symbol value plus addend. Thus compiler
pool labels are normalized only with measured destination/data evidence.
The two one-function owners have identical text sizes, alignment, flags, and
no owned data sections. Their stricter m2c_object_evidence comparison also passes.

Fresh project objdiff gives init and fn_801CBA90 100%, but updateChat
**99.931274%**, and hero_move .text **99.94169%**. This differs from the stored
reports and must not be silently presented as fresh objdiff 100. updateChat's
four argument differences, at function offsets 24, 100, 512, and 1004, are
`lbl_8047D034 + 0` versus compiler-local `@4 + 4`: both resolve to .sdata2 + 4
and load the same four bytes. Its raw code is identical independently of score.
The scratch-function verifier conservatively rejects this whole-TU pool form;
the explicit owner-level normalization above resolves that limitation.

All 34 retail hero_move functions have identical raw bytes and all **926**
normalized function relocations agree. The candidate has an extra unreferenced
172-byte `proc1Step`, documented in the source header as dead-stripped; none of
its symbols is a relocation target. This explains the object text sizes
23,152 versus 22,980 and updateChat's object-offset shift of 172 bytes.
The candidate .sdata2 is 172 bytes, identical to retail's first 172 bytes;
retail's remaining four bytes are zero alignment padding. Both align to 8;
MWCC marks the candidate pool writable (flags 3 versus target 2). Neither pool
has relocations. The existing linked 176-byte pool at 0x8047D030 matches retail,
SHA-256 `8117778b5a16da6cde910f6607cd99135748e9b0cbb2d34569a953aac69a5a4b`.
Init's external string region 0x80267840..0x80267A7C and seven-byte string at
0x8047BF28 also match in the existing linked DOL. Fresh linkage must continue
to preserve helper stripping, pool placement/padding, and external data.

## Source policy and actionable caveats

- **Init:** the menuColosseumBattleInit/Exit exception row explicitly admits `peephole off`
  and string-table-base local. However, src/game/menu/menuColosseumBattle.c:1780
  has no adjacent `RULE-EXCEPTION(title-path)` marker; the marker at 1749 is
  Exit-only and is excluded from the init carve. Parent/source owner should
  add the required scoped marker citing the existing row before declaring
  source-policy paperwork complete. This audit did not edit it or broaden
  policy. Under `fn_800FF548() == 0`, init calls unlinked
  `toolentryDebugPokemonCreate` (99.45087%) and `fn_8006B5D0` (93.87719%).
  Its other 14 distinct direct callees are report-linked 100%. Acceptance of
  init does not authorize bypassing that conditional branch's native gates.
- **updateChat:** the size-optimization pragma is scoped and tagged at
  src/game/hero_move.c:3293, supported by its named RULE_EXCEPTIONS.md entry. The
  `heroMoveInitEventChat` inline copy is tagged at 1724 and covered by the
  separate updateChat/cbPoison entry;
  both exceptions matter. Of 28 distinct emitted direct callees, 27 are
  report-linked 100%; `memcpy` is 100% exact but belongs to incomplete
  main/crt/__start_mem_80003458. Native must use its separately accepted memory
  primitive or preserve the dependency gate, not call it linked progress.
- **fn_801CBA90:** scoped `scheduling off` and its source tag are supported by
  the field script functions entry in RULE_EXCEPTIONS.md. Its sole direct
  callee `fn_800FF58C` is linked 100%.
  The body calls it with 0x395 and returns zero. The native binding must honor
  that body: gs_range_801C766C.c:1421 currently declares a void(s32) caller
  signature and passes an unused zero, while the owner defines s32(void).
  This existing cross-TU declaration mismatch does not affect the measured
  PPC bytes; do not copy incompatible declarations into a native C call.
  This remains a conditional saved-encounter gate, not the reached bar-exit
  fade blocker. Sibling gate fn_8006A65C is report-linked 100%, but its body
  was not part of this three-function byte audit.

The repository quality scanner passed every line of both carve owners,
menuColosseumBattle.c, and hero_move.c: no active assembly/.inc violation.
That scanner does not certify pragma exceptions or source-tag completeness;
those were reviewed separately above. Direct-callee counts are one level only;
they do not certify indirect dispatch, transitive closure, native bindings,
or headed execution. The 18:38-18:44 refusals are still the latest runtime
evidence. After the parent's final checks and init annotation follow-up,
native can consume this acceptance evidence and retest the corresponding
Colosseum/talking paths without treating the admitted pragmas as unexplained.
