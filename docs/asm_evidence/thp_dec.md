# Dolphin THP decoder (THPDec.c) inline assembly in Pokémon Colosseum (GC6E01)

`dolphin/thp/THPDec.c` builds the whole THP video decoder translation unit
(0x801E5548-0x801ECFE0) from `src/dolphin/thp/THP_range_801E1B54.c`. Nintendo
wrote the decoder's hot paths as C functions whose bodies are mostly inline
`asm` blocks: the paired-single inverse DCTs and the Huffman bit reader. Only
the functions named below are admitted. Each body uses the exact retail
mnemonics and the unit matches the retail bytes in objdiff and in the final
link (`ninja`: 26 files OK).

Several of these functions are `inline` and have no retail symbol of their
own: MWCC expands them into their callers, so their asm appears inside the
retail routines named in each section.

The Huffman routines branch from one asm block to labels in later blocks.
Those labels are written as label-only asm blocks (`asm { _Read4: }`), the
form the vendor source uses for `_RECV_END`, so every branch stays inside the
routine's own asm. This was checked to leave `THPDec.o` byte-identical.

Common origin: Nintendo Dolphin SDK THP library (`THPDec.c`), linked into
Colosseum's main.dol. The exact SDK build is not established.

## __THPInverseDCTNoYPos

Inline; expanded into `__THPDecompressiMCURow512x448`,
`__THPDecompressiMCURow640x480` and `__THPDecompressiMCURowNxN` (two blocks
of Y, plus U and V, per MCU).

