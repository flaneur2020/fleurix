#ifndef SETJMP_H
#define SETJMP_H

struct jmp_buf {
    int    esp;
    int    eip;
    int    ebx; // - callee registers
    int    ecx;
    int    edx;
    int    edi;
    int    esi;
    int    ebp;
    uint   __sigmask;
};

#endif
