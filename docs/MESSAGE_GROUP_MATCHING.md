# Message group matching

The active objective is to byte-match the message subsystem and everything
it touches or references. Partial scores, reported exact functions, strict
source validation and newly linked progress are separate evidence categories.

The initial relocation-based inventory covers 37 reported message functions
and 331 functions in their outgoing dependency closure, with 258 incoming
callers recorded separately. This is a lower bound: indirect calls, data
dependencies and references through section symbols still need auditing.
Generated inventories and experiment reports live under ignored
`build/local_llm_campaign/manual/message_group_goal/`.

## GSmsgOpen boundary correction

The old symbol definition ended GSmsgOpen at 0x800FC2A4, assigning its last
four bytes to a separate `fn_800FC2A4` void stub. The correct source function
occupies 0x800FC244 through 0x800FC2A7 (0x64 bytes). The last instruction is
the compiler's unreachable trailing return after the list-insertion loop.

Evidence collected before changing the canonical symbol definition:

- Unmodified retail GSmsgOpen bytes plus the adjacent four-byte stub equal
  the compiled 100-byte C function byte-for-byte, not merely by fuzzy score.
- Both ranges have the same single relocation: offset zero, ELF type 109
  (PowerPC embedded SDA21), symbol `lbl_80478B08`, addend zero.
- All configured retail objects were readable; none contains a relocation
  to `fn_800FC2A4`.
- Scanning retail text found no direct branch to 0x800FC2A4, and scanning
  retail text/data found no stored word equal to that address.
- Active-source references outside the old definition are unused forward
  declarations, not calls. Other owners are left untouched.

The canonical symbol now includes those four bytes and the duplicate C stub
is removed. The retail byte range and source address traceability are
preserved; no object split, compiler option, alias or generated data is added.
`open-boundary-evidence.json` records the original census and byte comparison.

This corrects function accounting: the report loses one spurious function.
A resulting 100% GSmsgOpen is exact source in an incomplete CodeCandidate
object, not permission to link the entire object or claim the group complete.

## Remaining quality audit

Legacy 100% results still need a source-policy audit. GScharCpy and GSmsgDaemon
have now had their local compiler controls removed and their old exact scores
withdrawn; see the copy/daemon audit below. Their pre-attempt code is preserved
as research material. The bounded-message round below removes the final local
compiler-control block, in fn_800F96E4. It remains nonexact; absence of those
controls does not establish strict source quality or completion of the wider
dependency closure.

## Current handoff

The retained glyph lookup is 91.2% (previously 77.66%); moving its result
handling outside the binary-search loop regressed to 62.9% and was rejected.
GSmsgOpen is raw and reported 100%. GScharMakeFromSJIS, GSmsgClose,
GSmsgSetCtrlFunc and _msgGetLength__FPCUs remain 100% after removing their
local compiler controls and disabled assembly arms. No incomplete object
was promoted to linked progress. The corrected inventory has 36 group
functions and 330 functions in the known outgoing closure, 83 nonexact.

## Exec and parser round

The GSmsgExec investigation recovered another expansion of GSmsgFindMessage,
corrected narrowed search counters, and recovered literal 1.0f scale
initialization from the retail read-only constant at 0x8047CD08. The indexed
font scan and initialization also improve fn_800FBB34.

Canonical report changes:

| Function | Before | Retained |
| --- | ---: | ---: |
| GSmsgExec | 57.79470% | 91.65563% |
| fn_800FBB34 | 86.81208% | 90.30202% |
| fn_800FBE7C | 83.24324% | 87.567566% |
| _msgGetSize__FPCUs | 68.336494% | 79.990524% |
| fn_800FC7E0 | 68.59178% | 70.72055% |

GSmsgIsCheck remains 100% after removing local compiler controls and its
disabled assembly arm. Its 116 bytes and sole SDA relocation independently
equal the retail function. This does not add new matched or linked bytes;
the previous report already counted it as exact.

The renderer and size parser share GSmsgReadCode. The retail expansions
repeat the cursor load, halfword read, signed return-stack pop and separate
returned-code check. The first renderer expansion reproduces that sequence;
the second retail expansion loops back to the reader after a pop, not to the
outer saved-cursor comparison. No standalone helper is emitted in any of the
10 active message scoring objects.

Semantic corrections include forwarding fn_800FBE7C's third argument,
advancing ordinary characters by two bytes and control IDs by one byte in
_msgGetSize, and reloading the control-table manager after callbacks may have
changed it. The legacy win_msg declaration of fn_800FBE7C still needs a
cross-owner type audit before a linked promotion. No incomplete object was
promoted. All newly edited candidate bodies in this round are free of local
compiler controls; other legacy bodies in this owner still require auditing.

Rejected experiments are preserved under ignored
`build/local_llm_campaign/manual/message_group_exec/`: a font-ID switch,
a font-spacing-only helper, and an isolated GC/1.2.5n compiler probe.
The active compiler/configuration was not changed. The renderer's higher
pragma-assisted intermediate score was not retained.

Configure, all_source/report, full link/retail SHA, progress, added-source
quality scan and diff checks pass. Comparing all 8,602 report functions finds
five improvements and no regressions. Raw comparisons, source snapshots,
exact fingerprints and canonical deltas are retained in that same directory.

## Indirect dependencies

The installed msgctrlcode table at 0x80363778 contains 95 eight-byte entries.
Its handlers and their known outgoing references expand the lower-bound
function closure from 330 to 1,193, with 140 reported nonexact functions.
Every non-null table handler resolves to a reported function. The table has
26 mode-0, 58 mode-1 and 11 mode-2 entries, and no mode-3 entry.

This is still not a completion boundary. Runtime table mutation, other
indirect calls, data dependencies and strict audits of reported exact code
remain open. In particular, the parser's existing mode-3 fallback is not
proven equivalent to retail and is not admitted as a strict match. The
generated callback-audit.json records the table hash and new dependencies.

The renderer control-dispatch and return-stack audit is recorded below.

## Dispatch and bounds round

Retained scores, measured against the start of this round:

| Function | Canonical before | Canonical retained | Raw retained |
| --- | ---: | ---: | ---: |
| fn_800FC7E0 | 70.72055% | 84.489044% | 84.44109% |
| _msgGetSize__FPCUs | 79.990524% | 88.69194% | 88.64455% |
| GSmsgGetRect | 67.083954% | 85.38025% | 85.21975% |
| fn_800FD348 | 62.2723% | 86.74178% | 86.62441% |

These are partial source improvements: zero new exact functions, zero new
matched bytes and zero newly linked bytes. Full-link success does not prove
behavioral equivalence of these unlinked CodeCandidate bodies.

The MessageControl record has two mode bits, stop/execute/measure bits and
a callback at offset four. A test-only fixture compiled with the active MWCC
recipe verifies the eight-byte stride, all eight flag patterns and all callback
relocations. The fixture lives in ignored build state and is never linked.

GSmsgDispatchControl reconstructs the repeated renderer dispatch expansions.
Both have table/flag guards, a callback, mode-dependent destination lookup and
a signed-depth return-stack push; only the first consumes the post-callback
stop flag. The corrected push saves the existing cursor before installing the
destination. The renderer now keeps quote state separate from the stop flag
and saves its three return addresses in an actual array, not by indexing past
three independent scalar locals.

The nested GSmsgFindMessage version was rejected: the GC/1.2.5 scoring object
emitted a standalone lookup referenced by the renderer and rectangle parser.
An isolated compilation of the pre-round baseline proves this was new. The
retained dispatcher expands that lookup locally, preserving the binary search
without compiler-control changes. GSmsgFindMessage, GSmsgDispatchControl and
GSmsgReadCode now emit no standalone symbols in all ten message scoring objects.
The rejected version's higher renderer/rectangle scores are not retained.

The size parser uses the verified record fields and the retail stack-update
order. GSmsgGetRect now uses the recovered lookup and reader: ordinary codes
advance two bytes, control IDs advance one byte, and nested strings resume at
the saved cursor. It reloads the control-table manager after callbacks, updates
bounds after control codes as well as ordinary glyphs, and preserves the retail
floating-point grouping when packing the final height. Its lookup's optional
group-output check and the reader's separate returned-code check repeat the
same retail inlining fingerprints found in earlier rounds.

Graphics fixes include the single-mask signature of fn_800D888C, the f64 cos
signature, full-width sprite coordinates, signed marker-counter arithmetic,
and truncating the marker's base position separately from its displacement.
fn_800FD348 takes only the work pointer: its retail caller supplies no floating
argument and its body does not consume one. The unused floating parameter and
spaceBias assignments are removed; its cumulative draw count is a byte, as
shown by the retail truncation. GSmsgGetRect and fn_800FD348 no longer carry
local compiler-control pragmas or disabled assembly arms. No .inc file changed.

Configure, all_source/report, full link, retail SHA-1, progress, added-source
quality scan, layout probe and diff checks pass. All 8,602 report functions were
compared: four improve and none regress. Evidence, rejected variants, source
snapshots and per-command logs are under ignored
`build/local_llm_campaign/manual/message_group_dispatch/`.

Remaining: mode-3 destinations are still unproven. The retained dispatcher
keeps its previous current-cursor fallback; the size parser keeps its previous
null fallback. Neither is admitted as equivalent to retail on that path.
The installed table's lack of mode-3 entries does not prove runtime mutation
or another installation impossible. Cross-owner declarations of
windowDrawSprite and fn_800FBE7C also need auditing before linked promotion.

Next: audit control-table installation/mutation and the shared font-initializer
expansions. GSmsgInitRuby is another parser consumer still awaiting the same
cursor/return-stack analysis. The dependency goal remains the entire message
group and its references, not just the four improved functions.

