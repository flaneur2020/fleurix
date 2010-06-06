OUTPUT_FORMAT("binary")
ENTRY("boot")
SECTIONS {
    .text 0x7c00 : {
        *(.text)
        *(.rodata)
    }
    .data : {
        *(.data)
    }
}
