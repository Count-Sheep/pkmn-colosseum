/* REL 24 has data relocations and no rodata section. */
SECTIONS
{
    .text 0 : { *(.text) }
    .data 0 : { *(.data) }
    /DISCARD/ : { *(.rodata) }
}
