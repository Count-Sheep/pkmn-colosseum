/*
 * GNU ld partial-link script for REL 125 (common_rel).
 *
 * The module was built with the SN Systems toolchain, whose linker is GNU ld
 * based, and makerel copied the partial link's ELF section table into the
 * REL: [1] .text, [2] .rela.text, [3] .rodata, [4] .data, [5] .rela.data,
 * [6-10] the empty small-data and bss sections of the SN objects. dtk rel
 * make keeps ELF section indices, so the partial link must reproduce that
 * order. Used as the module's ldscript_template (it has no placeholders).
 */
SECTIONS
{
    .text 0 : { *(.text) }
    .rodata 0 : { *(.rodata) }
    .data 0 : { *(.data) }
}