## Color ABI and ruby parser round

| Function | Canonical before | Canonical retained | Raw retained |
| --- | ---: | ---: | ---: |
| GSmsgSetColor | 77.17391% | 80.21739% | 80.0% |
| GSmsgInitRuby | 65.11429% | 70.83929% | 70.80357% |
| fn_800FC7E0 | 84.489044% | 84.70959% | 84.65479% |
| fn_800D461C | 72.19769% | 72.62603% | 72.59308% |

The color ABI is now GXColor by value in the message callers, the graphics
command decoder and the graphics setter fn_800DBEB4. Three independent retail
call sites show a local four-byte color copied again to the outgoing argument
slot. In particular, command 80 in fn_800D461C first memcpy-copies the input
color, then makes a separate outgoing copy. The old pointer declaration omitted
that second copy. This uses the existing SDK GXColor type, not a new helper or
an invented wrapper structure.

The graphics setter remains raw and reported 100%: its 104 text bytes and all
four relocations independently equal retail. Passing GXColor through its
variadic command-buffer call naturally produces the retail stack copy. Its
direct render-state path stores the packed color word, using the same GXColor
word-read idiom already present in the SDK fog implementation. The intermediate
aggregate-store variant regressed to 98.84615% and is not retained. No new exact
function or newly linked byte is claimed for this already-exact setter.

The two local white-color initializers are backed by the retail bytes at
0x8047CD00 and 0x8047CD04, both FF FF FF FF. GSmsgSetColor now uses named RGBA
fields and no local compiler controls or disabled assembly arm. Remaining
differences include scheduling and redundant channel truncation; no pragma or
speculative constructor/helper was added to force those instructions.

GSmsgInitRuby now uses the recovered reader and dispatcher, so its normal
characters advance two bytes, control IDs advance one byte and nested-message
calls preserve the previous cursor. Its three return addresses are saved and
restored as an array. A memcpy variant emitted calls absent from retail and
was rejected; ordinary bounded array-copy loops produce the observed three
stack slots without a new helper. Its local compiler controls and disabled
assembly arm are removed.

The ruby width accumulation now groups the scaled glyph width and spacing
before adding the running width, matching the retail fused operation. Centering
uses signed division by two, which rounds toward zero. Exhaustively comparing
the target's sign-bit/add/arithmetic-shift sequence over all 131,071 possible
differences between two s16 widths passes. The old signed-shift formula differs
on all 65,535 negative differences. This checks the centering arithmetic only,
not equivalence of the entire still-partial parser. Mode-3 dispatch remains an
open semantic audit, as in the previous round.

Configure, all_source/report, full link, retail SHA-1, progress, added-source
quality checks and diff checks pass. No report regression across 8,602
functions, no .inc edit, no newly emitted message helper in any of the ten
message objects, no new matched bytes and no linked delta. All three edited
owners were claimed throughout verification. Saved variants, rejected probes,
source hashes and independent fingerprints are under ignored
`build/local_llm_campaign/manual/message_group_color_ruby/`.

Next: the shared font-initializer and mode-state expansions still need recovery;
control-table installation/mutation remains open. The newly established color
ABI provides a checked reference for the other message drawing paths.

## Lookup and initializer round

| Function | Canonical before | Canonical retained | Raw retained |
| --- | ---: | ---: | ---: |
| fn_800FB43C | 60.006897% | 89.72414% | 89.62069% |
| fn_800FB680 | 60.27397% | 89.69178% | 89.246574% |
| fn_800FB8C8 | 62.70968% | 93.0% | 92.90323% |

These initializers now use the recovered message lookup, including full-width
u32 key reads instead of the legacy single-byte reads. Font scans use signed
indices and retail eight-byte entries. Renderer calls have four arguments,
without unused floating arguments. Local compiler controls, disabled assembly
arms and redundant legacy conditions were removed. The fn_800FB8C8 forward
declaration now agrees with its narrow argument types.

GSmsgGetRect and _msgGetSize explicitly convert computed font spacing to s32
before narrowing to s8. All 256 byte-sized heights agree with the retail
conversion using original constants; this checks only that arithmetic, not
the entire parser. Neither score changed.

Rejected: wrapping GSmsgGetGSchar in GSmsgFindMessage lowered its raw score
from 95.13513% to 94.324326%. Calling GSmsgSetFontInfo from the size parser
emitted a call absent from retail and lowered its raw score to 69.843605%.
Marking the public setter inline removed its required standalone symbol.
Those experiments were restored; no helper or compiler option was added.

Configure, all_source/report, full link, retail SHA, progress, added-source
quality checks and diff checks pass. All 8,602 report functions were compared:
three improve and none regress. No .inc changes, new emitted inline helpers,
new exact functions, new matched bytes or newly linked bytes. Evidence and
rejected variants are in ignored
`build/local_llm_campaign/manual/message_group_lookup_font/`.

Next: fn_800FAEF8 supplies another font-initialization and character-rendering
expansion. Mode-3 control semantics, runtime table mutation, cross-owner ABI
audits and the wider dependency closure remain open.

## Formatted message drawing round

fn_800FAEF8 improves from 73.22552% to 81.47181% in the canonical report;
raw comparison improves from 72.44511% to 80.87834%. It remains partial,
with zero new exact functions, matched bytes or newly linked bytes.

The reconstructed path now supplies both destination and source to the SJIS
converter. The converter's active implementation and the retail argument setup
agree on u16* output and const u8* input. Its formatting call uses the existing
logVsnprintf_float signature. Canonical sprite/texture call names replace stale
address aliases. The function returns the retail zero result, and the manager
pointer is reloaded after graphics/texture calls before subsequent accesses.

The glyph-data pointer now adds the font's data offset and only the low 24
bits of the glyph record. The high byte is passed separately as signed glyph
metadata. Width is captured before the draw call, matching retail's preserved
value rather than reloading potentially modified data. The recovered reader
supplies the repeated halfword/cursor/return-stack expansion. No new standalone
helper is emitted in any of the ten message scoring objects.

Fallback drawing truncates Y to s32 before adding two and narrowing to s16.
Its scaled width is assigned to an f32 local before adding 2.0f, reproducing
retail's separate fmuls/fadds rounding. Across 2,560 sampled byte-width/scale
combinations, 24 differ from a fused operation. This is evidence for retaining
the intermediate rounding, not a full behavioral proof. A direct fused
expression and a double-literal expression were not retained; a font-ID switch
also produced a different branch structure and was restored to the existing
if/else scan. No compiler-control pragma was added; the target's old local
controls and disabled assembly arm were removed.

The related GScharMakeFromSJIS wrapper now explicitly forwards its two typed
arguments instead of relying on incidental register preservation through a
no-argument declaration. Its 32 text bytes and sole relocation independently
equal retail, preserving the previously reported 100% without adding progress.
Other owners' stale declarations/call sites still need auditing before promotion.

Configure, all_source/report, full link, retail SHA, progress, quality scan and
diff checks pass. Comparing all 8,602 report functions finds one improvement
and no regressions; there are no .inc edits. All source outside the two edited
functions and four corresponding declarations equals this round's baseline.
The formatted function's 25 direct calls match retail in order and targets,
but instruction scheduling, register allocation and font-scan differences
remain. Variants, source snapshots, command logs and fingerprints are retained
under ignored `build/local_llm_campaign/manual/message_group_formatted/`.

The wider message/dependency objective is unchanged. Glyph renderer
fn_800FD69C and shared font/work initialization provide remaining comparison
evidence; mode-3 dispatch and indirect-dependency audits are still unresolved.

## Glyph atlas and drawing round

| Function | Canonical before | Canonical retained | Raw retained |
| --- | ---: | ---: | ---: |
| fn_800FD69C | 61.481617% | 64.20221% | 64.20221% |
| fn_800FC7E0 | 84.70959% | 85.15753% | 85.10274% |
| fn_800FD348 | 86.74178% | 88.23005% | 88.11268% |
| fn_800FAEF8 | 81.47181% | 81.388725% | 80.79525% |

The glyph renderer now has a typed pixel pointer and signed-short dimensions
and vertical offset, consistent with retail's parameter use. Its callers use
pointer arithmetic for the font-relative glyph address. The renderer's clear
and copy loops use 32-bit counters and coordinates, retail pixel-space bounds
and a full-width packed-row stride; the compiler supplies the loop unrolling.
Signed tile coordinates use multiplication instead of left-shifting negative
values. Atlas manager fields are reloaded at the retail mutation boundaries.

Outline alpha uses unsigned arithmetic. Foreground color is fetched separately
for each foreground vertex after the positioning call, and the final atlas X
advance uses the current manager and cursor after the drawing flush. The old
cached values could lose intervening changes. Normal, shadow and outline modes
retain retail's vertex order. Local optimization pragmas and the disabled
assembly arm were removed; no new helper, compiler option or .inc edit was made.

The formatted-draw score decrease is deliberate and reviewed, not hidden.
Its body is unchanged, but the typed renderer declaration restores the retail
extsh after the glyph draw and the subsequent integer-to-float conversion order.
The earlier version omitted that sign-extension instruction. The report still
scores the new alignment 0.083085 points lower; the faithful type is retained.
The other two caller scores improve. No other report function changes among
8,602 compared entries. No new exact functions, matched bytes or linked bytes.

