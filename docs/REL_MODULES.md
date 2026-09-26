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

## REL members on the disc

| Archive | Member | Module id |
| --- | --- | --- |
| `common.fsys` | `common_rel` | 125 |
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
