#ifndef SETJMP_H
#define SETJMP_H

struct jmp_buf {
    int    eip;
    int    esp;
    int    ebx; // - callee registers
    int    ecx;
    int    edx;
    int    esi;
    int    edi;
    int    ebp;
    uint   __sigmask;
};

#endif
