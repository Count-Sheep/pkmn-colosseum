# `fn_801093C8` (XD menuOffScreenDraw): resolved

Linked (lane D11). The remaining `f2`/`f3`/`f4` allocation cycle at
instructions 44-55 came from reading the fade constants through extern
stand-ins (`lbl_8047CE48` etc.). Written as literals (`255.0f`, `100.0f`,
`0.0f`, `640.0f`, `480.0f`, `1.0f`), the allocation matches retail exactly.

The pool 0x8047CE48-0x8047CE70 holds exactly those literals in first-use
order, then the two int-to-float doubles. `menuOffScreenFadeSet`, `Create`
and `Init` also read its 0.0f/1.0f, so it is one TU's pool, and
fn_801093C8 opens that TU. XD agrees: its menuOffScreen.cpp starts with
menuOffScreenDraw at 0x801145EC (TeamOrre/xd-decomp
config/GXXE01/symbols.txt @4989794). So `menu_offscreen.c` now covers
.text 0x801093C8-0x80109894 plus that .sdata2 pool. The old
`win_sprite_suffix_801093C8.c` carve is gone, and the gs_model .sdata2
blob is split around the pool (`game/data/sdata2_8047CE70.c` takes
0x8047CE70-0x8047CE98).
