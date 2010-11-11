[bits 32]
[extern main]

jmp main

[global _do_swtch]
_do_swtch:
    mov eax, dword [esp+4]
    pop dword [eax]
    mov dword [eax+4], esp
    mov dword [eax+8], ebx
    mov dword [eax+12], ecx
    mov dword [eax+16], edx
    mov dword [eax+20], esi
    mov dword [eax+24], edi
    mov dword [eax+28], ebp

    mov eax, dword [esp+4]

    mov ebp, dword [eax+28]
    mov edi, dword [eax+24]
    mov esi, dword [eax+20]
    mov edx, dword [eax+16]
    mov ecx, dword [eax+12]
    mov ebx, dword [eax+8]
    mov esp, dword [eax+4]
    push dword [eax]
    ret
