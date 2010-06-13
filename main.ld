ENTRY("_entry")
SECTIONS {
    .text 0x1000 : {
        *(.text)
        *(.rodata)
        . = ALIGN(4096);
    }
    .data : {
        *(.data)
        . = ALIGN(4096);
    }
}
