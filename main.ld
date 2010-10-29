ENTRY(main)
SECTIONS {
    __begin__ = .;
    .text 0x10000 : {
        __code__ = .;
        bin/entry.o(.text) bin/main.o(.text) *(.text);
        . = ALIGN(4096);
    }
    .data : {
        __data__ = .;
        *(.rodata);
        *(.data);
        . = ALIGN(4096);
    }
    .bss : {
        __bss__ = .;
        *(.bss);
        . = ALIGN(4096);
    }
    __end__ = .;
    buffers = 0x100000;
}
