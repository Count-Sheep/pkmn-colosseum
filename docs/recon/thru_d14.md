# GScolsys2Thru (0x801101B4-0x80111864): lane D14 notes (2026-09-30)

## TU and pool

The GScolsys2Thru TU is .text 0x801101B4-0x80111864: fn_801101B4,
GetFixedMdlEventList, GetMdlEventList, GetEventList (linked carve) and
GetEventID. Its .sdata2 pool is 0x8047CF48-0x8047CF60: two {1, 2, 4} mask
initialisers, then 0.0f and 1.0f. fn_80111864 uses the next pool
(0x8047CF60: 0.0f and 1.0f again), so it belongs to the following TU. A
whole-TU link needs the three candidate functions exact.

## Fixed here

GScolsys2ThruGetFixedMdlEventList goes from 99.41% to 100%. The grid bounds are a
GScolsys2Vec3 lo/hi pair, computed in the order lo.x, lo.z, hi.x, hi.z. This
is the same shape as the sphere unit's fn_8010E53C, and it gives retail's
float registers.

## Still walled: the vertex-transform loop in pass 1

This affects fn_801101B4 (getMdlEventListPass) and GetMdlEventList (12 rows
each). Pass 2 and pass 3 already match. Retail's preheader is
`addi dst,r1,verts; mr src,tri; li v,0`, with dst in the higher register.

- The pre-RA scheduler turns every form we tried into `mr src` first,
  whatever the IR order (li/addi/mr or li/mr/addi).
- Retail's pass 2 shows how retail gets addi first. The dst induction
  variable is initialised as a copy of a verts address that is computed
  just before it (`addi r20; mr r22,r18; mr r19,r20`). That copy dependency
  schedules the addi first. In pass 1 the address dies at the copy, so
  coalescing hides it.
- I reproduced this with an inline helper that returns its `out` parameter
  when the caller uses the result (I1/J2/J4 in scratchpad d14). The addi
  then comes first, but the returned pointer is reused after the loop,
  where retail rematerialises the address, and src/v swap registers.
- These did not keep the copy: parameter temporaries that are discarded,
  modified or cast. Neither did user src/dst pointers in any init or
  increment order. The best of those gets GetMdlEventList to 2 rows
  (Cfsd: registers right, preheader order wrong).
- Next lead: a source form in which the dst initialiser copies an address
  temporary that dies at the loop, without a use after the loop.