Verification includes an extracted-C host harness covering 31,500 cases:
atlas row wrapping, odd widths, tile boundaries, all drawing modes (including
invalid-mode fallback), and callbacks mutating foreground color or replacing
the manager after flush. All 131,072 atlas bytes and every emitted vertex are
compared per case under AddressSanitizer and UndefinedBehaviorSanitizer.
Negative controls reinstating either stale cache fail the expected assertions.
The harness uses a host-sized manager address and adapts the single 32-bit
texture-pointer load; it is not a PPC emulator or proof of complete equivalence.
An initial host-pointer alignment error in the harness was fixed; its diagnostic
is preserved, and the final run fails closed on sanitizer output.

Additional checks cover all 262,144 atlas coordinates, all 65,536 signed width
values for row strides and loop bounds, all 256 alpha values, the retail 1/512
UV constant, and all 41 direct call targets in order. No standalone message
helper is emitted in any of the ten scoring objects. Configure, all_source,
canonical report, full link, retail SHA, progress, quality scan and diff checks
pass. Variants, source snapshots, tests, the reviewed regression and per-command
logs are retained under ignored
`build/local_llm_campaign/manual/message_group_glyph_draw/`.

Remaining glyph differences include atlas-address expression code generation,
floating-point scheduling, register allocation and redundant conversions.
Cross-owner declarations and the wider message/control dependency audits are
still open; these partial objects are not promoted.

## Font lifecycle and glyph interface round

| Function | Canonical before | Canonical retained | Raw retained |
| --- | ---: | ---: | ---: |
| GSmsgFontOpen | 72.38947% | 98.926315% | 98.926315% |
| GSmsgInit | 47.264366% | 48.356323% | 48.356323% |
| GSmsgGetRect | 85.38025% | 85.6642% | 85.5037% |
| GSmsgInitRuby | 70.83929% | 70.875% | 70.83929% |
| fn_800FAEF8 | 81.388725% | 81.448074% | 80.8546% |
| fn_800FC7E0 | 85.15753% | 85.130135% | 85.07534% |

GSmsgFontOpen now shares the evidenced font-slot, font-bank and glyph-entry
layouts with lookup and initialization. Registration copies the eight-byte
file header before installing its bank pointer, matching retail's copy and
overwrite sequence. Its scans use signed 32-bit indices, and the full-table
test reloads the current manager's count. Invalid IDs, duplicates, full-table
logging and partial registration on a later error retain their retail behavior.
The bank's data offset at +4 is also evidenced by all three drawing consumers.
No artificial layout or memcpy call was introduced to force the match.

GSmsgFontClose remains raw 100% after removing its local compiler controls,
disabled assembly arm and register hints. Independent verification compares
all 244 text bytes after resolving the target object's self-branch relocation
at offset 232; the compiler already resolves that branch locally. The external
SDA21 relocation at offset zero also matches. Nonexternal-relocated instructions
agree with the original DOL. This is stronger verification of an already
reported exact function, not a new exact function or new matched bytes.

GSmsgInit now uses the canonical allocator names, reloads the manager at call
boundaries, uses literal 1.0f scales and ordinary task/font initialization loops.
The old handwritten eight-way unrolling and local compiler controls are gone.
Its active scoring object still uses the legacy O1 configuration. An isolated
O4,p probe reaches raw 85.344826% and generates unrolling itself, but that is
diagnostic evidence only: no active compiler option or object split changed,
and that score is not canonical progress. Compiler provenance remains open.

The glyph lookup's declaration and definition now explicitly take a work
pointer, u16 character code and optional bank-output pointer. The unsigned-short
argument is evidenced by the retail symbol's Us encoding and its truncation.
Its search algorithm is unchanged and remains raw 91.2%. Direct-index and
cached-code search variants scored 87.6% and 89.6% and were rejected. No invented
inline search helper was admitted on the strength of an unexplained extra
retail branch.

The typed lookup interface alone changes code generation in four callers;
their bodies are unchanged. Three improve and the main renderer falls by
0.027395 points. The latter retains the evidenced argument type despite that
small allocation/scheduling regression. Both target and retained output move
the character code into r4 before the glyph call. All direct callee sequences
checked in this round agree with retail; neither that nor the type evidence
establishes byte exactness for these partial callers.

Verification covers 245 initializer cases, 1,875 registration cases, 1,904
duplicate-registration checks and 6,576 registration/lookup checks under ASan
and UBSan. A separate lookup harness passes 2,097,546 calls, including every
16-bit code across 16 bank configurations, empty and chained banks, 65,535-entry
banks, duplicate codes, null output pointers and unchanged output on misses.
These are native semantic tests with explicit pointer-layout adaptations,
not PPC emulation. A separate active-MWCC probe verifies 21 PPC sizes/offsets.

Configure, all_source/report, full link, retail SHA, progress, added-source
quality scan and diff checks pass. All 8,602 report functions were compared;
only the six table rows changed, with the one reviewed regression above.
No standalone recovered helper is emitted in the ten message objects. No .inc
file changed, no object was promoted, and matched-function, matched-code and
linked-code deltas are all zero. Variants, source snapshots and verification
artifacts are retained under ignored
`build/local_llm_campaign/manual/message_group_font_lifecycle/`.

The wider objective remains open. Font registration's residual code generation,
the glyph search's extra retail branch/index calculation, initializer compiler
provenance, legacy exact-source quality and indirect dependencies still need
resolution; these results do not establish completion of the message group.

## Character conversion and alignment audit

| Function | Canonical before | Canonical retained | Raw retained |
| --- | ---: | ---: | ---: |
| fn_800F9C04 | 58.4375% | 91.25% | 91.25% |
| GScharLenCpy | 50.963856% | 63.759037% | 63.759037% |
| GSmsgAdjustAlign | 100% | 99.92063% | 99.7619% |

The forward character converter now preserves two unusual but explicit retail
behaviours: null input returns zero without touching the destination, and the
input cursor advances only when an output character is written. Consequently,
null output with a nonterminating first byte returns the requested count rather
than scanning to a later terminator. The previous source did both incorrectly.
The two encoding branches now return independently, as retail does. The signed
mode switch follows retail's cmpwi comparisons; the decrementing count remains
unsigned. Input is const, and the original two 256-entry tables are preserved.
Local compiler controls and the disabled assembly arm were removed.

GScharLenCpy now performs the size increment and division in unsigned arithmetic,
matching retail's addi/srwi sequence rather than the previous signed shift and
potential signed overflow. It clamps the character count before copying, then
zero-pads through a normal postincrement loop. The compiler generates the
eight-way unrolling; no handwritten unrolling or compiler-control pragma remains.
An ordinary for-loop variant scored 44.542168% and was not retained.

The alignment audit deliberately removes a legacy reported exact result.
With its local compiler controls removed, the first lha at function offset 20
uses the incoming r3 instead of saved r31. Both registers still hold the same
work pointer there. All other instruction words in the 252-byte function agree;
the GSmsgGetRect relocation and both eight-byte conversion constants also pair.
The latter constants are independently checked against retail bytes
4330000080000000. No alias, helper or register hint was added to force r31.
This remains clean partial source, not a strict byte match. Canonical accounting
therefore falls by one matched function and 252 matched code bytes; linked code
is unchanged. The old pragma-dependent result was not a strict campaign win.

ASan/UBSan tests pass 98,304 converter cases using the retail table contents,
69,012 bounded-copy cases and 2,318,848 alignment cases. They cover null input
and output, 0xFF termination, both tables and default modes, copy-size overflow
boundaries, truncation/padding, every signed text width, all alignment modes,
floating-point rounding and state changes during the rectangle call. Three
negative controls fail as expected when each prior converter/size mistake is
restored. These native tests stub measurement/rectangle calls and route memcpy
through a length-checking wrapper; they are not PPC emulation or whole-game proof.

Configure, all_source/report, full link, retail SHA, progress, source quality
and diff checks pass. Comparing all 8,602 report functions finds only the three
changes above. Source outside those three functions equals the round baseline;
no .inc files, build options, shared headers or object splits changed. The
source-owner queue refresh adds alignment as a manual partial and preserves
all existing attempts; no fleet-wide sync or worker restart was used.
Artifacts and rejected variants are retained under ignored
`build/local_llm_campaign/manual/message_group_alignment/`.

Remaining differences include converter branch simplification/index masking,
copy-loop strength reduction and scheduling, and alignment's single load-base
operand. GScharCpy, GSmsgDaemon and the reverse conversion still carry legacy
controls and are not strict admissions. The wider group objective remains open.

## Compiler comparison and reverse conversion

An isolated whole-owner comparison tested GC/1.1p1, 1.2.5, 1.2.5n, 1.3 and
1.3.2 with the same source and O4,p flags across 18 cleaned functions. It did
not establish a broadly better compiler configuration. GC/1.3 and 1.3.2 produce
identical scores for all 18 targets. Older compilers slightly improve forward
conversion (93.125% versus 91.25%) but substantially worsen alignment, message
execution and several already exact wrappers. This is diagnostic evidence,
not proof of the original compiler version or permission to change build flags.
The active mixed scoring configuration remains unchanged. All five compiler
binaries, common tools, configuration and split/symbol files retained their
fingerprints throughout the probe and final audit.

fn_800F9AEC now uses structured, bounded pointer scans with the original two
encoding branches. Local compiler controls, register hints, the byte-value
temporary and hand-shaped goto flow are removed. Input and table pointers are
const. The routine preserves first-match selection, the 0xB7 fallback, zero-word
termination, null input/output behaviour, count-only scanning and the absence
of an appended byte terminator. The indexed for-loop experiment was rejected:
automatic eight-way unrolling diverged substantially from the retail loop.

