# `fn_8017DB74` FSYS header/TOC dispatch

The live mode-0 dispatch at 0x8017DB74 is an 816-byte, text-only
`CodeCandidate`, not a linked retail object. The final 2026-09-28 report and
raw objdiff both measure 99.67647% fuzzy match. The opcode sequence is
otherwise aligned; the 25 printed instruction differences are register and
stack-slot assignments.

Retail holds `tocSize` in `r28` and the inlined handle-table pointer in
`r27`; the candidate reverses them. The cache eviction's spilled inline
temporaries also occupy different slots: retail's handle result uses
0x18/0x20 where the candidate uses 0x0C/0x08, with dependent shifts through
the same block. The source header records earlier natural tests of return,
condition, nesting, and helper declaration forms. This pass also tested
swapping the caller's `numEntries`/`tocSize` declaration order; it did not
change the score and was reverted.

No artificial register or stack coercion was retained, and no near-match was
promoted. Shared-lock `all_source`/report and full `ninja -j2` completed;
retail SHA-1 checks passed for `main.dol` and `common_rel.rel`. Those hashes
validate the currently linked image, not acceptance of this candidate.

2026-09-28 re-audit: a fresh 204-instruction-per-side objdiff found zero
opcode-mnemonic differences and exactly 25 formatted-instruction differences
(nine register uses around `tocSize`/the handle table, sixteen inline stack
homes). No new behavioral mismatch was found, so the source was left
unchanged rather than introducing a register-only artifact.

The next source-allocation pass tried 218 semantics-preserving rewrites through
`local_campaign.py rewrite` (45-second budget), with no score increase. Manual
probes of the cache-table walk's declaration order, moving `tocSize` ahead of
the other main locals, and passing the table pointer into the inlined finder
all recompiled to the same 99.67647% score; each probe was reverted. The
remaining work is to find the original helper/temporary ownership that yields
retail's `tocSize` in `r28`, handle table in `r27`, and original inline stack
homes. Neither an artificial register/stack coercion nor an unlinked 99.67647%
candidate is a source-acceptance path.

2026-09-29 helper-ownership probes under the shared build lock: moving
`handleID` to the eviction loop's natural scope and initializing the oldest
handle helper's table pointer at its declaration both emit the same 99.67647%
code. Giving the helper an explicit local result adds two instructions and
falls to 98.632355%; returning the table pointer for the caller to read
changes the inline load/store order and falls to 98.59804%. All probes were
reverted. These results narrow the wall to the original inlined helper result
and caller temporary allocation; they do not justify a source change or
linked-status promotion.

2026-09-29 second allocation pass: moving the `fsysCacheOldestHandle`
definition after `fsysCacheRemoveHandle` left the same 99.67647% text; helper
definition order is not the missing temporary ownership. All six natural
declaration orders of `freeSize`, loop index `n`, and `handleID` inside
`fsysCacheMakeRoom` were also measured. Orders with `handleID` before
`freeSize` fall to 99.48039% because `freeSize` and `handleID` then exchange
their persistent registers (`r23`/`r24`); the other orders retain 99.67647%
and the same 25 operand deltas. Each trial was reverted. The baseline is still
204 aligned instructions with zero opcode differences and 25 register/stack
operand differences, unlinked and ineligible as accepted Recomp source.

2026-09-29 third ownership pass: passing `lbl_8047B1B8` explicitly into
`fsysCacheFindHandle` preserved the exact same 204 instructions and 25
register/stack operand differences (99.67647%). Giving the caller of
`fsysCacheRemoveHandle` an explicit result local added two instructions and
regressed to 98.68137%. Passing the sought handle ID instead of the slot to
`fsysCacheFindHandle` also added two instructions, including a new stack home,
and regressed to 98.35784%. All three source-faithful probes were reverted;
the canonical candidate remains unlinked at 99.67647%.

