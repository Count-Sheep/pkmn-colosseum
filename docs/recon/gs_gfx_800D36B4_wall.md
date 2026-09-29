# `fn_800D36B4` fog-color setter

This 0xC8-byte function remains a 66.16% fuzzy `CodeCandidate` in the
`gs_gfx.c` owner, not linked progress. Retail loads the scale constant,
enable value, and first input float before its 0x30-byte stack frame; the
current GC/1.3 C build creates the frame first. Its first ~40 instructions
also interleave the four input reads, float-to-byte conversions, and
fog-enable write differently. The final four-byte zero test and flag clear
already align. `docs/FUZZY_TARGET_FEASIBILITY.md` records prior natural
rewrite trials and why the old local peephole pragma was removed.

In the 2026-09-28 revisit, replacing raw byte offsets with the repository's
typed `GSRenderState` fog fields compiled identically (66.16%) and was
reverted. A controlled `-O4,s` compile likewise measured 66.16%, with the
same frame-before-load ordering; it was diagnostic only and no flag change
was retained. These checks found no new behavior or data-flow evidence for
an admissible source edit. The pre-existing source and unrelated dirty work
were preserved.

Shared-lock `all_source`/report and full `ninja -j2` passed. Retail SHA-1
checks passed for `main.dol` and `common_rel.rel`, neither of which links
this candidate carve. Do not promote it on fuzzy score or restore a local
compiler-control pragma as an acceptance shortcut.

Another 2026-09-28 check separated the first input read into its own `f32`
local before conversion. The locked build still produced the same 66.16%
objdiff and the same frame-first instruction sequence, so the source edit was
reverted. Retail reads inputs 0, 1, and 2 before writing the fog-enable byte,
then reads input 3 after that write. Since the input pointer can in principle
alias the output state, merely moving the fourth read across the write to
force an instruction match is not a semantics-preserving C rewrite. The
remaining mismatch needs evidence for the actual source expression/compiler
shape, not a declaration-order tweak.

On 2026-09-29, the `fn_801123D4` callsite in `floor.c` was checked: it passes a
four-`f32` `FloorColor` aggregate by value, although this candidate declares
the callee parameter as `f32*`. A matching aggregate parameter compiled to the
same 66.16% body, so the discrepancy does not explain the current scheduling
wall. With that aggregate parameter, setting the enable flag before the four
conversions raised the fuzzy score only to 66.36%; putting the enable write
between the third and fourth conversions (the apparent retail load boundary)
regressed to 59.78%. Direct per-component conversion/stores scored 52.02%.
These were locked, source-backed diagnostic builds only. The baseline
parameter/body were restored; none of the variants is an exact or linked
function. The retail load/write ordering still needs stronger source evidence.

Later 2026-09-29 callsite audit: the only direct `bl fn_800D36B4` in the
assembled game sources is `floor.c` at 0x8011243C. It copies the four floats
of `sFloorFadeColor` into a fresh 16-byte caller-stack argument at `r1+0x8`
and passes its address in `r3`. This confirms an aggregate-by-value ABI for
that callsite, but it does not establish every possible indirect caller or
prove that the global graphics-state pointer cannot alias the input pointer.
The already-tested aggregate parameter produced the same 66.16% callee body.
Retail schedules the scale load, `li 1`, and first input load before frame
allocation, then reads input components 0-2 before the enable write and
component 3 afterward. The baseline candidate allocates its frame first and
reads component 3 before the enable write. This follow-up found no new
source/data-flow fact that justifies moving a read across that write or
changing optimization controls. Source and link status remain unchanged.

## Lane D2 pass (2026-09-29): sibling solved, standalone still a wall

- **Same TU as fn_800D2B90.** Only these two functions reference the pooled
  255.0f at 0x8047C9F0, so they come from one translation unit.
  fn_800D2B90 expands this setter inline, with the colour passed by value:
  retail copies it to a stack temporary. fn_800D2B90 is now exact with a
  `static inline gsSetFogColor(GSRenderColor)` whose body is `enable = 1;
  byte = 255.0f * color.x; ...; zero test` (the literal scale and direct
  float-to-u8 stores). See gs_render_800D2B90_round2.md.
- **Why the standalone differs.** Retail's standalone schedules the scale
  load, `li 1` and the first parameter load ahead of `stwu`, and reads
  components 0-2 before the enable store. That needs the parameter loads to
  be free of the byte stores and the frame store, as the stack copy is in
  fn_800D2B90. Every standalone form MWCC accepts lowers the by-value
  parameter to an implicit pointer (frontend `*(color)`), so its loads alias
  the stores: C or C++, pointer or by-value, literal or extern scale,
  enable-first or e1 order (components 0-2, enable, component 3), u8, s32
  or f32 temporaries, all enable positions. The best reach 23-24 differing
  instructions (the enable-last and e1 orders; objdiff about 66%).
- A wrapper `fn_800D36B4(c) { gsSetFogColor(c); }` gets the free
  scheduling but materialises retail-absent struct copies (43 differences
  on GC/1.3). Pragmas `peephole off`, `scheduling off/601/602/603/604/740/
  750/7400/7450/8240`, `optimization_level 1-3`, `optimize_for_size` and
  `opt_pointer_analysis[_mode]` do not reproduce it; `scheduling 821/850`
  hang the compiler.
- **Link route.** A lone fn_800D2B90 carve cannot own 0x8047C9F0 (this
  function's retail object needs the name lbl_8047C9F0). Once this is exact,
  0x800D2B90-0x800D377C can link as one object with its .sdata2 pool
  entries.

## Resolution (2026-09-29, lane D2): exact and linked

The compiler was the missing piece. Under GC/2.5-2.7, the by-value form
(`void fn_800D36B4(GSRenderColor color) { enable = 1; byte = 255.0f *
color.x; ...; zero test }`, with the TU's 255.0f literal) is byte-exact:
those versions schedule the parameter loads ahead of `stwu` and the enable
store, and GC/1.0-2.0 and 3.0 do not. Every other function from
0x800D2B90 to 0x800D377C is identical under GC/1.3 and GC/2.5, and the
.sdata2 entries 0x8047C9F0-0x8047CA10 (255.0f, the unsigned int-to-float
bias, 0.0f, 640.0f, 480.0f, 1.0f) are used only by those functions. So the
span now links as one GC/2.5 object, game/gs_gfx_range_800D2B90.c, that
owns the pool as literals. It replaces eight carves and candidates and
their extern stand-ins. The retail DOL/REL SHA-1 check passes.