The retained reverse converter is raw and canonical 90%, down from the legacy
92.42857% result. This reviewed regression trades an inadmissible source shape
for ordinary, semantically tested source; it is not a new byte match. Remaining
differences include branch simplification, register allocation and redundant
byte narrowing. No compiler controls, artificial aliases or helpers were added
to reproduce those instructions.

Native ASan/UBSan tests pass 266,256 reverse-conversion cases and 512 paired
forward/reverse cases. Every 16-bit code is checked against both retail tables,
with output enabled and disabled. Other cases cover null pointers, empty and
multi-character strings, first-match behaviour for duplicate entries, mode
fallback, unchanged input and untouched output tails. A negative control using
0xB6 instead of 0xB7 fails as expected. These tests use disjoint buffers and
the original table values; they do not emulate PPC execution or prove behaviour
for every possible aliasing arrangement.

Configure, all_source/report, full link, retail SHA, progress, quality scan and
diff checks pass. Exactly one of 8,602 report functions changes: the reverse
converter's reviewed regression. Matched-function, matched-code and linked-code
deltas are zero. Source outside that function and its const-input declaration
equals the baseline, and no recovered helper is emitted in the ten message
objects. No .inc file changed. The source-owner queue refresh retains attempt
history and requires no worker restart or fleet-wide sync.

Probe objects, compressed raw diffs, source variants, fingerprints and tests
are retained under ignored
`build/local_llm_campaign/manual/message_group_compiler_recovery/`.
GScharCpy and GSmsgDaemon still require strict-source cleanup; reverse conversion
no longer carries local controls, but remains nonexact. Compiler provenance,
the other message residuals and the full dependency closure remain open.

## Shift-JIS dependency audit

fn_80080ED8 improves from canonical 63.4% to 79.27059%, and from raw
60.835293% to 78%. It directly serves GScharMakeFromSJIS and fn_800FAEF8.
Byte classification now uses the source byte, and conversion shares its
source-byte accounting and return path. The return counts consumed input
bytes, not output characters. Null input, count-only mode, termination and
halfwidth-kana mapping retain the retail behavior.

The two flat u16 tables have evidenced spans of 31 * 184 and 29 * 184
elements. Row pointers into these flat arrays preserve cross-row indexing.
A nested-array variant scored raw 86.35294% but was rejected: some accesses
cross a row boundary while remaining inside the complete table, which is
undefined for nested-array indexing. A sanitizer negative control reproduces
that bounds violation. All 15,360 lead/trail address calculations agree with
retail's table-offset arithmetic.

Native ASan/UBSan tests pass 39,162 cases, including 4,176 cross-row lookups,
using the retail table values. Three negative controls fail as expected.
The tests use disjoint buffers and do not emulate PPC. Of the lead/trail
combinations, 144 address outside the complete declared table: these were
tested only in count-only mode, not decoded. No new malformed-input guard or
clamping was added, and these tests do not establish arbitrary-input safety.

Configure, all_source/report, full link, retail SHA, progress, source-quality
and diff checks pass. Exactly one of 8,602 report functions improves, with
no regressions and no new exact functions, matched bytes or linked bytes.
Table relocation types and referent symbols pair; this is not byte/offset
relocation exactness. Source outside this function is unchanged. No .inc,
shared-header, compiler-option or split changes were made. The active scoring
object still uses GC/1.3 despite its legacy gc125 filename.

Artifacts and rejected variants are retained under ignored
`build/local_llm_campaign/manual/message_group_sjis/`. The known dependency
closure remains 1,193 functions with 141 reported nonexact; indirect and data
coverage and strict audits of reported exact source remain unfinished.

## Font setter and unsigned string comparison

GSmsgSetFontInfo now uses the established FontSlot layout shared with font
registration and glyph lookup. Its canonical 91.63265% and raw 91.53061%
scores are unchanged. The existing four font record definitions move earlier
in the same owner without changing their fields; 21 PPC sizes/offsets pass
an active-MWCC check. No shared header or new inline helper was introduced.

The setter's early-return experiment emits the same code and was not retained.
Isolated C and C++ language-mode probes also both score raw 91.53061%; they do
not resolve the remaining branch, narrowing and scheduling differences or
establish original language provenance. Earlier switch and spacing-helper
failures remain rejected. The floating-point spacing expression is 0.5 times
the height plus 1.0. At heights 254 and 255 its truncated result is 128, so a
direct floating-to-signed-byte cast is out of range. A sanitizer negative
control confirms that failure; the safe intermediate integer conversion stays.
The compiler-generated unsigned-to-double bias independently matches the
retail constant 4330000000000000.

GScharCmp improves from 72.458336% to canonical/raw 95.833336%. It now uses
const u16 inputs, unsigned length comparisons and ordinary indexed loops.
The old source's signed length ordering contradicts retail's cmplw. Manual
cursor/count/index loop shaping, local compiler controls and the disabled
assembly arm are removed. The established three length cases and the order
of both length-parser calls are retained.

The comparison-only O2 override in configure.py is removed, restoring the
normal O4,p configuration used by most of this owner. No replacement tuning
flag, compiler version or split was added. Under the old O2 override the
clean indexed source scored 57.927082% and did not generate the retail's
pointer/count-loop transformations; normal optimization generates those
itself. This is a candidate-build cleanup, not proof of original TU boundaries
or permission to promote the object. Its legacy filename still ends in o2.

Independent comparison finds exactly three differing instruction words, at
function offsets 16, 20 and 28, all in the prologue's save/move scheduling.
Both REL24 call relocations match in offset, type, symbol and addend. The
remaining 352-byte tail agrees with the target object, and its nonrelocated
instructions agree with the original DOL. The function remains nonexact.

Native ASan/UBSan tests pass 393,220 setter cases and 294,943 comparison cases.
Coverage includes all font-ID values, all width/height pairs for four font IDs,
duplicate selection, missing/empty/full tables, all 16-bit character values,
prefix ordering, overlapping comparison inputs and length-parser call order.
Three negative controls fail as expected. The length parser is stubbed; large
unsigned lengths are tested only with zero common length or an immediate
mismatch. Font table pointers and struct sizes are adapted for the native host.
These checks are not PPC emulation or arbitrary-invalid-pointer safety proof.

Configure, all_source/report, full link, retail SHA, progress, added-source
quality and diff checks pass. Comparing all 8,602 functions finds only the
comparison improvement, with no regressions and no new exact functions,
matched bytes or linked bytes. No .inc file changed. Both local workers were
verified live, gracefully stopped and confirmed exited before the shared
configuration edit; their existing profiles resume after validation. Owner-only
queue refresh preserves attempts and proposals without a fleet-wide sync.

Source snapshots, rejected probes, runtime tests and verification evidence are
under ignored `build/local_llm_campaign/manual/message_group_font_setter/`.
The wider message and dependency objective remains unfinished.

## Copy and daemon strict-source audit

| Function | Legacy canonical/raw | Clean canonical/raw |
| --- | ---: | ---: |
| GScharCpy | 100% | 89.13793% |
| GSmsgDaemon | 100% | 96% |

These are deliberate withdrawals of inadmissible exact-source accounting,
not new byte matches. Both functions lose local optimization/peephole controls
and disabled assembly arms. The copy source is const; its size-parser
declaration now agrees with the actual signed-return, const-u16-input
definition. It preserves null-destination early return, null-source word
termination, byte-count forwarding, call order and the original destination
return. An outer guarded-copy variant scored 75% and was rejected.

The owner now includes the existing game/gs_texture.h instead of conflicting
local texture declarations. UnlockImage has its canonical u32 return and
handle-pointer argument, LockImage takes a handle and mip level, and Create
uses its declared signed dimensions/formats. The daemon loads a texture
pointer rather than declaring an integer-argument override. No shared header,
build flag, compiler version, symbol or split changed this round. Other
functions' executable statements are unchanged; their stale block-scope
texture declarations were removed.

The daemon retains the signed-byte atlas index, unlock call, post-call global
manager reloads, cursor reset to (2, 1), and low-bit index toggle. Independent
byte comparison finds exactly one missing retail instruction: extsb r0, r0
at offset 76. Removing that word from the 100-byte target leaves the compiled
96-byte text exactly; all five relocations also pair in offset, type, target
and addend. The extension is redundant before a byte store, but its absence
still prevents a byte match. No volatile qualifier, invented helper or other
shaping was added to retain it. The copy's remaining differences are mainly
prologue scheduling and the fused destination null check.

Native ASan/UBSan tests pass 65,616 copy cases and 131,072 daemon cases.
They check null combinations, all copy counts through 1024, unchanged tails,
parser side effects before copying, all 256 signed index encodings and toggle
values, and manager replacement/index mutation during unlock. Four negative
controls fail as expected for unsigned indexing, stale manager use, copying
character counts instead of bytes, and calling the parser for null output.
The texture pointer is represented by an opaque 32-bit token on the host;
parser, unlock and copy calls are instrumented stubs. Four huge copies only
record the forwarded length, and atlas-index tests use a sufficiently large
containing buffer. These are not real texture operations, PPC emulation or
proof that invalid indices are safe for the production manager allocation.

Configure, all_source/report, full link, retail SHA, progress, source quality
and diff checks pass. Comparing all 8,602 report functions finds exactly the
two reviewed regressions above: -2 reported exact functions and -216 matched
code bytes, with zero linked-code change. The drawing callers and initializer
retain their scores after the header correction. No .inc file was changed.

The lower-bound dependency closure remains 1,193 functions, now with 143
reported nonexact after these withdrawals. Both functions return to the
owner's review worklist with source hashes and audit records; existing attempts
are preserved, and no fleet-wide sync or worker restart is needed. At the end
of this round the only local compiler-control block left in gs_msg.c was
fn_800F96E4; the following round removes it. Indirect
dependencies, data coverage, compiler/TU provenance and the full goal remain
open. Evidence is retained under ignored
`build/local_llm_campaign/manual/message_group_copy_daemon/`.

