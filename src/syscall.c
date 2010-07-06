
#include <param.h>
#include <x86.h>
#include <kern.h>
#include <sysc.h>

static uint sys_routines[NSYSC+1] = {
    &sys_fork,
    0,
}; 

void sys_fork(struct regs *r){
    printf("fork();\n");
}

void do_syscall(struct regs *r){
    void (*handler)(struct regs *r);
    handler = sys_routines[r->eax];
    
    if(handler){
        handler(r);
    }
}
