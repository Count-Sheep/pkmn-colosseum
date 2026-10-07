/* REL 2 has no rodata and retains the SN object's empty small-data/BSS sections. */
SECTIONS
{
    .text 0 : { *(.text) }
    .data 0 : { *(.data) }
    /DISCARD/ : { *(.rodata) }
}
