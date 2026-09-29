# `fn_8017A814` / `fn_8017A95C` FSYS async completions

The two 0x148-byte callback bodies share the text-only
`gs_range_8017A814_suffix.c` `CodeCandidate` owner. Each measures
99.756096% fuzzy match in the 2026-09-28 report, but neither is exact or
linked. Whole-object acceptance still requires both functions to match and
the retail DOL/REL and quality gates to pass.

Fresh objdiff confirms exactly three differing instructions in each body:
retail builds the `lbl_80453FEC` manager address with `lis r5` and
`addi r5,r5`, then loads the active slot via `0x1C(r5)`; the candidate uses
`r4` in those same three places. All other instructions align. The owner
source records the earlier exhaustive natural source, callback-signature,
helper, compiler-version, flag, declaration, and volatile-access probes.
Those did not alter the register choice. A dead `userData` copy already
present in both source functions appears to account for retail's unused
`r30`, but that is only register evidence, not a license for further
score-only shaping.

This follow-up made no source change: no new source-semantic evidence for an
`r5` lifetime was found. Shared-lock `all_source`/report and full `ninja -j2`
were up to date; retail SHA-1 checks passed for `main.dol` and
`common_rel.rel`. Those hashes validate the currently linked image, which
does not include this suffix candidate.

## 2026-09-29 callback-contract check

The exact `fn_801808E4` ARQ dispatcher in
`src/game/gs_range_8017FA5C_exact_80180320.c` invokes the saved callback
as `callback((void*)entry->flush, entry->callbackArg)`. The accepted
`fn_80180584` wrapper stores the fifth argument in `callbackArg`, and the
accepted `fn_8017B5C0` and `fn_8017BD34` callers pass their `FSYSSlot*`
there. Thus the slot reaches these two callbacks in the second argument;
the first argument is the dispatcher’s flush value. As a targeted type probe,
both candidate callbacks were compiled with `void*` instead of `s32` for
the first parameter, matching the dispatcher's declared callback type. Both
remained 99.756096% with the same `r4`/`r5` manager-address mismatch, so the
probe was restored. The exact sibling ARQ callback `fn_8017F25C` also uses
an `s32` first parameter. This evidence rules out the first-parameter C type
as the missing register-lifetime explanation; it does not justify an
otherwise-dead operation to force `r5`.

After restoring the original signatures, shared-lock `ninja -j2 all_source
build/GC6E01/report.json` and `ninja -j2` passed. The latter checked the
retail SHA-1 for both `main.dol` and `common_rel.rel`. Both callbacks remain
candidate-only at 99.756096%; no object promotion or new linked progress
resulted.

## 2026-09-29 register-lifetime follow-up

Fresh instruction-by-instruction objdiff on `fn_8017A814` again found only
the three `r5` (retail) versus `r4` (candidate) manager-address instructions;
the two callback bodies remain identical in shape and score. Moving the
already-present dead `(void)request` expression after the manager load left
the score at 99.756096% and still used `r4`. Expressing the manager through a
named `FSYSManager*` local regressed the score to 96.98781%, assigning that
extra local to `r30` and changing the prologue; it did not recover the retail
`r5` sequence. Both probes were restored. The local campaign's register replay
is unavailable on this host (`register replay unavailable (tools missing, or
the replay failed)`), so no allocator-lifetime claim is made from it.

The exact ARQ dispatcher and callers still establish the callback contract
described above. These results provide no evidence for a source-semantic
change that would make either callback exact. The suffix remains a 656-byte
unlinked `CodeCandidate`; neither 99.756096% function can be accepted as
linked progress until both match and the whole object passes the normal gates.
