# Function naming scheme (GC6E01)

Status: **approved by the user, 2026-10-20**, together with the AGENTS.md
amendment in section 7. Decisions recorded at approval: inferred names are
marked with a comment and a CSV row (no name suffix); game T3 gets a 10%
spot check; parked functions get a second-pass T3 name (section 4); the CSV
is backfilled from git history for earlier unlogged renames, as rows marked
`historical`. The coordinator is the Opus orchestration session; the user
decides human-review rows.

Scope: the 3,636 `fn_80xxxxxx` functions in `config/GC6E01/symbols.txt` at
`436fe656a`. The REL modules have no `fn_` functions. Each function is renamed
exactly once into the best name the evidence supports. It is renamed again
only when stronger evidence turns up later (an upgrade, section 4).

Ledger: `docs/function_renames.csv` (section 5). Checker:
`tools/check_naming.py` (section 8).

## 1. Evidence tiers

Use the highest tier the evidence reaches. A name never claims a higher tier
than its evidence. The `evidence_ref` forms are fixed, so the checker can parse
them.

### T1: original name

The exact spelling the original developers used, from an authoritative source.
Qualifying sources, strongest first:

| Kind | Qualifies when | `evidence_ref` |
|---|---|---|
| `map` | The function lines up with a **live** or `UNUSED` entry of the XD JP demo's own linker map `NXXJ01.map` (StarsMmd/Colo-XD-PBR-symbol-maps @ `6b51d3af`), as aligned by `tools/xd_map_xref.py`. It must sit in the same object, in the same order between two anchors, with equal size and a retail body similarity of at least 0.95, or with a shared rare string. A `size`-only row is **not** enough. | `map:NXXJ01:<object.o>:<name>` |
| `self` | A string in this DOL names the function itself, and that string is referenced **only** by this function. The name must be the string's subject: `name:`, `name()`, `[name]` or `name(0x..)` at the start, or a `__FUNCTION__` object. No other symbol already carries the name. A string that names a callee (`"GSfileOpen failed"`) does not qualify. | `str:0x8xxxxxxx` |
| `lib` | Library code (`src/dolphin`, `src/hsd`, `src/musyx`, `src/trk`, `src/crt`) whose name appears in a reference source **that got its names from symbols**: the Dolphin SDK debug libraries, a linker map (`NXXJ01.map` also covers the SDK, MusyX and HSD objects), or a decompilation that documents map or symbol provenance. The body must match the reference function. | `lib:<owner/repo>@<commit>:<path>:<name>` |
| `xd` | A name from XD retail (TeamOrre/xd-decomp `config/GXXE01/symbols.txt` @ `4989794e`). It counts only when the same name is also in `NXXJ01.map`, which proves XD's name original, and the bodies are identical apart from register and address differences. They must also share a rare string (fan-out ≤ 2), or have the same position in the map object. | `xd:0x80xxxxxx+str:0x8xxxxxxx`, or `xd:0x80xxxxxx+map:<object.o>` |

Not T1, however confident it looks:
- `__FILE__` literals. They prove the unit and its prefix, never the name.
- The other StarsMmd maps, such as `GC6E01 official only.map`. They are noisy
  ports of `NXXJ01.map`.
- XD retail names that are absent from `NXXJ01.map`.
- `structural_ports.json` fingerprint matches.
- `config/GC6E01/subsystem_map.json` labels.

C++ names: a T1 name keeps the exact form its source shows. If the map shows
`_menuFightIsUse__FP16MENU_WAZA_STATUSUs`, that mangled string is the name.
Write the source the way the house already does: the mangled spelling as a
plain C identifier, with the same parameter types the mangling encodes. Never
demangle a name. Never shorten a mangled name to its base. Never add or
change mangling.

Record: the tier, the kind, the `evidence_ref`, and a one-line `evidence`
(for example `NXXJ01 camera.o, between cameraSetMirrorFlag/cameraSetOffsetScale, size 0x24, sim 0.98`).

### T2: reconstructed from proven siblings

