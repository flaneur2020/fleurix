#include <param.h>
#include <x86.h>
#include <kern.h>
#include <syscall.h>
#include <sched.h>


int errno = 0;

/***********************************************************/

void sys_debug(struct trap_frame *r){
    printf("%x", current->p_pid);
    //debug_proc_list();
}

void sys_fork(struct trap_frame *r){
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

void do_syscall(struct trap_frame *r){
    //printf("do_syscall();\n");
    void (*handler)(struct trap_frame *r);
    handler = sys_routines[r->eax];
    
    if(handler){
        handler(r);
    }
}

