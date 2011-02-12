OUTPUT_FORMAT("a.out-i386-linux")
ENTRY(main)
SECTIONS {
    c = main;
    . = 0x8000000;
    .text : {
        __text__ = .;
        *(.text .rodata .gnu.linkonce.*)
    }
    .data : {
        __data__ = .;
        *(.data)
    }
    .bss : {
        __bss__ = .;
        *(.bss)
    }
    /DISCARD/ : {
        *(.note .note.* .gnu.* .dynsym .comment .pdr .init .plt .fini .interp)
    }
}

