#include <param.h>
#include <x86.h>
#include <proto.h>
#include <proc.h>

#include <unistd.h>

/*
 * sys1.c
 *
 * syscalls on proc, time and misc.
 *
 * */

/* --------------------------- */

int sys_debug(struct trap *tf){
    printf("%d", tf->ebx);
}

int sys_fork(struct trap *tf){
    int ret = copy_proc(tf);
    if (ret<0){
        panic("bad fork()\n");
    }
    return ret;
}

int sys_nice(struct trap *tf){
    int n = tf->ebx & 0xff;
    if (n > 20) n = 20;
    if (n<-19) n=-19;
}

int sys_getpid(struct trap *tf){
    printf("pid: %x\n", cu->p_pid);
    return cu->p_pid;
}
