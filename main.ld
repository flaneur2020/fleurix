ENTRY(main)
SECTIONS {
    .text 0x10000 : {
        bin/main.o *(.text)
        . = ALIGN(4096);
    }
    .data : {
        *(.data)
        *(.rodata)
        . = ALIGN(4096);
    }
}