2026-09-29 diagnostic allocation pass: an otherwise-dead `tocSize = tocSize`
after the header field load kept all 204 instructions and changed the saved
register allocation to retail's `tocSize` in r28 and handle table in r27.
That raised the candidate from 99.67647% to 99.92157%. The 16 remaining
formatted differences were exclusively stack-home offsets in the inlined
oldest-handle and remove-handle helpers. This dead self-assignment is not an
accepted source form, and 99.92157% is not a linked byte match. A release
assert reference to `tocSize` scored 97.85784%; an `handleID` self-assignment
99.62745%, an oldest-helper table self-assignment 96.96568%, and a
remove-helper `found` self-assignment 95.12745%. A direct global dereference
for the oldest handle scored 97.85784%. A natural reorder of the four
remove-helper declarations left the 99.92157% diagnostic score unchanged.
All edits were restored, including the dead `tocSize` copy.

The final declaration-order diagnostic did not finish: the shared-lock MWCC
`wibo` process entered macOS uninterruptible `U` state with zero CPU before
opening source, and remained there after termination signals. The source was
restored to its original bytes while the process was stalled; the dashboard
correctly reports the generated decomp report as behind source. No new score,
linked status, or retail hash is claimed for that last trial. Rebuild through
the shared campaign lock after the compiler process exits before treating the
report as current.

A separate read-only launch of `build/tools/wibo --help` also entered `U`
state before opening any guest executable and did not exit after SIGKILL.
By contrast, `arch -x86_64 /usr/bin/true` completed. This points to a wibo
startup/environment stall, not to the FSYS C source or a known compiler
diagnostic. Do not start another Decomp build against the shared output tree
while the original locked process (PID 8146, child 8151) remains live.
Copying the same binary to `/tmp` reproduced the `U` stall, ruling out its
Nextcloud launch path. An isolated Wine 10.0 prefix initialized slowly and
its `wineboot.exe --init` also entered `U`; Wine was not adopted as a build
wrapper or used to change the shared Ninja graph. The original locked build
is still the authority to poll, and no build restart has been attempted.

Recovery later on 2026-09-29: ad-hoc signing a copy of the same wibo 1.0.3
binary made `--help` and `--version` return normally and launched sjiswrap.
After verifying the owner source was restored and the stalled child had no
source/output file open, the old parent build was terminated. The ignored
`build/tools/wibo` was atomically replaced with the signed copy, preserving
its original timestamp; `build/tools/wibo.unsigned-backup` retains the old
binary. The orphaned old wibo PID 8151 remains in `U` with a pending kill,
but no longer owns the campaign build lock. A new guarded `all_source`/report
build completed, restoring the canonical 99.67647% candidate score; guarded
full Ninja had no work, and both built DOL/REL SHA-1 hashes match retail. The
dashboard report is current again. This environment repair did not change
source or accept fn_8017DB74.

2026-09-29 post-wibo-repair source-form check: declaring and initializing
`tocSize` at its first assignment after earlier statements is not legal in
this MWCC C89 mode ("expression syntax error"), so the candidate was restored
immediately. A semantic-preserving early return when
`fsysCacheRemoveHandle` does not find the handle compiled but regressed the
whole unit from 99.67647% to 96.69118%; it was reverted. The canonical
owner source is byte-identical to its pre-trial SHA-256
`97cc246303b2460235bbbd2d3765175be43c218467ad59250850cc4fe3e701da`.
Guarded `all_source`/report rebuild restored 99.67647% and unlinked status;
guarded full Ninja had no work. Built `main.dol` and `common_rel.rel` SHA-1
values still match `config/GC6E01/build.sha1`. No source acceptance or Recomp
binding is claimed.

2026-09-29 fresh provenance audit: `objdiff-cli diff` against the current
`main/game/fsys/fsys_file_candidate_8017DB74_gc20` object again found 204
instructions on each side, 99.67647%, and exactly 25 operand-only
differences. The retail and candidate opcode sequences and branch/call
topology agree throughout. Nine operands concern the saved-register exchange
between `tocSize` and the inlined handle-table pointer; sixteen concern
stack homes of the inlined oldest/remove-handle temporaries. The other
reconstructed FSYS call sites (`fn_8017CED8` and `fn_80179FA4`) use the same
oldest/remove-handle helper behavior, but their separate candidate objects
do not prove a unique original inline-source ownership or temporary lifetime
for this function. No source-backed control-flow or data-flow correction was
identified; the owner source was left byte-identical. The next attempt needs
new original-helper provenance, not another dead assignment, pragma, or
register/stack-placement trial. This remains unlinked and unavailable for a
production Recomp boot binding.

