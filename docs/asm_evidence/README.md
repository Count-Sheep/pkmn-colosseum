# Authentic hand-written assembly: evidence

User decision (2026-09-28): a function the original developers wrote in
assembly cannot be decompiled to C. When a translation unit contains one, the
unit can only link if the asm body is in the source. That is admitted, one
function at a time, only with documented evidence. Nothing is assumed.

The quality scan (`.github/scripts/quality_scan.py`) enforces the rule. An asm
function passes only if all of these hold:

1. It has an entry in `registry.json`: `path`, `function`, `mnemonics` (the
   exact instruction set of the retail routine; anything else fails) and
   `evidence` (the document below).
   An optional `address` (`fn_XXXXXXXX`) names the same function when the
   report exposes a static routine by address; it grants no additional asm
   allowance and is checked against the same source path and evidence.
2. The evidence document has a `## <function>` section with all three fields,
   each with real content:
   - **Why it cannot be C:** the instructions or conventions in the retail
     routine that MWCC never emits for this unit, for example paired-single
     ops, `stmw` in a `-use_lmw_stmw off` unit, raw `lis`/`@l` addressing of
     small-data constants, or register games outside the ABI.
   - **Other decompilations:** at least one other project that also keeps
     this routine as assembly, given as a GitHub URL with a commit hash, and
     the file or line where it does so.
   - **Origin:** where the routine comes from: library, vendor, version.
3. The source cites the evidence document in a comment within 40 lines above
   the asm function.
4. Branches stay inside the asm body, with one exception for library code
   (`src/dolphin/`, `src/trk/`, `src/crt/`): a routine that ends in, or takes,
   a branch to another function lists those functions in its registry entry
   as `"branch_targets": [...]`. Each target must be a function in
   `config/GC6E01/symbols.txt`, and the evidence section needs a fourth field:
   - **External branch targets:** each branch and its target, and why it is
     not a call (typically a tail branch MWCC turns into a frame plus `bl`).
   `.github/scripts/verify_asm_branch_targets.py` then proves the list from
   retail data: the retail routine in `build/GC6E01/asm` must branch to exactly
   the declared functions. Run it in the local gate with the full link; the
   SHA-1 check proves the bytes. A branch back to the routine's own start is
   allowed without declaring it.

Rules that still apply: no `.inc` files and no asm wrappers around game logic.
Local compiler pragmas stay forbidden. Registered assembly counts as progress
only because the evidence shows it cannot be C.

## First-party (game) assembly

User decision, 2026-10-03. Colosseum's own code has no other decompilation to
cite, so a game routine the developers wrote in assembly (context switching,
stack moves into the locked cache, and the like) is admitted with a compiler
probe instead:

1. Write the closest C candidate for the routine and run
   `.github/scripts/asm_compiler_probe.py --unit-src <unit .c> --func <name>
   --candidate <c file> --out docs/asm_evidence/probes/<name>.txt`. It
   compiles the candidate with every GameCube MWCC version in
   `build/compilers/GC`, using the unit's flags from `build.ninja`, and
   compares the routine's instruction words with the retail object
   (relocated fields masked). The report ends in `verdict: no-match (N
   compilers compared)` or `verdict: MATCH ...`.
2. Register the routine with `"tier": "first-party"` (only under `src/game/`).
   Its evidence section needs **Why it cannot be C:**, **Compiler probe:**
   (naming the probe report) and **Origin:**. The scan rejects a missing
   probe, a probe of another function, a MATCH verdict, or a no-match over
   fewer than five compilers. `branch_targets` work as for library code.
3. Commit the probe report and its C candidate next to it
   (`docs/asm_evidence/probes/<name>.c`) so anyone can rerun it.

First-party asm is not decompiled C: `.github/scripts/asm_evidence_summary.py`
reports registered asm by tier so progress figures can keep it separate.

