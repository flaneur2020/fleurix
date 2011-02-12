OUTPUT_FORMAT("binary")
ENTRY(_start)
SECTIONS {
    .text 0x7c00 : {
        *(.text)
        *(.rodata)
    }
    .data : {
        *(.data)
    }
}
