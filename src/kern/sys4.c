#include <param.h>
#include <x86.h>
#include <proto.h>
#include <proc.h>
#include <signal.h>
#include <setjmp.h>

#include <unistd.h>

/*
 * sys4.c 
 * syscalls on signal
 *
 * */

/* kill(int pid, int sig); */
int sys_kill(struct trap *tf){
    int pid = (int)tf->ebx;
    int sig = (int)tf->ecx;
    
    return do_kill(pid, sig);
}

int sys_pause(struct trap *tf){
}

int sys_signal(struct trap *tf){
}

int sys_sigaction(struct trap *tf){
    int pid = (int) tf->ebx;
    int sa = (struct sigaction*) tf->ecx;
    int old_sa = (struct sigaction*)tf->edx;

    return do_sigaction(pid, sa, old_sa);
}

/* return to the  */
int sys_sigreturn(struct trap *tf){
    struct jmp_buf *ujbuf;
    ujbuf = (struct jmp_buf*)(tf->eip-46);
    tf->esp = ujbuf->esp;
    tf->eip = ujbuf->eip;
    tf->ebx = ujbuf->ebx;
    tf->ecx = ujbuf->ecx;
    tf->edx = ujbuf->edx;
    tf->edi = ujbuf->edi;
    tf->esi = ujbuf->esi;
    tf->ebp = ujbuf->ebp;
    cu->p_sigmask = ujbuf->__sigmask;
    return 0;
}

int sys_waitpid(struct trap *tf){
}

int sys_alarm(struct trap *tf){
}

/* --------------------------- */

int sys_sync(struct trap *tf){
    syserr(ENOSYS);
}

