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
    printf("%d", tf->ebx);
}

/***********************************************************/

void do_syscall(struct trap *r){
    //printf("do_syscall();\n");
    void (*fn)(struct trap *r);
    fn = sys_routines[r->eax];
    
    if(fn){
        (*fn)(r);
    }
}

