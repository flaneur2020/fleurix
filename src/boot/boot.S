[bits 16]

align 4

[global _start]
_start:

    ; read the rest sectors to 0x10000
_reset_drive.1:
    mov     ah, 0                ; RESET-command
    int     13h                  ; Call interrupt 13h
    or      ah, ah               ; Check for error code
    jnz     _reset_drive.1       ; Try again if ah != 0

    ; load kernel to 0x10000 = 1000h:0000h
    mov     ax, 1000h
    mov     es, ax
    mov     ds, ax
    mov     bx, 0000h           ; Destination address = 1000h:0000

    mov     ah, 02h             ; READ SECTOR-command
    mov     al, 64              ; Number of sectors to read
    mov     ch, 0               ; Cylinder = 0
    mov     cl, 02h             ; Sector = 2
    mov     dh, 0               ; Head = 0
    int     13h                 ; Call interrupt 13h
    or      ah, ah              ; Check for error code
    jnz     _reset_drive.1      ; Try again if ah != 0

    ;
    ; prepare to enter protect mode
    ;

    ; Enable the A20
seta20.1:
    in      al, byte 0x64
    test    al, byte 0x2
    jnz     seta20.1
    mov     al, byte 0xd1
    out     byte 0x64, al
seta20.2:
    in      al, byte 0x64
    test    al, byte 0x2
    jnz     seta20.2
    mov     al, byte 0xdf
    out     byte 0x60, al

    ; clear registers
    xor     ax, ax
    mov     ds, ax
    mov     ss, ax
    mov     es, ax

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
    ; move stack to 0xfffc! it grows downwards
    mov     esp, 0FFFCh
    ; jump to C!
    ; jump to 0x10000!
    ; never return it should  be
    jmp     08h:010000h

_hang: 
    jmp     _hang

gdt:
gdt_null:
    dd  0, 0
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

; Magic number for sector
times 510-($-$$) db 0
dw 0xAA55