## Bounded message expansion

fn_800F96E4 improves from 50.957363% to canonical 79.81396% and raw
79.79457%. The candidate is 980 bytes versus retail's 1,032 bytes. It is not
byte-exact, and no object is promoted or newly linked.

The function now reuses the previously evidenced GSmsgFindMessage,
GSmsgReadCode and GSmsgDispatchControl helpers. Their definitions and the
associated record definitions move earlier in the same owner, unchanged.
No standalone helper appears in any of the ten message scoring objects.
Its explicit prototype describes destination, signed capacity and message key.
The disabled assembly arm and local compiler controls are removed; none
remain in gs_msg.c. No .inc, header, compiler-option, symbol or split change
was made this round.

Recovered behavior includes full-width binary-search counters, two-byte
character reads, single-byte control IDs, nested return-stack handling and a
two-byte output terminator. Capacity is measured in halfwords including the
terminator; the byte allowance uses unsigned arithmetic. An overflowing
control record rewinds the output pointer and terminates rather than continuing
with a reset count. The parser preserves callback-induced cursor changes and
reloads the manager for subsequent dispatch and mode-2 lookup.

PowerPC differential tests execute the original DOL function and the relocated
candidate instructions on identical fixtures. All 2,757 cases agree in return
value, complete output/arena bytes, the 0x68-byte work record, callback/log
traces and manager changes; stack and nonvolatile GPR preservation also pass.
Coverage includes invalid arguments, all tested capacity boundaries, odd-sized
control payloads, modes 0/1/2, nested expansion, depth-three overflow logging,
post-callback mode mutation, manager/control-table replacement, chained groups
and search tables up to 65,535 entries. Three emulator-only instruction
mutations are detected: four-byte character stepping, a one-byte terminator,
and continuing after payload overflow. No bad variant touches active source.

These are finite, stubbed PPC32 tests, not a full GameCube execution or C
undefined-behavior proof. memset, GSlogWrite and control callbacks are
instrumented stubs. Candidate relocations are resolved into test memory, with
its generated 1.0f constant checked against retail bytes; this is not evidence
of production data or relocation exactness. Mode 3 remains unresolved: retail
retains a register value while the existing helper uses a defined cursor
fallback. Its absence from the installed table does not close runtime mutation.

The ignored test environment pins Unicorn 2.1.4. On this Mac its cache
initialization traps inside the sandbox, so tests ran outside the sandbox but
still under the campaign build lock. Reproduction requires an active Codex
claim on this owner and that provisioned venv:

```bash
python3 tools/local_campaign.py build --worker Codex -- \
  build/local_llm_campaign/manual/message_group_bounded_message/venv/bin/python \
  build/local_llm_campaign/manual/message_group_bounded_message/ppc_check.py
```

Configure, all_source/report, full link, retail SHA, progress, source scope,
added-source quality and diff checks pass. Of 8,602 report functions only this
one improves; matched functions, matched bytes and linked bytes are unchanged.
Both existing local-model worker handles were verified live; no restart or
fleet-wide sync was needed. Owner-only queue refresh preserves attempt history.
Source snapshots, comparisons, tests, logs and review evidence remain under
ignored `build/local_llm_campaign/manual/message_group_bounded_message/`.
The full message/dependency objective remains open.

## Length wrapper arithmetic audit

_msgGetLength__FPCUs remains canonical/raw 100%, with its existing cross-file
const-void pointer interface unchanged. Its addition now converts the size
result to u32 before adding one. Previously the cast happened after a signed
addition, so an injected INT32_MAX size result caused undefined signed overflow.
Retail uses wrapping addi followed by an unsigned right shift. Casting the
shifted value to s32 before subtracting one also keeps the final subtraction
in its representable range, from -1 through INT32_MAX - 1.

Independent comparison verifies all 44 instruction bytes and the sole REL24
relocation to _msgGetSize__FPCUs. Resolving that relocation at the retail address
reproduces all 44 original DOL bytes. This is an audit of already reported exact
source, not a new exact function, object promotion or increase in linked bytes.

An extracted-source ASan/UBSan harness passes 1,786,433 cases against an
independent 64-bit arithmetic reference. Tests cover zero, positive/negative
boundaries, wraparound, deterministic random bit patterns, null/non-null input
forwarding and exactly one size-parser call. Restoring the original expression
in an ignored negative-control fixture triggers the expected signed-overflow
diagnostic. The size parser is stubbed: these boundary returns are not claimed
to be reachable from actual game messages, and the tests do not prove the
parser or its callers complete.

The production change is one return expression. A const-u16 interface probe
also kept all message scores unchanged, but was not retained because other
owners still declare the existing interface. GScharLenCpy was inspected but
not modified: its known gap is copy-loop strength reduction, and the previously
rejected loop variants were not repeated or hidden behind compiler controls.

Configure, all_source/report, full link, retail SHA, progress, source scope,
quality and diff checks pass. All 8,602 function scores are unchanged, as are
matched functions, matched bytes and linked bytes. No .inc, header, config or
split change was made, and no existing helper is emitted in the ten message
objects. Owner-only queue hashes are refreshed without deleting attempts or
restarting the fleet. Evidence is under ignored
`build/local_llm_campaign/manual/message_group_length_arithmetic/`.

## Signed TEV color setter

fn_800BC36C improves from canonical/raw 77.93104% to 100%, adding one exact
function and 116 matched bytes. Linked bytes remain unchanged: its scoring
object is still incomplete, with GXSetTevOp, fn_800BC228 and fn_800BC290 left
nonexact. No symbol rename, split, compiler flag or promotion was made.

This is required-object completion work, not a newly matched member of the
known outgoing call closure. The measured dependency path is:

```text
fn_800FAEF8 -> fn_800D67BC -> fn_800D892C -> fn_800D923C -> fn_800BC228
```

fn_800BC36C shares main/dolphin/sdk_candidate_800BBC0C with that last dependency.
Completing that object is necessary for a future linked promotion without
inventing another split. The lower-bound call closure itself remains 1,193
functions with 143 reported nonexact, not 142. Indirect and runtime-mutated
dependencies remain open.

