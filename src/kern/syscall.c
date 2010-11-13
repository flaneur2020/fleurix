#include <param.h>
#include <x86.h>
#include <kern.h>
#include <proc.h>
#include <unistd.h>

int errno = 0;

/***********************************************************/

void sys_debug(struct trap *tf){
}

void sys_fork(struct trap *tf){
    int ret = copy_proc(tf);
    if (ret<0){
        panic("error fork()\n");
    }
    tf->eax = ret;
}

void sys_nice(struct trap *tf){
    current->p_nice = tf->ebx & 0xff;
}

void sys_putn(struct trap *tf){
    printf("%x", tf->ebx);
}

/***********************************************************/

static uint sys_routines[NSYSC] = {
    [SYS_debug] = &sys_debug,
    [SYS_putn]  = &sys_putn,
    [SYS_fork]  = &sys_fork,
    [SYS_nice]  = &sys_nice,
    0,
}; 

void do_syscall(struct trap *r){
    //printf("do_syscall();\n");
    void (*handler)(struct trap *r);
    handler = sys_routines[r->eax];
    
    if(handler){
        handler(r);
    }
}

