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
    dump_tf(tf);
    for(;;);
    return 0;
}

int sys_fork(struct trap *tf){
    return do_fork(tf);
}

int sys__exit(struct trap *tf){
    int code = (int)tf->ebx;
    return do_exit(code);
}

/* exec(char *path, char **argv) */
int sys_exec(struct trap *tf){
    int r;
    char *path = (char*)tf->ebx;
    char **argv = (char**)tf->ecx;

    if (vm_verify(path, strlen(path)+1)<0 || vm_verify(argv, 1)<0) {
        syserr(EINVAL);
    }
    return do_exec(path, argv);
}

int sys_brk(struct trap *tf){
}

/* waitpid(int pid, int *stat, int opt) */
int sys_waitpid(struct trap *tf){
    int pid = (int)tf->ebx;
    int *stat = (int*)tf->ecx;
    int opt = (int)tf->edx;

    return do_waitpid(pid, stat, opt);
}

int sys_wait(struct trap *tf){
}

/* --------------------------------------- */