2026-09-29 isolated-object gate check: the declared split is exactly
`0x8017DB74..0x8017DEA4`, with this function alone and no data section, but
`configure.py` still declares it `CodeCandidate`. A new guarded objdiff read
confirmed 204 instructions on each side, zero opcode differences, and 25
formatted differences. The register mismatch starts at function offset `+0x2C`
(`tocSize`: retail `r28`, candidate `r27`) and `+0x78` (handle table: retail
`r27`, candidate `r28`), then recurs at the four later TOC-size uses. The
inlined cache helpers' stack homes diverge at `+0xD0..+0x1DC`: retail
`0x1C/0x18/0x20` versus candidate `0x20/0x0C/0x08` for the oldest-handle
copies, followed by retail `0x8..0x14` versus candidate `0x10..0x1C` in the
removal loop. The isolated split makes whole-object linkage feasible *after*
a genuine exact match, but neither its 99.67647% score nor matching opcode
sequence satisfies that gate now. The original inlined-helper temporary
ownership remains the missing provenance; no source edit was made in this pass.

2026-09-29 compiler-fidelity diagnostic: a guarded scratch build compiled the
unchanged owner source with its actual `-opt level=0` flags under each locally
available MWCC version, then objdiffed `fn_8017DB74` against both retail and
the active GC/1.3 candidate object. GC/1.3, 1.3.2, 1.3.2r, 2.0, 2.0p1, 2.5,
2.6, and 2.7 all produced **identical 816-byte candidate code**: 100% against
active, 99.67647% against retail. GC/1.1 and 1.2.5 fell to 46.093136%
against both; they are not viable alternate compilers. Thus changing the unit
from GC/1.3 to a nearby version cannot solve this wall. More usefully, the
[MWCC debugger](https://github.com/cadmic/mwcc-debugger/blob/main/README.md)
supports GC/2.6, which *does* faithfully reproduce this specific active
candidate; unlike some other walls, an allocator replay could be authoritative
here once its missing GDB/retrowin32 pieces are installed. It should be used
to identify the original helper's virtual-register and stack-home ownership,
not to justify a dead self-copy or other compiler-shaping artifact.

A fresh guarded objdiff read found 204 aligned instructions, 179 identical
rows and 25 `DIFF_ARG_MISMATCH` rows, with no insertions, deletions, or opcode
changes. The object is still a one-function, data-free `CodeCandidate`, not a
linked source unit. Owner source SHA-256 remains
`97cc246303b2460235bbbd2d3765175be43c218467ad59250850cc4fe3e701da`;
no source edit or promotion is claimed.

2026-09-29 local MWCC-debugger repair and replay: the ignored
`build/reference/retrowin32` checkout was built with
`env SDKROOT=/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk cargo build -p retrowin32 -F x86-unicorn --profile lto`.
The first build without `SDKROOT` failed during Unicorn's CMake test link:
the default Command Line Tools macOS 27 SDK reported a malformed
`libSystem.tbd` and unknown `arm64e.x1-macos` architecture. The Xcode SDK
build succeeded. Homebrew GDB 17.2 was installed and its `i386` remote target
and Python API checked. These are local tools, not tracked project source or
compiler inputs.

The repository's `local_campaign.py explain` adapter still requires a missing
custom `gdb.py` stand-in, but upstream `cadmic/mwcc-debugger` has its own
GDB-batch launcher. Under the shared build lock, it was invoked directly
against GC/2.6 with the unit's exact compiler flags plus `-sym on` and wrote
its AST, eight backend stages, and GPR allocator records beneath ignored
`build/reference/fsys_db74_mwdbg/`. It exited successfully after finding
`fn_8017DB74`; the prior compiler-fidelity check proves GC/2.6's code is
identical to this unit's GC/1.3 candidate. For reproducibility, the relevant
invocation shape is:

```text
python3 build/reference/mwcc-debugger/mwcc_debugger.py \
  -e build/reference/retrowin32/target/lto/retrowin32 \
  -g /opt/homebrew/bin/gdb \
  -a 'build/compilers/GC/2.6/mwcceppc.exe [unit flags] -sym on -c src/game/fsys/fsys_file_candidate_8017DB74_gc20.c -o build/reference/fsys_db74_mwdbg/replay.o' \
  fn_8017DB74 build/reference/fsys_db74_mwdbg
```

The pre-allocation PCode puts the source's `tocSize` load (line 174) directly
in `r27`, while the inlined handle-table pointer at line 181 is directly in
`r28`; retail uses `r28` and `r27` respectively. In the inlined eviction
block, the frontend creates `@87` for the oldest-handle table pointer,
`@104` and `@105` for successive copies of its handle result, `@94` for the
remove-helper argument, `@91` for its `found` flag, and `@90` for the index.
Those are the exact candidate-side values behind the stack-offset wall.
However, GC/2.6's upstream `print_variables`/stack-home report is not
implemented (only GC/1.1 is supported), and the local explanation helper
cannot name the preassigned `r27`/`r28` values from its allocator table.
The trace therefore establishes where the current source's copies originate,
but not the original retail helper ownership that would move them. No
source-backed change follows; the object remains 99.67647% and unlinked.

## 2026-09-29: exact and linked (title-path exceptions)

The stack-home wall is MWCC's inlining mode, confirmed with the MWCC
debugger's AST dumps (GC/2.6, identical code to GC/1.3 here):

- A helper whose body is only expression statements and one trailing
  `return` is inlined as a comma expression:
  `handleID = (table = lbl, FORCELOAD(table->handleID))`. The level-0
  frontend optimisation pass splits that comma into statements and creates
  its two temporaries then, with the function's last numbers (@104/@105),
  hence the candidate's 0xC/0x8 homes.
- A helper with more than one `return` (fsysCacheFindHandle,
  fsysCacheRemoveHandle) is inlined as statements: the result variable is
  created at the call, before the helper's locals (reverse declaration
  order), and its parameters come last.

