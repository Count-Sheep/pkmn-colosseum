# Phenac City REL modules (M1_*)

The story route after the first battle (world map -> Phenac City -> cutscenes
-> Pokemon Center -> save) loads Phenac's floor modules through
`fn_8017F484`. All 17 RELs held by the `M1_*.fsys` archives are integrated, so
every Phenac floor, its `_bf` variants and the Phenac Stadium module are
covered: the outdoor area (`M1_out` 15, `M1_out_bf` 132), the Pokemon Center
(`M1_pc_1F` 16, `M1_pc_B1` 17) and the rest of the city.

Each archive holds one REL member named after the floor (resource group 14,
type `0x1C`). Extracted files stay ignored under `orig/GC6E01/files/`;
`configure.py` (`fsys_modules`) unpacks them before the split, so a clean
`orig` needs the 17 `M1_*.fsys` archives copied from the disc.

| Archive / member | Module id | Text | Data | SHA-1 |
| --- | --- | --- | --- | --- |
| `M1_out` | 15 | `0x188` | `0x60C` | `99055f9741b4c61583ca33749fa7c120a3163010` |
| `M1_pc_1F` | 16 | `0x134` | `0x114` | `93c70532f39515bfec46a296f9320f148d4ada34` |
| `M1_pc_B1` | 17 | `0x188` | `0xBC` | `d754050147e539176a73a443fc50e440f8042536` |
| `M1_houseA_1F` | 18 | `0x188` | `0x2EC` | `58087a5c18ed1cc36ffc21846935b8407e47a0dd` |
| `M1_houseA_2F` | 19 | `0x188` | `0x214` | `b0633667d12ad8d6ae77d096784070aff9697575` |
| `M1_houseB_1F` | 20 | `0x188` | `0x228` | `28421e879a21bcf447543654d4ccd246309f255b` |
| `M1_houseC_1F` | 21 | `0x188` | `0x228` | `14dce2d47eeab97905f57e93d7c2078f06f8ca01` |
| `M1_shop_1F` | 22 | `0x134` | `0xF4` | `86185ad8a2a265af74aca8ea3c493f5f0f5eb874` |
| `M1_shop_2F` | 23 | `0x134` | `0xC0` | `824fa7bcaba906b4317f302606d38e7efbcbb706` |
| `M1_gym_1F` | 24 | `0x134` | `0x1CC` | `4a9d6dc26d279a9abb01dd41dd50ccc589a88412` |
| `M1_gym_B1` | 25 | `0x134` | `0x174` | `9b11f873e8344ba63f7cd72bdf08294fdc2efbd8` |
| `M1_stadium_1F` | 26 | `0x188` | `0x134` | `32b5c0ee6c26606e582cadfccb1cd84396944d39` |
| `M1_carde_1` | 27 | `0x134` | `0x2EC` | `466e5fabeb4001a83264941b4a01187c9920e1e6` |
| `M1_out_bf` | 132 | `0x8C` | `0x2C` | `92020fe1a4f32513db2d3dac0a08006ee764abc7` |
| `M1_gym_1F_bf` | 133 | `0x8C` | `0x2C` | `6f069dc29b289c3dea471868840cead275a75aec` |
| `M1_houseA_1F_bf` | 134 | `0x8C` | `0x2C` | `40af492b17ef6f665921d6ec0a813132570f15ce` |
| `M1_water_colo` | 157 | `0xE0` | `0x48` | `997fa17f513421af5abee482103913d312a0bf2d` |

All of them are SN ProDG `-O0 -G0` table-publication modules, the same shape
as RELs 5, 129 and 131: `_prolog` stores pointers to the module's tables and
their counts into main.dol `.sbss` slots, `_epilog` clears the same slots in
the same order, `_unresolved` is empty, and there is no rodata or BSS. The
`_bf` modules publish one 40-byte record and its count (like REL 129); the
others publish two or three table/count/descriptor triples plus a 40-byte
record. Each module imports only itself and main. Data relocations point only
at the module's own count words and tables (the `FieldTable` descriptors).

Sources are `src/rel/<module>/<module>.c`, one complete `Matching` unit per
module. The record contents are kept as raw words (floats included); naming
their fields is future work. Every unit scores 100% in objdiff and every
rebuilt REL is byte-identical to retail (`build.sha1`).

In main's `symbols.txt` the `.sbss` blocks that receive these pointers
(`lbl_80478FC0`, `lbl_80479050`, `lbl_80479170`, `lbl_80479764`,
`lbl_80479D4C`, `lbl_80479E64`) are split into 4-byte labels at each
destination (labels added, none renamed); the remainder of each block keeps a
filler label. The object-map freeze was regenerated for the 17 added units.