The setter now masks signed alpha/green components to eleven bits before
shifting them, avoiding negative signed shifts. It uses the owner's existing
GX_BP_REG macro for the four FIFO packets and no longer caches the constant
GX-state pointer or interleaves packing with hand-written FIFO stores.
The ordinary masked packing is also present in the
[dolsdk2001 GXSetTevColorS10 reference](https://raw.githubusercontent.com/doldecomp/dolsdk2001/master/src/gx/GXTev.c).
That reference is corroborating evidence, not Colosseum source provenance;
retail instructions remain authoritative, including this version's final
zero store to GX state.

All 116 raw instruction bytes equal the target object. The sole gx SDA21
relocation uses instruction offset 40 in dtk and halfword offset 42 in MWCC;
these raw relocation records are not identical. An ignored probe compiled
with the active GC/1.2.5n recipe and no source pragmas emits the identical
function. The real Metrowerks linker resolves it at the retail code and gx
addresses using the retail SDA2 base, reproducing all original DOL function
bytes and the gx constant. This verifies the effective relocation rather than
silently normalizing away the record difference. It is not a full candidate
game link or permission to promote the incomplete object.

Extracted-source ASan/UBSan tests pass 1,148,580 cases: every 16-bit encoding
of each component across the four valid register IDs, plus sampled component
combinations and ID bit patterns. They check the byte/word packet sequence,
all repeated writes, register packing, unchanged surrounding GX state and
the final state update occurring after the FIFO writes. Three negative
controls fail for a restored negative signed shift, a missing repeat packet
and an incorrect alpha mask. The FIFO is a native fixture, not a real GPU;
the component Cartesian product is not exhaustively tested.

The initial macro-only experiments on fn_800BC228 and fn_800BC290 left both
at 99.42308% and were restored. They still differ in the FIFO address register;
no artificial alias, hint or compiler-control change was retained to force it.
Source outside fn_800BC36C equals the round baseline. The other 54 scored
functions across the owner's fourteen units retain their raw scores.

Configure, all_source/report, full link, retail SHA, progress, source scope,
quality and diff checks pass; the 8,602-function report has no regressions.
No .inc or shared header changed. The two existing workers remained live,
with no restart or fleet-wide sync. Owner-only queue refresh retains attempts
and exposes the exact source for review. All probes, rejected variants,
tests and audit records are under ignored
`build/local_llm_campaign/manual/message_group_tev_ops/`.

## TEV preset recovery and exact ownership probe

The live GXSetTevOp improves from 88.828575% to canonical 92.4% and raw
92.25714%. It uses one sequential register value and direct GX-state accesses,
consistent with the SDK reference. No new helper, allocation hint, local
compiler control, signature, symbol, split or shared-header change is retained.
Only this function changes in the SDK owner during this round; the previous
fn_800BC36C exact result remains intact.

The first preset object in data_80313590.c now defines twenty u32 words,
matching the SDK owner's existing declaration and retail word loads, rather
than eighty u8 elements behind an incompatible declaration. Its complete
compiled data owner retains identical allocatable section bytes, alignments,
symbol positions/types/sizes and relocations. All eighty preset bytes match
retail; the other six data objects are unchanged. This already-linked data
object passes the full retail link/hash check after the type correction.

Extracted-source ASan/UBSan tests pass 419,840 cases over all sixteen valid
stages and five modes. They cover preserved register-ID bytes and alpha swap
nibbles, walking bits, sampled register/table words, FIFO packet width/order,
cache updates between the packets, final state clearing and unchanged
surrounding state. Four negative controls reject reversed stage selection,
a lost alpha nibble, an early cache update and a missing packet. These are
finite native FIFO fixtures, not actual GPU execution or evidence for invalid
stage/mode calls. A separate no-source-pragma probe reproduces the live
function's instructions and relocations.

A stronger reconstruction was found in the locally checked-out Dusklight
SDK reference, commit c55cc18003998e9d3a336fb538ffac958d104d1c,
`libs/dolphin/src/gx/GXTev.c`. It has four separate five-entry bitfield tables:
TEVCOpTableST0, TEVCOpTableST1, TEVAOpTableST0 and TEVAOpTableST1.
The corresponding offsets within the current lbl_80313590 object are
0, 0x14, 0x28 and 0x3C. This is corroborating decompilation evidence, not
Colosseum original-source provenance.

An ignored probe compiles those reference definitions and GXSetTevOp using
the active GC/1.2.5n recipe, with the existing GX state/FIFO API substituted
and the three debug-only checks omitted. No source pragma is present. The
real Metrowerks linker places the code, tables and gx constant at their retail
addresses with the retail SDA2 base. All 140 function bytes, all 80 preset
bytes and the gx constant equal the original DOL. This checks resolved
relocations, not only a fuzzy score or zeroed relocation operands.

A copied full scoring translation unit also reproduces that exact function's
instructions and relocations and the full 80-byte preset section. Its other
26 compiled functions retain identical instructions and relocations. The
copied owner retains unrelated legacy compiler controls, so this is not an
owner-wide source-quality approval; GXSetTevOp's independence is established
by the separate pragma-free probe.

The exact reconstruction is NOT integrated or counted as accepted progress.
Its four static tables need to be paired with the code through the canonical
data ownership, rather than duplicated in the live data owner. At present
those 80 bytes belong to Matching game/data/data_80313590, while the code
owner is CodeCandidate. Moving them now would withdraw 80 linked data bytes
until the code object is completed. The remaining code neighbours are
fn_800BC228 and fn_800BC290, both raw 99.42308%; their canonical type/macro
recovery and the owner-wide quality audit are the next integration work.
Do not solve the ownership mismatch with aliases, invented aggregate types,
overlapping splits or a text-only promotion. A shared-config change requires
the coordinated fleet maintenance procedure.

The retained live changes pass configure, all_source/report, full retail
link/SHA, progress, source scope, quality and diff checks. Of 8,602 report
functions, only GXSetTevOp's score changes; its other 54 scored owner peers
are unchanged. New matched functions/bytes and new linked code/data are all
zero. The known message dependency closure is not reduced by this probe.
Owner-only queue refresh preserves attempts and the earlier exact review,
and records the exact probe separately from the live partial candidate.
The two local-model workers were verified live and were not restarted.
Evidence, variants, test fixtures and the exact source reconstruction remain
under ignored `build/local_llm_campaign/manual/message_group_tev_presets/`.

## TEV color and alpha operation setters

fn_800BC228 and fn_800BC290 both improve from canonical/raw 99.42308% to
100%. Each is 104 bytes, for two newly exact functions and 208 matched code
bytes. Both are in the known outgoing message/control-handler dependency
closure, which remains 1,193 functions but now has 141 reported nonexact,
down from 143. Their common dependency path is:

```text
fn_800FAEF8 -> fn_800D67BC -> fn_800D892C -> fn_800D923C
                                                    -> fn_800BC228
                                                    -> fn_800BC290
```

The plain-C reconstruction follows GXSetTevColorOp and GXSetTevAlphaOp in
the same local Dusklight SDK reference described above. The comparison-mode
field is expressed as `(op >> 1) & 3`, and the clamp input explicitly retains
its low byte. Both setters use the existing GX_BP_REG macro and direct GX
state accesses. The cached GX pointer and individually interleaved FIFO
stores are removed. The public signatures and address-based symbol names
are unchanged; the reference's narrower parameter types were not needed.
No allocation hint, artificial alias, helper or local compiler control was
introduced. The signed right shift only occurs in the `op > 1` branch, so
negative values do not depend on implementation-defined right shifting.

All 208 raw instruction bytes equal the target. Each setter has two GX-state
SDA21 relocations: dtk records instruction offsets 0 and 60, whereas MWCC
records halfword offsets 2 and 62. Those metadata records are not literally
identical. A pragma-free isolated compile reproduces the live instructions
and relocations. The real Metrowerks linker resolves both functions at their
retail addresses with the retail GX constant and SDA2 base, reproducing all
208 original DOL bytes. The ignored fixture's entry point calls both setters
to retain them through normal linker reachability; no production FORCEACTIVE
or linker configuration changes were made.

Extracted-source ASan/UBSan tests pass 252,480 cases over both setters,
all valid stage indices, arithmetic and comparison operation encodings,
ordinary parameter combinations, signed operation boundaries and sampled
full-width words. They check register packing, FIFO packet width/order,
unchanged state until the packet is emitted, the final cache/status update
and unchanged surrounding state. Five negative controls reject the wrong
operation branch, lost comparison bits, a lost clamp bit, an early cache
update and a missing packet. The native intrinsic/FIFO fixtures are not an
actual GPU execution. Tests of reserved operations and out-of-domain words
exercise defined bit operations, not additional supported SDK API values;
out-of-range stage indices are excluded.

Only these two bodies differ from the round baseline. All other 53 scored
functions across the owner's fourteen units retain their raw scores, and
the compiled non-text sections are unchanged. Configure, all_source/report,
full retail link/SHA, progress, source scope, quality and diff checks pass.
The 8,602-function report has exactly these two improvements and no
regressions. No .inc, data owner, header, signature, compiler option, symbol
or split changed. Newly linked code and data both remain zero.

GXSetTevOp is now the only reported nonexact function in this scoring object.
Its retained exact four-table reconstruction still needs canonical data
ownership integration. The existing whole-owner compiler-control and ABI
audits remain prerequisites to promotion; this round does not make the
entire owner eligible for linking. The wider message objective remains open.

The owner-only queue refresh records both exact candidates, preserves the
earlier fn_800BC36C review and all local-model attempts, and updates the
remaining-function counts without a fleet-wide sync or restart. Source
snapshots, full-owner comparisons, semantic tests, actual-link evidence and
review reports are retained under ignored
`build/local_llm_campaign/manual/message_group_tev_setters/`.

## Live TEV preset ownership integration

GXSetTevOp is now canonical/raw 100% in the live scoring object, adding
one exact function and 140 matched code bytes. Its four five-word preset
tables are compiled in that same owner. They use ordinary u32 arrays with
the SDK-evidenced names TEVCOpTableST0/1 and TEVAOpTableST0/1; no bitfield
type-punning, aliases or invented aggregate layout is needed. The existing
function signature remains unchanged. All 80 data bytes match retail.

The relocation census covers 2,288 target objects and finds only the two
GXSetTevOp address-materialization relocations referring to the former
lbl_80313590 object. Canonical symbols now identify the four local arrays at
0x80313590, 0x803135A4, 0x803135B8 and 0x803135CC, each 0x14 bytes. The split
moves only 0x80313590..0x803135E0 into the existing SDK scoring object.
The data_80313590 source keeps the remaining 508 bytes and six symbols at
their original retail addresses. Its 109 relocations and all remaining
contents, relative symbol positions and alignment are unchanged. dtk also
reorders five existing data-unit blocks after the mixed code/data owner to
preserve linker section order; their ranges are unchanged. No text split,
compiler option, configure.py status or shared header changed.

All six legacy compiler-control pragma lines were removed from
sdk_range_800BB30C.c. __GXFlushTextureState stays raw 100%, and no other
scored function regresses. An isolated compile with no source pragmas
reproduces the live __GXFlushTextureState and GXSetTevOp instructions and
relocations. Real linking verifies their 176 combined code bytes, all 80
preset bytes and the GX pointer constant at retail addresses. The test
entry calls both functions through ordinary references; no production
FORCEACTIVE change is involved. This establishes the new function/data
pair and the flush function's independence from compiler controls, not a
whole-owner semantic or ABI audit.

The updated four-array native fixture passes 419,840 ASan/UBSan cases and
four negative controls. It indexes each array independently, without
assuming native adjacency, and checks valid stage/mode combinations,
preserved bits, sampled words, FIFO ordering and cache/status sequencing.
Configure, all_source/report, full retail link/SHA, progress, source scope,
quality and diff checks pass. All other 54 scored owner functions retain
their raw scores; the 8,602-function canonical report changes only
GXSetTevOp. The known call closure remains 1,193 functions with 141 reported
nonexact. GXSetTevOp is required-object work outside that lower-bound closure.

This is NOT a linked promotion. Matched data is unchanged; linked code is
unchanged; linked data decreases by 80 bytes because the presets move from
an already Matching data object to a CodeCandidate. The retail build still
uses the original SDK code/data object, so its successful hash does not
constitute a full candidate-code link. The linked-data withdrawal is an
explicit ownership correction, not increased progress.

The audit also exposes five canonical/raw discrepancies in this owner:

| Function | Canonical | Raw |
| --- | ---: | ---: |
| fn_800BB81C | 100% | 99.943184% |
| fn_800BBCE0 | 100% | 99.85981% |
| GXProject | 100% | 99.78494% |
| fn_800BD454 | 100% | 99.791664% |
| fn_800BD91C | 100% | 97.22642% |

Do not assume simple numeric rounding explains these discrepancies. The
current sync implementation excludes canonical-100 functions, so they were
absent from the worklist. This owner-only update adds all five, uses raw
scores for its residual counts, and preserves all existing attempt fields.
fn_800BD454 and fn_800BD91C can enter the model queue; the other three are
blocked_source_context because their definitions occur in intermediate
scoring wrappers rather than the resolved root. They need manual matching
or a source-resolution fix before automated editing. A future full sync
must preserve or remeasure these exceptions instead of silently dropping
them again; the global canonical-only sync limitation is not fixed here.

Although the SDK scoring object's canonical function list is now all 100%,
fn_800BBCE0 remains raw-nonexact. Its comparison includes unresolved constant
relocation ownership, and the compiled wrapper emits seven additional
functions outside this scored range. Both facts prevent simply promoting
the existing object. The next integration audit must cover these issues
and actual source/data ownership, not just the canonical percentage.

The workers were gracefully stopped and their old process handles confirmed
absent before shared configuration edits. After verification and the 29-item
owner-only queue update, the same worker arguments, models and context
limits were restarted. No global sync or attempt-history reset was performed.
Evidence, snapshots, the exact source/data pair, ownership census, test
fixtures, reports and maintenance manifest are under ignored
`build/local_llm_campaign/manual/message_group_tev_integration/`.

## Unit-wide nopeephole and single-function links

The GSmsg translation unit (0x800F96E4-0x800FE35C) was built with the default
GC/1.3 -O4,p flags plus `-opt nopeephole`. Compiling the unchanged source
with that one extra flag scores all 36 functions equal or higher than plain
-O4,p, none lower. GSmsgInit shows the clearest sign: `clrlwi r31,r30,16;
slwi r3,r31,3`, which the peephole pass would fuse into `clrlslwi`. The old
per-unit GC/1.2.5n, -O3, -O4,s and -O1/-schedule settings were legacy guesses.
In particular, GSmsgInit's font-slot loop is unrolled eight ways, which is
-O4 output. All ten scoring units now share the one setting (29 functions up,
0 down in the canonical report).

`struct MessageSystem` (0x2C bytes at lbl_804024E8, reached through
lbl_80478B08) now types the shared state. GSmsgInit, GSmsgFontOpen,
GSmsgDaemon, GScharCpy, GScharLenCpy, GScharCmp, GSmsgAdjustAlign and
fn_800FBE7C are exact. GSmsgFindMessage now masks the key into a separate
index local, with the declaration order retail's register assignment
implies. That makes GSmsgGetGSchar (now just the expansion) and
GSmsgGetLength exact and raises every other expansion. The GScharCmp,
GSmsgGetGSchar, GSmsgGetLength and GSmsgInit units each cover a single
function. Their wrappers compile only that function (GS_MSG_*_ONLY), and all
four units are linked.

The 0x800FB680 prefix unit cannot be linked as split. GSmsgClose, GSmsgOpen,
GSmsgFontClose, GSmsgFontOpen and GSmsgSetCtrlFunc are exact. fn_800FB680,
fn_800FB8C8, fn_800FBB34 and GSmsgExec convert integers to double. Each
conversion loads a compiler-generated magic constant (lbl_8047CD10 signed,
lbl_8047CD28 unsigned) that retail keeps in the shared data unit
`game/data/sdata2_8047CC98.c`. A text-only unit compiles its own private
copy, so those relocations can never pair. Only a unit that owns the TU's
.sdata2 slice can link them. Options: rejoin the TU with its .sdata2 slice,
or split 0x800FC1D0-0x800FC528 (the five exact functions, no data) into its
own unit. The second option is a splits.txt change and was not made here.

Open residuals: the font line-height `if` in GSmsgSetFontInfo, GSmsgExec
and fn_800FB680 compiles in retail to `cmplwi 1; beq; bne`. Only a
redundant `id == 1 || id == 1` reproduces this, so it was rejected.

## Nested control-message lookup

The inline control dispatcher now exits both lookup loops directly when a
nested message key is found. The previous exit inferred success from
`lo < hi`, which was true at that point but left an extra loop test and a
different control-flow shape. The direct exit preserves the same found and
not-found behavior and improves four canonical functions without regression:
fn_800F96E4 83.78682% to 84.5814%, GSmsgGetRect 91.44198% to 91.94815%,
GSmsgInitRuby 80.078575% to 80.810715%, and fn_800FC7E0 88.57671% to
88.94658%. These remain CodeCandidate scores, not newly linked functions.

An unsigned depth-byte trial gave GSmsgGetRect a further 0.02469 points but
regressed GSmsgInitRuby by 0.535715 points; it was removed. The retained
change passes configure, source/report, full retail DOL/REL hash validation,
quality tests, and diff checks. The remaining GSmsgGetRect differences include
the shared work-pointer register allocation, control-result lifetime, font
scan branching, and conversion-constant relocation ownership.

## Rectangle local-order follow-up

Moving GSmsgGetRect's existing zero-initialized `lineStart` and uninitialized
`fontId` declarations to the start of its local block improves its canonical
score from 91.94815% to 92.6395% (raw objdiff 91.78765% to 92.47901%).
The declarations retain their types and values; the function's control flow
and the shared dispatcher's source are unchanged. Ninety-six then 101 bounded
semantics-preserving rewrite probes found these two moves. Initializing or
reordering the dispatcher's `next` declaration and using a prototype-style
signature for GSmsgGetRect emitted the previous code and were reverted.

The object remains a CodeCandidate: GSmsgInitRuby and fn_800FAEF8 are
nonexact, retail keeps the dispatch destination in saved `r30` where the
candidate uses `r3`, the frame is 0x50 versus 0x40, and shared conversion
constants still need their original translation-unit data ownership. This
is neither an exact function nor linked title-path progress. The all-source
report, full retail DOL/REL hash check, and diff checks pass.

## Rectangle global-work initialization

Retail `GSmsgGetRect` reloads the address of `lbl_80401E48` after `memset`
and initializes its first seven fields through that address, while preserving
the work pointer in `r29` for the later scan. Accessing those fields through
the named global instead of the saved local reproduces this sequence without
changing the record or initialization order. Raw objdiff improves from
92.47901% to 93.18272% (412 aligned instructions). Extending the direct
global access to the following font-id and key stores scored 93.145676%, so
that trial was reverted. The retained function remains nonexact in the same
unlinked CodeCandidate object; its 0x50 retail versus 0x40 candidate frame,
control-dispatch register lifetime, and constant ownership remain open.
Configure, all-source/report, full link, retail DOL and REL hashes, progress,
quality-scan tests and source wrapper scan pass.

## GSmsgGetRect report-exact (lane D3, 2026-09-29)

`GSmsgGetRect` now scores 100% in the canonical report (93.34321% before),
and `GSmsgSetFontInfo` 100% (97.755104%). No object is newly linked. The
source changes, each measured on the rebuilt objects:

- **Control dispatcher** (`GSmsgDispatchControl`, XD's dead-stripped
  `_msgCallCtrlFunc__FP13MSG_TASK_WORKUc`, NXXJ01.map GSmsg.o UNUSED 0x1D8).
  Mode 2 is `GSmsgFindMessage(result, NULL)`: retail repeats the lookup's
  zero-key test (`cmplwi r3,0; bne; li r30,0`). The switch has no `default`;
  retail's default arm jumps straight to the push with `next` unassigned,
  which is why `next` lives in saved r30. The enable test indexes
  `table[control]` in each arm of an if/else, and the pointer is taken after
  the test (retail's `slwi; lbzx` per arm, then `slwi; add r25`). The push is
  `stack[depth++] = cursor`, incrementing the raw depth byte.
- **Font lookup** (`msgSetFontInfo`). Retail expands GSmsgSetFontInfo's body
  in GSmsgGetRect after storing the font id through the global work record,
  so the id is forwarded from that store (`lbz r0; sth r0,0x20(r4); ...
  clrlwi r4,r0,16`) instead of being reloaded. The body is repeated in
  GSmsgSetFontInfo and the renderer initializers, which admits the helper.
  MWCC's `-inline auto` does not inline GSmsgSetFontInfo itself, and marking
  it `inline` drops the standalone symbol, so the helper is `static inline`
  and the standalone keeps its own body (routing it through the helper
  swaps r5/r7 there).
- **The `cmplwi 1; beq; bne` line-height test.** XD's GSmsgSetFontInfo
  (GXXE01 0x80107200; trevor403/xd-asm b1087f18,
  `code/func_FUN_80107200.s`) tests `id == 1 || id == 3` for the six-pixel
  line height. Colosseum emits the same two-test disjunction with both ids
  equal to 1, so every expansion now reads `id == 1 || id == 1` with that
  citation. This supersedes the earlier rejection of the form as redundant.
- GSmsgGetRect itself: the lineStart advance converts `(s32)` (retail uses
  the signed bias there), the space half-width is `work[0x22] / 2` (MWCC
  emits `srwi` for the promoted byte), the final height is assigned back to
  `maxY`, and the locals put `maxX`, `maxY`, `lineStart` in retail's
  r28/r27/r26. `GSmsgFindMessage` declares `mid, index, group` in that
  order, which gives retail's group/index registers without changing
  GSmsgGetGSchar or GSmsgGetLength.

Side effects (canonical report): fn_800F96E4 84.58 to 92.22, GSmsgInitRuby
80.81 to 87.93, fn_800FAEF8 88.83 to 89.15, fn_800FB43C 96.03 to 96.93,
fn_800FB680 96.06 to 96.95, fn_800FB8C8 97.32 to 98.16, fn_800FBB34 98.19
to 99.06, GSmsgExec 98.64 to 99.64, fn_800FC7E0 88.95 to 94.70,
_msgGetSize 98.13 to 98.65. Nothing regressed.

**Why GSmsgGetRect still cannot link.** Its code loads 1.0f (lbl_8047CD08),
both int-to-double biases (lbl_8047CD10 signed, lbl_8047CD28 unsigned) and
the line-height 1.0/0.5 (lbl_8047CD18/CD20). Every other GSmsg function is
linked from its retail target object, and those reference the same
constants by name, so the pool cannot move into a carve: a compiled carve
emits anonymous `@` literals and would leave those names undefined. Tested
on 2026-09-29: an IsCheck+GetRect carve emits `1.0f, 1.0, 0.5, unsigned,
signed` (retail order is `1.0f, signed, 1.0, 0.5, unsigned`, because
GSmsgAdjustAlign creates the signed bias first). Named `.sdata2`
definitions in the carve are not merged with the compiler's literals
(the section doubles). A hand-written union conversion against the named
bias compiles to `fsub; frsp`, not retail's `fsubs`. GSmsgGetRect is
therefore accepted only when the whole GSmsg TU (0x800F96E4-0x800FE35C,
with its `.sdata2` pool at 0x8047CD00) links as one object. The same applies
to fn_800FB680/fn_800FB8C8/fn_800FBB34/GSmsgExec and every other function
that converts integers to floats.

## GSmsg TU link: whole-TU only (lane D3 handoff, parked 2026-09-29)

Report-exact on branch `claude/decomp-d3-title-msg`: GSmsgGetRect,
GSmsgSetFontInfo, GSmsgExec, fn_800FBB34 and GSmsgSetColor (the last
through `(color >> n) & 0xFF`, which MWCC emits as `extrwi`). None of
them is linked. Parked at the coordinator's request, because fn_800FB680
is in the frozen bench main set.

**Why only a whole-TU link works, even with tagged exceptions:**

- The conversions compile to anonymous `.sdata2` literals: the signed and
  unsigned int-to-double biases, plus 1.0f, 1.0 and 0.5 when they are
  written as literals. Retail keeps them in the TU pool
  lbl_8047CD08-lbl_8047CD28.
- Eight unlinked GSmsg objects reference that pool by name: F96E4 prefix,
  FA064 suffix, r47, FB43C, FB680 prefix, FBF74, FC7E0 suffix and FE010.
  The linked GSmsgInit unit also names lbl_8047CD08. A carve that owns
  the slice leaves those names undefined.
- Named stand-in definitions in a carve are not merged with the
  compiler's literals, so `.sdata2` doubles in size.
- A hand-written union conversion against a named bias compiles to
  `fsub; frsp`, not `fsubs`, and GC/1.3 has no `__fsubs` intrinsic.
- The only linkable object therefore owns `.text` 0x800F96E4-0x800FE35C
  and `.sdata2` 0x8047CD00-0x8047CD50. Its other data (rodata strings,
  `.data` lbl_80315678, `.bss` task records, `.sdata` lbl_80478B08) can
  stay extern, as the linked GSmsgInit unit already does.

**Pool order.** Retail's pool is `FFFFFFFF, FFFFFFFF, 1.0f, signed bias,
1.0, 0.5, unsigned bias, 2.0f, 0.5f, 0.4f, 8.0f, pi, 25.0f, 4.0f,
1/512`. MWCC emits named file-scope `.sdata2` constants first, then
literals in creation order. Within a function, front-end literals come
before code-generation literals, which is why 1.0 lands ahead of 0.5 and
both land ahead of the unsigned bias in `0.5 * (f64)x + 1.0`. The two
`FFFFFFFF` words are most likely two file-scope `const GXColor` whites.
CD00 is used by fn_800FC7E0 and CD04 by GSmsgSetColor. Across the TU,
first use runs fn_800F96E4 (1.0f), GSmsgAdjustAlign (signed bias),
then GSmsgSetFontInfo (1.0, 0.5, unsigned bias). An
IsCheck+GetRect-only compile emits `1.0f, 1.0, 0.5, unsigned, signed`.

**Remaining blockers.** Non-literal diff rows, raw objdiff, after
cdaf874a:

| Function | Rows | Note |
|---|---:|---|
| fn_800FB680 | 4 | entry `addi r0; mr r31,r0` copy of the work address; retail FB680/FB43C/FB8C8 materialize it straight into r31, FBB34 has the copy. It appears whenever `work` is set before the lookup inline (or a hand-expanded lookup), and not with a bare `if (key == 0) return` |
| fn_800FB43C | ~4 | same as fn_800FB680 |
| fn_800FB8C8 | ~8 | same copy, plus the right-align x arithmetic |
| fn_800F9C04 | 7 | |
| fn_800FBD88 | 11 | |
| fn_800F9AEC | 18 | |
| fn_800FD348 | 21 | |
| _msgGetSize__FPCUs | 37 | |
| fn_800F96E4 | 83 | dispatcher/reader expansion; register cascade |
| GSmsgInitRuby | 85 | register cascade |
| fn_800FAEF8 | 134 | register cascade |
| fn_800FC7E0 | 141 | XD `_msgMainSub`; saved-register cascade, frame 0x80 vs 0x7C |
| fn_800FD69C | 281 | glyph texture swizzle (XD `_msgMakeTexture`); register cascade |

XD evidence used so far: NXXJ01.map GSmsg.o (StarsMmd/Colo-XD-PBR-symbol-maps
6b51d3af), which lists dead-stripped `_msgCallCtrlFunc` (0x1D8),
`_msgGetNextCode` (0x58), `_msgGetMsgAddr` (0xA0) and `_msgInitTask`
(0x6C). The XD asm comes from trevor403/xd-asm b1087f18, and the XD
addresses from TeamOrre/xd-decomp symbols.txt at 4989794e. XD
GSmsgGetRect is `code/func_FUN_80107554.s` and XD GSmsgSetFontInfo is
`code/func_FUN_80107200.s`.

## Lane D9 round (2026-09-29)

Newly exact (whole-TU compile, msgdiff and canonical report): fn_800FD348,
fn_800FB43C, fn_800FB680, fn_800FB8C8, GSmsgInitRuby.

- **Print wrappers.** XD's GSmsgPrint2 and GSmsgPrintRight call
  GSmsgPrintRect (trevor403/xd-asm b1087f18, `func_FUN_80108464.s`,
  `func_FUN_80108494.s`); Colosseum inlines it into fn_800FB43C/FB680/FB8C8
  and keeps fn_800FBB34 as the out-of-line body. Only the inlined body
  materializes the work address straight into r31 (the out-of-line one
  copies it through r0, in both games). MWCC inlines `msgPrintRect` but calls
  its nested inlines out of line unless `#pragma always_inline on` is in
  force around the three callers (RULE-EXCEPTION, listed); `inline_depth`
  and `-inline auto,deferred` do not change that.
- **GSmsgInitRuby.** The saved mode is `(s8)arg0[0x45]` (a separate byte
  temporary, as retail's `lbz r4` / `extsb r21,r4`); with the one-vreg
  `*(s8*)` load the saved registers rotate. The resume stack is saved
  first, and the ruby height is computed before the counts are stored and
  written after the x offset (a local, `rubyHeight`).
- **fn_800FD348.** Local declaration order only.

Raised, not exact (rows are whole-TU msgdiff lines):

| Function | Before | Now | What moved it | What is left |
|---|---:|---:|---|---|
| fn_800FAEF8 (XD GSprint, `func_GSvtr_DrawText.s`) | 177 | 71 | XD `_msgSetChar` (`func_FUN_8010a144.s`, NXXJ01 0x2B4) as the inline `msgSetChar`, shared with fn_800FC7E0; set-up addressed through the buffer (`base + 0x5D0`), `work` only for the loop, which gives retail's base r31 / work r30 / colour r29; struct-typed glyph address `font + dataOffset + (offset & 0xFFFFFF)` | retail stores the task set-up through `mr r7,r30` (a surviving copy of work) except the first flag store, which goes through base+0x5D0; the string terminator is `stb 255(r4)` off the argument register rather than folded to 0x5CF. XD has the same shape. Not reproduced with inline params, return values, explicit copies or double definitions (sweeps in lane notes) |
| fn_800FC7E0 (XD `_msgMainSub`) | 180 | 122 | arg1/arg3 word-sized, cast at use (retail's clrlwi stays in the loop; the K&R `u8` form narrows once and hoists it); `normalFlag = 0` at entry; `msgSetChar` (gives retail's stack layout: fontNode 8, colour copy 12, colour 16) | one more saved GPR in retail (r19); saved-register cascade |
| fn_800FD69C (XD `_msgMakeTexture`, `func_FUN_8010a3f8.s`, near-identical registers) | 649 | 256 | `x + arg2 + 2 >= 0x200` and `atlasY += h + 2`; declaration-order hill-climb | the swizzle store: retail builds `stbx v, hi*32, (lo + image)`, MWCC canonicalizes every C form tried (array, pointer, 2-D array, integer, cast, pointer-variable, operand orders) to `stbx v, image, (lo + hi*32)` |
| fn_800F9AEC, fn_800F9C04 (XD GBAMakeFromGSchar / GScharMakeFromGBA) | 1 | 1 | — | retail's leaf `cmpwi 9; b default` has no `beq`. The front end already gives case 9 and default one label (`CASE 0x9: L@8`, `DEFAULT: L@8`), yet every GC compiler from 1.0 to 3.0a5.2, C or C++, any -O level, keeps the beq. XD's own tree (`func_FUN_8010643c.s`) drops the leaf beq the same way, while the pivots keep theirs |
