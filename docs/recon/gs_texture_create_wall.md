# GStextureCreate matching wall

`GStextureCreate` (`0x800EF5FC`, `0x718` bytes) is the remaining function in
`main/game/gs_texture_candidate_800EF5FC`. The 2026-09-28 objdiff report
scores it at 95.40308%; the unit is a `CodeCandidate`, so it contributes no
linked source. The current candidate and retail function both contain 460
instructions.

The current source implements the observed validation, format selection,
texture-pool search, allocation, mip layout and GX object initialization. The
instruction diff shows no obvious missing call or branch. Its differences are
concentrated in register lifetime and the instruction order that follows:

- Retail copies width and height from `r3`/`r4` to `r9`/`r8` at instructions
  2–3. The candidate keeps them in their incoming registers; the display-size
  path consequently loads the two fields into different registers.
- Retail keeps the adjusted height in `r26`, the incremented mip count in
  `r29`, and the free texture slot in `r30`. The candidate uses `r29`, `r26`
  and `r31` respectively. This affects the pool walk and most texture-field
  accesses through the return.
- At retail instructions 175–185, the pixel-count multiplication and mip-count
  increment occur in a different order from the candidate. Later, retail
  derives the stored mip count from its saved `r29` value; the candidate
  recomputes it from `r26` near instruction 291.
- Retail uses `r31` for the GX TLUT format at the tail; the candidate uses
  `r30`. The source header records earlier helper-form and declaration-order
  experiments that did not resolve this conflict.

The candidate source header records 178 semantics-preserving rewrites and 150
declaration permutations already tried on 2026-09-28, with no exact result.
An additional controlled check changed the public and candidate width/height
parameters to `u16`, as suggested by several callers and the archived old
candidate. The current compiler emitted 459 rather than 460 instructions and
the canonical score fell to 95.27093%; the retail entry's `mr r9,r3` /
`mr r8,r4` copies were still absent. Restoring `s32` parameters restored the
baseline. Reordering the two display-dimension loads also fell slightly to
95.37445%. Moving the shared GX helper's TLUT-format declaration before its
other locals left this function at 95.40308% and kept `GStextureLoad` exact,
so declaration order alone does not explain the `r30`/`r31` conflict. All
three probes were reverted; the active function and shared header remain as
before.

The `u16` probe specifically rules out a parameter-width-only explanation,
not a source-level copy into mutable width/height locals. No signature change
is accepted from the archived candidate without a measured match, and the
first live recomp call to this function therefore remains blocked.

This audit found no source-faithful correction. A future change needs evidence
for a genuine source-level lifetime or computation difference and must be
checked with the full candidate object and `build/GC6E01/report.json`; a
register-only rearrangement is not an accepted decompilation win.

An additional 2026-09-29 check compared the 460 instruction rows through the
shared build lock. Changing `mipLevels++` to `++mipLevels` left the canonical
score at 95.40308%. Expanding the source-backed `textureInitGXObjects` helper
directly into this candidate, with the same GX format/TLUT decisions and field
writes, reduced it to 94.94053%. Reversing the display-default assignments to
load width before height (as retail does) reduced it to 95.37445%. These
experiments were reverted. In particular, the latter result shows that the
isolated display-load order is coupled to wider register allocation; matching
those two instructions alone does not improve the object.

A further 2026-09-29 check tested the mip-count lifetime directly. Naming the
computed `mipLevels + 1` as `totalLevels`, then using it for both the size loop
and texture fields, scored only 93.07929% (461 aligned instructions). The
candidate still held the incoming mip count in `r26`, not retail's `r29`, and
emitted its increment later than retail. Inlining the pool scan into the
function body with an explicit remaining-count check scored 94.552864% (463
aligned instructions); it introduced a loop index and an extra end comparison
absent from retail. Both source-level probes were reverted.

The first instruction-level divergence remains at retail rows 2–3:
`mr r9,r3; mr r8,r4` before the saved-register prologue, with no candidate
counterpart. The first post-pool scheduling divergence is retail row 175:
`mullw r28,r27,r26` (adjusted width times adjusted height), followed by
`clrlwi r3,r29,24` and the bits-per-pixel load before `addi r29,r3,1`.
The candidate instead increments a temporary from saved `r26`, computes the
pixel count later, and re-derives the incremented mip count after allocation.
This is not evidence for a safe register-only edit: the opening copies and
long-lived values need a source-level explanation that also keeps the pool
walk, allocation calls, GX tail, and object sections paired. A useful next
experiment is a controlled front-end/lifetime comparison against a verified
sister-title definition or contemporary GStexture source, if one can be found;
do not import a guessed signature or a pure-copy temporary solely to obtain
the two entry `mr` instructions.

