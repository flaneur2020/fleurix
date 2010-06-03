OUTPUT_FORMAT("binary")
ENTRY("boot")
SECTIONS {
    . = 0x7c00;
    .text : {
      *(.text)
    }
}