The name follows a naming pattern shown by T1 names in **the same unit or the
same map object**. Minimum evidence:
1. At least **two** T1 (or pre-existing, unchallenged) siblings that show the
   pattern. Examples: an `Init`/`Main`/`Exit` triple with one member missing;
   a `…BiosGetX`/`…BiosSetX` accessor pair; a numbered family such as
   `fightTrainerAiWazaDamage081`.
2. Structural proof that the function fills the gap. It sits in the same
   callback table or vtable slot row as its siblings, or its body has the
   same shape and differs only in the varying constant or field.
3. The varying part comes from a fact, not a guess. It can be the table index
   (zero-padded like the siblings), the field name a sibling already uses for
   the same offset, or a string.

T2 never produces a mangled name. If the siblings are mangled, the function
gets T3 under section 2, or waits for T1.

Record: `evidence_ref` = `pattern:<sibling1>,<sibling2>[,…]`, and an
`evidence` line that states the slot or derivation.

### T3: context name

T3 is a descriptive name under section 2's grammar. It is the fallback once T1
and T2 have been checked and missed. Minimum evidence: the function has been
**read**. The name must be supported by at least one concrete fact that the
`evidence` column records: a caller or callee name, a string it uses, a struct
field or flag it reads or writes, a table it is registered in, or its return
value contract.

Record: `evidence_ref` = `ctx:<fact>[;<fact>]`. Facts look like
`calls:GSfileOpen`, `str:0x8027...`, `field:FIGHT_TRAINER+0x1C`,
`table:lbl_802E1234[3]` and `caller:menuShopMain`.

## 2. Context-name (T3) syntax

Grammar: `<prefix><Token>+`, lowerCamelCase with no underscores. For example
`menuShopDrawCursor`, `fightOutPokemonCheckSleep` and `GSmodelGetJointCount`.

1. **Prefix.** Take the first token that applies:
   1. the subsystem stem shared by the T1, T2 and pre-existing names in the
      same `splits.txt` unit, such as `menuShop` or `fightTrainerAi`;
   2. the stem of the `NXXJ01.map` object the function aligns to (`camera.o`
      gives `camera`);
   3. the stem implied by a `__FILE__` literal the unit references, written
      the way proven names already write that subsystem (`menuCB_Rule.c`
      gives `menuRule`, only if a proven `menuRule…` name exists);
   4. the prefix of the proven callers, if every caller is in one subsystem.

   **Mechanical check:** the prefix's first token must already start at least
   three existing non-`fn_` function names, as in `menu`, `fight`, `GSmodel`,
   `HSD_` (library only) or `pokemon`. A new prefix needs coordinator
   approval. If no rule applies, the function is **parked** (section 4), not
   named.
2. **Verb.** The name must contain at least one verb token from this list,
   which is taken from the existing names: `Init Exit Term Create Delete
   Destroy Alloc Free Release Open Close Main Ctrl Proc Exec Update Draw Disp
   Render Set Get Is Has Check Calc Conv Make Setup Load Read Write Save Clear
   Reset Start Stop Wait Play Add Remove Erase Push Pop Find Search Select Sort
   Copy Swap Cmp Compare Count Move Enable Disable Entry Regist Change Apply
   Begin End Inc Dec Callback CB Handler`. The house order is
   prefix + object + verb + object (`heroItemDecItemDataId`,
   `pokemonBiosGetHp`). Use `Bios` only for plain field accessors, as the
   house does.
3. **Nouns.** Use the vocabulary that existing names already use for the same
   concept, including romanized Japanese (`Waza`, `Tokusei`, `Kinomi`,
   `Joutai`, `Soubi`). Do not translate it into English.
4. **Banned tokens:** `Unk`, `Unknown`, `Misc`, `Helper`, `Thing`, `Stuff`,
   `Temp`, `Tmp`, `Fn`, `Func` followed by a digit, `Dummy` (except for a
   genuinely empty body), any run of 6 or more hex digits, `_8xxxxxxx` address
   suffixes, and a leading `_`. A leading underscore marks an original static
   in this codebase, so T3 may not claim it.
