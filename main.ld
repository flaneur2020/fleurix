OUTPUT_FORMAT("binary")
ENTRY("init")
SECTIONS {
    .text 0x1000 : {
        *(.text)
        *(.rodata)
    }
    .data : {
        *(.data)
    }
}
