align 4

[bits 16]
    jmp _start

;; read_sect(sn, *buf), load a sector to memory.
;; input : si - logical sector number(starts at 0)
;;         di - *buf
;;
_read_sect:
    push    cx
    push    bx
    mov     ax, si              ; disk sector number
    mov     bx, di              ; buffer address.
    ; ch = cylinder = sn / 36
    mov     cl, 36
    div     cl
    mov     ch, al
    ; dh = head = (sn%36)/18
    mov     al, ah
    mov     ah, 0
    mov     cl, 18
    div     cl
    mov     dh, al
    ; cl = sector = (ln%36)%18+1
    mov     cl, ah
    inc     cl
    ; dl = drive = 0;
    mov     dl, 0
    ; raise int 13h
    mov     ax, 201h
    int     13h
    pop     bx
    pop     cx
    ret

;; -----------------------------------------------------------------------
;;
;; bootloader starts here
;;

[global _start]
_start:
    xor     ax, ax
    mov     ds, ax
    mov     ss, ax
    mov     sp, 0x2000

_reset_drive:
    mov     ah, 0                ; RESET-command
    int     0x13                 ; Call interrupt 13h
    or      ah, ah               ; Check for error code
    jnz     _reset_drive         ; Try again if ah != 0

    ;
    ; load kernel to 0x10000 (es:bx = 0x1000: 0000)
    ;
    mov     ax, 0x1000
    mov     es, ax               ; es = 1000h
    mov     di, 0                ;
    mov     si, 1                ;
    mov     cx, 128              ; read 128 sectors, 64kb
_read_sect_loop:
    call    _read_sect
    inc     si
    add     di, 0x200
    dec     cx
    jnz     _read_sect_loop


    ;
    ; prepare to enter protect mode
    ;

    ; Enable the A20
seta20.1:
    in      al, 0x64
    test    al, 0x2
    jnz     seta20.1
    mov     al, 0xd1
    out     byte 0x64, al
seta20.2:
    in      al, 0x64
    test    al, 0x2
    jnz     seta20.2
    mov     al, 0xdf
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

;; -------------------------------------------------------------

[bits 32]

_start_pm:
    mov     ax,  10h
    mov     ds,  ax
    mov     ss,  ax
    mov     es,  ax
    mov     gs,  ax
    ; move stack to 0x1000
    mov     esp, 1000h

    ; copy 0x10000 to 0x100000
    cld
    mov     esi, 10000h
    mov     edi, 100000h
    mov     ecx, 10000h ; copy 64kb
    rep movsb

    ; jump to C!
    ; never return it should  be
    jmp     08h:100000h

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
