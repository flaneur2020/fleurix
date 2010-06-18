[bits 16]
align 4

_start:

    ; read the rest sectors to 0000:1000
_reset_drive:
    mov     ah, 0                ; RESET-command
    int     13h                  ; Call interrupt 13h
    or      ah, ah               ; Check for error code
    jnz     _reset_drive         ; Try again if ah != 0

    ; a TODO here, this code cannot load too much code :(
    mov     ax, 0
    mov     es, ax
    mov     bx, 0x1000          ; Destination address = 1000:0000

    mov     ah, 02h             ; READ SECTOR-command
    mov     al, 02fh            ; Number of sectors to read
    mov     ch, 0               ; Cylinder = 0
    mov     cl, 02h             ; Sector = 2
    mov     dh, 0               ; Head = 0
    int     13h                 ; Call interrupt 13h
    or      ah, ah              ; Check for error code
    jnz     _reset_drive        ; Try again if ah != 0

    ;
    ; prepare to enter protect mode
    ;

    ; Enable the A20
    in      al, 92h
    or      al, 02h
    out     92h, al

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
    mov     esp, 090000h
    ; jump to C!
    ; never return it should  be
    jmp     08h:01000h

_hang: 
    jmp     _hang


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

; Magic number for sector
times 510-($-$$) db 0
dw 0xAA55
