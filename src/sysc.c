#include <param.h>
#include <x86.h>
#include <kern.h>
#include <sysc.h>

int errno = 0;

/***********************************************************/

void sys_debug(struct regs *r){
    printf("int x80;; eip=%x! \n", r->eip);
}

void sys_fork(struct regs *r){
    int ret = copy_proc(r);
    if (ret<0){
        panic("error fork()\n");
    }
    r->eax = ret;
}

// NOTE: for debug right now
_syscall0(0, int, debug);
_syscall0(1, int, fork);

/***********************************************************/

static uint sys_routines[NSYSC] = {
    [0] = &sys_debug,
    [1] = &sys_fork,
    0,
}; 

void do_syscall(struct regs *r){
    printf("do_syscall();\n");
    void (*handler)(struct regs *r);
    handler = sys_routines[r->eax];
    
    if(handler){
        handler(r);
    }
}

