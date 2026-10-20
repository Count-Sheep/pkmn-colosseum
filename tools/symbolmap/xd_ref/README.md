# xd_ref: Pokémon XD naming reference

Symbol tables copied from TeamOrre/xd-decomp (https://github.com/TeamOrre/xd-decomp,
commit 4989794e):
- `NXXJ01/symbols.txt`: the JP demo disc. Its names come from the disc's own
  linker map (`Master.MAP`), so they are original developer names. This is
  the only primary map; see `docs/NAMING_SCHEME.md`, tier T1 `map`.
- `GXXE01/symbols.txt`: USA retail. The names were ported from the demo map
  and are used as the address bridge.

`GXXE01/_xdsplit` is a local dtk split of the XD retail DOL. It is binary
derived and never committed (it is gitignored). To regenerate it, run
`dtk dol split GXXE01/config.mine.yml _xdsplit` in a folder that has
`orig/GXXE01/sys/main.dol`; the default location is `build/xd_ref/_xdsplit`.
