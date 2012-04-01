#include <param.h>
#include <x86.h>
#include <proto.h>
#include <proc.h>

#include <unistd.h>

/*
 * sys.c 
 *
 * this file implies the common handler of all syscalls - do_syscall();
 * each syscall has got one syscall number, and a trap frame as parameters, 
 * eax is it's number, and ebx, ecx, edx, as parameters, return value 
 * stores in eax.
 *
 * commonly returns -errno on error.
 *
 * */

int errno = 0;

static uint sys_routines[NSYSC] = {
    [NR_debug]      = &sys_debug,
    [NR_setup]      = &sys_setup,
    //
    [NR_access]     = &sys_access,
    [NR_creat]      = &sys_creat,
    [NR_open]       = &sys_open,
    [NR_close]      = &sys_close,
    [NR_fcntl]      = &sys_fcntl,
    [NR_write]      = &sys_write,
    [NR_read]       = &sys_read,
    [NR_stat]       = &sys_stat,
    [NR_fstat]      = &sys_fstat,
    [NR_dup]        = &sys_dup,
    [NR_dup2]       = &sys_dup2,
    [NR_link]       = &sys_link,
    [NR_unlink]     = &sys_unlink,
    [NR_chroot]     = &sys_chroot,
    [NR_chdir]      = &sys_chdir,
    //
    [NR_fork]       = &sys_fork,
    [NR_exec]       = &sys_exec,
    [NR__exit]      = &sys__exit,
    [NR_waitpid]    = &sys_waitpid,
    [NR_wait]       = &sys_wait,
    //
    [NR_nice]       = &sys_nice,
    [NR_getpid]     = &sys_getpid,
    [NR_getppid]    = &sys_getppid,
    [NR_getuid]     = &sys_getuid,
    [NR_geteuid]    = &sys_geteuid,
    [NR_getgid]     = &sys_getgid,
    [NR_getegid]    = &sys_getegid,
    [NR_getpgrp]    = &sys_getpgrp,
    [NR_setpgrp]    = &sys_setpgrp,
    [NR_setreuid]   = &sys_setreuid,
    [NR_setregid]   = &sys_setregid,
    //
    [NR_pause]      = &sys_pause,
    [NR_kill]       = &sys_kill,
    [NR_signal]     = &sys_signal,
    [NR_sigaction]  = &sys_sigaction,
    [NR_sigreturn]  = &sys_sigreturn,
    &nosys,
};

/***********************************************************/

int nosys(struct trap *tf){
    syserr(ENOSYS);
    return -1;
}

/* ---------------------------------------------- */

/* returns 1 on current is super user(aka. root) */
uint suser(){
    if (cu->p_euid == 0) {
        return 1;
    }
    return 0;
}

/* returns a error code. always returns -1. */
int syserr(uint err){
    cu->p_error = err;
    return -1;
}

/* common handlers for all syscalls, if an syserr raised,
 * returns a negative number: 0 - cu->p_error. */
void do_syscall(struct trap *tf){
    int ret;
    int (*func)(struct trap *tf);

    if (tf->eax > NSYSC) {
        panic("bad syscall");
    }
    cu->p_error = 0;
    func = sys_routines[tf->eax];
    
    if (func == NULL)
        func = &nosys;
    ret = (*func)(tf);
    //
    if (ret < 0)
        tf->eax = 0 - cu->p_error;
    else
        tf->eax = ret;
}
