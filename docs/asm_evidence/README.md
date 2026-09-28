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

Rules that still apply: no `.inc` files and no asm wrappers around game logic.
Local compiler pragmas stay forbidden. Registered assembly counts as progress
only because the evidence shows it cannot be C.
