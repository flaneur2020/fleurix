#include <param.h>
#include <x86.h>
#include <proto.h>
#include <proc.h>

#include <unistd.h>

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

void nosys(struct trap *tf){
    tf->eax = -ENOSYS;
}

void sys_debug(struct trap *tf){
    printf("%d", tf->ebx);
}

void sys_fork(struct trap *tf){
    int ret = copy_proc(tf);
    if (ret<0){
        panic("error fork()\n");
    }
    tf->eax = ret;
}

void sys_nice(struct trap *tf){
    int n = tf->ebx & 0xff;
    if (n > 20) n = 20;
    if (n<-19) n=-19;
}

void sys_getpid(struct trap *tf){
    printf("pid: %x\n", cu->p_pid);
    tf->eax = cu->p_pid;
}

/* ---------------------------------------------- */

/* returns 1 on current is super user(aka. root) */
uint suser(){
    if (cu->p_uid == 0) {
        return 1;
    }
    return 0;
}

/* get a char from userspace */
char getuc(char *va){
}

/* put a char into userspace */
char putuc(char *va){
}

/* returns a error code. maybe useless */
void syserr(uint err){
    cu->p_error = err;
}

/* common handlers for all syscalls */
void do_syscall(struct trap *tf){
    void (*fn)(struct trap *tf);
    if (tf->eax > NSYSC) {
        panic("bad syscall");
    }
    fn = sys_routines[tf->eax];
    
    if(fn){
        (*fn)(tf);
    }
}

