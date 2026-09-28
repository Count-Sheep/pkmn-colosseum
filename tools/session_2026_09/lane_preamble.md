You are one lane of a parallel swarm clearing decomp-side recomp boot blockers in the Pokémon Colosseum (GC6E01) decompilation. You are in an ISOLATED git worktree (your current directory). The main checkout is /Users/cs/Nextcloud/VSCode/Pokemon-GC (call it MAIN). Never edit anything in MAIN.

## Setup (do first)
1. `git rev-parse --show-toplevel` must NOT be MAIN. The worktree may have been created from `master`, which is behind. If `git status` is clean, run `git reset --hard c6610f95` and confirm `git log --oneline -1` shows c6610f95.
2. `mkdir -p orig/GC6E01/sys orig/GC6E01/files && cp MAIN/orig/GC6E01/sys/main.dol orig/GC6E01/sys/ && cp MAIN/orig/GC6E01/files/common_rel.rel orig/GC6E01/files/` (the build now also links REL 125, built with the ProDG toolchain in build/compilers; never put the disc image directly in orig/GC6E01/)
3. `mkdir -p build && cp -R MAIN/build/compilers MAIN/build/tools MAIN/build/binutils build/` (copies; never symlink).
4. `python3 configure.py --no-progress && ninja -j2 all_source build/GC6E01/report.json`, then copy report.json somewhere private as your baseline.
5. Use a private scratch directory inside your worktree (e.g. `.lane/`, never commit it). Other lanes share the session scratchpad and have overwritten each other's files before.
Up to six lanes share this machine: use `ninja -j2` for every build.

Read AGENTS.md and docs/CAMPAIGN_OPERATIONS.md ("Strict acceptance policy" and "Reconstructed inline helpers") and follow them strictly. You don't need campaign claims or the local_campaign build wrapper: this worktree is yours alone.

## What "accepted" means (the recomp's checker)
A boot-critical function is accepted only when: fuzzy == 100%, its unit is linked (Matching, not CodeCandidate), and no compiler-control pragma (optimization_level / optimize_for_size / scheduling / peephole / opt_propagation) is active at its definition, and it has no asm. Measure with:
  COLO_DECOMP_ROOT=$(pwd) python3 /Users/cs/Nextcloud/VSCode/pkmn-colosseum-recomp/tools/boot_status.py
(run it from your worktree so it reads YOUR report). Don't edit the recomp repo.

## Policy decisions already made in this campaign (follow them)
- Local compiler-control pragmas are forbidden. If a whole translation unit really was built with different flags, the accepted fix is a unit-wide `extra_cflags` flag in configure.py, backed by evidence that every function in the unit matches with that single setting and no local pragmas. Precedent: main.c `-opt nopeephole` (commit bd516e61). Apply it only with that evidence, and state the evidence in the commit message.
- When you remove or move a pragma, make sure no other function's effective setting changes (other units may #include the same file). Pragmas without push/pop leak into later code. Scope what you must keep with push/pop, and check the full report for regressions.
- static inline helpers only under the policy's admissible case: repeated expansion at 2+ call sites, or inline fingerprints. A helper that fixes one site but lowers another is register shaping: reject it.
- An uninitialised read is acceptable only when the retail code demonstrably does the same (no initialising instruction in the target). Document it in a source comment.
- To link a CodeCandidate unit: every function in it must be exact under the policy, it usually needs a standalone source file covering exactly the unit's split range in address order (candidate wrappers that #include a whole big .c emit symbols other objects already own), and its data must pair or stay extern. Then switch CodeCandidate → Matching and confirm main.dol SHA1.
- Constructs already REJECTED in this campaign (don't spend time on them; report the wall instead):
  - invented or reconstructed dead "stand-in" functions whose only purpose is placing strings/constants in pool order. Real functions taken from the Melee decomp ARE fine; see tobj.c 0631ea73.
  - single-use static inline helpers whose only evidence is register or stack-slot allocation (no repeated expansion, no listed fingerprint);
    EXCEPTION (user, 2026-09-27): an EXTRA copy instruction (e.g. `li r28,0; mr r31,r28`) that controlled tests show only an inlined body produces qualifies as the 'routed through a temp' fingerprint; see docs/CAMPAIGN_OPERATIONS.md. A mere register choice (no extra instruction) does not.
  - block-scope extern declarations used to stop a call binding to an in-unit definition, i.e. to prevent inlining (see the OSLoadFont re-audit);
  - unit-wide `-pragma "inline_max_size(N)"` or similar inlining controls used to suppress one specific inline;
  - dead copies or locals whose only purpose is forcing a register save.
  If exactness depends on one of these, commit the exact-as-possible work as an unlinked candidate TU (as mtx.c/jobj.c were) and report the instruction-level evidence.
- MWCC saved-register colouring model (lane FL2, verified on gs_floor, see src/game/gs_floor.c comment): inlined helpers' locals are coloured FIRST, helper by helper in source order; the FIRST inline expansion in the function colours its locals in REVERSE declaration order, then its parameters, while every LATER expansion (of any helper) colours its parameters first, then its locals in declaration order; loop variables reused across two loops order their second ranges by how the first loop's header assigns them (see fn_800FF970 comment in src/game/gs_floor.c); then the function's own locals in declaration order (top level, then blocks); second live ranges of a variable reused across loops come last. Each value takes the lowest already-used register that doesn't conflict, else a new one counting down from r31. Use this to design declaration orders instead of blind search.
- Handwritten-assembly functions (evidence: no frame, preserved volatile registers, direct r1/SPR games, a CR bit wrong for the call) can't be accepted. Report the instruction evidence and move on.

## Rules
- None of: asm, .inc edits, new compiler-control pragmas, dummy/self assignments, volatile for register colouring, invented helpers, shaping gotos, artificial unions/aliases. Natural, semantically faithful C with real types where evidence supports them.
- Don't change splits.txt/symbols.txt unless your lane instructions allow it and you have a concrete build/report reason; explain it in the commit.
- Keep a change only if it raises a target and nothing in the full report regresses (compare with your baseline). Revert what doesn't help. Don't sink hours into a register-allocation wall: after a solid attempt, document it and move on.
- Finish with a full `ninja -j2` and confirm both `build/GC6E01/main.dol: OK` and `build/GC6E01/common_rel/common_rel.rel: OK`.
- Commit on your worktree branch in logical commits whose messages explain what and why. End each message with the line:
  Co-Authored-By: Claude Opus 5.5 (1M context) <noreply@anthropic.com>
  Don't push, merge or rebase onto anything else.

## Report
Worktree path, branch, commit hashes; boot_status before/after for your rows; per function before → after %, the diagnosis, and what you changed (or why it's blocked, with evidence); any unit you linked or couldn't link and why; full-report regression check; SHA1 confirmation.
