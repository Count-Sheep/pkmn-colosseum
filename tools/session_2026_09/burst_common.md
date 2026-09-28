# Title-blocker burst: rules shared by every lane (read after lane_preamble.md)

Eight lanes are working at once to get past the last title-path blockers: `boot_status.py` shows 205/210 path functions accepted; the 5 left are fn_8013024C (row 7), fn_80132C6C (15), fn_800F1A0C (23), fn_80181850 (30), cameraPlayAnime (43). Each lane owns one wall. Stay inside your lane's files. If you find something useful for another lane's wall, put it in your report; don't edit that lane's files.

## Setup differences from the preamble
- Reset target: `git reset --hard $(git -C /Users/cs/Nextcloud/VSCode/Pokemon-GC rev-parse HEAD)` (the main checkout's current HEAD), not the commit named in the preamble.
- Eight lanes share this 8-core Mac: use `ninja -j2` for every build. Don't run the full `ninja` more often than you need to.
- Checker: `COLO_DECOMP_ROOT=$(pwd) python3 /Users/cs/Nextcloud/VSCode/pkmn-colosseum-recomp/tools/boot_status.py` (run it from your worktree).

## Policy: binding, and no changes
The user's instruction: get past this blocker WITHOUT breaking policy. docs/CAMPAIGN_OPERATIONS.md, as written, is the law. Its "Reconstructed inline helpers" section includes the inlining-only-copies clause and the same-engine sister-title (Pokémon XD) clause.
- Never propose, rely on or stretch a policy change.
- Any exact form whose admissibility is a judgement call stays unlinked and gets reported. Precedent: GSgfxDLBegin's helper was rated weaker than the written fingerprints and kept as CodeCandidate (baae25fa).
- Already rejected, don't retry:
  - a temporary or copy whose only effect is scheduling or registers (spline's `f32 t = tension;`);
  - single-use helpers backed only by register choice;
  - invented stand-in functions or data for pool/string placement;
  - local compiler-control pragmas;
  - volatile or dummy/self assignments;
  - block-scope externs to stop inlining.
- Unit-wide `extra_cflags` are allowed only with evidence that every already-exact chunk stays exact and nothing regresses (precedents: main.c `-opt nopeephole`, the floor_character/field_camera/mail commits, fsys `-opt level=0`).

## Rulings made since the first burst (all in docs/CAMPAIGN_OPERATIONS.md; use them only where the evidence fits exactly)
- "Named computed values": a local naming a COMPUTED value (not a copy or a constant) in its own statement, where retail's instruction order shows that statement position. Example: THPPlayer.c's `src`.
- Authentic hand-written library assembly: admitted only per function through docs/asm_evidence/registry.json, with an evidence document (the instructions MWCC can't emit, another decomp that keeps it as asm with a GitHub URL and commit, and its origin). The quality scan enforces this.
- Rejected recently, so don't retry:
  - an extern named stand-in for a TU's own compiler-pool literal (windowOpen, 7402d254);
  - dead stores to inline parameters (gs_vm `desc = 0`);
  - a local holding a constant (the hero_move neck mode is waiting on a user ruling, so don't apply it).

## Evidence sources you may use
- Front-end optimiser trace (from lane B7b): /private/tmp/claude-501/-Users-cs-Nextcloud-VSCode-Pokemon-GC/326b227e-fc3b-4cb8-b8dc-28a01eb4c933/scratchpad/colouring/irolog_B7b/ (README). It shows which MWCC front-end phase propagates or folds what; GC/2.6 must first reproduce your unit exactly.
- The XD demo linker map (NXXJ01.map, StarsMmd/Colo-XD-PBR-symbol-maps 6b51d3af) and tools/xd_map_xref.py. Map names alone don't satisfy the XD clause.
- **Colosseum itself:** build/GC6E01/asm, DOL/REL bytes, strings.
- **Tools in the tree:**
  - `tools/find_inline_expansions.py`: register-agnostic matcher; `--ghidra-dir` searches XD;
  - `python3 tools/local_campaign.py explain SYM --worker LANE`: MWCC register replay; check it says the replay is exact;
  - `python3 tools/local_campaign.py rewrite SYM --seconds 300 --worker LANE`.
- **Allocator simulator:** /private/tmp/claude-501/-Users-cs-Nextcloud-VSCode-Pokemon-GC/326b227e-fc3b-4cb8-b8dc-28a01eb4c933/scratchpad/colouring/tw/.
- **Pokémon XD:** TeamOrre/xd-decomp (symbols.txt), trevor403/xd-asm (disassembly). Clone into your `.lane/`.
- **Melee:** doldecomp/melee is allowed reuse; there's a local copy at build/reference/melee/src.
- **Other public decomps:** zeldaret/tww, doldecomp/sms, doldecomp/mkdd, dolsdk2004/2001, AxioDL/musyx.
- **Internet research is encouraged:** GitHub code search, Dolphin symbol maps, TCRF (tcrf.net) pages on Colosseum/XD prototypes, demos and region builds, and published map files.
- **Don't download game disc images or copyrighted binaries.** Public text such as symbol maps, disassembly listings and decomp sources is fine. Always record where a fact came from (URL and commit).

## Deliverables and rhythm
- Commit every success, and every recorded finding (as a source-header comment), separately on your worktree branch. Each success needs a full `ninja -j2` with `build/GC6E01/main.dol: OK` and `build/GC6E01/common_rel/common_rel.rel: OK`, and a full-report regression check against your baseline. A function that only moved to a new unit at the same score is not a regression; say so.
- When a unit goes fully exact, link it (CodeCandidate → Matching, standalone source or carve as needed, data paired honestly, SHA1 OK) and run the checker.
- Stop after about 3 hours, or when your wall is cleared. Report:
  - worktree, branch and commits;
  - checker line before/after, and your row's status before/after;
  - per function, before → after % with the reason;
  - evidence found, with sources;
  - judgement calls you did NOT apply;
  - leads for your wall and for other lanes.
