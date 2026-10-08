# REL modules 3 and 5

These are the first modules reached after the native port's accepted REL 131
path: the world-map branch loads REL 3, and entering the Outskirt Stand shop
loads REL 5. Both are complete table-publication modules built with the same
SN ProDG `-O0 -G0` pipeline as the other accepted RELs. Extracted retail files
remain ignored under `orig/GC6E01/files/`.

## REL 3: worldmap_menu_rel

Retail source: `world_map.fsys`, member `worldmap_menu_rel`, resource group 14,
type `0x1C`, member ID `0x0D701C00`. The decoded REL is `0xA50` bytes and has
SHA-1 `09653861366421e1d6c81698074a70b3d55625da`.

It is version 3 with nine section entries, no BSS, and two nonempty sections:
`0x134` bytes of text and `0x6D8` bytes of data. Its complete executable
inventory is `_prolog` (`0x0`, `0x9C` bytes), `_epilog` (`0x9C`, `0x7C`
bytes), and empty `_unresolved` (`0x118`, `0x1C` bytes). None calls another
function; there are no constructors, callbacks, or branch relocations.

The prolog publishes four table/count pairs, in this order; epilog clears the
same eight destinations in the same order:

| Main destination | Module data | Role |
| --- | --- | --- |
| `0x80478DDC` | `0x0` | Nine 16-byte world-map menu records |
| `0x80478DD8` | `0x6C8` | Count: 9 |
| `0x80478DE4` | `0x90` | Twelve 40-byte title-slot records |
| `0x80478DE0` | `0x6CC` | Count: 12 |
| `0x80478DEC` | `0x270` | Twenty 16-byte camera/demo records |
| `0x80478DE8` | `0x6D0` | Count: 20 |
| `0x80478DF4` | `0x3B0` | Sixty-six 12-byte directional links |
| `0x80478DF0` | `0x6D4` | Count: 66 |

The only imports are self (module 3) and main (module 0). Text contains 16
self `ADDR16_HA/LO` relocations for the eight module-data addresses and 32
main `ADDR16_HA/LO` relocations for the prolog/epilog stores. Data contains no
relocations.

## REL 5: S1_shop_1F

Retail source: `S1_shop_1F.fsys`, member `S1_shop_1F`, resource group 14,
type `0x1C`, member ID `0x02581C00`. The decoded REL is `0x740` bytes and has
SHA-1 `db78b6f44ca01c2090ec197117f5169494fbb6bf`.

It is version 3 with ten section entries, no BSS, and two nonempty sections:
`0x188` bytes of text and `0x2A4` bytes of data. Its complete executable
inventory is `_prolog` (`0x0`, `0xCC` bytes), `_epilog` (`0xCC`, `0xA0`
bytes), and empty `_unresolved` (`0x16C`, `0x1C` bytes). None calls another
function; there are no constructors, callbacks, or branch relocations.

The prolog publishes three table/count/descriptor triples and one table/count
pair. Each descriptor stores its count pointer followed by its table pointer.
Epilog clears the same eleven destinations in the same order:

| Main destination | Module data | Role |
| --- | --- | --- |
| `0x8047904C` | `0x0` | Sixteen 24-byte news/camera records |
| `0x80479048` | `0x27C` | Count: 16 |
| `0x80479044` | `0x280` | First descriptor pair |
| `0x80479D3C` | `0x180` | Five 36-byte person records |
| `0x80479D38` | `0x288` | Count: 5 |
| `0x80479D34` | `0x28C` | Second descriptor pair |
| `0x80479754` | `0x234` | Two 16-byte point records |
| `0x80479750` | `0x294` | Count: 2 |
| `0x8047974C` | `0x298` | Third descriptor pair |
| `0x8047A250` | `0x254` | One 40-byte record |
| `0x8047A24C` | `0x2A0` | Count: 1 |

The only imports are self (module 5) and main (module 0). Text contains 22
self `ADDR16_HA/LO` relocations and 44 main `ADDR16_HA/LO` relocations. Data
contains six self `ADDR32` relocations for the three descriptor pairs.

## Acceptance

The full modules are reconstructed in
`src/rel/worldmap_menu_rel/worldmap_menu_rel.c` and
`src/rel/S1_shop_1F/S1_shop_1F.c`. Each source is one complete `Matching`
unit. All six functions, all 700 text bytes, and all 2,428 data bytes score
100% in the report. Direct `cmp` checks confirm both rebuilt REL files are
byte-identical to retail, including headers, section tables, relocation
streams, and data.

The full build passes every canonical SHA-1 check. This adds six exact and
linked functions, 700 code bytes, and 2,428 linked data bytes. It establishes
the entire executable closure of both modules; no behavior beyond their
publication lifecycle is hidden inside either REL. The native port still has
to bind these contracts and rerun the headed routes.