- Why it cannot be C: the 8x8 inverse DCT runs entirely in paired-single registers: `psq_l` loads the coefficients through GQR5 (dequantising s16 to float in the load), `ps_mul`/`ps_madd`/`ps_msub`/`ps_nmsub`/`ps_merge*` do two lanes at a time, and `psq_st`/`psq_stu` write the results back through GQR6 (float to u8 with saturation). The loop counter lives in CTR (`mtctr`/`bdnz`). MWCC does not emit paired-single instructions or quantised loads/stores from C.
- Other decompilations: [projectPiki/pikmin2, commit 999140f3, `src/Dolphin/thp/THPDec.c`](https://github.com/projectPiki/pikmin2/blob/999140f3521e22619547d5d61862a6577ea73882/src/Dolphin/thp/THPDec.c#L592-L896) keeps `__THPInverseDCTNoYPos` as the same C function with inline `asm` blocks.
- Origin: Nintendo Dolphin SDK THP library, `THPDec.c`, inline assembly in the vendor source.

## __THPInverseDCTY8

Inline; expanded into the same three `__THPDecompressiMCURow*` routines (the
two lower Y blocks of each MCU).

- Why it cannot be C: as `__THPInverseDCTNoYPos`, with the output rows strided for the Y plane: paired-single arithmetic (`ps_add`, `ps_sub`, `ps_mul`, `ps_madd`, `ps_msub`, `ps_nmsub`, `ps_merge00/01/10/11`) between GQR-quantised `psq_l` loads and `psq_st`/`psq_stu` stores, in a CTR loop. MWCC does not emit paired-single instructions or quantised loads/stores from C.
- Other decompilations: [projectPiki/pikmin2, commit 999140f3, `src/Dolphin/thp/THPDec.c`](https://github.com/projectPiki/pikmin2/blob/999140f3521e22619547d5d61862a6577ea73882/src/Dolphin/thp/THPDec.c#L898-L1206) keeps `__THPInverseDCTY8` as the same C function with inline `asm` blocks.
- Origin: Nintendo Dolphin SDK THP library, `THPDec.c`, inline assembly in the vendor source.

## __THPHuffDecodeTab

Inline; expanded into `__THPHuffDecodeDCTCompU` and `__THPHuffDecodeDCTCompV`
(and the Y decoder's DC table) for every DC/AC symbol.

- Why it cannot be C: it is the bit reader's table lookup, written by hand around a bit buffer held in named `register` variables (`cnt`, `cb`, `code`, `increment`, `tmp`) that the asm blocks share. The fast path (`rlwnm` peek, two `lbzx` table reads) branches with `bgt`/`beq` from its asm block into slow paths that the source places after the function's `return` statements (`_FailedCheckEnoughBits`, `_notEnoughBits`, `_Read4`, `_getfullword`, ...), and those paths branch back between blocks. No instruction here is outside what MWCC can emit; what C cannot express is this control flow: branches from inside one asm block into another, register-pinned state across blocks, and reachable code after `return`.
- Other decompilations: [projectPiki/pikmin2, commit 999140f3, `src/Dolphin/thp/THPDec.c`](https://github.com/projectPiki/pikmin2/blob/999140f3521e22619547d5d61862a6577ea73882/src/Dolphin/thp/THPDec.c#L1274-L1512) keeps `__THPHuffDecodeTab` as the same inline function built from `asm` blocks with the same labels.
- Origin: Nintendo Dolphin SDK THP library, `THPDec.c`, inline assembly in the vendor source.

## __THPHuffDecodeDCTCompY

Retail address 0x801EB644, size 0x67C.

- Why it cannot be C: the luminance block decoder carries its own copy of the Huffman lookup and the coefficient bit reader as inline asm over register-pinned state (`cnt`, `cb`, `code`, `ssss`, `rrrr`). Its asm blocks branch into labels of later blocks (`_FailedCheckEnoughBits`, `_FCEB_Done`, `_Read4`, `_getfullword`, `_1bitleft`, `_DoneDecodeTab`, `_RECV_SSSS_ZERO`) that sit between C statements, and C `goto`s join the same labels. As for `__THPHuffDecodeTab`, the instructions are ordinary integer ones; the hand-built control flow across asm blocks and the fixed register assignment are what C cannot reproduce.
- Other decompilations: [projectPiki/pikmin2, commit 999140f3, `src/Dolphin/thp/THPDec.c`](https://github.com/projectPiki/pikmin2/blob/999140f3521e22619547d5d61862a6577ea73882/src/Dolphin/thp/THPDec.c#L1659-L2043) keeps `__THPHuffDecodeDCTCompY` as the same C function with inline `asm` blocks.
- Origin: Nintendo Dolphin SDK THP library, `THPDec.c`, inline assembly in the vendor source.

## __THPHuffDecodeDCTCompU

Retail address 0x801EBCC0, size 0x6A8.

- Why it cannot be C: the U-block decoder reads each DC difference and AC coefficient with an inline-asm bit extractor over register-pinned state (`cnt`, `cnt33`, `cb`, `tmp`, `v`): the fast path's `subfc.` sets CR0 for the `bgt _notEnoughBitsDIFF` into the next asm block, which refills the bit buffer with `lwzu` and rejoins at `_DoneDIFF`. The branches cross from one asm block into another, which no C statement produces.
- Other decompilations: [projectPiki/pikmin2, commit 999140f3, `src/Dolphin/thp/THPDec.c`](https://github.com/projectPiki/pikmin2/blob/999140f3521e22619547d5d61862a6577ea73882/src/Dolphin/thp/THPDec.c#L2049-L2171) keeps `__THPHuffDecodeDCTCompU` as the same C function with inline `asm` blocks.
- Origin: Nintendo Dolphin SDK THP library, `THPDec.c`, inline assembly in the vendor source.

## __THPHuffDecodeDCTCompV

Retail address 0x801EC368, size 0x6A8.

- Why it cannot be C: the V-block twin of `__THPHuffDecodeDCTCompU`: the same inline-asm bit extractor over register-pinned state, with branches from one asm block into the refill path of the next (`_notEnoughBits...`, `_Done...`). The branches cross asm blocks, which no C statement produces.
- Other decompilations: [projectPiki/pikmin2, commit 999140f3, `src/Dolphin/thp/THPDec.c`](https://github.com/projectPiki/pikmin2/blob/999140f3521e22619547d5d61862a6577ea73882/src/Dolphin/thp/THPDec.c#L2177-L2302) keeps `__THPHuffDecodeDCTCompV` as the same C function with inline `asm` blocks.
- Origin: Nintendo Dolphin SDK THP library, `THPDec.c`, inline assembly in the vendor source.

## __THPGQRSetup

Inline; expanded into `__THPDecompressYUV` before the MCU rows are decoded.

- Why it cannot be C: it saves the graphics quantisation registers GQR5 and GQR6 with `mfspr` (SPR 917/918) and loads the decoder's own quantisation modes with `li`/`oris`/`mtspr` (GQR5 = s16 dequantise for the coefficient loads, GQR6 = u8 quantise with scale for the pixel stores). MWCC never emits `mfspr`/`mtspr` from C.
- Other decompilations: [projectPiki/pikmin2, commit 999140f3, `src/Dolphin/thp/THPDec.c`](https://github.com/projectPiki/pikmin2/blob/999140f3521e22619547d5d61862a6577ea73882/src/Dolphin/thp/THPDec.c#L453-L477) keeps `__THPGQRSetup` as the same static inline function with the same two `asm` blocks.
- Origin: Nintendo Dolphin SDK THP library, `THPDec.c`, inline assembly in the vendor source.

## __THPGQRRestore

Inline; expanded into `__THPDecompressYUV` after the MCU rows are decoded.

- Why it cannot be C: it writes the saved GQR5 and GQR6 values back with `mtspr` (SPR 917/918). MWCC never emits `mtspr` from C.
- Other decompilations: [projectPiki/pikmin2, commit 999140f3, `src/Dolphin/thp/THPDec.c`](https://github.com/projectPiki/pikmin2/blob/999140f3521e22619547d5d61862a6577ea73882/src/Dolphin/thp/THPDec.c#L479-L491) keeps `__THPGQRRestore` as the same static inline function with the same `asm` block.
- Origin: Nintendo Dolphin SDK THP library, `THPDec.c`, inline assembly in the vendor source.

## THPInit

Retail address 0x801ECA10, size 0xA0.

- Why it cannot be C: after setting up the locked-cache work pointers it programs GQR2-GQR5 for the fast u8/u16/s8/s16 casts (`li`/`oris`/`mtspr` to SPR 914-917), the body of the SDK's `OSInitFastCast`. MWCC never emits `mtspr` from C.
- Other decompilations: [projectPiki/pikmin2, commit 999140f3, `src/Dolphin/thp/THPDec.c`](https://github.com/projectPiki/pikmin2/blob/999140f3521e22619547d5d61862a6577ea73882/src/Dolphin/thp/THPDec.c#L2309-L2334) has `THPInit` call `OSInitFastCast()`, which is the same `asm` block of four `li`/`oris`/`mtspr` triples in [`include/Dolphin/OS/OSFastCast.h`](https://github.com/projectPiki/pikmin2/blob/999140f3521e22619547d5d61862a6577ea73882/include/Dolphin/OS/OSFastCast.h#L28-L46). Colosseum's source writes that inline function's asm in place.
- Origin: Nintendo Dolphin SDK THP library, `THPDec.c` (with the OS library's inline `OSInitFastCast` from `OSFastCast.h`), inline assembly in the vendor source.