5. **Length:** at most **40** characters. The 95th percentile of existing
   names is 40, the median 22.
6. **Collisions:** the name must not already exist in any module's
   `symbols.txt`. It must not equal any name in
   `config/GC6E01/symbolmap/xd_vocabulary.json`, because an XD name on the
   wrong function would later block or fake a T1. If the natural name is
   taken, add a distinguishing object token (`…ByIndex`, `…Sub`); never add a
   counter or address.
7. **Static versus global:** a rename never changes linkage. `static` in the
   source and `scope:` in `symbols.txt` stay exactly as they are. Several
   statics in different units may share a name only for T1 (as `OnReset` and
   `cb` already do). Every T2 and T3 name is unique across all modules.
8. **Library units** (`src/dolphin`, `hsd`, `musyx`, `trk`, `crt`): T3 is
   allowed only after `lib` and `map` were checked and missed. The `evidence`
   field must say so (`lib-miss:<repo>@<commit>`). The name uses the
   library's own convention, such as `HSD_JObj…`, `__OS…` or `snd…`. These rows
   always need coordinator review (section 6).

**Inferred-name marking: recommendation.** Mark the tier with a source comment
and the CSV, never with a name suffix.
- A suffix such as `_inferred` or `_T3` pollutes every call site and every
  native-port reference. It would have to be renamed again when a T1 turns up,
  and it breaks the house style that the original names define.
- The CSV is already the authoritative ledger. One comment at the definition
  puts the same fact where a reader sees it.

The tag goes on the line or lines immediately above every definition of the
function (within 4 lines), in every source file that defines it:

```c
/* NAME-T3 fn_8001ABCD */
static void menuShopDrawCursor(MenuShopWork* work)
```

The format is exactly `NAME-T<1|2|3> fn_<8 hex digits>`. The address token is
deliberately the only place the old `fn_` spelling survives, so
`grep -rn "NAME-T3" src` lists every inferred name. A tag outside a comment is
forbidden, and so is a tag inside a string.

## 3. Byte-exactness