On 2026-09-29 a further guarded source-order check moved the first
`tex->bitsPerPixel * pixelCount / 8` computation ahead of the
`tex->totalSize = 0` store, testing both before and after `mipLevels++`.
Both forms compiled and scored 95.17181%, below the 95.40308% baseline.
They did move the pixel-count `mullw` into retail row 175, but MWCC still
allocated adjusted height to `r29` instead of retail `r26`, kept the mip
parameter in `r26` instead of retail `r29`, and advanced the count before
loading the bits-per-pixel byte. The source and object were restored. This
isolates the row-175 scheduling difference from the longer-lived register
conflict; merely moving a side-effect-free calculation cannot resolve it.

A source search of TeamOrre's Pokémon XD decomp at commit
[`4989794e6c6430684e033bc56f4bb97c9a921e73`](https://github.com/TeamOrre/xd-decomp/tree/4989794e6c6430684e033bc56f4bb97c9a921e73)
found only Dolphin GD/GX texture files, not a GStextureCreate sister-title
definition. The archived Colosseum campaign candidate is not independent
evidence: it uses different, demonstrably mismatching layout and mip-size
semantics. No external source has yet justified altering the signature or
creating extra width/height copies solely for register placement.

## 2026-09-29 owner audit

The canonical guarded report still gives `GStextureCreate` 95.40308% over
1,816 bytes and keeps its one-function object unlinked (`complete_units = 0`).
The current owner source SHA-256 is
`f83f9c394ffe9fb83c4b06be27cff99080332b7466b112e0b66529bc8d84e463`;
no source edit survived this audit. The built `main.dol` SHA-1 is the expected
`870e8b9693ca780782d80f22a6a4572d8ba9458f`. These hashes describe the
current shared tree, not a newly linked texture object.

The retained instruction diff again identifies the opening absent `mr r9,r3`
and `mr r8,r4`, adjusted-height/mip-count/free-slot register lifetime, and
post-pool multiplication order. Its retail and candidate paths both have the
format-validation, allocation, GX-init and error branches; a guessed pure-copy
local would be compiler shaping, not evidence of original behavior. A GitHub
code search for `GStextureCreate` found only this Colosseum repository, and
the broader web search found no independently sourced definition. The local
MWCC register replay (`local_campaign.py explain`) was unavailable because its
optional debugger tools are missing or the replay failed. No justified
source-level correction emerged, so the exact and linked deltas are both zero.

## 2026-09-29 guarded recheck

`Codex-texture-create` claimed the single-function candidate owner and read the
current aligned objdiff. The first two target instructions remain the absent
width/height copies; the adjusted-height, mip-count and free-slot register
lifetimes continue to differ. No new source-backed format case, allocation
branch or GX call was missing. The optional register replay still reports
unavailable. A further exact-name search did not produce an independent
GStextureCreate definition; copying arguments or moving pure arithmetic only
to manipulate registers would not satisfy the source-faithfulness rule.

The guarded `configure.py --no-progress`, `ninja -j2 all_source
build/GC6E01/report.json`, `ninja -j2` and `configure.py progress` all pass
after the unrelated FSYS owner restored a transient C89 declaration error.
Both `build/GC6E01/main.dol` and `common_rel.rel` pass
`shasum -a 1 -c config/GC6E01/build.sha1`. The refreshed report still records
`GStextureCreate` at 95.40308% over 1,816 bytes with `complete: false`; its
source SHA-256 remains
`f83f9c394ffe9fb83c4b06be27cff99080332b7466b112e0b66529bc8d84e463`.
No candidate edit or exact/linked promotion is claimed. The concrete next
evidence needed is an independently sourced contemporary implementation or a
working compiler register replay that explains the entry copies and the
long-lived register order without a pure code-generation rewrite.

The subsequent whole-owner `Codex-texture-audit` pass compared the current
candidate with title and message callsites as well as the exact shared
`textureInitGXObjects` helper. `GSmsgInit` requests two 0x200-by-0x200
textures in format 0x40, while title callers request a display-sized
format-0x44 texture with zero dimensions. These calls support the existing
zero-dimension fallback and GX initialization boundary, but their differing
local prototypes do not establish a new parameter type or original local
lifetime. The guarded objdiff remains 460 retail and 460 candidate
instructions with the entry copies absent and the same post-pool scheduling
gap. No source change or promotion follows from this evidence.

The later `texture-boot-decomp` audit rechecked that aligned objdiff and
the canonical report: 95.40308% across 1,816 retail bytes, 460 instructions
on both sides, `complete: false`, and no linked object. It did not find a
missing source-level format case, branch, call, or field write that explains
the remaining deltas. The optional allocator replay still cannot run here:
its `gdb.py` stand-in and `retrowin32` binary are absent from
`build/tools/mwdbg`, and there is no local `gdb`. The public
`cadmic/mwcc-debugger` source alone does not provide that stand-in. Its
source and the relevant `encounter/retrowin32` fork were fetched only into
ignored `build/reference/` for inspection; neither changed compiler inputs
or tracked source. A guarded `ninja -j2 all_source build/GC6E01/report.json`,
full `ninja -j2`, and `configure.py progress` passed; both retail SHA-1 checks
passed. The candidate source SHA-256 remains
`f83f9c394ffe9fb83c4b06be27cff99080332b7466b112e0b66529bc8d84e463`.
This remains an unlinked texture-creation blocker, not accepted progress.

## 2026-09-29 compiler replay compatibility check

`Codex-texture-next` claimed the candidate owner, kept its source unchanged, and
compiled that exact source into temporary objects under the shared build lock
using each locally available MWCC version with the active unit's command-line
flags. Objdiff compared each temporary `GStextureCreate` with the canonical
GC/1.3-built candidate object (not with retail):

| MWCC version | Match to GC/1.3 candidate | Function size |
| --- | ---: | ---: |
| GC/1.1 | 62.60486% | 1,892 bytes |
| GC/1.3, 1.3.2, 1.3.2r, 2.0, 2.0p1 | 100.0% | 1,812 bytes |
| GC/2.5, 2.6, 2.7 | 96.84327% | 1,812 bytes |

This changes the register-replay next step: the current optional debugger
supports only GC/1.1 and GC/2.6. Neither reproduces this object's actual
compiler output, so installing only its missing `gdb.py`/`retrowin32` pieces
would still leave the replay **unfaithful** for `GStextureCreate`. A trustworthy
allocator trace needs debugger support for one of GC/1.3 through GC/2.0p1,
or independent source evidence explaining the entry copies and live-range
ordering. The `GC/2.6` result may still be useful for hypotheses, but must
not be treated as authoritative for a source or promotion decision.

The canonical report remains 95.40308% over 1,816 retail bytes, with
`metadata.complete: false` and zero newly linked source. Candidate SHA-256
remains `f83f9c394ffe9fb83c4b06be27cff99080332b7466b112e0b66529bc8d84e463`.
No source change or exact/linked promotion is claimed.

## 2026-09-29 lane D3: 95.40 → 97.68

The canonical report now gives 97.67621% over 1,816 bytes. The object is
still an unlinked CodeCandidate. Changes and what each fixed:

- **Entry copies.** Writing the size-adjust log arguments as
  `width & 0xFFFF, height & 0xFFFF` (was `(u16)width, (u16)height`) makes
  MWCC build the format string in r3 before it computes the two
  arguments. Width and height are still live at that point, so they
  cannot be coalesced into r3/r4, and retail's `mr r9,r3; mr r8,r4` appear
  (95.40 → 96.99). Loading width before height in the display default then
  matches (97.04).
- **Mip count.** An int `levels = mipLevels + 1`, read through `(u8)`,
  gives retail's `clrlwi r3,rN,24; addi rN,r3,1` (in-place, untruncated).
  `mipLevels++` on the u8 parameter instead makes MWCC rematerialize
  `mip + 1` at each use.
- **Declaration order matters here**, contrary to the earlier 150-permutation
  result, once `levels` exists. A sampled search set adjHeight, adjWidth and
  pixelCount to retail's r26/r27/r28.
- **tex/TLUT r30/r31.** With the pool scan in the `textureFindFree` inline
  (XD `texFindFreeTexture__Fv`, UNUSED 0x30), the returned pointer gets r31
  and the GX helper's uninitialized TLUT-format local gets r30. With the scan
  written in place (`goto found`), retail's TLUT r31 / tex r30 comes out, and
  so does every `tex->` access. With `tex = texFindExt()` as an external
  call, TLUT also gets r31, so the inline's result is what outranks it.

Remaining 23 rows:

- Retail keeps the mip parameter and the incremented count in one saved
  register (r29 from `mr r29,r7`). Here the parameter is r28 and `levels`
  is r29. That displaces the pixel-count `mullw` (retail computes it before
  the increment) and swaps h/maxLevels (r4/r5) in the w/h loop.
- The in-place scan emits `beq found`; retail has the inline-return shape
  `bne next; b found`. A separate `slot` variable restores that shape but
  leaves `slot` in r3 with a copy.

Tried without gain:

- In-place increment of an int parameter (header prototype hidden;
  `mipLevels = (u8)mipLevels + 1`). This drops mip to r26.
- An entry copy `levels = mipLevels`, which outranks tex.
- A u8 `levels`.
- K&R definitions.
- Clamp-output forms, including the ternary.
- Four findFree loop forms × 100 declaration orders.
- `register`/`volatile`, and permuting the GX helper's declarations (no
  effect).
- An auto-inlined non-static GX helper (not inlined).
- Four maxLevels loop forms.

`GStextureLoad` stays exact throughout. The goto and the `levels` local are
research forms. A 100% result through them would need RULE-EXCEPTION rows
(goto/label shaping; int local read through casts).
