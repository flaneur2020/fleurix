ENTRY(main)
SECTIONS {
    .text 0x100000 : {
        __kbegin__ = .;
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
    __kend__ = .;
}