**Why a rename cannot change the shipped bytes:**
- `main.dol` has no symbol table.
- Each REL resolves its references through its import table. That table is a
  list of module ids (here, itself and `main`), each with relocation entries
  of `(offset, type, section, addend)`, and contains no names. dtk's
  `rel make` resolves names against the linked main ELF **at build time**
  only, which `REL_M1.md` documents ("each module imports only itself and
  main").
- Code refers to functions only through relocations, and the linker turns
  those into addresses.
- Name length and order change only the `.symtab` and `.strtab` of the
  intermediate `.o` files, never a linked byte.
- MWCC's anonymous `@NNNN` numbering and its emission order do not depend on
  identifier spelling.

**Where a name does reach the bytes. The checker enforces all of these:**
1. **`__FUNCTION__` / `__func__`.** The name becomes `.data`. This is used in
   `src/game/menu.c` and `src/game/people/people.c`. A function whose body
   uses either has already been proven by its string, and renaming it is
   **forbidden**.
2. **Stringized assert conditions.** `HSD_ASSERT(line, cond)` and
   `HSD_ASSERTREPORT` emit `#cond` as `.rodata`. No function whose name
   appears inside an `ASSERT` invocation may be renamed.
3. **String literals.** No rename may edit any string literal. Assert and log
   strings and `__FILE__` names are data, even when they spell the function's
   name. `tools/naming/rename_symbol.py` replaces whole words everywhere,
   including inside strings, so the worker must use a literal-aware rename. If
   it does not, the checker's `--base` literal diff catches it.
4. **Force-active lists.** `configure.py`'s `force_active_symbols` has 182
   `fn_` entries. If the old name stays, the linker strips the function, and
   both the SHA-1 and the link fail.

**The live reference set is updated in the same commit:**
- `config/GC6E01/symbols.txt` (and any module `symbols.txt`).
- `src/**` and `include/**`: code, prototypes, `extern`s, function-pointer
  table initializers, and comments.
- `configure.py`.
- `docs/RULE_EXCEPTIONS.md`.
- `docs/asm_evidence/registry.json` (including `branch_targets`) and the
  evidence documents it cites.
- `.github/regression_policy_cleanup.json`.
- `config/GC6E01/subsystem_map.json`.

**Never edited by a rename:**
- Unit and file names (`OSEXI_fn_8009E7A8.c`, `*_range_*`, `splits.txt`
  units, `object_map.freeze.json`).
- History docs (`docs/recon/**`, handoffs, `archive/`).
- `build/`.

The CSV is the bridge from those old references.

**objdiff and the regression check match by name.**
`.github/scripts/check_regression.py` treats a name missing from the head as
unchanged (`head.get(n, base[n])`). A renamed function that regressed would
therefore pass silently. Every rename batch remaps the base report first:
`tools/check_naming.py --remap-report base.json base.remapped.json`, then
`check_regression.py base.remapped.json head.json`.

**Required gate per batch.** All of these must pass on a checkout that has
`orig/`:
1. `python3 tools/check_naming.py --base <batch base commit>`
2. `python configure.py --no-progress && ninja`: the full link prints
   **26 files OK** (`build.sha1`: main.dol plus 25 RELs).
3. `check_regression.py` against the remapped base report: no drop, and
   matched functions still 8,677/8,677.
4. `python3 tools/check_object_map_freeze.py`: unit topology and totals are
   unchanged.
5. The quality and asm-wrapper scans, and `tools/verify_decomp_rules.py` once
   it lands.

## 4. Process per rename

1. Claim the batch's owner through `tools/local_campaign.py claim`.
2. **One batch = one `splits.txt` unit**, or one map object when that object
   spans several units. Maximum 40 renames per batch. Inside the batch, run
   the tiers in order T1 → T2 → T3, because T1 names in the unit set the
   prefix for T3.
3. For each function: choose the tier and the name, apply the rename to the
   live reference set, add the source tag, and append one CSV row.
4. Run the gate (section 3). On any failure, revert the whole batch; never
   land part of one.
5. Commit with the message `Names: <unit> — <n> renames (T1 a / T2 b / T3 c) [batch <batch_id>]`.
   Run `tools/check_naming.py --fill-commits` in the next batch (or a
   ledger-only commit) to record the hash.

**Existing non-`fn_` names are out of scope.** If T1 evidence contradicts
one, do not rename it. Add it to `docs/function_rename_review.csv`
(`address,current_name,proposed_name,evidence_ref,note`) for human review.
The same applies to any conflict in section 6. If the user approves the
rename, its CSV row's `evidence` starts with `human-approved YYYY-MM-DD:`.
That is the only way a chain may start from a non-`fn_` name.

**Parking.** In the first pass, a function with no defensible prefix or name
stays `fn_` and goes to the review CSV with the note `parked: <reason>`. It is
parked, not guessed. **Second pass (user decision, 2026-10-20):** once the first
pass is done, every parked function gets the best context name its unit,
callers and data support. That pass is run by the coordinator (an Opus
session) and is marked T3, so in the end no `fn_` name remains.

**Upgrades.** When T1 evidence later appears for a T2 or T3 name, append a
new row whose `old_name` is the current name. Never edit an earlier row. Rows
for one address form a chain: the first row's `old_name` is
`fn_<address>`, and each later row starts from the previous `new_name`.

## 5. CSV ledger: `docs/function_renames.csv`

The file is UTF-8, uses `,` separators and quotes as needed. It is
append-only: rows are never edited, except to fill `commit`.

| Column | Content |
|---|---|
| `address` | `0x8001ABCD` (for a REL: the section-relative address as in its `symbols.txt`) |
| `module` | `main`, or the REL module directory name |
| `old_name` | name before this row |
| `new_name` | name after this row |
| `tier` | `T1`, `T2`, `T3` |
| `evidence` | one line, ≤ 160 chars |
| `evidence_ref` | `map:` / `str:` / `lib:` / `xd:` / `pattern:` / `ctx:` form above |
| `unit` | `splits.txt` unit, e.g. `game/menu/menuCB_80056C54.c` |
| `source_file` | primary defining file under `src/` |
| `batch_id` | `YYYYMMDD-<unit-stem>-<n>` |
| `commit` | landing commit (blank until filled) |
| `date` | `YYYY-MM-DD` |

**Native port.** `pkmn-colosseum-recomp` references the decomp through its
`decomp/pkmn-colosseum` submodule and has about 21k `fn_80…` references in its
own code. After it bumps the submodule:
1. Read the CSV in file order. Key each row by `(module, address)` and keep
   only the last `new_name` of each chain.
2. Replace `\bold_name\b` → `new_name` in code, outside string literals, for
   every `old_name` in the chain.
3. Re-key any address-keyed data (evidence JSON) by `address`, not by name.

Only rows whose `commit` is reachable from the submodule's pinned commit
apply.

## 6. Review rules

| Rows | Lands |
|---|---|
| T1 `map`, `self`, `lib`, `xd` | Automatically after the gate. The coordinator spot-checks 10%. |
| T2 | After coordinator approval of the batch. The coordinator reads every row's pattern and slot proof. |
| T3, game units | Automatically after the gate. Spot-check **10% of each batch, minimum 3 rows** (`check_naming.py --sample <batch_id>`, seeded by `batch_id`). If 2 or more sampled rows fail, the whole batch is reviewed and the worker prompt is corrected before the next batch. |
| T3, library units | Coordinator review, every row. |
| Any row that contradicts a T1 source, a new prefix, or a rename of a non-`fn_` name | Human (user) review. |

**When sources disagree.** Precedence: `map` > `self` > `lib` > `xd` > T2 > T3.
- Two T1 sources that disagree are never resolved automatically. The function
  is parked for human review with both refs.
- A T3 candidate that equals a name on another function anywhere in the
  evidence is renamed or parked.
- Shared-checkout edits by other agents are not part of a batch. Measure them
  first (see the memory note on shared-tree attribution).

## 7. AGENTS.md amendment (applied 2026-10-20)

Replace the hard rule "Do not rename symbols or change splits without a
concrete build/report reason." with:

> - Do not change splits without a concrete build/report reason. Rename
>   symbols only (a) with a concrete build/report reason, or (b) under the
>   function-naming campaign in `docs/NAMING_SCHEME.md` (user decision,
>   2026-10-20): `fn_` functions only; every rename is recorded in
>   `docs/function_renames.csv`, tagged `NAME-T<n>` at its definition, and
>   lands only in a batch that passes `tools/check_naming.py`, the full link
>   with SHA-1 checks, the remapped regression check and the object-map
>   freeze. Never edit a string literal, a unit or file name, or a name that
>   is not `fn_`; a T1 conflict with an existing name goes to human review.

Extend the Naming section with:

> Original names (T1) outrank reconstructed (T2) and context (T3) names; see
> `docs/NAMING_SCHEME.md` for the evidence each tier needs.

## 8. Checker: `tools/check_naming.py`

```sh
python3 tools/check_naming.py                       # validate CSV vs symbols + sources
python3 tools/check_naming.py --base <rev>          # + no string literal changed since <rev>
python3 tools/check_naming.py --remap-report IN OUT # rename IN's functions via the CSV
python3 tools/check_naming.py --sample <batch_id>   # print the T3 spot-check sample
python3 tools/check_naming.py --fill-commits        # fill blank commits from git log
```

It checks the following:
- the CSV schema;
- the chain for each address;
- each final `new_name` is a function at that address, and no old name
  survives in any `symbols.txt`;
- old names appear in the live reference set only on `NAME-T` tag lines;
- there are no duplicate final names, and no T2 or T3 name is shared or
  matches the XD vocabulary;
- the tier syntax rules (sections 1 and 2) hold;
- each tag is present and matches its tier, with no orphan tags;
- the `__FUNCTION__` and `ASSERT` guards hold;
- with `--base`, the string literals of every changed `src` and `include`
  file are identical.
