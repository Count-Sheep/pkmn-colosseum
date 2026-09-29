# GScolsys2 debug draw (0x8010D20C - 0x8010DE00): status notes (lane D10)

Unit: `src/game/gs_colsys_candidate_8010D20C.c` (CodeCandidate, GC/1.3, -O3).
It blocks recomp boot row 40.

## Structure

This part of the TU is XD's `GScolsys2Draw.o` (NXXJ01.map, StarsMmd/Colo-XD-PBR-symbol-maps):

| XD | Colosseum |
|---|---|
| drawWalkMdl (0x1E8) | fn_8010D20C (0x1BC) |
| drawHitMdl (0x134, live) | inlined edge drawer (edgeGroup0 / edgeGroup1, colour 0xFFFFFFFF) |
| drawSunMdl / drawCheckMdl / drawThruMdl (UNUSED, 0xCC each) | inlined face drawers, one colour each (0xFF00FFC0, 0xFFFF00C0, 0x00FFFFC0) |
| makeDisplayListFixedObj | GScolsys2Draw (records the display list) |
| GScolsys2Draw | fn_8010D8D4 (draws non-fixed objects, replays the list) |

The four colours are the first four words of the TU's `.sdata2` pool
(0x8047CEB8 - 0x8047CEC4), followed by fn_8010D20C's floats. File-scope
`static const GXColor` objects declared before fn_8010D20C give that order.

## What the source now gets

- Each helper copies its colour into a stack local after the NULL test
  (`lwz r0,<colour>; stw r0,<slot>`). That is one local per expansion,
  as in retail.
- The face helpers load the four colour bytes into callee-saved registers
  outside the loop. The edge helper rereads them from the stack before
  every fn_800D5CB8 call. The only form found that stops the hoisting in
  the edge helper is an address-taken copy,
  `*(u32*)&color = *(const u32*)&sHitColor;`. A by-value helper parameter
  or a pointer read either hoists the bytes or makes a fresh copy per call.
  Linking this form would need a RULE-EXCEPTION.
- fn_8010D20C: `(s32)tri->surface / 15.0f` gives retail's single byte load
  and bitfield extraction.

## Remaining differences

- fn_8010D20C, 99.3%: only register order. Retail colours the loop index
  after the matrix and group parameters, the 0x4330 conversion constant and
  the vertex pointer (index r26). Here it is coloured right after `tri`
  (r30). A K=25 colouring replay (GC/2.6 mwcc-debugger + D7's simulator)
  finds no single renumbering, merge or extra interference that gives
  retail. Retail's index must have a much lower interference degree. About
  720 declaration orders and the loop shapes were tried.
- GScolsys2Draw, 96.2%, and fn_8010D8D4, 93.4%: register order. One more
  callee-saved register is used here, and the face helpers hoist only
  three of the four colour bytes (red is reloaded).
