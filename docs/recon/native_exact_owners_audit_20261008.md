# Native Exact Owners: Independent Audit, 2026-10-08

Status: PASS for the two accepted owners and four shared-source sibling functions.

Auditor: CodexMenuMiddle. Checked 2026-10-08 at approximately 21:28 UTC.
Base HEAD: `7fe1001712fa82b3d9be5db53d9a6fe95970e97f`.
This audits the current **dirty working tree**, including the parent's Matching
changes, not an assertion that these changes are already in that commit.
No source, configuration, split, registry, or shared documentation was edited
by this audit. Main and the other agents' owners were left alone.

## Complete Object Comparison

Compared complete compiler objects under `build/GC6E01/src/` against the retail
split objects at the corresponding `build/GC6E01/obj/` paths, using the existing
ELF reader in `tools/decomp_work/permuter/owner_extract.py`. No function was
extracted or trimmed for this comparison.

| Object (relative to either object root) | Owned functions | Allocated bytes | Relocations | Result |
| --- | --- | ---: | ---: | --- |
| `game/toolentry.o` | `toolentryDebugPokemonCreate` | 692 | 44 | Exact |
| `game/menu/menu_middle_r48_8006B5D0_prefix.o` | `fn_8006B5D0` | 228 | 11 | Exact |
| `game/toolentry_candidate_8025D164.o` | `fn_8025D164` | 296 | 14 | Exact |
| `game/toolentry_candidate_8025D788.o` | Three functions below | 396 | 9 | Exact |

The three functions in the last wrapper are `toolentryCopyHero` (128 bytes),
`toolentryTaisenGetEntryPokemonNum` (148 bytes), and
`toolentryTaisenGetPokemonNum` (120 bytes). Together with `fn_8025D164`, these
are the four accepted siblings compiled by wrappers that include `toolentry.c`.

For all four objects:

- The complete set of nonempty `SHF_ALLOC` sections is exactly `.text`.
- Section size, every raw byte, type, flags, and alignment match; alignment is 4.
- Every allocated-section relocation matches after normalizing ELF symbol-table
  indices to symbol names: section, offset, relocation type, symbol, and addend.
- All defined function symbols match by name, section, offset, size, and binding.
- There are no extra emitted functions, inline-helper bodies, or allocated data.
  Nonallocated compiler/debug metadata and symbol-table ordering are not required
  to match and were not treated as game bytes.

Complete `.text` SHA-256 values, identical in source and target:

```text
toolentry                  17ab8f53062ca15bf84aee5551a66d29fbbec5a09069a632cc9e77b65560a655
menu_middle_r48_prefix     5d4edc5eb192d912288dfb1f0ffa9e4360083b4a7bc8aceef8b6702aa969e452
toolentry_8025D164          aa6ce4d2462096c8ea03b51cb2d826f062802ae6c0ccb4a148ca6173c8487841
toolentry_8025D788          33a851b979abb0a9079bbd16ab407edd454771e67039983afcc171a1d649444a
```

## Build, Link, and Policy

All builds and verification commands ran through
`python3 tools/local_campaign.py build --worker CodexMenuMiddle -- COMMAND`.

1. `ninja all_source build/GC6E01/report.json`: PASS. The refreshed report marks
   all four audited units `complete: true`, with 100% function and code matches.
2. Whole-object audit above: PASS, repeated after the link with unchanged scoped
   source fingerprints. Local reproducibility artifacts are the ignored
   `build/native-exact-owners-audit-20261008.py` and matching `.json`.
3. `ninja`: PASS; rebuilt `main.elf`, `main.dol`, REL outputs, and completed
   `CHECK config/GC6E01/build.sha1` with **7 files OK**. The generated main link
   rule selects the compiled `src/` objects for the audited owners, not their
   retail `obj/` replacements. Canonical outputs checked: main, common_rel,
   mail, S1_out, pocket_menu, worldmap_menu_rel, and S1_shop_1F.
4. Current `quality_scan.py` API against `git diff --unified=0 HEAD` for the five
   scoped source files below: PASS. This checked working-tree added lines, not
   an empty HEAD-to-HEAD diff.
5. `python3 .github/scripts/check_asm_wrappers.py`: PASS, 2,315 source files,
   no live `.inc` assembly wrappers.
6. `python3 .github/scripts/test_quality_scan.py`: PASS, 41 tests.
7. `python3 .github/scripts/check_metric_integrity.py build/GC6E01/report.json
   build/GC6E01/main.dol`: PASS, 8,620 unique function addresses with no duplicate
   ownership; code denominator 2,499,424 versus DOL text 2,503,264 (0.15%).

Existing compiler Shift-JIS/uninitialized-variable warnings and linker
environment/floating-point-setting warnings remain; neither build failed.

`configure.py` marks both newly accepted owners Matching. The toolentry owner
uses `-DTOOLENTRY_DEBUG_ONLY` and `-schedule on`. Under that guard,
`toolentryTaisenFreePokemonData` is static inline: its body remains available
to the caller without emitting a duplicate standalone release function.
The complete-object check confirms this property rather than merely trusting
the preprocessor guard.

Toolentry remains accepted under the **existing documented byte-first shaping
exceptions**, not a claim to have recovered the original source organization:
the hero-copy inline boundary, empty six-iteration loop, one-use creation helper,
and loop-index reuse are recorded in `docs/RULE_EXCEPTIONS.md` and
`docs/recon/toolentry_debug_pokemon_residual_20261008.md`. No assembly or new
policy exception was introduced. The registry rows still say "link validation
pending"; the independent link above resolves that factual check for this
snapshot. The shared registry was intentionally not edited.

The middle owner uses the typed C loop in
`src/game/menu/menu_middle_range_8006B5D0.c`, included by its prefix wrapper;
there is no added assembly or matching-only compiler pragma in that change.

## Source Fingerprints and Scope

SHA-256 of audited source files, stable across the object audit and link:

```text
src/game/toolentry.c
  3b414754da5509800b2b7906e6ccf7b3658ff3a9d34638b87c0286dcbf9ec029
src/game/menu/menu_middle_range_8006B5D0.c
  7e1fdb5f85542bb33c8a4054b71090f9969b70da9a75fb99cc2898ac6755ded3
src/game/menu/menu_middle_r48_8006B5D0_prefix.c
  c26ed4cb927ece0202c4a71ad2670eb6abb2e86978b9d73481883f2d1bae6248
src/game/toolentry_candidate_8025D164.c
  9548fb6a85716788cc8de936ef0e8837243e71ee0046ade1962ea07207a5ce70
src/game/toolentry_candidate_8025D788.c
  f7fc3f4c302655408bc9f3d771bf28719ee9b0511ab00e686219d4e8cd05b53f
```

This is owner-level decomp acceptance evidence, not a native binding test or
a new transitive-closure audit. Other toolentry candidate units are not promoted
by these results. Changes to the audited inputs require revalidation; no commit
or push was performed.
