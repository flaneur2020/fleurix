
#include <param.h>
#include <x86.h>
#include <kern.h>
#include <sysc.h>

int errno = 0;

/***********************************************************/

void sys_fork(struct regs *r){
    copy_proc(r);
    printf("fork();\n");
}

_syscall0(0, int, fork);

/***********************************************************/

static uint sys_routines[NSYSC] = {
    [0] = &sys_fork,
    0,
}; 

void do_syscall(struct regs *r){
    void (*handler)(struct regs *r);
    handler = sys_routines[r->eax];
    
    if(handler){
        handler(r);
    }
}

