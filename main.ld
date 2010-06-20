ENTRY(main)
SECTIONS {
    .text 0x1000 : {
        bin/main.o *(.text)
        *(.rodata)
        . = ALIGN(4096);
    }
    .data : {
        *(.data)
        . = ALIGN(4096);
    }
}
