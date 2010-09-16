
#include <param.h>
#include <x86.h>
#include <kern.h>
#include <sysc.h>

int errno = 0;

/***********************************************************/

void sys_debug(struct regs *r){
    printf("eip: %x\n", r->eip);
}

void sys_fork(struct regs *r){
    int ret;
    ret = copy_proc(r);
    if (ret<0){
        panic("error fork()\n");
    }
    r->eax = ret;
}

// NOTE: for debug right now
void sys_putc(struct regs *r){
    char ch = r->eax;
    putch(ch);
    r->eax = 0;
}

_syscall0(0, int, debug);
_syscall0(1, int, fork);
_syscall1(2, int, putc, char);

/***********************************************************/

static uint sys_routines[NSYSC] = {
    [0] = &sys_debug,
    [1] = &sys_fork,
    [2] = &sys_putc,
    0,
}; 

void do_syscall(struct regs *r){
    void (*handler)(struct regs *r);
    handler = sys_routines[r->eax];
    
    if(handler){
        handler(r);
    }
}

