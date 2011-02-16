; the entry of all user programs
; make some initialization and jump to main()
; TODO:

[global _start]
[extern main]
_start:
    call main
_spin:
    jmp _spin

[section .data]
[global errno]
errno dw 0
