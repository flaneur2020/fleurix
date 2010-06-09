[bits 32]
extern main
jmp main

; TODO: fill some isr here

; isr without error code
%macro ISR__ 1
    global _isr%1
    _isr%1:
        cli
        push    byte 0
        push    byte %1
        jmp     isr_common_stub
%endmacro
; isr 
%macro ISR_E 1
    global _isr%1
    _isr%1:
        cli
        push    byte %1
        jmp     isr_common_stub
%endmacro

ISR__ 0	    ;Division By Zero Exception	            No
ISR__ 1	    ;Debug Exception	                    No
ISR__ 2	    ;Non Maskable Interrupt Exception	    No
ISR__ 3	    ;Breakpoint Exception	                No
ISR__ 4	    ;Into Detected Overflow Exception	    No
ISR__ 5	    ;Out of Bounds Exception	            No
ISR__ 6	    ;Invalid Opcode Exception	            No
ISR__ 7	    ;No Coprocessor Exception	            No
ISR_E 8	    ;Double Fault Exception	                Yes
ISR__ 9	    ;Coprocessor Segment Overrun Exception	No
ISR_E 10	;Bad TSS Exception	                    Yes
ISR_E 11	;Segment Not Present Exception	        Yes
ISR_E 12	;Stack Fault Exception	                Yes
ISR_E 13	;General Protection Fault Exception	    Yes
ISR_E 14	;Page Fault Exception	                Yes
ISR__ 15	;Unknown Interrupt Exception	        No
ISR__ 16	;Coprocessor Fault Exception	        No
ISR__ 17	;Alignment Check Exception (486+)	    No
ISR__ 18	;Machine Check Exception (Pentium/586+)	No
ISR__ 19
ISR__ 20
ISR__ 21
ISR__ 22
ISR__ 23
ISR__ 24
ISR__ 25
ISR__ 26
ISR__ 27
ISR__ 28
ISR__ 29
ISR__ 30
ISR__ 31

[extern fault_handler]

isr_common_stub:
    pusha
    push ds
    push es
    push fs
    push gs
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov eax, esp
    push eax
    mov eax, fault_handler
    call eax
    pop eax
    pop gs
    pop fs
    pop es
    pop ds
    popa
    add esp, 8
    iret


