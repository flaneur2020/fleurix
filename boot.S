[bits 16]
[section .text]

_start:
    mov     ax, cs
    mov     ds, ax
    mov     es, ax
    call    _print_str

_print_str:
    mov     ax, str
    mov     cx, len
    mov     bp, ax
    mov     bx, 0x000c
    mov     dl, 0
    mov     ax, 0x1301
    int     0x10

_loop: 
    jmp     _loop

str: db      "screw you guys all fucked up~",10,13
len: equ     $-str

times 510-($-$$) db 0
dw 0xAA55
