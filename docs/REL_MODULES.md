# REL modules

Pokémon Colosseum keeps its relocatable modules inside Genius Sonority FSYS
archives rather than as loose `.rel` files on the disc. The game loads any
FSYS member whose resource group is `0x0E` as a REL: the group's post-read
callback (`fn_8017F484`, `src/game/gs_range_8017F3F8_middle.c`) links the
module with `OSLink` and calls its `_prolog`; the release callback
(`fn_8017F6B4`) calls `_epilog` and unlinks it.

dtk cannot read FSYS archives, so each module is extracted to
`orig/GC6E01/files/` before the build. Those files are game data: they are
gitignored by `orig/*/*` and must never be committed.

**Keep the disc image out of `orig/GC6E01/` itself.** When a disc image sits
directly in the `object_base` directory, dtk reads every object from inside
the image, and the extracted `files/common_rel.rel` is never found (the build
fails with `files/common_rel.rel not found`). Once `sys/main.dol` has been
extracted, move the image into a subdirectory such as `orig/GC6E01/disc/`
(still gitignored) and read it from there.

## Automatic extraction

`configure.py` lists the modules to unpack per archive (`fsys_modules`) and
emits `pre-split` ninja steps that run `tools/fsys_extract.py` before
`dtk dol split`. A clean `orig` needs `sys/main.dol`, `files/common.fsys`,
`files/pocket_menu.fsys`, `files/s1_out.fsys`, `files/world_map.fsys`, and
`files/S1_shop_1F.fsys`: `python configure.py && ninja` extracts
`common_rel.rel`, `mail.rel`, `pocket_menu.rel`, `S1_out.rel`,
`worldmap_menu_rel.rel`, and `S1_shop_1F.rel` itself. The
manual steps below are what that step does.

## Extracting a module

1. Copy the archive out of the disc image (read in place):

   ```sh
   build/tools/dtk vfs cp "orig/GC6E01/disc/Pokemon Colosseum (USA).iso:files/common.fsys" orig/GC6E01/files/
   ```

2. List it and unpack the REL member:

   ```sh
   python3 tools/fsys_extract.py list orig/GC6E01/files/common.fsys
   python3 tools/fsys_extract.py extract orig/GC6E01/files/common.fsys common_rel -o orig/GC6E01/files
   ```

   `extract` without member names unpacks everything. Members of file type
   `0x1C` are written as `.rel` and checked (module id, section table and
   import table must parse); other members are written as `.bin`.

3. Check the result:

   ```sh
   build/tools/dtk shasum orig/GC6E01/files/common_rel.rel
   # e816761813900aa0fd4f87d012b3b2a19d2f003e  orig/GC6E01/files/common_rel.rel
   ```

`tools/fsys_extract.py` follows the layout the game's own loader walks
(`fn_8017E30C` in `src/game/fsys/fsys_file.c`) and decodes packed members with
the same LZSS as `fn_8017F2C4` (`src/game/gs_range_8017F2C4.c`); its module
docstring documents the header and entry fields.

## Build integration

Each integrated module is a `modules:` entry in `config/GC6E01/config.yml`
with its own `symbols.txt`/`splits.txt` under `config/GC6E01/<module>/`, and
its output `build/GC6E01/<module>/<module>.rel` is listed in
`config/GC6E01/build.sha1`, so `ninja` checks it together with `main.dol`.
Undeclared parts of a module link from dtk's extracted objects, as for the
DOL. Like the DOL, the build needs the extracted `.rel` in
`orig/GC6E01/files/`, so a CI build container must carry it too.

### The modules were built with SN Systems ProDG, not CodeWarrior

REL 125 carries three independent fingerprints of the SN toolchain:

- **Compiler.** Its code has GCC's `-O0` frame (`mr r31,r1` ...
  `lwz r11,0(r1); lwz r31,-4(r11); mr r1,r11`) and GCC's register choice
  (`lis r9` / `lis r11` / `addi r0`), which CodeWarrior never emits. ProDG
  3.5 through 3.9.3 all compile `_prolog` to the same bytes, so the code does
  not pin the version; 3.5 is used because its driver runs SN's assembler
  under wibo (the 3.7+ drivers fail to start theirs).
