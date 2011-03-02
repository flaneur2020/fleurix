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
    int ret = do_fork(tf);
    if (ret<0){
        panic("bad fork()\n");
    }
    return ret;
}

int sys_exit(struct trap *tf){
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

/* --------------------------------------- */
