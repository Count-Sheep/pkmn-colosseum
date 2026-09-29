# Recomp boot row 30 (People/world per-frame update): colsys status (lane D10, 2026-09-30)

## Linked this lane

- `heroMoveSetEventList` and `heroMoveAddAutoEvent`: text-only carve
  `hero_move_exact_8012BAD0.c` (36c9cf09).
- `fn_8010FAF4` (XD checkCollision) and `GScolsys2HumanCollision`: the
  GScolsys2Human head unit with its `.sdata2` pool (3ace0b28).

## Candidates improved (not linked)

| Unit | Function | Before | Now | What is left |
|---|---|---|---|---|
| GScolsys2Thru_candidate_801101B4 | GScolsys2ThruGetEventList | 81.5 | 100 | - |
| | GScolsys2ThruGetEventID | 98.4 | 100 (instructions) | pool literals, when linked |
| | GScolsys2ThruGetMdlEventList | 94.8 | 97.9 | per-pass register colouring |
| | fn_801101B4 / GetFixedMdlEventList | 87.1 / 58.5 | same | not reworked; still use local optimization_level pragmas |
| gs_colsys_candidate_8010E53C | fn_8010EB28 (XD checkHitMdl) | 67.3 | 100 | - |
| | fn_8010E53C (XD checkHitFixedMdl) | 40.3 | 97.1 | register colouring (see below) |

## XD structure (NXXJ01.map, StarsMmd/Colo-XD-PBR-symbol-maps)

The Colosseum functions are the same size as their XD counterparts:
- GScolsys2Hit.o: checkHitFixedMdl 0x5EC and checkHitMdl 0x4BC.
- GScolsys2Thru.o: getFixedMdlEventList 0x7C8 and getMdlEventList 0x60C.

Both objects keep stripped copies of getCpPointPoly (0xF8), getCpPointLine
(0x124) and getCpPointPoint (0x110). The Hit versions copy the closest point
out; the Thru versions do not. Written as `static inline` helpers, they give
retail's per-pass 0/1 flag and the `{1, 2, 4}` mask initialisers. Each
`{1, 2, 4}` initialiser is a pooled 6-byte `.sdata2` constant, which is the
pool start of each TU.

## Walls

- **getCpPolyVec (GScolsys2Walk, 99.38) and GScolsy2UtilChkInTri
  (GScolsys2Util, 99.56).** Both stop at the same float-register pair in
  the unrolled edge test. Retail puts (p.z - z) in f2 and the next vertex's
  z term in f3; the colouring replay needs (p.z - z) numbered after (vn.z
  - z). None of these moved it: product and operand orders, compare forms,
  GSvec and scalar locals, inline helpers with every argument order,
  C++ mode, and every GC compiler version.
- **fn_8010E138 (GScolsys2Walk, 99.67).** The grid collector's
  x/z/cellCountX temporaries take r5/r4/r3; retail uses r3/r4/r5.
  Declaration orders do not change it.
- **fn_8010E53C, 97.1.** Retail gives each pass's loop variables its own
  registers: pass 1 uses r15-r19, pass 3 uses r26-r31, and the params sit
  in r20-r22. The source reuses one set of registers. Block-scoped and
  inline-pass variants were tried; both are worse or equal.
- **Linking the sphere unit** will also need gs_colsys_exact_8010EFE4.c
  folded in. That carve reads the pool's 0.0f/1.0f through extern
  lbl_8047CF00/CF04, which a literal-owning unit would not export.
