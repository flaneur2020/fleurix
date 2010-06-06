OUTPUT_FORMAT("binary")
ENTRY("entry")
SECTIONS {
    .text 0x1000 : {
        *(.text)
        *(.rodata)
    }
    .data : {
        *(.data)
    }
}
