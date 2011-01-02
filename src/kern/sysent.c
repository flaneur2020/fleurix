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
    [NR_debug]  = &sys_debug,
    [NR_setup]  = &sys_setup,
    [NR_access] = &sys_access,
    [NR_fork]   = &sys_fork,
    [NR_nice]   = &sys_nice,
    [NR_getpid] = &sys_getpid,
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
    if (cu->p_uid == 0) {
        return 1;
    }
    return 0;
}

/* returns a error code. maybe useless */
void syserr(uint err){
    cu->p_error = err;
}

/* common handlers for all syscalls */
void do_syscall(struct trap *tf){
    int ret;
    int (*func)(struct trap *tf);

    if (tf->eax > NSYSC) {
        panic("bad syscall");
    }
    cu->p_error = 0;
    func = sys_routines[tf->eax];
    
    if(func != NULL)
        ret = (*func)(tf);
    tf->eax = ret;
    tf->ebx = cu->p_error;
}
