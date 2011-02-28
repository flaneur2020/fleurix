OUTPUT_FORMAT("a.out-i386-linux")
ENTRY(_start)
SECTIONS {
    c = _start;
    . = 0x8000020;
    .text : {
        __text__ = .;
        bin/usr/entry.o(.text) *(.text)
        *(.rodata .gnu.linkonce.*)
        . = ALIGN(0x1000);
        __text_end__ = .;
    }
    .data : {
        __data__ = .;
        *(.data)
        . = ALIGN(0x1000);
    }
    .bss : {
        __bss__ = .;
        *(.bss)
        . = ALIGN(0x1000);
    }
    /DISCARD/ : {
        *(.note .note.* .gnu.* .dynsym .comment .pdr .init .plt .fini .interp)
    }
}