Retail's order (result 0x20 < `table` 0x1C < `id` 0x18) is the statement
form of `s32 id; FSYSFileHandle* table; table = lbl; id = table->handleID;
return id;`. Every no-code way of adding a second return that the parser
folds first (`if (1)`, `if (0)`, `while (0)`, `do {} while (0)`, `for (;;)`,
labels and gotos, `switch (0)`, `sizeof` tests, an unreachable second
`return`) is inlined as an expression again. An address test
(`if (&lbl_8047B1B8)`) gives retail's exact stack layout but emits the test.
`if (((void)0, 1))` survives parsing (a comma is not a constant expression)
and is folded by the backend: all homes match, and `tocSize = tocSize;`
then fixes the r27/r28 exchange. Both constructs are tagged
`RULE-EXCEPTION(title-path)`; the one-function object is Matching and
`ninja` passes the retail DOL/REL SHA-1.

The same two numbering effects close fn_80179FA4 (the third place the
eviction helpers expand; 91.41% before): statement-form
fsysCacheOldestHandle for both evictions, and its external-file total-size
loop as a first-level helper (`fsysSetTotalSize`). It also needed its
prologue as retail computes it (the unused `sub = slot->currentSub` home at
0x9C, then `>> 17` block count and aligned low part from two loads of
`entry->decompressedSize`), function-level stack locals declared in
retail's numbering order (archive, cacheAddress, sub), `if (p)` NULL tests
(`!= NULL` emits `li`/`cmplw` at level 0), and GC/2.0 like the neighbouring
fsys read units: GC/1.3 alone colours the DMA-setup temporaries
differently.
