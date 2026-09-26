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

## Extracting a module

1. Copy the archive out of the disc image (read in place):

   ```sh
   build/tools/dtk vfs cp "orig/GC6E01/Pokemon Colosseum (USA).iso:files/common.fsys" orig/GC6E01/files/
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

## REL members on the disc

| Archive | Member | Module id |
| --- | --- | --- |
| `common.fsys` | `common_rel` | 125 (integrated) |
| `common.fsys` | `mail` | 1 |
| `pocket_menu.fsys`, `colosseumbattle_menu.fsys` | `pocket_menu` | 2 |
| `world_map.fsys` | `worldmap_menu_rel` | 3 |
| `toolbattle_menu.fsys` | `toolbattle_menu` | 163 |
| `waza_viewer.fsys` | `waza_viewer_rel` (stored unpacked) | 166 |
| `pda_menu.fsys` | `pda_menu` | 167 |

Every floor archive (`D*`, `M*`, `S*`, `T1_*`; 161 of them) also holds one REL
named after the floor, with its own module id (4-168 outside the ids above;
for example `S1_out` is 131). `s1_out.fsys` is the only floor archive named in
main.dol. None of the title-screen archives (`nintendo_logo`, `genius_logo`,
`pokemon_logo`, `opening_demo`, `title`, `ex_title`, `topmenu`) holds a REL.