- **Assembler.** Absolute relocations against local string labels keep the
  section-relative addend in place as well as in the RELA entry (the song
  table's pointers read `0x6D4`, `0x6BC`, ... in the retail file). SN's
  assembler does this; GNU as and mwcc write zero.
- **Linker.** The REL section table is the ELF section table of a GNU ld
  partial link: `[1] .text [2] .rela.text [3] .rodata [4] .data
  [5] .rela.data [6-10]` the SN objects' empty `.sdata`/`.sdata2`/`.bss`/
  `.sbss`/`.sbss2`, eleven entries in all. SN's linker is GNU ld based.

The build follows the same chain:

- `ProDG/<version>` compiler versions (the `Rel` helper in `configure.py`)
  select the `prodg` rule in `tools/project.py`: `ngccc -c` under wibo with
  `SN_NGC_PATH` pointing at the compiler directory, then an `objcopy` pass.
  SN's assembler leaves `.symtab`'s `sh_info` one short of the last local
  symbol, which GNU ld rejects; `objcopy` rewrites the symbol table without
  touching section data or relocations. Flags are `-O0 -G0`; `-G0` keeps
  small data out of the module.
- Modules listed in `config.gnu_ld_modules` are partial-linked with GNU
  `ld -r` instead of mwld, using the module's `ldscript_template`
  (`config/GC6E01/common_rel/ldscript.tpl`, a GNU ld script that fixes the
  `.text`, `.rodata`, `.data` order). `dtk rel make` keeps ELF section
  indices, so this is what gives the REL its section table.

dtk zeroes relocated words in the objects it extracts, so an extracted unit
can never reproduce the in-place addends above. Every unit whose data holds
such relocations has to come from SN-built source for the REL to match (the
two sound tables in REL 125). objdiff compares against those zeroed target
objects, so it scores the pointer words of these units as one-byte
mismatches even though the linked REL is byte-identical; the `build.sha1`
check is the authority for these units.

### REL 125 source

- `src/rel/common_rel/common_rel.c`: `_prolog`, `_epilog`, `_unresolved`
  (all of `.text`) and the 48 table counts at the end of `.data`. `_prolog`
  points 47 pairs of main.dol `.sbss` pointers at a table and its count.
- `src/rel/common_rel/snd_song_table.c`, `snd_sample_table.c`: the two
  tables with string pointers (see above).
- `include/rel/common_rel.h`: their entry types.

The remaining tables are still dtk-extracted.

### REL 1 source

- `src/rel/mail/mail.c`: the whole module. `_prolog`, `_epilog` and
  `_unresolved` (all of `.text`), the two mail tables and their counts (all
  of `.data`). `_prolog` points two pairs of main.dol `.sbss` pointers
  (`lbl_80478EA4`/`EA0`, `lbl_80478E9C`/`E98`) at a table and its count; the
  readers are `game/mail.c` and the `mail_*` units.
- Extract it like REL 125: `python3 tools/fsys_extract.py extract
  orig/GC6E01/files/common.fsys mail -o orig/GC6E01/files`
  (`28bc997c8bc065db08fefc3c361fd5982e39dcbc`).
- The module has no `.rodata` and no `.data` relocations, so its section
  table is nine entries (`[3] .data`); `config/GC6E01/mail/ldscript.tpl`
  discards the compiler's empty `.rodata` to reproduce it.

### REL 2 source

- `src/rel/pocket_menu/pocket_menu.c`: the whole module. `_prolog`, `_epilog`
  and `_unresolved` publish and clear four shop-table/count pairs in main.dol
  `.sbss`; the four table payloads and counts make up all of `.data`.
- Extract it with `python3 tools/fsys_extract.py extract
  orig/GC6E01/files/pocket_menu.fsys pocket_menu -o orig/GC6E01/files`
  (`77ab4475c3dda3b2a7bca406adaa565d89fc0d9e`).
- Like REL 1, it has no `.rodata` and uses the nine-entry SN section layout.

## REL members on the disc

| Archive | Member | Module id |
| --- | --- | --- |
| `common.fsys` | `common_rel` | 125 (integrated) |
| `common.fsys` | `mail` | 1 (integrated) |
| `s1_out.fsys` | `S1_out` | 131 (integrated; [inventory](REL_131.md)) |
| `pocket_menu.fsys`, `colosseumbattle_menu.fsys` | `pocket_menu` | 2 (integrated) |
| `world_map.fsys` | `worldmap_menu_rel` | 3 (integrated; [inventory](REL_3_5.md)) |
| `S1_shop_1F.fsys` | `S1_shop_1F` | 5 (integrated; [inventory](REL_3_5.md)) |
| `toolbattle_menu.fsys` | `toolbattle_menu` | 163 |
| `waza_viewer.fsys` | `waza_viewer_rel` (stored unpacked) | 166 |
| `pda_menu.fsys` | `pda_menu` | 167 |

Every floor archive (`D*`, `M*`, `S*`, `T1_*`; 161 of them) also holds one REL
named after the floor, with its own module id (4-168 outside the ids above;
for example `S1_out` is 131). `s1_out.fsys` is the only floor archive named in
main.dol. None of the title-screen archives (`nintendo_logo`, `genius_logo`,
`pokemon_logo`, `opening_demo`, `title`, `ex_title`, `topmenu`) holds a REL.
