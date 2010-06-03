[bits 16]
[section .text]

align 4
extern  main

_start:
    ;
    ; prepare to enter protect mode
    ;
    xor     ax,  ax
    mov     ds,  ax

    ; clear the intrrupt
    cli
    ; load gdt
    lgdt    [gdt_desc]
    ; switch on PE in cr0
    mov     eax, cr0
    or      eax, 1
    mov     cr0, eax
    ; jump, set seg registers as selector
    jmp     08h:_start_pm

[bits 32]
_start_pm:
    mov     ax,  10h
    mov     ds,  ax
    mov     ss,  ax
    mov     esp, 090000h              
    call    main

_loop: 
    jmp     _loop


gdt:
gdt_null:
    dd  0
    dd  0
gdt_code:
    ; dword 1
    dw  0xffff
    dw  0
    ; dword 2
    db  0
    db  10011010b
    db  11001111b
    db  0
gdt_data:
    ; dword 1
    dw  0xffff
    dw  0
    ; dword 2
    db  0
    db  10010010b
    db  11001111b
    db  0
gdt_end:

gdt_desc:
    dw  gdt_end - gdt - 1
    dd  gdt

;times 510-($-$$) db 0
;dw 0xAA55
