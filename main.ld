OUTPUT_FORMAT("binary")
ENTRY("main")
SECTIONS {
    .text 0x1000 : {
        *(.text)
        *(.rodata)
    }
    .data : {
        *(.data)
    }
}
