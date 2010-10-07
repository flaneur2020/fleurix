#include <param.h>
#include <x86.h>
#include <kern.h>
#include <sched.h>
#include <unistd.h>

int errno = 0;

/***********************************************************/

void sys_debug(struct trap_frame *tf){
    printf("%x", tf->ebx);
}

void sys_fork(struct trap_frame *tf){
    int ret = copy_proc(tf);
    if (ret<0){
        panic("error fork()\n");
    }
    tf->eax = ret;
}

/***********************************************************/

static uint sys_routines[NSYSC] = {
    [0] = &sys_debug,
    [1] = &sys_fork,
    0,
}; 

void do_syscall(struct trap_frame *r){
    //printf("do_syscall();\n");
    void (*handler)(struct trap_frame *r);
    handler = sys_routines[r->eax];
    
    if(handler){
        handler(r);
    }
}

