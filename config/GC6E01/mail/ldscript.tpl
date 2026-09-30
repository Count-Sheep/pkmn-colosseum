/*
 * GNU ld partial-link script for REL 1 (mail).
 *
 * Like REL 125 (see config/GC6E01/common_rel/ldscript.tpl) the module was
 * partial-linked with SN's GNU-ld-based linker and makerel copied the ELF
 * section table into the REL. mail has no .rodata and no .data
 * relocations, so its table is [1] .text, [2] .rela.text, [3] .data,
 * [4-8] the SN object's empty .sdata/.sdata2/.bss/.sbss/.sbss2: nine
 * entries. The compiler's empty .rodata is discarded to keep .data at
 * index 3.
 */
SECTIONS
{
    .text 0 : { *(.text) }
    .data 0 : { *(.data) }
    /DISCARD/ : { *(.rodata) }
}
