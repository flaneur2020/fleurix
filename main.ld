ENTRY(main)
SECTIONS {
    __begin__ = .;
    .text 0x10000 : {
        __code__ = .;
        bin/main.o *(.text);
        . = ALIGN(4096);
    }
    .data : {
        __data__ = .;
        *(.data);
        *(.rodata);
        . = ALIGN(4096);
    }
    .bss : {
        __bss__ = .;
        *(.bss);
        . = ALIGN(4096);
    }
    __end__ = .;
}
