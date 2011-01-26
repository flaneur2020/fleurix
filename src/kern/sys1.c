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
    printk("%d", tf->ebx);
}

int sys_fork(struct trap *tf){
    int ret = copy_proc(tf);
    if (ret<0){
        panic("bad fork()\n");
    }
    return ret;
}

int sys_exit(struct trap *tf){
}

int sys_exec(struct trap *tf){
}

int sys_brk(struct trap *tf){
}

int sys_pause(struct trap *tf){
}

/* --------------------------------------- */
